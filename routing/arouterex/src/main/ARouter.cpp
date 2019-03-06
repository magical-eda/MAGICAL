#include "ARouter.h"
#include "parser/ParserIspd08.h"
#include "parser/TechfileReader.h"
#include "parser/lefdef/LefReader.h"
#include "writer/gdsii/WriteGdsLayout.h"
#include "writer/gdsii/WriteGdsLayoutDb.h"
#include "grdr/IntraCellRouter.h"
#include "parser/ParserGds.h"
#include "parser/ParserIOPin.h"
#include "parser/ParserSymNet.h"
#include "grdr/PreProsessor.h"

PROJECT_NAMESPACE_BEGIN

void ARouter::operator()(int argc, char** argv)
{
    // parse in the program arguments
    this->readCMD(argc, argv);
    // Start message printer timer
    MsgPrinter::startTimer();
    // Open log file
    if (!_db.progArgs().logFile().empty())
    {
        MsgPrinter::openLogFile(_db.progArgs().logFile());
    }
    // Read other things
    read();
    
    // solve the routing core function
    bool routingSuccess = this->solve();
    if (!routingSuccess)
    {
        WRN("Routing failed! \n");
    }
    

    //Close log file
    if (!_db.progArgs().logFile().empty())
    {
        MsgPrinter::closeLogFile();
    }
}

bool ARouter::readCMD(int argc, char** argv)
{
    _db.parse(argc, argv);

    return true;
}

bool ARouter::read()
{
    // tech LEF
    LefReaderNameSpace::readLef(_db.progArgs().techLefFile(), _db.techDB(), _db.macroDB());
    // Design benchmark
    switch(_db.progArgs().designType())
    {
        case DesignBenchmarkType::ISPD08:
        {
            if (!ParserIspd08 (_db, _db.progArgs().designFileName(), _db.techDB().micro2DBU(1)).read()) { return false; }
            break;
        }
        case DesignBenchmarkType::ISPD08PIN:
        {
            if (!ParserIspd08(_db, _db.progArgs().designFileName(), _db.techDB().micro2DBU(1)).readPinShapeBenchmark()) { return false; }
            break;
        }
    }
    // Techfile
    TechfileReader::readTechfile(_db.progArgs().techLayerFile(), _db.techDB());
    // Placement layout
    if (!readPlacementLayout())
    {
        return false;
    }
    // IO pins
    if (_db.progArgs().isIopinFileSet())
    {
        ParserIOPin(_db).read(_db.progArgs().iopinFileName());
    }
    // Symnet 
    if (_db.progArgs().isSymNetFileSet())
    {
        ParserSymNet(_db).read(_db.progArgs().symNetFileName());
    }
    return true;
}

bool ARouter::readPlacementLayout()
{
    if (!_db.progArgs().isPlacementLayoutFileSet())
    {
        // No Placement layout file set, skip
        return true;
    }
    // Read in the placement layout and store the shapes as blockages
    ParserGds gdsParser = ParserGds(_db);
    if (!gdsParser.readPlacementLayout(_db.progArgs().placementLayoutFileName()))
    {
        ERR("Read the placement layout file failed !\n");
        return false;
    }
    // Save the read placement shapes as blockages
    if (!gdsParser.saveShapesAsBlockages())
    {
        ERR("Save placement layout failed !\n");
        _db.blockageArray().clear();
        // Not return because no blockages is ok for the router to run
    }
    return true;
}

bool ARouter::readTerminalFile()
{
    // Terminal file
    if (!ProcessTerminal(_db).process())
    {
        ERR("Process terminal configuration file failed !\n");
        return false;
    }
    return true;
}

bool ARouter::solve()
{
    //MsgPrinter::screenOff();
    if (!preprocessing()) { ERR("Data preprocessing failed !\n"); return false;}
    if (!solveGR())       { ERR("Global routing failed! \n"); return false; }
    if (!convertGR2DR())  { ERR("Failure in converting global routing result to detailed router! \n"); return false; }
    if (!solveDR())       { ERR("Detailed Routing failed! \n");  return false; }
    if (writeGDS())       { INF("Layout saved in %s \n", _db.progArgs().outputFile().c_str()); }
    else                  { ERR("GDS writer failed! \n"); return false; }
    return true;
}

bool ARouter::preprocessing()
{
    // Init the SearchInstruct
    _db.initInstr();
    // Terminal setting files
    if (!readTerminalFile())
    {
        return false;
    }
    // Herustically reordering terminals
    //this->orderTerminalDecendingBBox();
    // Symnets
    if (!ProcessSymNet(_db).process())
    {
        ERR("Process symmetry net configuration file failed !\n");
        return false;
    }
    // Self sym
    if (!ProcessSelfSymNet(_db).process())
    {
        ERR("Process self symmetry net configuration file failed !\n");
        return false;
    }
    return true;
}

bool ARouter::solveGR()
{
    INF("Start global routing... \n");
    if (OVERRIDE_GLOBAL_ROUTING_SETTING)
    {
        INF("Override global routing setting with tech LEF information \n");
        _db.overrideGlobalSettingWithLef();
    }
    if (!SKIP_GLOBAL_ROUTING)
    {
        return GlobalRouter(_db).solve();
    }
    else
    {
        return true;
    }
}

bool ARouter::convertGR2DR()
{
   // v2 to v1
   return GrDB_v2_to_DrDB_v2_converter(_db).convert();
}

bool ARouter::solveDR()
{
    //MsgPrinter::screenOff();
    INF("Start detailed routing...\n");
    //_db.drDB().initAll(&_db);
   return DetailedRouter(_db).solveGridlessAstarNCR();
}

bool ARouter::writeGDS()
{
    //if (0)
    if (_db.progArgs().isPlacementLayoutFileSet())
    {
        //WriteGdsLayoutDb(_db).debugWriteNet(0, "debug/crossp");
        //WriteGdsLayoutDb(_db).debugWriteNet(4, "debug/intern");
        //WriteGdsLayoutDb(_db).debugWriteNet(5, "debug/interp");
        //WriteGdsLayoutDb(_db).debugWriteNetArray({0, 4, 5}, "debug/short");
        return WriteGdsLayoutDb(_db).writeGds(true);
    }
    else
    {
        std::string outputFileName = _db.progArgs().outputFile();
        return WriteGdsLayout(_db).writeLayout(outputFileName);
    }
}


void ARouter::orderTerminalDecendingBBox()
{
    for (auto &net : _db.grDB().nets())
    {
        auto func  = [&] (IndexType termIdx1, IndexType termIdx2)
        {
            return _db.terminal(termIdx1).bbox().area() > _db.terminal(termIdx2).bbox().area();
        };
        std::sort(net.terminalArray().begin(), net.terminalArray().end(), func);
    }
}

PROJECT_NAMESPACE_END

