/**
 * @file   dbNet.hpp
 * @brief  Circuit Element - Detailed Routing Net
 * @author Hao Chen
 * @date   09/21/2019
 *
 **/

#ifndef _DB_NET_HPP_
#define _DB_NET_HPP_

#include "net/netNode.hpp"
#include "src/geo/box.hpp"

PROJECT_NAMESPACE_START

class Net {
 public:
  Net(const String_t& n = "", const UInt_t idx = MAX_UINT)
    : _name(n), _idx(idx), _symNetIdx(MAX_UINT),
      _bSelfSym(false), _bRouted(false), _bIOPort(false),
      _grFailCnt(0), _drFailCnt(0) {}
  ~Net() {}

  
  //////////////////////////////////
  //  Getter                      //
  //////////////////////////////////
  const String_t&             name()                    const { return _name; }
  UInt_t                      idx()                     const { return _idx; }
  UInt_t                      symNetIdx()               const { return _symNetIdx; }
  UInt_t                      numPins()                 const { return _vPinIndices.size(); }
  UInt_t                      pinIdx(const UInt_t i)    const { return _vPinIndices[i]; }
  const Vector_t<UInt_t>&     vPinIndices()             const { return _vPinIndices; }
  UInt_t                      numNodes()                const { return _vNodes.size(); }
  NetNode&                    node(const UInt_t i)            { return _vNodes[i]; }
  const NetNode&              node(const UInt_t i)      const { return _vNodes[i]; }
  Vector_t<NetNode>&          vNodes()                        { return _vNodes; }
  const Vector_t<NetNode>&    vNodes()                  const { return _vNodes; }
  bool                        hasSymNet()               const { return _symNetIdx != MAX_UINT; }
  bool                        bSelfSym()                const { return _bSelfSym; }
  bool                        bIOPort()                 const { return _bIOPort; }
  
  // global routing
  Int_t                                       grFailCnt()    const { return _grFailCnt; }
  UInt_t                                      numGuides()    const { return _vGuides.size(); }
  Vector_t<Pair_t<Box<Int_t>, Int_t>>&        vGuides()            { return _vGuides; }
  const Vector_t<Pair_t<Box<Int_t>, Int_t>>&  vGuides()      const { return _vGuides; }
  // detailed routing
  Int_t                       drFailCnt()                    const { return _drFailCnt; }
  bool                        bRouted()                      const { return _bRouted; }
  Vector_t<Pair_t<Box<Int_t>, Int_t>>&        vWires()             { return _vWires; }
  const Vector_t<Pair_t<Box<Int_t>, Int_t>>&  vWires()       const { return _vWires; }

  //////////////////////////////////
  //  Setter                      //
  //////////////////////////////////
  void setName(const String_t& n);
  void setSelfSym();
  void setIOPort();
  void setSymNetIdx(const UInt_t i);
  void setRouted(const bool b = true);
  void addPinIdx(const UInt_t i);
  void addNode(const NetNode& n);
  void addGrFail();
  void addDrFail();
  void clearGrFail();
  void clearDrFail();
  void setGuides(const Vector_t<Pair_t<Box<Int_t>, Int_t>>& v);

 private:
  String_t                            _name;
  UInt_t                              _idx;
  UInt_t                              _symNetIdx; // MAX_UINT if no sym net
  bool                                _bSelfSym;
  bool                                _bRouted;
  bool                                _bIOPort;
  Vector_t<UInt_t>                    _vPinIndices;
  Vector_t<NetNode>                   _vNodes;
  Int_t                               _grFailCnt;
  Int_t                               _drFailCnt;
  Vector_t<Pair_t<Box<Int_t>, Int_t>> _vGuides; // from global routing
  Vector_t<Pair_t<Box<Int_t>, Int_t>> _vWires;
};

//////////////////////////////////
//  Iterators                   //
//////////////////////////////////
#define Net_ForEachPinIdx(net, idx, i) \
  for (i = 0, idx = net.pinIdx(i); i < net.numPins(); ++i, idx = net.pinIdx(i))

#define Net_ForEachNetNode(net, pNode_, i) \
  for (i = 0; i < net.numNodes() and (pNode_ = &net.node(i)); ++i)

#define Net_ForEachNetNodeC(net, cpNode_, i) \
  for (i = 0; i < net.numNodes() and (cpNode_ = &net.node(i)); ++i)

PROJECT_NAMESPACE_END

#endif /// _DB_NET_HPP_
