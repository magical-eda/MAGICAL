/**
 * @file   apiPy.cpp
 * @brief  API for python
 * @author Hao Chen
 * @date   12/22/2019
 *
 **/

#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>
#include "src/global/global.hpp"
#include "src/db/dbCir.hpp"
#include "src/parser/parser.hpp"
#include "src/acs/acsMgr.hpp"
#include "src/gr/grMgr.hpp"
#include "src/dr/drMgr.hpp"
#include "src/drc/drcMgr.hpp"
#include "src/post/postMgr.hpp"
#include "src/writer/writer.hpp"
#include "src/util/timeUsage.hpp"

namespace py = pybind11;

namespace apiPy {
  using namespace PROJECT_NAMESPACE;
  class AnaroutePy {
   public:
    AnaroutePy() : _par(_cir), _wr(_cir) {}
    ~AnaroutePy() {}
    /////////////////////////////////////
    // parse
    /////////////////////////////////////
    void parseLef(const String_t& filename) { _par.parseLef(filename); }
    void parseTechfile(const String_t& filename) { _par.parseTechfile(filename); }
    void parseGds(const String_t& filename) { _par.parseGds(filename); }
    void parseIspd08(const String_t& filename) { _par.parseIspd08(filename); }
    void parseSymNet(const String_t& filename) { _par.parseSymNet(filename); }
    void parseIOPin(const String_t& filename) { _par.parseIOPin(filename); }
    void parsePower(const String_t& filename) { _par.parsePower(filename); }
    void setCircuitName(const String_t &cirName) { _cir.setName(cirName); }

    /////////////////////////////////////
    // add data
    /////////////////////////////////////
    void setCirBBox(const Int_t xl, const Int_t yl, const Int_t xh, const Int_t yh) {
      _cir.setXL(xl);
      _cir.setYL(yl);
      _cir.setXH(xh);
      _cir.setYH(yh);
    }
    void setGridStep(const Int_t s) {
      _cir.setGridStep(s);
    }
    void setGridOffsetX(const Int_t x) {
      _cir.setGridOffsetX(x);
    }
    void setGridOffsetY(const Int_t y) {
      _cir.setGridOffsetY(y);
    }
    void setSymAxisX(const Int_t x) {
      _cir.setSymAxisX(x);
    }
    Int_t addNet(const String_t& netName, const Int_t minWidth, const Int_t minCuts, const bool bPG, const Int_t numCutsRow = 0, const Int_t numCutsCol = 0) {
      Net net(netName, _cir.numNets());
      net.setPower(bPG);
      net.setMinWidth(minWidth);
      net.setMinCuts(minCuts);
      net.setNumCutsRow(numCutsRow);
      net.setNumCutsCol(numCutsCol);
      _cir.addNet(net);
      return net.idx();
    }
    Int_t addPin(const String_t& pinName, const bool bPG, const bool bStripe) {
      Pin pin;
      pin.setName(pinName);
      pin.setPower(bPG);
      pin.setStripe(bStripe);
      pin.setIdx(_cir.numPins());
      pin.resizeLayerBoxes(_cir.lef().numLayers());
      _cir.addPin(pin);
      return pin.idx();
    }
    void addPin2Net(const String_t& pinName, const String_t& netName) {
      Net& net = _cir.net(netName);
      Pin& pin = _cir.pin(pinName);
      pin.setNetName(net.name());
      pin.setNetIdx(net.idx());
      net.addPinIdx(pin.idx());
      // update bbox
      UInt_t i, layerIdx;
      const Box<Int_t>* cpBox;
      Pin_ForEachLayerIdx(pin, layerIdx) {
        Pin_ForEachLayerBox(pin, layerIdx, cpBox, i) {
          net.updateBBox(*cpBox);
        }
      }
    }
    void addPin2Net(const UInt_t pinIdx, const UInt_t netIdx) {
      Net& net = _cir.net(netIdx);
      Pin& pin = _cir.pin(pinIdx);
      pin.setNetName(net.name());
      pin.setNetIdx(net.idx());
      net.addPinIdx(pin.idx());
      // update bbox
      UInt_t i, layerIdx;
      const Box<Int_t>* cpBox;
      Pin_ForEachLayerIdx(pin, layerIdx) {
        Pin_ForEachLayerBox(pin, layerIdx, cpBox, i) {
          net.updateBBox(*cpBox);
        }
      }
    }
    /// note: layerIdx should be metal layer, ex. 1 for M1, 2 for M2 ... 
    void addShape2Pin(const String_t& pinName, const UInt_t layerIdx, const Int_t xl, const Int_t yl, const Int_t xh, const Int_t yh) {
      Pin& pin = _cir.pin(pinName);
      Box<Int_t> box(xl, yl, xh, yh);
      pin.addBox(_cir.lef().routingLayerIdx2LayerIdx(layerIdx), box);
    }
    void addShape2Pin(const UInt_t pinIdx, const UInt_t layerIdx, const Int_t xl, const Int_t yl, const Int_t xh, const Int_t yh) {
      Pin& pin = _cir.pin(pinIdx);
      Box<Int_t> box(xl, yl, xh, yh);
      pin.addBox(_cir.lef().routingLayerIdx2LayerIdx(layerIdx), box);
    }
    /// note: layer should be metal layer, ex. 1 for M1
    void addBlk(const UInt_t blkIdx, const UInt_t layerIdx, const Int_t xl, const Int_t yl, const Int_t xh, const Int_t yh) {
      const UInt_t layer = _cir.lef().routingLayerIdx2LayerIdx(layerIdx);
      Blk blk(blkIdx, layer, Box<Int_t>(xl, yl, xh, yh));
      _cir.addBlk(layer, blk);
    }
    void addSymNet(const String_t& s1, const String_t& s2) {
      Net& n1 = _cir.net(s1);
      Net& n2 = _cir.net(s2);
      n1.setSymNetIdx(n2.idx());
      n2.setSymNetIdx(n1.idx());
    }
    void addSymNet(const UInt_t i1, const UInt_t i2) {
      Net& n1 = _cir.net(i1);
      Net& n2 = _cir.net(i2);
      n1.setSymNetIdx(n2.idx());
      n2.setSymNetIdx(n1.idx());
    }
    void addSelfSymNet(const String_t& n) {
      Net& net = _cir.net(n);
      net.setSelfSym();
    }
    void addSelfSymNet(const UInt_t i) {
      Net& net = _cir.net(i);
      net.setSelfSym();
    }
    void addIOPort(const String_t& netName) {
      _cir.net(netName).setIOPort();
    }
    
    /////////////////////////////////////
    // solve
    /////////////////////////////////////
    bool solve(const bool bUseSymFile = false) {
      TimeUsage timer;
      timer.start(TimeUsage::FULL);
      _cir.resizeVVPinIndices(_cir.lef().numLayers());
      _cir.resizeVVBlkIndices(_cir.lef().numLayers());
      _cir.buildSpatial();
      _cir.markBlks();
      _cir.checkNetSymSelfSym();
      //GrMgr gr(_cir);
      //gr.solve();
      _cir.buildSpatialNetGuides();
      
      AcsMgr acs(_cir);
      acs.computeAcs();

      _cir.visualize();
      
      DrcMgr drc(_cir);
      DrMgr dr(_cir, drc);
      if (!dr.solve(true, bUseSymFile))
        return false;

      PostMgr post(_cir);
      post.solve();
      
      _cir.computeNSetAllNetStatistics();
      timer.showUsage("Anaroute", TimeUsage::FULL);
      
      return true;
    }

    void init() {
      std::srand(1234);
      _cir.resizeVVPinIndices(_cir.lef().numLayers());
      _cir.resizeVVBlkIndices(_cir.lef().numLayers());
      _par.correctPinNBlkLoc();
      _cir.buildSpatial();
      _cir.markBlks();
      _cir.checkNetSymSelfSym();
      AcsMgr acs(_cir);
      acs.computeAcs();
    }

    void solveGR() {
      GrMgr gr(_cir);
      gr.solve();
    }

    bool solveDR(const bool bUseSymFile) {
      _cir.buildSpatialNetGuides();
      DrcMgr drc(_cir);
      DrMgr dr(_cir, drc);
      return dr.solve(true, bUseSymFile);
    }

    void postProcess() {
      PostMgr post(_cir);
      post.solve();
    }

    // evaluate
    void showEvaluation() {
      _cir.computeTotalStatistics();
    }
    Int_t computeTotalWireLength() {
      return _cir.computeTotalWireLength();
    }
    Int_t computeTotalSymWireLength() {
      return _cir.computeTotalSymWireLength();
    }
    Int_t computeTotalViaCnt() {
      return _cir.computeTotalViaCnt();
    }
    Float_t computeTotalWireArea() {
      return _cir.computeTotalWireArea();
    }
    Float_t computeTotalDegSym() {
      return _cir.computeTotalDegSym();
    }
    Float_t computeAvgDegSym() {
      return _cir.computeAvgDegSym();
    }


    // write
    void writeLayoutGds(const String_t& placefilename, const String_t& outfilename, const bool bFlatten) {
      _wr.writeLayoutGds(placefilename, outfilename, bFlatten);
    }
    void writeDumb(const String_t& placefilename, const String_t& outfilename) {
      _wr.writeDumb(placefilename, outfilename);
    }

   private:
    CirDB  _cir;
    Parser _par;
    Writer _wr;
  };
}

void initPyAPI(py::module& m) {
  namespace pro = PROJECT_NAMESPACE;
  py::class_<apiPy::AnaroutePy>(m, "AnaroutePy")
    .def(py::init<>())
    .def("parseLef", &apiPy::AnaroutePy::parseLef)
    .def("parseTechfile", &apiPy::AnaroutePy::parseTechfile)
    .def("parseGds", &apiPy::AnaroutePy::parseGds)
    .def("parseIspd08", &apiPy::AnaroutePy::parseIspd08)
    .def("parseSymNet", &apiPy::AnaroutePy::parseSymNet)
    .def("parsePower", &apiPy::AnaroutePy::parsePower)
    .def("parseIOPin", &apiPy::AnaroutePy::parseIOPin)
    .def("setCirBBox", &apiPy::AnaroutePy::setCirBBox)
    .def("setGridStep", &apiPy::AnaroutePy::setGridStep)
    .def("setGridOffsetX", &apiPy::AnaroutePy::setGridOffsetX)
    .def("setGridOffsetY", &apiPy::AnaroutePy::setGridOffsetY)
    .def("setSymAxisX", &apiPy::AnaroutePy::setSymAxisX)
    .def("addNet", py::overload_cast<const pro::String_t&, const pro::Int_t, const pro::Int_t, const bool, const pro::Int_t, const pro::Int_t>(&apiPy::AnaroutePy::addNet))
    .def("addPin", py::overload_cast<const pro::String_t&, const bool, const bool>(&apiPy::AnaroutePy::addPin))
    .def("addPin2Net", py::overload_cast<const pro::String_t&, const pro::String_t&>(&apiPy::AnaroutePy::addPin2Net))
    .def("addPin2Net", py::overload_cast<const pro::UInt_t, const pro::UInt_t>(&apiPy::AnaroutePy::addPin2Net))
    .def("addShape2Pin", py::overload_cast<const pro::String_t&, const pro::UInt_t, const pro::Int_t, const pro::Int_t, const pro::Int_t, const pro::Int_t>(&apiPy::AnaroutePy::addShape2Pin))
    .def("addShape2Pin", py::overload_cast<const pro::UInt_t, const pro::UInt_t, const pro::Int_t, const pro::Int_t, const pro::Int_t, const pro::Int_t>(&apiPy::AnaroutePy::addShape2Pin))
    .def("addBlk", &apiPy::AnaroutePy::addBlk)
    .def("addSymNet", py::overload_cast<const pro::String_t&, const pro::String_t&>(&apiPy::AnaroutePy::addSymNet))
    .def("addSymNet", py::overload_cast<const pro::UInt_t, const pro::UInt_t>(&apiPy::AnaroutePy::addSymNet))
    .def("addSelfSymNet", py::overload_cast<const pro::String_t&>(&apiPy::AnaroutePy::addSelfSymNet))
    .def("addSelfSymNet", py::overload_cast<const pro::UInt_t>(&apiPy::AnaroutePy::addSelfSymNet))
    .def("addIOPort", &apiPy::AnaroutePy::addIOPort)
    .def("solve", &apiPy::AnaroutePy::solve)
    .def("init", &apiPy::AnaroutePy::init)
    .def("solveGR", &apiPy::AnaroutePy::solveGR)
    .def("solveDR", &apiPy::AnaroutePy::solveDR)
    .def("postProcess", &apiPy::AnaroutePy::postProcess)
    .def("evaluate", &apiPy::AnaroutePy::showEvaluation)
    .def("writeLayoutGds", &apiPy::AnaroutePy::writeLayoutGds)
    .def("writeDumb", &apiPy::AnaroutePy::writeDumb)
    .def("setCircuitName", &apiPy::AnaroutePy::setCircuitName)
    .def("computeTotalWireLength", &apiPy::AnaroutePy::computeTotalWireLength)
    .def("computeTotalSymWireLength", &apiPy::AnaroutePy::computeTotalSymWireLength)
    .def("computeTotalViaCnt", &apiPy::AnaroutePy::computeTotalViaCnt)
    .def("computeTotalWireArea", &apiPy::AnaroutePy::computeTotalWireArea)
    .def("computeTotalDegSym", &apiPy::AnaroutePy::computeTotalDegSym)
    .def("computeAvgDegSym", &apiPy::AnaroutePy::computeAvgDegSym);

}

PYBIND11_MAKE_OPAQUE(std::vector<PROJECT_NAMESPACE::UInt_t>);

PYBIND11_MODULE(anaroutePy, m) {
  initPyAPI(m);
}
