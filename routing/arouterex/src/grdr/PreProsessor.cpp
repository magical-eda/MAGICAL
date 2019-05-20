#include "PreProsessor.h"

PROJECT_NAMESPACE_BEGIN

/* Terminal */
bool ProcessTerminal::process()
{
    if (_db.progArgs().isTerminalFileSet())
    {
        if (!processTerminalFile())
        {
            return false;
        }
    }
    else
    {
        if (!processNoTerminalFile())
        {
            return false;
        }
    }
    // Calculate bounding box for the terminal
    for (auto &term : _db.terminalArray())
    {
        term.findBBox();
    }
    return true;
}

bool ProcessTerminal::processNoTerminalFile()
{
    // if no terminal file is given, regard each pin shape as a individual default terminal
    IndexType numTerminals = _db.grDB().phyPinArray().size();
    _db.terminalArray().resize(numTerminals);

    // each pin corresponding to one terminal
    for (IndexType pinIdx = 0; pinIdx < numTerminals; ++pinIdx)
    {
        _db.terminal(pinIdx).addPin(pinIdx);
    }

    // Add terminals to the nets
    for (IndexType netIdx = 0; netIdx <  _db.grDB().nets().size(); ++netIdx)
    {
        auto &net = _db.grDB().nets().at(netIdx);
        for (IndexType pinIdx : net.physPinArray())
        {
            net.addTerminal(pinIdx);
            _db.terminal(pinIdx).setNetIdx(netIdx);
        }
    }
    for (IndexType termIdx = 0; termIdx < numTerminals; ++termIdx)
    {
        _intra.intraCellRouteDEFAULT(termIdx);
    }
    return true;
}

bool ProcessTerminal::processTerminalFile()
{
    // parse the file in
    if (!_parser.read(_db.progArgs().terminalFileName()))
    {
        return false;
    }
    // Configure the nets to make they know what terminals they contain
    for (IndexType netIdx = 0; netIdx <  _db.grDB().nets().size(); ++netIdx)
    {
        auto &net = _db.grDB().nets().at(netIdx);
        for (IndexType pinIdx : net.physPinArray())
        {
            auto deviceType = _parser.deviceTypeArray().at(pinIdx);
            // The terminal order is 1. defaults, 2. CMOSs
            IndexType termIdx = 0;
            if (deviceType == TerminalDeviceType::DEFAULT)
            {
                termIdx = _parser.deviceIdxArray().at(pinIdx);
            }
            else if (deviceType == TerminalDeviceType::CMOS)
            {
                termIdx = _parser.numDefault() + _parser.deviceIdxArray().at(pinIdx) * 3 + _parser.attributesArray().at(pinIdx);
            }
            bool added = false;
            for (IndexType termAdded : net.terminalArray())
            {
                if (termAdded == termIdx)
                {
                    added = true;
                    break;
                }
            }
            if (!added)
            {
                net.addTerminal(termIdx);
                _db.terminal(termIdx).setNetIdx(netIdx);
            }
        }
    }
    // Calculate the total number of terminals needed
    IndexType numTerminals = _parser.numDefault() + _parser.numCMOS() * 3;
    _db.terminalArray().resize(numTerminals);
    // Add the pins to the terminals
    for (IndexType pinIdx = 0; pinIdx < _parser.deviceIdxArray().size(); ++pinIdx)
    {
        auto deviceType = _parser.deviceTypeArray().at(pinIdx);
        IndexType termIdx = 0;
        // The terminal order is 1. defaults, 2. CMOSs
        if (deviceType == TerminalDeviceType::DEFAULT)
        {
            termIdx = _parser.deviceIdxArray().at(pinIdx);
        }
        else if (deviceType == TerminalDeviceType::CMOS)
        {
            termIdx = _parser.numDefault() + _parser.deviceIdxArray().at(pinIdx) * 3 + _parser.attributesArray().at(pinIdx);
        }
        _db.terminal(termIdx).addPin(pinIdx);
    }
    // Process default terminals
    for (IndexType termIdx = 0; termIdx < _parser.numDefault(); ++ termIdx)
    {
        _intra.intraCellRouteDEFAULT(termIdx);
    }
    // Process the CMOS terminals
    for (IndexType cmosIdx = 0; cmosIdx < _parser.numCMOS();  ++ cmosIdx)
    {
        IndexType gateTermIdx = _parser.numDefault() + cmosIdx *3 ;
        IndexType sourceTermIdx = gateTermIdx + 1;
        IndexType drainTermIdx = sourceTermIdx + 1;
        _intra.intraCellRouteCMOS(gateTermIdx, sourceTermIdx, drainTermIdx);
    }
    return true;
}

/* Symnet */
bool ProcessSymNet::process()
{
    for (IndexType symNetIdx = 0; symNetIdx < _db.numSymNets(); ++symNetIdx)
    {
        if (!processOneSymNet(symNetIdx))
        {
            return false;
        }
    }
    return true;
}

bool ProcessSymNet::processOneSymNet(IndexType symNetIdx)
{
    auto &symNet = _db.symNet(symNetIdx);
    // Do not consider Steiner points yet!
    // Two nets involved in this symmetry pair
    auto &netFirst = _db.grDB().nets().at(symNet.firstNetIdx());
    auto &netSecond = _db.grDB().nets().at(symNet.secondNetIdx());
    // Check they have the same number of pins
    if (netFirst.numTerminals() != netSecond.numTerminals())
    {
        netFirst.invalidateSym();
        netSecond.invalidateSym();
        ERR("Process Symmetry Net: Two sym nets do not have the same number of terminals! %d %d Their symmetry constraint is ignored. \n",  netFirst.numTerminals(), netSecond.numTerminals());
        return true;
    }
    if (netFirst.numTerminals() <= 1)
    {
        netFirst.invalidateSym();
        netSecond.invalidateSym();
        ERR("Process Symmetry Net: Symmetry net pair %s %s only has %d terminal each. Their symmetry constraint is ignored. \n", netFirst.name().c_str(), netSecond.name().c_str(),  netFirst.numTerminals());
        return true;
    }
    // First try horizontal symmetry
    bool foundSym = this->configureHorizontalSym(symNetIdx);
    // If not horizontal, try vertical
    if (!foundSym)
    {
        foundSym = this->configureVerticalSym(symNetIdx);
    }
    else
    {
        INF("%s: %s %s horizontal %d \n", __FUNCTION__, netFirst.name().c_str(), netSecond.name().c_str(), symNet.symAxis());
        return true;
    }
    // If not found symmetry, mark as invalid
    if (!foundSym)
    {
        ERR("Process Symmetry Net: Failed to recognize symmetry of net pair %s %s! Their symmetry constraint is ignored. \n", netFirst.name().c_str(), netSecond.name().c_str());
        symNet.setSymType(SymType::NONE);
        netFirst.invalidateSym();
        netSecond.invalidateSym();
        return true;
    }
    else
    {
       INF("%s: %s %s vertical \n", __FUNCTION__, netFirst.name().c_str(), netSecond.name().c_str());
    }
    return true;
}

bool ProcessSymNet::configureHorizontalSym(IndexType symNetIdx)
{
    auto &symNet = _db.symNet(symNetIdx);
    auto &net1 = _db.grDB().nets().at(symNet.firstNetIdx());
    auto &net2 = _db.grDB().nets().at(symNet.secondNetIdx());
    // Get the terminal indices of the two nets
    auto termIdxArray1 = net1.terminalArray();
    auto termIdxArray2 = net2.terminalArray();
    // Grab the terminal array from the database
    const auto &termArray = _db.terminalArray();
    // Check horizontal symmetry
    // Sort the terminals by the x coordinate of termial primary point 
    // First define a couple of lambda functions for the sorting
    auto sortedByXFunc = [&](IndexType termIdx1, IndexType termIdx2) { return termArray.at(termIdx1).primarySearchPoint().loc().x() == termArray.at(termIdx2).primarySearchPoint().loc().x() ? 
                                                                            termArray.at(termIdx1).primarySearchPoint().loc().y() > termArray.at(termIdx2).primarySearchPoint().loc().y() :
                                                                            termArray.at(termIdx1).primarySearchPoint().loc().x() > termArray.at(termIdx2).primarySearchPoint().loc().x(); };
    auto sortedByXReveredFunc = [&](IndexType termIdx1, IndexType termIdx2) { return termArray.at(termIdx1).primarySearchPoint().loc().x() == termArray.at(termIdx2).primarySearchPoint().loc().x() ? 
                                                                                   termArray.at(termIdx1).primarySearchPoint().loc().y() > termArray.at(termIdx2).primarySearchPoint().loc().y() :
                                                                                   termArray.at(termIdx1).primarySearchPoint().loc().x() < termArray.at(termIdx2).primarySearchPoint().loc().x(); };
    // Sort the pins by the x and x_reversed seperatedly, and check whether each pair of pins sharing the same symmetric axis
    std::sort(termIdxArray1.begin(), termIdxArray1.end(), sortedByXFunc);
    std::sort(termIdxArray2.begin(), termIdxArray2.end(), sortedByXReveredFunc);
    // The symmetric axis of the first pins of the two nets
    LocType horizontalSymAxis = ( termArray.at(termIdxArray1.front()).primarySearchPoint().loc().x() + termArray.at(termIdxArray2.front()).primarySearchPoint().loc().x() ) / 2;
    bool smaller = termArray.at(termIdxArray1.front()).primarySearchPoint().loc().x() < horizontalSymAxis; // Whether the net1 is at the smaller side of the axis
    // starting from idx = 0, should be one useless comparision. But who cares.
    for (IndexType idx = 0; idx < termIdxArray1.size(); ++idx)
    {
        //DBG("axis %d \n", (termArray.at(termIdxArray1.at(idx)).primarySearchPoint().loc().x() +termArray.at(termIdxArray2.at(idx)).primarySearchPoint().loc().x() ) / 2 );
        if ((termArray.at(termIdxArray1.at(idx)).primarySearchPoint().loc().x() < horizontalSymAxis) != smaller)
        {
            return false;
        }
        if (!this->isTwoTerminalsSymmetric(termArray.at(termIdxArray1.at(idx)), termArray.at(termIdxArray2.at(idx)), horizontalSymAxis, SymType::HORIZONTAL))
        {
            return false;
        }
    }
    // Swap the two nets if the first net is on the larger side of the axis
    if (!smaller)
    {
        IndexType net1 = symNet.firstNetIdx();
        IndexType net2 = symNet.secondNetIdx();
        symNet.setFirstNetIdx(net2);
        symNet.setSecondNetIdx(net1);
        // Configure the sym net and return 
        this->configureSymNet(symNetIdx, SymType::HORIZONTAL, horizontalSymAxis, termIdxArray2, termIdxArray1);
    }
    else
    {
        // Configure the sym net and return 
        this->configureSymNet(symNetIdx, SymType::HORIZONTAL, horizontalSymAxis, termIdxArray1, termIdxArray2);
    }
    return true;
}


bool ProcessSymNet::configureVerticalSym(IndexType symNetIdx)
{
    auto &symNet = _db.symNet(symNetIdx);
    auto &net1 = _db.grDB().nets().at(symNet.firstNetIdx());
    auto &net2 = _db.grDB().nets().at(symNet.secondNetIdx());
    // Get the terminal indices of the two nets
    auto termIdxArray1 = net1.terminalArray();
    auto termIdxArray2 = net2.terminalArray();
    // Grab the terminal array from the database
    const auto &termArray = _db.terminalArray();
    // Check vertical symmetry
    // Sort the terminals by the x coordinate of termial primary point 
    // First define a couple of lambda functions for the sorting
    auto sortedByYFunc = [&](IndexType termIdx1, IndexType termIdx2) { return termArray.at(termIdx1).primarySearchPoint().loc().y() == termArray.at(termIdx2).primarySearchPoint().loc().y() ? 
                                                                            termArray.at(termIdx1).primarySearchPoint().loc().x() > termArray.at(termIdx2).primarySearchPoint().loc().x() :
                                                                            termArray.at(termIdx1).primarySearchPoint().loc().y() > termArray.at(termIdx2).primarySearchPoint().loc().y(); };
    auto sortedByYReveredFunc = [&](IndexType termIdx1, IndexType termIdx2) { return termArray.at(termIdx1).primarySearchPoint().loc().y() == termArray.at(termIdx2).primarySearchPoint().loc().y() ? 
                                                                                   termArray.at(termIdx1).primarySearchPoint().loc().x() > termArray.at(termIdx2).primarySearchPoint().loc().x() :
                                                                                   termArray.at(termIdx1).primarySearchPoint().loc().y() < termArray.at(termIdx2).primarySearchPoint().loc().y(); };
    // Sort the pins by the x and x_reversed seperatedly, and check whether each pair of pins sharing the same symmetric axis
    std::sort(termIdxArray1.begin(), termIdxArray1.end(), sortedByYFunc);
    std::sort(termIdxArray2.begin(), termIdxArray2.end(), sortedByYReveredFunc);
    // The symmetric axis of the first pins of the two nets
    LocType verticalSymAxis = ( termArray.at(termIdxArray1.front()).primarySearchPoint().loc().y() + termArray.at(termIdxArray2.front()).primarySearchPoint().loc().y() ) / 2;
    bool smaller = termArray.at(termIdxArray1.front()).primarySearchPoint().loc().y() < verticalSymAxis; // Whether the net1 is at the smaller side of the axis
    // starting from idx = 0, should be one useless comparision. But who cares.
    for (IndexType idx = 0; idx < termIdxArray1.size(); ++idx)
    {
        if ((termArray.at(termIdxArray1.at(idx)).primarySearchPoint().loc().y() < verticalSymAxis) != smaller)
        {
            return false;
        }
        if (!this->isTwoTerminalsSymmetric(termArray.at(termIdxArray1.at(idx)), termArray.at(termIdxArray2.at(idx)), verticalSymAxis, SymType::VERTICAL))
        {
            return false;
        }
    }
    // Swap the two nets if the first net is on the larger side of the axis
    if (!smaller)
    {
        IndexType net1 = symNet.firstNetIdx();
        IndexType net2 = symNet.secondNetIdx();
        symNet.setFirstNetIdx(net2);
        symNet.setSecondNetIdx(net1);
        // Configure the sym net and return 
        this->configureSymNet(symNetIdx, SymType::VERTICAL, verticalSymAxis, termIdxArray2, termIdxArray1);
    }
    else
    {
        // Configure the sym net and return 
        this->configureSymNet(symNetIdx, SymType::VERTICAL, verticalSymAxis, termIdxArray1, termIdxArray2);
    }
    return true;
}
void ProcessSymNet::configureSymNet(IndexType symNetIdx, SymType symType, LocType symAxis, const std::vector<IndexType> &terminals1, const std::vector<IndexType> &terminals2)
{
    auto &symNet = _db.symNet(symNetIdx);
    symNet.setSymType(symType);
    symNet.setSymAxis(symAxis);
    // add pair of pins
    symNet.terminalPairs().clear();
    for (IndexType idx = 0; idx < terminals1.size(); ++idx)
    {
        symNet.terminalPairs().emplace_back(std::make_pair(terminals1.at(idx), terminals2.at(idx)));
    }
}

bool ProcessSymNet::isTwoTerminalsSymmetric(const Terminal &term1, const Terminal &term2, LocType symAxis, SymType symType)
{
    // Just check the primary search point is within the other terminal after flipping
    auto pt = term1.primarySearchPoint();
    //DBG("%s %d \n", pt.loc().toStr().c_str(), symAxis);
    if (symType == SymType::HORIZONTAL)
    {
        LocType newX = symAxis * 2 - pt.loc().x();
        pt.loc().setX(newX);
    }
    else
    {
        pt.loc().setY(symAxis * 2 - pt.loc().y());
    }
    //DBG("%s: search pt %s \n", __FUNCTION__, pt.loc().toStr().c_str());
    if (term2.isPointConnected(pt.loc(), DRUtil::routeLayer2AllLayer(pt.layer())))
    {
        return true;
    }
    return false;
}

/* SelfSymnet */
bool ProcessSelfSymNet::process()
{
    for (IndexType symNetIdx = 0; symNetIdx < _db.numSelfSymNets(); ++symNetIdx)
    {
        if (!processOneSelfSymNet(symNetIdx))
        {
            return false;
        }
    }
    return true;
}

bool ProcessSelfSymNet::processOneSelfSymNet(IndexType symNetIdx)
{
    auto &symNet = _db.selfSymNet(symNetIdx);
    // Do not consider Steiner points yet!
    // Two nets involved in this symmetry pair
    auto &net = _db.grDB().nets().at(symNet.netIdx());
    // Check they have the same number of pins
    if (net.numTerminals() %2 == 0)
    {
        net.invalidSelfSym();
        ERR("Process Self Symmetry Net: Self sym net does not have the odd number of terminals! %d  Their symmetry constraint is ignored. \n", symNet.netIdx() );
        return true;
    }
    if (net.numTerminals() <= 1)
    {
        net.invalidSelfSym();
        ERR("Process Self Symmetry Net: Symmetry net pair %s  only has %d terminal. Their symmetry constraint is ignored. \n", net.name().c_str(),  net.numTerminals());
        return true;
    }
    // First try horizontal symmetry
    bool foundSym = this->configureHorizontalSym(symNetIdx);
    // If not horizontal, try vertical
    if (!foundSym)
    {
        foundSym = this->configureVerticalSym(symNetIdx);
    }
    else
    {
       INF("%s: %s  horizontal axis %d \n", __FUNCTION__, net.name().c_str(), symNet.symAxis());
        return true;
    }
    // If not found symmetry, mark as invalid
    if (!foundSym)
    {
        ERR("Process Self Symmetry Net: The symmetry net %s do not have symmetry! Their symmetry constraint is ignored. \n", net.name().c_str());
        symNet.setSymType(SymType::NONE);
        net.invalidSelfSym();
        return true;
    }
    else
    {
       INF("%s: %s  vertical \n", __FUNCTION__, net.name().c_str());
    }
    return true;
}

bool ProcessSelfSymNet::configureHorizontalSym(IndexType symNetIdx)
{
    auto &symNet = _db.selfSymNet(symNetIdx);
    auto &net = _db.grDB().nets().at(symNet.netIdx());
    // Get the terminal indices of the two nets
    auto termIdxArray = net.terminalArray();
    // Grab the terminal array from the database
    const auto &termArray = _db.terminalArray();
    // Check horizontal symmetry
    // Sort the terminals by the x coordinate of termial primary point 
    // First define a couple of lambda functions for the sorting
    auto sortedByXFunc = [&](IndexType termIdx1, IndexType termIdx2) { return termArray.at(termIdx1).primarySearchPoint().loc().x() == termArray.at(termIdx2).primarySearchPoint().loc().x() ? 
                                                                            termArray.at(termIdx1).primarySearchPoint().loc().y() > termArray.at(termIdx2).primarySearchPoint().loc().y() :
                                                                            termArray.at(termIdx1).primarySearchPoint().loc().x() < termArray.at(termIdx2).primarySearchPoint().loc().x(); };
    // Sort the pins by the x and x_reversed seperatedly, and check whether each pair of pins sharing the same symmetric axis
    std::sort(termIdxArray.begin(), termIdxArray.end(), sortedByXFunc);
    auto sortedByXFuncReverseY = [&](IndexType termIdx1, IndexType termIdx2) { return termArray.at(termIdx1).primarySearchPoint().loc().x() == termArray.at(termIdx2).primarySearchPoint().loc().x() ? 
                                                                            termArray.at(termIdx1).primarySearchPoint().loc().y() < termArray.at(termIdx2).primarySearchPoint().loc().y() :
                                                                            termArray.at(termIdx1).primarySearchPoint().loc().x() < termArray.at(termIdx2).primarySearchPoint().loc().x(); };
    // Reverse the y if x the same for the later half of the array
    std::sort(termIdxArray.begin() + (termIdxArray).size() / 2, termIdxArray.end(), sortedByXFuncReverseY);
    // The symmetric axis of the first pins of the two nets
    LocType horizontalSymAxis = ( termArray.at(termIdxArray.front()).primarySearchPoint().loc().x() + termArray.at(termIdxArray.back()).primarySearchPoint().loc().x() ) / 2;
    // starting from idx = 0, should be one useless comparision. But who cares.
    for (IndexType idx = 0; idx < termIdxArray.size()/2; ++idx)
    {
        if ((termArray.at(termIdxArray.at(idx)).primarySearchPoint().loc().x() >= horizontalSymAxis))
        {
            return false;
        }
        if (!this->isTwoTerminalsSymmetric(termArray.at(termIdxArray.at(idx)), termArray.at(termIdxArray.at(termIdxArray.size() - 1 - idx)), horizontalSymAxis, SymType::HORIZONTAL))
        {
            return false;
        }
    }
    // Swap the two nets if the first net is on the larger side of the axis
    // Configure the sym net and return 
    this->configureSelfSymNet(symNetIdx, SymType::HORIZONTAL, horizontalSymAxis, termIdxArray);
    return true;
}


bool ProcessSelfSymNet::configureVerticalSym(IndexType symNetIdx)
{
    auto &symNet = _db.selfSymNet(symNetIdx);
    auto &net = _db.grDB().nets().at(symNet.netIdx());
    // Get the terminal indices of the two nets
    auto termIdxArray = net.terminalArray();
    // Grab the terminal array from the database
    const auto &termArray = _db.terminalArray();
    // Check horizontal symmetry
    // Sort the terminals by the x coordinate of termial primary point 
    // First define a couple of lambda functions for the sorting
    auto sortedByXFunc = [&](IndexType termIdx1, IndexType termIdx2) { return termArray.at(termIdx1).primarySearchPoint().loc().y() == termArray.at(termIdx2).primarySearchPoint().loc().y() ? 
                                                                            termArray.at(termIdx1).primarySearchPoint().loc().x() > termArray.at(termIdx2).primarySearchPoint().loc().x() :
                                                                            termArray.at(termIdx1).primarySearchPoint().loc().y() < termArray.at(termIdx2).primarySearchPoint().loc().y(); };
    // Sort the pins by the x and x_reversed seperatedly, and check whether each pair of pins sharing the same symmetric axis
    std::sort(termIdxArray.begin(), termIdxArray.end(), sortedByXFunc);
    auto sortedByXFuncReverseX = [&](IndexType termIdx1, IndexType termIdx2) { return termArray.at(termIdx1).primarySearchPoint().loc().y() == termArray.at(termIdx2).primarySearchPoint().loc().y() ? 
                                                                            termArray.at(termIdx1).primarySearchPoint().loc().x() > termArray.at(termIdx2).primarySearchPoint().loc().x() :
                                                                            termArray.at(termIdx1).primarySearchPoint().loc().y() < termArray.at(termIdx2).primarySearchPoint().loc().y(); };
    // Reverse the y if x the same for the later half of the array
    std::sort(termIdxArray.begin() + (termIdxArray).size() / 2, termIdxArray.end(), sortedByXFuncReverseX);
    // The symmetric axis of the first pins of the two nets
    LocType verSymAxis = ( termArray.at(termIdxArray.front()).primarySearchPoint().loc().y() + termArray.at(termIdxArray.back()).primarySearchPoint().loc().y() ) / 2;
    // starting from idx = 0, should be one useless comparision. But who cares.
    for (IndexType idx = 0; idx < termIdxArray.size()/2; ++idx)
    {
        if ((termArray.at(termIdxArray.at(idx)).primarySearchPoint().loc().y() >= verSymAxis))
        {
            return false;
        }
        if (!this->isTwoTerminalsSymmetric(termArray.at(termIdxArray.at(idx)), termArray.at(termIdxArray.at(termIdxArray.size() - 1 - idx)), verSymAxis, SymType::VERTICAL))
        {
            return false;
        }
    }
    // Swap the two nets if the first net is on the larger side of the axis
    // Configure the sym net and return 
    this->configureSelfSymNet(symNetIdx, SymType::HORIZONTAL, verSymAxis, termIdxArray);
    return true;
}
void ProcessSelfSymNet::configureSelfSymNet(IndexType symNetIdx, SymType symType, LocType symAxis, const std::vector<IndexType> &terminals)
{
    auto &symNet = _db.selfSymNet(symNetIdx);
    symNet.setSymType(symType);
    symNet.setSymAxis(symAxis);
    // Set the middle terminal
    IndexType midTerm = terminals.at(terminals.size() / 2 );
    symNet.setMiddle(midTerm);
    // add pair of pins
    symNet.terminalPairs().clear();
    for (IndexType idx = 0; idx < terminals.size() / 2; ++idx)
    {
        symNet.terminalPairs().emplace_back(std::make_pair(terminals.at(idx), terminals.at( terminals.size() - 1 - idx)));
    }
}

bool ProcessSelfSymNet::isTwoTerminalsSymmetric(const Terminal &term1, const Terminal &term2, LocType symAxis, SymType symType)
{
    // Just check the primary search point is within the other terminal after flipping
    auto pt = term1.primarySearchPoint();
    //DBG("%s %d \n", pt.loc().toStr().c_str(), symAxis);
    if (symType == SymType::HORIZONTAL)
    {
        LocType newX = symAxis * 2 - pt.loc().x();
        pt.loc().setX(newX);
    }
    else
    {
        pt.loc().setY(symAxis * 2 - pt.loc().y());
    }
    //DBG("%s: search pt %s \n", __FUNCTION__, pt.loc().toStr().c_str());
    if (term2.isPointConnected(pt.loc(), DRUtil::routeLayer2AllLayer(pt.layer())))
    {
        return true;
    }
    return false;
}

bool ProcessBlockage::process()
{
    if (!mergePins()) { return false; }
    return true;
}

bool ProcessBlockage::mergePins()
{
    // Query the shapeMap and only save the ones not overlapping with pins
    /// TODO: Here just using the bruteforce comparision, if the run time is intolerantable, use r tree instead
    for (auto &terminal : _db.terminalArray())
    {
        std::set<IndexType> removeBlockageList;
        std::set<IndexType> boxesToAdd;
        std::vector<Blockage> blocksFound;
        for (IndexType blockIdx = 0; blockIdx < _db.numBlockages(); ++blockIdx)
        {
            const auto &block = _db.blockage(blockIdx);
            for (const auto &rect : terminal.intraCellRoute())
            {
                // If the two shapes are identical, remove the blockage
                if (block.layer() == rect.layer() && rect.box().intersect(block.rect()))
                {
                    blocksFound.emplace_back(block);
                    removeBlockageList.insert(blockIdx);
                    // Handle the small shape between horizontal pin segment and fingers
                    //if (!rect.box().overlap(block.rect()))
                    if (1)
                    {
                        boxesToAdd.insert(blockIdx);
                    }
                }
            }
        }
        // Add intersect shapes to the terminals
        for (IndexType blockIdx : boxesToAdd)
        {
            const auto &block = _db.blockage(blockIdx);
            terminal.addIntraBox(block.rect(), block.layer(), false, false);
            // Forbidding connecting to the shapes not defined in design input. The main reason is the property error for capacitor
            //terminal.addIntraBox(block.rect(), block.layer(), true, false);
        }
        // The removeBlockageList is in accending order, and therefore can be safely erased one by one in the reverse order
        for (auto it = removeBlockageList.rbegin(); it != removeBlockageList.rend(); ++it)
        {
            ::klib::eraseElementInVec<Blockage>(_db.blockageArray(), *it);
        }
        // Add neighbor
        removeBlockageList.clear();
        boxesToAdd.clear();
        std::vector<Blockage> secondBlocksFound;
        for (IndexType blockIdx = 0; blockIdx < _db.numBlockages(); ++blockIdx)
        {
            const auto &block = _db.blockage(blockIdx);
            for (const auto &rect : blocksFound)
            {
                // If the two shapes are identical, remove the blockage
                if (block.layer() == rect.layer() && rect.rect().intersect(block.rect()))
                {
                    secondBlocksFound.emplace_back(block);
                    removeBlockageList.insert(blockIdx);
                    // Handle the small shape between horizontal pin segment and fingers
                    //if (!rect.box().overlap(block.rect()))
                    if (1)
                    {
                        boxesToAdd.insert(blockIdx);
                    }
                }
            }
        }
        // Add intersect shapes to the terminals
        for (IndexType blockIdx : boxesToAdd)
        {
            const auto &block = _db.blockage(blockIdx);
            terminal.addIntraBox(block.rect(), block.layer(), false, false);
            // Forbidding connecting to the shapes not defined in design input. The main reason is the property error for capacitor
            //terminal.addIntraBox(block.rect(), block.layer(), true, false);
        }
        // The removeBlockageList is in accending order, and therefore can be safely erased one by one in the reverse order
        for (auto it = removeBlockageList.rbegin(); it != removeBlockageList.rend(); ++it)
        {
            ::klib::eraseElementInVec<Blockage>(_db.blockageArray(), *it);
        }
        // Add neighbor
        removeBlockageList.clear();
        boxesToAdd.clear();
        for (IndexType blockIdx = 0; blockIdx < _db.numBlockages(); ++blockIdx)
        {
            const auto &block = _db.blockage(blockIdx);
            for (const auto &rect : secondBlocksFound)
            {
                // If the two shapes are identical, remove the blockage
                if (block.layer() == rect.layer() && rect.rect().intersect(block.rect()))
                {
                    removeBlockageList.insert(blockIdx);
                    // Handle the small shape between horizontal pin segment and fingers
                    //if (!rect.box().overlap(block.rect()))
                    if (1)
                    {
                        boxesToAdd.insert(blockIdx);
                    }
                }
            }
        }
        // Add intersect shapes to the terminals
        for (IndexType blockIdx : boxesToAdd)
        {
            const auto &block = _db.blockage(blockIdx);
            terminal.addIntraBox(block.rect(), block.layer(), false, false);
            // Forbidding connecting to the shapes not defined in design input. The main reason is the property error for capacitor
            //terminal.addIntraBox(block.rect(), block.layer(), true, false);
        }
        // The removeBlockageList is in accending order, and therefore can be safely erased one by one in the reverse order
        for (auto it = removeBlockageList.rbegin(); it != removeBlockageList.rend(); ++it)
        {
            ::klib::eraseElementInVec<Blockage>(_db.blockageArray(), *it);
        }
    }
    return true;
}

PROJECT_NAMESPACE_END
