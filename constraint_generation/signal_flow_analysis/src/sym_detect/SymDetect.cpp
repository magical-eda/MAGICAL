/*! @file sym_detect/SymDetect.cpp
    @brief Detect symmetric patterns
    @author Mingjie Liu
    @date 11/24/2018
*/
#include "sym_detect/SymDetect.h"
#include <iostream>
#include <fstream>
#include <algorithm>

PROJECT_NAMESPACE_BEGIN

void SymDetect::dumpSym(const std::string file) const
{
    std::ofstream outFile(file);
    std::cout << "Dumping symmetry constraints..." << std::endl;
    for (const std::vector<MosPair> & diffPair : _symGroup) //print hiSym Groups
    {
        if (!diffPair[0].valid())
            continue;
        for (const MosPair & pair : diffPair)
        {
            if (pair.mosId1() != pair.mosId2())
                outFile << _netlist.inst(pair.mosId1()).name() << " " 
                    << _netlist.inst(pair.mosId2()).name() << std::endl;
            else
                outFile << _netlist.inst(pair.mosId1()).name() << std::endl; 
        }
        outFile << std::endl;
    }
    std::cout << "Done..." << std::endl;
    outFile.close();
}

void SymDetect::dumpNet(const std::string file) const
{
    std::ofstream outFile(file);
    std::cout << "Dumping symmetry nets..." << std::endl;
    for (const NetPair & pair : _symNet)
    {
        if (pair.netId1() != pair.netId2())
            outFile << _netlist.net(pair.netId1()).name() << " "
                << _netlist.net(pair.netId2()).name() << std::endl;
    }
    for (const NetPair & pair : _symNet)
    {
        if (pair.netId1() == pair.netId2())
            outFile << _netlist.net(pair.netId1()).name() << std::endl;
    }
    std::cout << "Done..." << std::endl;
    outFile.close();
}

void SymDetect::print() const
{
    for (const std::vector<MosPair> & diffPair : _symGroup) //print hiSym Groups
    {
        if (!diffPair[0].valid())
            continue;
        std::cout << "BEGIN GROUP" << std::endl;
        for (const MosPair & pair : diffPair)
        {
            if (pair.mosId1() != pair.mosId2()) 
                std::cout << _netlist.inst(pair.mosId1()).name() << " " 
                    << _netlist.inst(pair.mosId2()).name() << std::endl;
            else
                std::cout << _netlist.inst(pair.mosId1()).name() << std::endl; 
        }
        std::cout << "END GROUP" << std::endl;
    }
    std::cout << "BEGIN NET" << std::endl;
    for (const NetPair & pair : _symNet)
    {
        if (pair.netId1() != pair.netId2())
            std::cout << _netlist.net(pair.netId1()).name() << " "
                << _netlist.net(pair.netId2()).name() << std::endl;
        else
            std::cout << _netlist.net(pair.netId1()).name() << std::endl;
    }
    std::cout << "END NET" << std::endl;
    for (const Bias & bias : _biasGroup) //print hiSym Groups
    {
        if (!bias.valid())
            continue;
        std::cout << "BEGIN BIAS" << std::endl;
        for (IndexType id : bias.bias())
        {
            std::cout << _netlist.inst(id).name() << " "; 
        }
        std::cout << std::endl << "END BIAS" << std::endl;
    }
} 

void SymDetect::getPatrnNetConn(std::vector<MosPair> & diffPair, IndexType netId,
                                    MosPattern srchPatrn) const
{
    std::vector<IndexType> srcMos;
    _netlist.getInstNetConn(srcMos, netId);  
    _netlist.fltrInstNetConnPinType(srcMos, netId, PinType::SOURCE); //filter connect by source
    _netlist.fltrInstMosType(srcMos, MosType::DIFF); //filter by MosType::DIFF
    for (IndexType i = 0; i < srcMos.size(); i++)
    {
        for (IndexType j = i + 1; j < srcMos.size(); j++)
        {
            MosPattern pattern = _pattern.pattern(srcMos[i], srcMos[j]);
            if (pattern == srchPatrn)
            {
                diffPair.emplace_back(srcMos[i], srcMos[j], pattern); //add only if matched MosPattern
            }
        }
    }
}

void SymDetect::getDiffPair(std::vector<MosPair> & diffPair) const
{
    diffPair.clear();
    for (IndexType netId = 0; netId < _netlist.numNet(); netId++)
        if (_netlist.isSignal(netId)) //Iterate for all signal nets for DIFF_SOURCE
        {
            getPatrnNetConn(diffPair, netId, MosPattern::DIFF_SOURCE); 
            getPatrnNetConn(diffPair, netId, MosPattern::CROSS_LOAD);
        }
    for (IndexType netId = 0; netId < _netlist.numNet(); netId++)
        if (!_netlist.isSignal(netId)) //Iterate for non signal nets for CROSS_LOAD
            getPatrnNetConn(diffPair, netId, MosPattern::CROSS_LOAD); 
}

// FIXME: Currently because placement issues, existPair is broken and only output 1 pair for each instId
bool SymDetect::existPair(const std::vector<MosPair> & library, IndexType instId1, IndexType instId2) const
{
    return existPair(library, instId1) && existPair(library, instId2);
    for (const MosPair & currPair : library)
    {
        if (currPair.mosId1() == instId1 && currPair.mosId2() == instId2)
            return true;
        if (currPair.mosId2() == instId1 && currPair.mosId1() == instId2)
            return true;
    }
    return false;
}

bool SymDetect::existNetPair(std::vector<NetPair> & library, IndexType netId1, IndexType netId2) const
{
    for (const NetPair & currPair : library)
    {
        if (currPair.netId1() == netId1 && currPair.netId2() == netId2)
            return true;
        if (currPair.netId2() == netId1 && currPair.netId1() == netId2)
            return true;
    }
    return false;
}

bool SymDetect::existNetPair(std::vector<NetPair> & library, IndexType netId) const
{
    for (const NetPair & currPair : library)
    {
        if (currPair.netId1() == netId)
            return true;
        if (currPair.netId2() == netId)
            return true;
    }
    return false;
}

bool SymDetect::existPair(const std::vector<MosPair> & library, IndexType instId) const
{
    for (const MosPair & currPair : library)
    {
        if (currPair.mosId1() == instId)
            return true;
        if (currPair.mosId2() == instId)
            return true;
    }
    return false;
}

bool SymDetect::endSrch(MosPair & currObj) const
{
    if (currObj.pattern() == MosPattern::PASSIVE)
        return _netlist.instNetId(currObj.mosId1(), currObj.nextPinType1()) == 
               _netlist.instNetId(currObj.mosId2(), currObj.nextPinType2()); // If reached connected passive device.
    if (currObj.pattern() == MosPattern::DIFF_SOURCE &&
        currObj.srchPinType1() == PinType::DRAIN)
        return true; // If reached new DIFF_SOURCE through drain. 
    if (currObj.pattern() == MosPattern::DIFF_SOURCE &&
        currObj.srchPinType1() == PinType::GATE &&
        currObj.srchPinType2() == PinType::GATE)
        return false; // If reached new DIFF_SOURCE through gate. Comment to exclude gate connections. 
    if (currObj.pattern() == MosPattern::CROSS_LOAD &&
        currObj.srchPinType1() == PinType::DRAIN)
        return true; // If reached new CROSS_LOAD through drain. 
   if (currObj.pattern() == MosPattern::LOAD)
        return true; // If LOAD pattern.
    if (currObj.srchPinType1() != PinType::SOURCE && 
        currObj.srchPinType1() != PinType::DRAIN)
        return true; // If reached by gate.
    return false;
}

bool SymDetect::validSrchObj(IndexType instId1, IndexType instId2,
                             IndexType srchPinId1, IndexType srchPinId2) const
{
    MosPattern currPtrn = _pattern.pattern(instId1, instId2);
    if (Pin::isPasvDev(_netlist.pin(srchPinId1).type()) &&
        Pin::isPasvDev(_netlist.pin(srchPinId2).type()))
        return currPtrn == MosPattern::PASSIVE; // Pass valid for all passive pairs.
    if (currPtrn == MosPattern::PASSIVE)
        return true;
// This needs update and more considerations.
    if (_netlist.getPinTypeInstPinConn(instId1, srchPinId1) != 
        _netlist.getPinTypeInstPinConn(instId2, srchPinId2))
        return false; //false if reached through different PinType
    if (_netlist.getPinTypeInstPinConn(instId1, srchPinId1) == PinType::GATE)
        return false; //block gate connected patterns
    if (_netlist.getPinTypeInstPinConn(instId2, srchPinId2) == PinType::GATE)
        return false;
//  Below is removed, DIFF_SOURCE removal updated.
//    if (currPtrn == MosPattern::DIFF_SOURCE ||
//        currPtrn == MosPattern::DIFF_CASCODE)
//        return false;
    if (currPtrn != MosPattern::INVALID)
        return true;
    return false;
}  

bool SymDetect::validDiffPair(IndexType instId1, IndexType instId2,
                            IndexType srchPinId1, IndexType srchPinId2) const
{
    if (_netlist.getPinTypeInstPinConn(instId1, srchPinId1) != PinType::GATE ||
        _netlist.getPinTypeInstPinConn(instId2, srchPinId2) != PinType::GATE)
        return false; // Should be reached through GATE.
    if (_pattern.pattern(instId1, instId2) == MosPattern::DIFF_SOURCE)
        return true; // Should be DIFF_SOURCE pair.
    return false;
}

void SymDetect::inVldDiffPairSrch(std::vector<MosPair> & diffPairSrch, MosPair & currPair) const 
{
    for (MosPair & pair : diffPairSrch)
        if (pair.isEqual(currPair))
            pair.inVld();
}

void SymDetect::pushNextSrchObj(std::vector<MosPair> & dfsVstPair, std::vector<MosPair> & dfsStack,
                                MosPair & currObj, std::vector<MosPair> & diffPairSrc) const
{
    if (endSrch(currObj))
        return; //return if endSrch
    IndexType srchPinId1 = _netlist.instPinId(currObj.mosId1(), currObj.nextPinType1());
    IndexType srchPinId2 = _netlist.instPinId(currObj.mosId2(), currObj.nextPinType2());
    std::vector<IndexType> Mos1, Mos2;
    _netlist.getInstPinConn(Mos1, srchPinId1); //Connected Inst through nextPinType
    _netlist.getInstPinConn(Mos2, srchPinId2);
    for (IndexType instId1: Mos1)
    {
        for (IndexType instId2 : Mos2)
        {
            if (validSrchObj(instId1, instId2, srchPinId1, srchPinId2) && // valid pattern
                !existPair(dfsVstPair, instId1, instId2) && //not visited before
                !existPair(dfsStack, instId1, instId2)) 
            {
                MosPair currPair(instId1, instId2, _pattern.pattern(instId1, instId2));
                currPair.setSrchPinType1(_netlist.getPinTypeInstPinConn(instId1, srchPinId1));
                currPair.setSrchPinType2(_netlist.getPinTypeInstPinConn(instId2, srchPinId2));
                dfsStack.push_back(currPair);
                inVldDiffPairSrch(diffPairSrc, currPair); //invalidate DFS sources
            }
            else if (validDiffPair(instId1, instId2, srchPinId1, srchPinId2) && // valid DIFF_SOURCE connected through gate.
                    !existPair(dfsVstPair, instId1, instId2) &&  // not visited
                    !existPair(dfsStack, instId1, instId2) &&
                    !existPair(diffPairSrc, instId1, instId2)) // not already as DFS source. we don't want to merge these groups.
            {
                MosPair currPair(instId1, instId2, MosPattern::DIFF_SOURCE);
                currPair.setSrchPinType1(_netlist.getPinTypeInstPinConn(instId1, srchPinId1));
                currPair.setSrchPinType2(_netlist.getPinTypeInstPinConn(instId2, srchPinId2));
                dfsStack.push_back(currPair);
                inVldDiffPairSrch(diffPairSrc, currPair);
            }
       }
    }
}

bool SymDetect::comBias(MosPair & currObj) const
{
    if (currObj.pattern() == MosPattern::LOAD ||
        currObj.pattern() == MosPattern::CASCODE)
    {
        if (_netlist.mosType(currObj.mosId1()) == MosType::DIFF &&
            _netlist.mosType(currObj.mosId2()) == MosType::DIFF &&
            _netlist.gateNetId(currObj.mosId1()) == _netlist.gateNetId(currObj.mosId2()))
        {
            return true;
        }
    }   
    return false;
}

void SymDetect::addBiasSym(std::vector<MosPair> & dfsVstPair, MosPair & currObj) const
{
    if (comBias(currObj))
    {
        std::vector<IndexType> Mos;
        _netlist.getInstNetConn(Mos, _netlist.gateNetId(currObj.mosId1()));
        _netlist.fltrInstMosType(Mos, MosType::DIODE);
        if (Mos.size() == 2 &&
                !existPair(dfsVstPair, Mos[0], Mos[1]) && 
                _pattern.pattern(Mos[0], Mos[1]) != MosPattern::INVALID)
            dfsVstPair.emplace_back(Mos[0], Mos[1], MosPattern::BIAS);
    }
}

void SymDetect::dfsDiffPair(std::vector<MosPair> & dfsVstPair, MosPair & diffPair, 
                            std::vector<MosPair> & diffPairSrc) const
{
    std::vector<MosPair> dfsStack;  //use vector to implement stack.
    dfsStack.push_back(diffPair);
    while (!dfsStack.empty()) //DFS 
    {
        MosPair currObj = dfsStack.back();
        dfsStack.pop_back();
        dfsVstPair.push_back(currObj); //pop current visit from stack and add to visited
        pushNextSrchObj(dfsVstPair, dfsStack, currObj, diffPairSrc);
        addBiasSym(dfsVstPair, currObj); //Added bias symmetry pairs.
    } 
}

void SymDetect::hiSymDetect(std::vector<std::vector<MosPair>> & symGroup) const 
{
    std::vector<MosPair> dfsVstPair;
    std::vector<MosPair> diffPairSrc;
    getDiffPair(diffPairSrc); //get all DFS source
    for (MosPair & pair : diffPairSrc) //for each source
    {
        if (pair.valid())
        {
            dfsVstPair.clear();
            dfsDiffPair(dfsVstPair, pair, diffPairSrc); //search
            addSelfSym(dfsVstPair); //add self symmetry pairs.
            symGroup.push_back(dfsVstPair); //add results to new group
        }
    }
    for (std::vector<MosPair> & group : symGroup)
    {
        for (MosPair & pair : diffPairSrc)
        {
            if (pair.isEqual(group[0]) && !pair.valid())
            {
                group[0].inVld();
            }
        }
    }
}

void SymDetect::getVldDrainMos(std::vector<IndexType> & vldMos, IndexType netId) const
{
    _netlist.getInstNetConn(vldMos, netId);  
    _netlist.fltrInstNetConnPinType(vldMos, netId, PinType::DRAIN); //filter connect by drain
    _netlist.fltrInstMosType(vldMos, MosType::DIFF); //filter by MosType::DIFF
}
    

void SymDetect::selfSymSrch(std::vector<MosPair> & dfsVstPair, MosPair & diffPair) const
{
    if (diffPair.pattern() != MosPattern::DIFF_SOURCE && diffPair.pattern() != MosPattern::CROSS_LOAD)
        return; //return if not of DIFF_SOURCE pattern.
    if (!_netlist.isSignal(_netlist.srcNetId(diffPair.mosId1())))
        return; //return if already reached ground.
    std::vector<IndexType> dfsStack; //used vector as stack implementation.
    getVldDrainMos(dfsStack, _netlist.srcNetId(diffPair.mosId1())); //push all valid drain connection to stack.
    while (!dfsStack.empty()) //dfs search
    {
        IndexType currMosId = dfsStack.back();
        dfsStack.pop_back();
        if (!existPair(dfsVstPair, currMosId)) //if not already as a pattern.
            dfsVstPair.emplace_back(currMosId, currMosId, MosPattern::SELF);
        IndexType netId = _netlist.srcNetId(currMosId);
        if (_netlist.isSignal(netId)) //search if not reached ground.
        {
            std::vector<IndexType> nextVst;
            getVldDrainMos(nextVst, _netlist.srcNetId(currMosId));
            for (IndexType id : nextVst)
                dfsStack.push_back(id); //add valid Inst to search.
        }
    }
}

void SymDetect::addSelfSym(std::vector<MosPair> & dfsVstPair) const
{
    for (MosPair & pair : dfsVstPair)
        selfSymSrch(dfsVstPair, pair);
}

void SymDetect::flattenSymGroup(std::vector<std::vector<MosPair>> & symGroup, std::vector<MosPair> & flatPair) const
{
    for (std::vector<MosPair> & group : symGroup)
    {
        for (MosPair & pair : group)
        {
            flatPair.push_back(pair);
        }
    }
}

bool SymDetect::checkNetSym(IndexType netId1, IndexType netId2) const
{
    // Very naive priliminary approach. Only check pin numbers.
    if (_netlist.net(netId1).pinIdArray().size() != 
        _netlist.net(netId2).pinIdArray().size())
        return false;
    int size = _netlist.net(netId1).pinIdArray().size();
    bool pinSym1[size] = {false};
    bool pinSym2[size] = {false};
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (pinSym2[j])
                continue;
            IndexType instId1, instId2;
            instId1 = _netlist.pin(_netlist.net(netId1).pinIdArray().at(i)).instId();
            instId2 = _netlist.pin(_netlist.net(netId2).pinIdArray().at(j)).instId();
            if (existPair(_flatPair, instId1, instId2))
            {
                pinSym1[i] = true;
                pinSym2[j] = true;
                break; //fixed bug
            }
        }
    }
    for (int i = 0; i < size; i++)
        if (!pinSym1[i])
            return false;
    return true;
}

bool SymDetect::validNetPair(IndexType netId1, IndexType netId2, std::vector<NetPair> & netPair) const
{
    if (netId1 == netId2 &&
        !existNetPair(netPair, netId1) &&
        _netlist.isSignal(netId1))
        return checkNetSym(netId1, netId2); 
    else if (netId1 != netId2 &&
                !existNetPair(netPair, netId1, netId2))
        return checkNetSym(netId1, netId2);
    return false;
}

void SymDetect::addSymNet(std::vector<NetPair> & netPair, MosPair & currObj) const
{
    if (currObj.pattern() == MosPattern::SELF)
        return;
    IndexType netId1, netId2;
    netId1 = _netlist.instNetId(currObj.mosId1(), currObj.srchPinType1());
    netId2 = _netlist.instNetId(currObj.mosId2(), currObj.srchPinType2());
    if (validNetPair(netId1, netId2, netPair))
        netPair.emplace_back(netId1, netId2);
    netId1 = _netlist.instNetId(currObj.mosId1(), currObj.nextPinType1());
    netId2 = _netlist.instNetId(currObj.mosId2(), currObj.nextPinType2());
    if (validNetPair(netId1, netId2, netPair))
        netPair.emplace_back(netId1, netId2);
    netId1 = _netlist.gateNetId(currObj.mosId1());
    netId2 = _netlist.gateNetId(currObj.mosId2());
    if (currObj.pattern() == MosPattern::DIFF_SOURCE &&
        validNetPair(netId1, netId2, netPair))
        netPair.emplace_back(netId1, netId2);
}

void SymDetect::addSelfSymNet(std::vector<NetPair> & netPair) const
{
    for (IndexType netId = 0; netId < _netlist.numNet(); netId++)
    {
        if (_netlist.isSignal(netId) &&
        validNetPair(netId, netId, netPair))
        netPair.emplace_back(netId, netId);
    }
}

void SymDetect::biasGroup(std::vector<MosPair> & flatPair, std::vector<Bias> & biasGroup,
                            std::vector<NetPair> & netPair) const
{
    std::vector<IndexType> vstNet;
    for (MosPair & pair : flatPair)
    {
        addSymNet(netPair, pair);
        if (comBias(pair))
        {
            IndexType netId = _netlist.gateNetId(pair.mosId1());
            if (std::find(vstNet.begin(), vstNet.end(), netId) == vstNet.end())
            {
                vstNet.push_back(netId);
                biasGroup.emplace_back(netId, _netlist);
            }
        }
    }
}

void SymDetect::biasMatch(std::vector<Bias> & biasGroup, std::vector<std::vector<MosPair>> & symGroup,
                            std::vector<MosPair> & flatPair) const
{
    for (Bias & bias : biasGroup)
    {
        if (bias.valid())
        {
            for (IndexType target : bias.bias())
            {
                if (target != bias.driver().at(0) &&
                    _pattern.pattern(target, bias.driver().at(0)) != MosPattern::INVALID &&
                    !existPair(flatPair, target))
                {
                    std::vector<MosPair> biasPair;
                    biasPair.emplace_back(bias.driver().at(0), target, MosPattern::BIAS);
                    symGroup.push_back(biasPair);
                }
            }
        }
    }
}

PROJECT_NAMESPACE_END
