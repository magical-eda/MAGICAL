/*! @file db/Netlist.cpp
    @brief Netlist class implementation.
    @author Mingjie Liu
    @date 11/24/2018
*/
#include "db/Netlist.h"
#include <algorithm>
#include <iostream>

PROJECT_NAMESPACE_BEGIN

/*! @brief Mos Pin Types */
static const PinType MOS_PIN_TYPE[4] = {PinType::DRAIN, PinType::GATE, PinType::SOURCE, PinType::BULK};
/*! @brief Res/Cap Pin Types */
static const PinType RES_PIN_TYPE[3] = {PinType::THIS, PinType::THAT, PinType::OTHER};

bool Netlist::isMos(InstType instType) const
{
    return instType == InstType::NMOS || instType == InstType::PMOS;  
}

bool Netlist::isPasvDev(InstType instType) const
{
    return instType == InstType::CAP || instType == InstType::RES;  
}

IndexType Netlist::addNet(const std::string name, IndexType netIdx)
{
    _netArray.emplace_back(name, netIdx);
    return _netArray.size()-1;
}

IndexType Netlist::addInst(const std::string name, InstType type, RealType wid, RealType len, RealType nf)
{
    _instArray.emplace_back(name, type, _instArray.size(), wid, len, nf);
    return _instArray.size()-1;
}

void Netlist::addInstPin(IndexType instIdx, IndexType netIdx, PinType pinType)
{
    _instArray.at(instIdx).addPinId(_pinArray.size());
    _netArray.at(netIdx).addPinId(_pinArray.size());
    _pinArray.emplace_back(_pinArray.size(), instIdx, netIdx, pinType);
}

void Netlist::init(InitDataObj & obj)
{
// Add all Net to Netlist 
    for (InitNet & net : obj.netArray)
        _netArray.emplace_back(net.name, net.id); 
// Add all Inst to Netlist 
    for (InitInst & inInst : obj.instArray)
    {
// If added Inst is Mosfet 
        if (isMos(inInst.type))
        {
            Inst inst(inInst.name, inInst.type, _instArray.size(), inInst.wid, inInst.len, inInst.nf);
            for (IndexType i = 0; i < 4; i++)
            {
                inst.addPinId(_pinArray.size()); // Add pin to Inst 
                _netArray[inInst.netIdArray.at(i)].addPinId(_pinArray.size()); // Add pin to Net 
                _pinArray.emplace_back(_pinArray.size(), inst.id(),
                                        inInst.netIdArray.at(i), MOS_PIN_TYPE[i]); // Add Pin    
            }
            _instArray.push_back(inst);
        }
// If added Inst is Res/Cap 
        else if (isPasvDev(inInst.type))
        {
            Inst inst(inInst.name, inInst.type, _instArray.size(), inInst.wid, inInst.len, inInst.nf);
            for (IndexType i = 0; i < inInst.netIdArray.size(); i++) //Modified for 2/3 pin passives
            {
                inst.addPinId(_pinArray.size());
                _netArray[inInst.netIdArray.at(i)].addPinId(_pinArray.size());
                _pinArray.emplace_back(_pinArray.size(), inst.id(), 
                                        inInst.netIdArray.at(i), RES_PIN_TYPE[i]);
            }
            _instArray.push_back(inst);
        }
// Handle Std Cells in future 
        else
        {
            Inst inst(inInst.name, inInst.type, _instArray.size());
            for (IndexType netId : inInst.netIdArray)
            {
                inst.addPinId(_pinArray.size());
                _netArray[netId].addPinId(_pinArray.size());
                _pinArray.emplace_back(_pinArray.size(), inst.id(), 
                                        netId, PinType::OTHER);                
            }    
            _instArray.push_back(inst);
        } 
    }
}

void Netlist::print_all() const
{
// Print Net 
    for (const Net & net : _netArray)
    {
        std::printf("Net %d, %s \n", net.id(), net.name().c_str());
    }
// Print Inst 
    for (const Inst & inst : _instArray)
    {
        std::printf("Instance %d, %s \n", inst.id(), inst.name().c_str()); 
        for (IndexType pinId : inst.pinIdArray())
            std::printf("Pin %d, from  Instance %s to net %d:%s \n",
                pinId, _instArray[_pinArray[pinId].instId()].name().c_str(),
                _pinArray[pinId].netId(), _netArray[_pinArray[pinId].netId()].name().c_str()); 
    }   
}

void Netlist::getInstNetConn(std::vector<IndexType> & instArray, IndexType netId) const
{
    instArray.clear(); // Reset 
    for(IndexType tempPinId : _netArray[netId].pinIdArray())
    {
        IndexType instId = _pinArray[tempPinId].instId();
        if(std::find(instArray.begin(), instArray.end(), instId) == instArray.end())
        {
            instArray.push_back(instId); // Only add if unique 
        }
    }
}

void Netlist::rmvInstHasPin(std::vector<IndexType> & instArray, IndexType pinId) const
{
// O(n) complexity guaranteed.
    auto it = instArray.begin();
    IndexType numRmv = 0;
    while (it != instArray.end() - numRmv)
    {
        IndexType instId = *it;
        if (_pinArray[pinId].instId() == instId)
        {      
            std::iter_swap(it, instArray.end() - 1 - numRmv);
            numRmv++;
        }
        else
            ++it;
    }
    instArray.erase(instArray.end() - numRmv, instArray.end());
// Below implementation using std::vector.erase(). O(n^2) complexity.
//    auto it = instArray.begin();
//    while (it != instArray.end())
//    {
//        IndexType instId = *it;
//        if (_pinArray[pinId].instId() == instId)
//            it = instArray.erase(it);
//        else
//            ++it;
//    }
}

void Netlist::getInstPinConn(std::vector<IndexType> & instArray, IndexType pinId) const
{
    IndexType netId = _pinArray[pinId].netId();
    getInstNetConn(instArray, netId);
    rmvInstHasPin(instArray, pinId);
}

void Netlist::fltrInstNetConnPinType(std::vector<IndexType> & instArray,
                IndexType netId, PinType connPinType) const
{
// O(n) complexity guaranteed.
    auto it = instArray.begin();
    IndexType numRmv = 0;
    while (it != instArray.end() - numRmv)
    {
        IndexType instId = *it;
        if (instNetId(instId, connPinType) != netId)
        { 
            std::iter_swap(it, instArray.end() - 1 - numRmv);
            numRmv++;
        }
        else
            ++it;
    }
    instArray.erase(instArray.end() - numRmv, instArray.end());
// Below implementation using std::vector.erase(). O(n^2) complexity.
//    auto it = instArray.begin();
//    while (it != instArray.end())
//    {
//        IndexType instId = *it;
//        if (instNetId(instId, connPinType) != netId)
//            it = instArray.erase(it);
//        else
//            ++it;
//    }
}

void Netlist::fltrInstPinConnPinType(std::vector<IndexType> & instArray, 
                IndexType pinId, PinType connPinType) const
{
    fltrInstNetConnPinType(instArray, _pinArray[pinId].netId(), connPinType);
}

void Netlist::fltrInstMosType(std::vector<IndexType> & instArray, MosType mosType) const
{
// O(n) complexity guaranteed.
    auto it = instArray.begin();
    IndexType numRmv = 0;
    while (it != instArray.end() - numRmv)
    {
        IndexType instId = *it;
        if (Netlist::mosType(instId) != mosType)
        {         
            std::iter_swap(it, instArray.end() - 1 - numRmv);
            numRmv++;
        }
        else
            ++it;
    }
    instArray.erase(instArray.end() - numRmv, instArray.end());
// Below implementation using std::vector.erase(). O(n^2) complexity.
//    auto it = instArray.begin();
//    while (it != instArray.end())
//    {
//        IndexType instId = *it; 
//        if (Netlist::mosType(instId) != mosType)
//            it = instArray.erase(it);
//        else
//            ++it;
//    }
}

void Netlist::fltrInstType(std::vector<IndexType> & instArray, InstType type) const
{
// O(n) complexity guaranteed.
    auto it = instArray.begin();
    IndexType numRmv = 0;
    while (it != instArray.end() - numRmv)
    {
        IndexType instId = *it;
        if (_instArray[instId].type() != type)
        {         
            std::iter_swap(it, instArray.end() - 1 - numRmv);
            numRmv++;
        }
        else
            ++it;
    }
    instArray.erase(instArray.end() - numRmv, instArray.end());
}

PinType Netlist::getPinTypeInstNetConn(IndexType instId, IndexType netId) const
{
    for (IndexType instPinId : _instArray[instId].pinIdArray())
        if (_pinArray[instPinId].netId() == netId)
            return _pinArray[instPinId].type();
    return PinType::OTHER; // Caution 
}

PinType Netlist::getPinTypeInstPinConn(IndexType instId, IndexType pinId) const
{
    return getPinTypeInstNetConn(instId, _pinArray[pinId].netId());
}

IndexType Netlist::instNetId(IndexType instId, PinType type) const
{
    for (IndexType pinId : _instArray[instId].pinIdArray())
        if (_pinArray[pinId].type() == type)
            return _pinArray[pinId].netId();
    return INDEX_TYPE_MAX; // Caution 
}    

MosType Netlist::mosType(IndexType mosId) const
{
// Note the dominance order. 
    if (instNetId(mosId, PinType::SOURCE) == instNetId(mosId, PinType::DRAIN))
        return MosType::CAP;
    else if (instNetId(mosId, PinType::GATE) == instNetId(mosId, PinType::DRAIN))
        return MosType::DIODE;
    else if (instNetId(mosId, PinType::GATE) == instNetId(mosId, PinType::SOURCE))
        return MosType::DUMMY;
    return MosType::DIFF;
}

IndexType Netlist::instPinId(IndexType instId, PinType pinType) const
{
    for (IndexType pinId : _instArray[instId].pinIdArray())
        if (_pinArray[pinId].type() == pinType)
            return pinId;
    return INDEX_TYPE_MAX; // Caution 
}   


PROJECT_NAMESPACE_END


