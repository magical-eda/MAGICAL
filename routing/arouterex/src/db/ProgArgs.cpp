#include <boost/algorithm/string.hpp> /// boost:iequals
#include "ProgArgs.h"

PROJECT_NAMESPACE_BEGIN

/* Command anoynmouse member function */


/// Construct and parse command anoynmouse options
CommandanoynmouseOptions::CommandanoynmouseOptions(int argc, char** argv)
{
    // Define options 
    // 1st argument is long name
    // 2nd argument is short name (no short name if '\0' specified)
    // 3rd argument is description
    // 4th argument is mandatory (optional. default is false)
    // 5th argument is default value  (optional. it used when mandatory is false)

    // 6th argument is extra constraint.
    //     example: cmdanoynmouse::range(1, 65535)
    //              cmdanoynmouse::oneof<std::string>("http", "https", "ssh", "ftp")
    _parser.add <std::string> ("designType", 'b', "input netlist type ispd08 etc.", false, "ispd08");
    _parser.add <std::string> ("designFile", 'f', "input netlist file", true);
    _parser.add <std::string> ("iopin", '\0', "input IO port definition file", false);
    _parser.add <std::string> ("symnet", '\0', "input symmetry net specification file", false);
    _parser.add <std::string> ("terminalFile", '\0', "input terminal file", false);
    _parser.add <std::string> ("placementLayout", '\0', "input GDS placement layout file", false);
    _parser.add <std::string> ("techLef", 'l', "technology LEF", true);
    _parser.add <std::string> ("techfile", 't', "techfile including layer ID", true);
    _parser.add <std::string> ("grAlgo", 'g', "global routing algorithm", false, "SequentialAStar");
    _parser.add <std::string> ("drAlgo", 'd', "detailed routing algorithm", false, "SequentialAStar");
    _parser.add <std::string> ("log", '\0', "log file", false, "");
    _parser.add <std::string> ("out", 'o', "Output layout file name", false, "out.gds");

    // boolean options don't need template
    //_parser.add               ("mute", '\0', "mute screen output");

    // Parse and check command anoynmouse
    // It returns only if command anoynmouse arguments are valid.
    // If arguments are invalid, a parser output error msgs then exit program.
    // If help flag ('--help' or '-?') is specified, a parser output usage message then exit program.
    _parser.parse_check(argc, argv);

    populateAndCheckOptions();
}


/*! Check command anoynmouse arguments and update options for regular parsing
 */
void CommandanoynmouseOptions::populateAndCheckOptions()
{
    // Populate options
    designType      = _parser.get<std::string>("designType");
    designFile      = _parser.get<std::string>("designFile");
    iopinFile       = _parser.get<std::string>("iopin");
    symnetFile      = _parser.get<std::string>("symnet");
    terminalFile    = _parser.get<std::string>("terminalFile");
    placementLayout = _parser.get<std::string>("placementLayout");
    techLef         = _parser.get<std::string>("techLef");
    techLayer       = _parser.get<std::string>("techfile");
    grAlgo          = _parser.get<std::string>("grAlgo");
    drAlgo          = _parser.get<std::string>("drAlgo");
    log             = _parser.get<std::string>("log");
    outputFile      = _parser.get<std::string>("out");
    //mute = _parser.exist("mute");
}

/* ProgArgs member function */
void ProgArgs::setDeisgnType(const std::string &designType)
{
    if (boost::iequals(designType, "ispd08"))
    {
        _designType = DesignBenchmarkType::ISPD08;
        return;
    }
    if (boost::iequals(designType, "ispd08pin"))
    {
        _designType = DesignBenchmarkType::ISPD08PIN;
        return;
    }
    ERR("%s: Unknown deisng type %s \n", __FUNCTION__, designType.c_str());
    /// others...
}

void ProgArgs::setGRAlgo(const std::string &grAlgo)
{
    if (boost::iequals(grAlgo, "sequentialastar"))
    {
        _grAlgo = GRAlgoType::SequentialAStar;
        return;
    }
    if (boost::iequals(grAlgo, "concurrenttopology"))
    {
        _grAlgo = GRAlgoType::ConcurrentTopologyGeneration;
        return;
    }
    /// others...
}

void ProgArgs::setDRAlgo(const std::string &drAlgo)
{
    if (boost::iequals(drAlgo, "sequentialastar"))
    {
        _drAlgo = DRAlgoType::SequentialAStar;
        return;
    }
    /// others...
}

ProgArgs parseProgArgsBrute(IntType argc, char** argv)
{
    bool hasDesignType = false;
    bool hasDesignName = false;
    bool hasTechLef = false;
    bool hasTechLayer = false;

    ProgArgs progArgs;
    
    while (argc)
    {
        if (std::strcmp(*argv, "-designType") == 0)
        {
            hasDesignType = true;
            argc--, argv++;
            progArgs.setDeisgnType(std::string(*argv));
            argc--, argv++;
            continue;
        }
        if (std::strcmp(*argv, "-designFile") == 0)
        {
            hasDesignName = true;
            argc--, argv++;
            progArgs.setDesignFileName(std::string(*argv));
            argc--, argv++;
            continue;
        }
        if (std::strcmp(*argv, "-lefTech")==0)
        {
            hasTechLef = true;
            argc--, argv++;
            progArgs.setTechLefFileName(std::string(*argv));
            argc--, argv++;
            continue;
        }
        if (std::strcmp(*argv, "-techLayer")==0)
        {
            hasTechLayer = true;
            argc--, argv++;
            progArgs.setTechLayerFileName(std::string(*argv));
            argc--, argv++;
            continue;
        }
        if (std::strcmp(*argv, "-grAlgo")==0)
        {
            argc--, argv++;
            progArgs.setGRAlgo(std::string(*argv));
            argc--, argv++;
            continue;
        }
        if (std::strcmp(*argv, "-drAlgo")==0)
        {
            argc--, argv++;
            progArgs.setDRAlgo(std::string(*argv));
            argc--, argv++;
            continue;
        }
        ERR("%s: Cannot parse argument %s \n", __FUNCTION__, *argv); 
    }
    AssertMsg(hasDesignType, "%s: No design type! \n", __FUNCTION__);
    AssertMsg(hasDesignName, "%s: No input design file!\n", __FUNCTION__);
    AssertMsg(hasTechLef, "%s: No technology lef! \n", __FUNCTION__);
    AssertMsg(hasTechLayer, "%s: No technology layer file!\n", __FUNCTION__);
    return progArgs;
}

std::string ProgArgs::toStr() const 
{
    std::ostringstream oss;
    oss << "Benchmark type: ";
    if (_designType == DesignBenchmarkType::ISPD08) { oss<<"ispd08"; }
    
    oss << " Benchmark file: "<<_designFileName;

    oss << " Lef: " << _techLefFileName;

    oss << " Techfile: "<<_techLayerFileName;

    oss << " GR algorithm: ";
    if (_grAlgo == GRAlgoType::SequentialAStar) { oss << "Sequential A* search"; }
    else if (_grAlgo == GRAlgoType::ConcurrentTopologyGeneration) { oss << " Concurrent topology candidate generation"; }

    oss << " DR algorithm: ";
    if (_drAlgo == DRAlgoType::SequentialAStar) { oss << "Sequential A* search"; }

    oss << " log: "<<_logFile;
    return oss.str();
}

ProgArgs parseProgArgsCMD(IntType argc, char** argv)
{
    CommandanoynmouseOptions opt(argc, argv);
    ProgArgs progArgs;
    progArgs.setDeisgnType(opt.designType);
    progArgs.setDesignFileName(opt.designFile);
    progArgs.setIoPinFileName(opt.iopinFile);
    progArgs.setSymNetFileName(opt.symnetFile);
    progArgs.setTerminalFileName(opt.terminalFile);
    progArgs.setPlacementLayoutFileName(opt.placementLayout);
    progArgs.setTechLefFileName(opt.techLef);
    progArgs.setTechLayerFileName(opt.techLayer);
    progArgs.setGRAlgo(opt.grAlgo);
    progArgs.setDRAlgo(opt.drAlgo);
    progArgs.setLogFile(opt.log);
    progArgs.setOutputFile(opt.outputFile);

    return progArgs;
}

PROJECT_NAMESPACE_END
