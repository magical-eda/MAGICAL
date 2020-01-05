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

PROJECT_NAMESPACE_START

Anaroute::Anaroute(int argc, char** argv) {
 
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
  const String_t outFile          = _args.get<String_t>("out");
  const String_t outGuideFile     = _args.get<String_t>("out_guide");
  //const String_t outGuideGdsFile  = _args.get<String_t>("out_guide_gds");
  const String_t dumbFile         = _args.get<String_t>("fuck");
  const bool     bFlatten         = _args.exist("flatten");

  // parse files
  timer.start(TimeUsage::PARTIAL);
  Parser par(cir);
  par.parseLef(lefFile);
  par.parseTechfile(techFile);
  par.parseGds(placeFile);
  par.parseIspd08(designFile);
  if (symnetFile != "")
    par.parseSymNet(symnetFile);
  if (iopinFile != "")
    par.parseIOPin(iopinFile);
  par.correctPinNBlkLoc(); // patch for placement bugs
  //cir.visualize();

  //cir.printInfo();
  cir.buildSpatial();
  cir.markBlks();
  timer.showUsage("Circuit database construction", TimeUsage::PARTIAL);
  
  // global routing
  timer.start(TimeUsage::PARTIAL);
  GrMgr gr(cir);
  gr.solve();
  cir.buildSpatialNetGuides();
  timer.showUsage("Global Routing", TimeUsage::PARTIAL);

  // track assignment
  TaMgr ta(cir);
  ta.solve();

  // detailed routing
  timer.start(TimeUsage::PARTIAL);
  DrcMgr drc(cir);
  DrMgr dr(cir, drc);
  dr.solve();
  timer.showUsage("Detailed Routing", TimeUsage::PARTIAL);

  // write files
  Writer wr(cir);
  wr.writeLayoutGds(_args.get<String_t>("placement_layout"), _args.get<String_t>("out"), bFlatten);
  if (outGuideFile != "")
    wr.writeGrGuide(_args.get<String_t>("out_guide"));
  //if (outGuideGdsFile != "")
    //wr.writeGrGuideGds("", _args.get<String_t>("out_guide_gds"));
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
  _args.add<String_t>("fuck", '\0', "fuck file", false);
  _args.add<String_t>("out_guide", '\0', "output global routing guide file", false);
  //_args.add<String_t>("out_guide_gds", '\0', "output global routing guide file (gds)", false);
  _args.add("flatten", '\0', "flatten output GDS");

  _args.parse_check(argc, argv);
}


PROJECT_NAMESPACE_END


