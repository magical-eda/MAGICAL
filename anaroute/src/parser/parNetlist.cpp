/**
 * @file   parNetlist.hpp
 * @brief  Netlist parser
 * @author Hao Chen
 * @date   02/03/2020
 *
 **/

#include <fstream>

#include "parNetlist.hpp"
#include "src/geo/box.hpp"

PROJECT_NAMESPACE_START

void NetlistReader::parse(const String_t& filename) {
  assert(_cir.lef().version() > 0);
  std::ifstream ifs(filename.c_str(), std::ifstream::in);
  if (!ifs.is_open()) {
    fprintf(stderr, "NetlistReader::%s ERROR: Cannot open file `%s`\n", __func__, filename.c_str());
    exit(0);
  }

  setScale();

  _cir.resizeVVPinIndices(_cir.lef().numLayers());
  
  String_t buf;
  String_t netName;
  Int_t routingLayerIdx;
  Int_t xl, yl, xh, yh;
  Int_t gridStep, symAxisX, xGrid, yGrid;
  Int_t gridOffsetX, gridOffsetY;
  Int_t netIdx;

  ifs >> buf; ifs >> gridStep;
  ifs >> buf; ifs >> gridOffsetX >> gridOffsetY;
  ifs >> buf; ifs >> symAxisX;

  gridStep = to_db_unit(gridStep);
  gridOffsetX = to_db_unit(gridOffsetX);
  gridOffsetY = to_db_unit(gridOffsetY);
  symAxisX = to_db_unit(symAxisX);
  _cir.setGridStep(gridStep);
  _cir.setGridOffsetX(gridOffsetX);
  _cir.setGridOffsetY(gridOffsetY);
  _cir.setSymAxisX(symAxisX);
  

  while (ifs >> netName >> routingLayerIdx
             >> xl >> yl >> xh >> yh 
             >> xGrid >> yGrid) {
    assert(xl < xh and yl < yh);
    //if (xl >= xh or yl >= yh) {
      //std::cerr << netName << " " << xl << " " << yl << " " << xh << " " << yh << std::endl;
      //exit(0);
    //}
    assert(routingLayerIdx > 0);
    const Int_t layerIdx = _cir.lef().routingLayerIdx2LayerIdx(routingLayerIdx - 1);
    xl = to_db_unit(xl);
    yl = to_db_unit(yl);
    xh = to_db_unit(xh);
    yh = to_db_unit(yh);
    
    Pin pin;
    pin.setIdx(_cir.numPins());
    
    pin.resizeLayerBoxes(_cir.lef().numLayers());
    pin.addBox(layerIdx, Box<Int_t>(xl, yl, xh, yh));
    
    pin.setNetName(netName);
    if (_cir.hasNet(netName)) {
      netIdx = _cir.str2NetIdx(netName);
      Net& net = _cir.net(netIdx);
      net.addPinIdx(pin.idx());
      updateNetBBox(net, pin);
      pin.setNetIdx(netIdx);
    }
    else {
      netIdx = _cir.numNets();
      Net net(netName, netIdx);
      net.addPinIdx(pin.idx());
      updateNetBBox(net, pin);
      pin.setNetIdx(netIdx);
      net.setMinWidth(to_db_unit(100));
      net.setMinCuts(1);
      _cir.addNet(net);
    }
    _cir.addPin(pin);

  }

  ifs.close();
}

void NetlistReader::parse2(const String_t& filename) {
  assert(_cir.lef().version() > 0);
  std::ifstream ifs(filename.c_str(), std::ifstream::in);
  if (!ifs.is_open()) {
    fprintf(stderr, "NetlistReader::%s ERROR: Cannot open file `%s`\n", __func__, filename.c_str());
    exit(0);
  }

  setScale();

  _cir.resizeVVPinIndices(_cir.lef().numLayers());

  String_t buf;
  String_t netName, pinName;
  Int_t routingLayerIdx;
  Int_t gridStep, gridOffsetX, gridOffsetY, symAxisX;
  Int_t xl, yl, xh, yh;
  bool bPower, bStripe;

  ifs >> buf; ifs >> gridStep;
  ifs >> buf; ifs >> gridOffsetX >> gridOffsetY;
  ifs >> buf; ifs >> symAxisX;

  gridStep = to_db_unit(gridStep);
  gridOffsetX = to_db_unit(gridOffsetX);
  gridOffsetY = to_db_unit(gridOffsetY);
  symAxisX = to_db_unit(symAxisX);
  _cir.setGridStep(gridStep);
  _cir.setGridOffsetX(gridOffsetX);
  _cir.setGridOffsetY(gridOffsetY);
  _cir.setSymAxisX(symAxisX);

  while (ifs >> netName >> pinName
             >> routingLayerIdx >> xl >> yl >> xh >> yh
             >> bPower >> bStripe) {
    assert(routingLayerIdx > 0);
    const Int_t layerIdx = _cir.lef().routingLayerIdx2LayerIdx(routingLayerIdx - 1);
    xl = to_db_unit(xl);
    yl = to_db_unit(yl);
    xh = to_db_unit(xh);
    yh = to_db_unit(yh);

    const bool hasNet = _cir.hasNet(netName);
    const bool hasPin = _cir.hasPin(pinName);
    
    if (hasNet) {
      Net& net = _cir.net(netName);
      if (hasPin) {
        Pin& pin = _cir.pin(pinName);
        assert(pin.netIdx() == net.idx());
        assert(pin.bPower() == bPower);
        assert(pin.bStripe() == bStripe);
        net.addPinIdx(pin.idx());
        pin.addBox(layerIdx, Box<Int_t>(xl, yl, xh, yh));
        updateNetBBox(net, pin);
      }
      else {
        Pin pin(pinName, netName, net.idx(), _cir.numPins());
        pin.setPower(bPower);
        pin.setStripe(bStripe);
        pin.resizeLayerBoxes(_cir.lef().numLayers());
        pin.addBox(layerIdx, Box<Int_t>(xl, yl, xh, yh));
        net.addPinIdx(pin.idx());
        updateNetBBox(net, pin);
        _cir.addPin(pin);
      }
    }
    else {
      Net net(netName, _cir.numNets());
      net.setPower(bPower);
      if (bPower) {
        net.setMinWidth(to_db_unit(500));
        net.setMinCuts(8);
        //net.setMinWidth(to_db_unit(100));
        //net.setMinCuts(1);
      }
      else {
        net.setMinWidth(to_db_unit(100));
        net.setMinCuts(1);
      }
      if (hasPin) {
        Pin& pin = _cir.pin(pinName);
        assert(pin.netIdx() == net.idx());
        assert(pin.bPower() == bPower);
        assert(pin.bStripe() == bStripe);
        pin.addBox(layerIdx, Box<Int_t>(xl, yl, xh, yh));
        net.addPinIdx(pin.idx());
        updateNetBBox(net, pin);
      }
      else {
        Pin pin(pinName, netName, net.idx(), _cir.numPins());
        pin.setPower(bPower);
        pin.setStripe(bStripe);
        pin.resizeLayerBoxes(_cir.lef().numLayers());
        pin.addBox(layerIdx, Box<Int_t>(xl, yl, xh, yh));
        net.addPinIdx(pin.idx());
        updateNetBBox(net, pin);
        _cir.addPin(pin);
      }
      _cir.addNet(net);
    }
  }

  ifs.close();
}

void NetlistReader::setScale() {
  assert(_cir.lef().units().databaseNumber() >= 1000 and _cir.lef().units().databaseNumber() % 1000 == 0);
  _scale = _cir.lef().units().databaseNumber() / 1000;
}

Int_t NetlistReader::to_db_unit(const Int_t n) const {
  return n * _scale;
}

void NetlistReader::updateNetBBox(Net& net, const Pin& pin) {
  UInt_t i, layerIdx;
  const Box<Int_t>* cpBox;
  Pin_ForEachLayerIdx(pin, layerIdx) {
    Pin_ForEachLayerBox(pin, layerIdx, cpBox, i) {
      net.updateBBox(*cpBox);
    }
  }
}

PROJECT_NAMESPACE_END
