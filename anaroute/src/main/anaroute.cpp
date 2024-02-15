/**
 * @file   anaroute.cpp
 * @brief  Analog Router
 * @author Hao Chen
 * @date   09/01/2019
 *
 **/

#include "anaroute.hpp"
#include "src/util/util.hpp"
#include "src/util/timeUsage.hpp"
#include "src/db/dbCir.hpp"
#include "src/parser/parser.hpp"
#include "src/gr/grMgr.hpp"
#include "src/ta/taMgr.hpp"
#include "src/dr/drMgr.hpp"
#include "src/drc/drcMgr.hpp"
#include "src/writer/writer.hpp"
#include "src/acs/acsMgr.hpp"
#include "src/post/postMgr.hpp"

PROJECT_NAMESPACE_START

Anaroute::Anaroute(int argc, char** argv) {
 
  std::srand(1234);
  util::showSysInfo();

  TimeUsage timer;
  timer.start(TimeUsage::FULL);

  parseArgs(argc, argv);
  
  CirDB cir;

  // args
  const String_t lefFile          = _args.get<String_t>("tech_lef");
  const String_t techFile         = _args.get<String_t>("tech_file");
  const String_t placeFile        = _args.get<String_t>("placement_layout");
  const String_t designFile       = _args.get<String_t>("design_file");
  const String_t symnetFile       = _args.get<String_t>("symnet");
  const String_t iopinFile        = _args.get<String_t>("iopin");
  const String_t powerFile        = _args.get<String_t>("power");
  const String_t specFile         = _args.get<String_t>("spec");
  const String_t outFile          = _args.get<String_t>("out");
  const String_t outGuideFile     = _args.get<String_t>("out_guide");
  //const String_t outGuideGdsFile  = _args.get<String_t>("out_guide_gds");
  const String_t dumbFile         = _args.get<String_t>("fuck");
  const bool     bFlatten         = _args.exist("flatten");
  
  bool bUseGrid = true;
  bool bUseSymFile = false;

  // parse files
  timer.start(TimeUsage::PARTIAL);
  Parser par(cir);
  par.parseLef(lefFile);
  par.parseTechfile(techFile);
  par.parseGds(placeFile);
  //par.parseIspd08(designFile);
  par.parseNetlist(designFile);
  if (symnetFile != "") {
    par.parseSymNet(symnetFile);
    bUseSymFile = true;
  }
  if (iopinFile != "")
    par.parseIOPin(iopinFile);
  if (powerFile != "")
    par.parsePower(powerFile);
  if (specFile != "")
    par.parseNetSpec(specFile);
  //par.correctPinNBlkLoc(); // patch for placement bugs

  //cir.printInfo();
  cir.buildSpatial();
  cir.markBlks();
  cir.checkNetSymSelfSym();
  timer.showUsage("Circuit database construction", TimeUsage::PARTIAL);
  //for (UInt_t i = 0; i < cir.numNets(); ++i) {
    //const auto& net = cir.net(i);
    //std::cerr << net.name() << " " << net.idx() + 1 << std::endl;
  //}
  
  // global routing
  //timer.start(TimeUsage::PARTIAL);
  //GrMgr gr(cir);
  //gr.solve();
  //timer.showUsage("Global Routing", TimeUsage::PARTIAL);
  cir.buildSpatialNetGuides();

  // track assignment
  //TaMgr ta(cir);
  //ta.solve();

  // Generate access points
  timer.start(TimeUsage::PARTIAL);
  AcsMgr acs(cir);
  acs.computeAcs();
  timer.showUsage("Access Points Generation", TimeUsage::PARTIAL);
  
  //cir.visualize();
  
  // detailed routing
  timer.start(TimeUsage::PARTIAL);
  DrcMgr drc(cir);
  DrMgr dr(cir, drc);
  dr.solve(bUseGrid, bUseSymFile);
  timer.showUsage("Detailed Routing", TimeUsage::PARTIAL);

  // post processing
  timer.start(TimeUsage::PARTIAL);
  PostMgr post(cir);
  post.solve();
  timer.showUsage("Post Processing", TimeUsage::PARTIAL);

  // evaluation
  cir.computeNSetAllNetStatistics();
  cir.computeTotalStatistics();

  // write files
  Writer wr(cir);
  //wr.writeIspd08(_args.get<String_t>("design_file") + ".ispd08");
  wr.writeLayoutGds(_args.get<String_t>("placement_layout"), _args.get<String_t>("out"), bFlatten);
  if (outGuideFile != "")
    wr.writeGrGuide(_args.get<String_t>("out_guide"));
  //wr.writeGrGuideGds("a.gds");
  if (dumbFile != "")
    wr.writeDumb(_args.get<String_t>("placement_layout"), _args.get<String_t>("fuck"));

  timer.showUsage("Anaroute", TimeUsage::FULL);
  printf("Peak Memory Usage: %.2f MB\n", util::getPeakMemoryUsage() / MEM_SCALE);
  
}

void Anaroute::parseArgs(int argc, char** argv) {
  // mandatory
  _args.add<String_t>("tech_lef", '\0', "technology lef file");
  _args.add<String_t>("tech_file", '\0', "technology layer file");
  _args.add<String_t>("design_file", '\0', "design file");
  _args.add<String_t>("placement_layout", '\0', "placement layout file");
  _args.add<String_t>("out", 'o', "output file");
  // non-mandatory
  _args.add<String_t>("symnet", '\0', "symmetric nets file", false);
  _args.add<String_t>("iopin", '\0', "IO-Pin file", false);
  _args.add<String_t>("power", '\0', "power net file", false);
  _args.add<String_t>("spec", '\0', "net spec file", false);
  _args.add<String_t>("fuck", '\0', "fuck file", false);
  _args.add<String_t>("out_guide", '\0', "output global routing guide file", false);
  //_args.add<String_t>("out_guide_gds", '\0', "output global routing guide file (gds)", false);
  _args.add("flatten", '\0', "flatten output GDS");

  _args.parse_check(argc, argv);
}


PROJECT_NAMESPACE_END


