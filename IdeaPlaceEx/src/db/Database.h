/**
 * @file Database.h
 * @brief The placement database data structure
 * @author Keren Zhu
 * @date 10/02/2019
 */

#ifndef IDEAPLACE_DATABASE_H_
#define IDEAPLACE_DATABASE_H_

#include "Cell.h"
#include "Net.h"
#include "Pin.h"
#include "Tech.h"
#include "Parameters.h"
#include "Constraints.h"

PROJECT_NAMESPACE_BEGIN

/// @class IDEAPLACE::Database
/// @brief the database class of the placement engine
class Database
{
    public:
        /// @brief default database
        explicit Database() = default;
        /*------------------------------*/ 
        /* Initialization               */
        /*------------------------------*/ 
        /// @brief initializing cells. Allocate the correct number of layers
        /// @return if successful
        bool initCells();
        /// @brief initializing a cell
        bool initCell(IndexType cellIdx);
        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get the technology-wrapper of the placement
        /// @return the technology
        const Tech & tech() const { return _tech; }
        /// @brief get the technology-wrapper of the placement
        /// @return the technology
        Tech & tech() { return _tech; }
        /// @brief get the placement parameter wrapper
        /// @return the placement parameter wrapper
        const Parameters & parameters() const { return _para; }
        /// @brief get the placement parameter wrapper
        /// @return the placement parameter wrapper
        Parameters & parameters() { return _para; }
        /*------------------------------*/ 
        /* Vector operations            */
        /*------------------------------*/ 
        /// @brief allocate a new symmetric group
        /// @return the index of the symmetric group
        IndexType allocateSymGrp() { _symGroups.emplace_back(SymGroup()); return _symGroups.size() - 1;}
        /// @brief get the number of symmetric groups
        /// @return the number of the symmetric groups
        IndexType numSymGroups() const { return _symGroups.size(); }
        /// @brief get a symmetric group
        /// @param the index of the symmetric group
        const SymGroup & symGroup(IndexType idx) const { return _symGroups.at(idx); }
        /// @brief get a symmetric group
        /// @param the index of the symmetric group
        SymGroup & symGroup(IndexType idx) { return _symGroups.at(idx); }
        /// @brief get the vector of symmetric groups
        /// @return the vector of symmetric groups
        const std::vector<SymGroup> & vSymGrpArray() const { return _symGroups; }
        /// @brief get the number of cells
        /// @return the number of cells
        IndexType numCells() const { return _cellArray.size(); }
        /// @brief get one cell from the array
        /// @param the index of the cell
        /// @return the cell of the index
        const Cell & cell(IndexType cellIdx) const { return _cellArray.at(cellIdx); }
        /// @brief get one cell from the array
        /// @param the index of the cell
        /// @return the cell of the index
        Cell & cell(IndexType cellIdx) { return _cellArray.at(cellIdx); }
        /// @brief allocate a new cell in the array
        /// @return the index of the new cell
        IndexType allocateCell() { _cellArray.emplace_back(Cell()); return _cellArray.size() - 1; }
        /// @brief get the cell array
        /// @return the cell array
        const std::vector<Cell> & vCellArray() const { return _cellArray; }
        /// @brief get the cell array
        /// @return the cell array
        std::vector<Cell> & vCellArray() { return _cellArray; }
        /// @brief set name of cell
        /// @param first: cellIdx
        /// @param second: cellName
        void setCellName(IndexType cellIdx, const std::string name) { _cellArray.at(cellIdx).setName(name); }
        /// @brief get the number of nets
        /// @param the index of the nets
        /// @return the net of the index
        IndexType numNets() const { return _netArray.size(); }
        /// @brief get one net from the array
        /// @param the index of the net
        /// @return the net of the index
        const Net & net(IndexType netIdx) const { return _netArray.at(netIdx); }
        /// @brief get one net from the array
        /// @param the index of the net
        /// @return the net of the index
        Net & net(IndexType netIdx) { return _netArray.at(netIdx); }
        /// @brief allocate a new net in the array
        /// @return index of the new net
        IndexType allocateNet() { _netArray.emplace_back(Net()); return _netArray.size() -1; }
        const std::vector<Net> & nets() const { return _netArray; }
        std::vector<Net> & nets() { return _netArray; }
        /// @brief get the number of pins
        /// @return the number of pins
        IndexType numPins() const { return _pinArray.size(); }
        /// @brief get one pin from the array
        /// @param the index of the pin
        /// @return the pin of the index
        const Pin & pin(IndexType pinIdx) const { return _pinArray.at(pinIdx); }
        /// @brief get one pin from the array
        /// @param the index of the pin
        /// @return the pin of the index
        Pin & pin(IndexType pinIdx) { return _pinArray.at(pinIdx); }
        /// @brief allocate a new pin in the array
        /// @return the index of the pin
        IndexType allocatePin() { _pinArray.emplace_back(Pin()); return _pinArray.size() - 1; }
        const std::vector<Pin> & vPinArray() const { return _pinArray; }
        std::vector<Pin> &vPinArray() { return _pinArray; }
        /// @brief get the vector of proximity groups
        /// @return the vector of proximity groups
        const std::vector<ProximityGroup> & proximityGrps() const { return _proximityGrps; }
        /// @brief get the vector of proximity groups
        /// @return the vector of proximity groups
        std::vector<ProximityGroup> & proximityGrps() { return _proximityGrps; }
        IndexType allocateProximityGroup() 
        { _proximityGrps.emplace_back(ProximityGroup()); return _proximityGrps.size() - 1; }
        const ProximityGroup &proximityGrp( IndexType grpIdx ) const { return _proximityGrps.at(grpIdx); }
        ProximityGroup &proximityGrp( IndexType grpIdx ) { return _proximityGrps.at(grpIdx); }

        const std::vector<SignalPath> &vSignalPaths() const { return _signalPaths; }
        std::vector<SignalPath> &vSignalPaths() { return _signalPaths; }
        IndexType allocateSignalPath() { _signalPaths.emplace_back(SignalPath()); return _signalPaths.size() - 1; }
        SignalPath & signalPath(IndexType idx) { return _signalPaths.at(idx); }
        const SignalPath & signalPath(IndexType idx) const { return _signalPaths.at(idx); }
        /// @brief split the signal path to seperate the sym pairs
        void splitSignalPathsBySymPairs();
        /// @brief add a new pin to a signal path
        /// @param first: the index of signal path
        /// @param second: the name of cell
        /// @param third: the name of pin
        void addPinToSignalPath(IndexType pathIdx, const std::string & cellName, const std::string & pinName)
        {
            auto & sig = this->signalPath(pathIdx);
            IndexType pinIdx = INDEX_TYPE_MAX;
            IndexType cellIdx = INDEX_TYPE_MAX;
            for (IndexType idx = 0; idx < this->numCells(); ++idx)
            {
                const auto & cell = this->cell(idx);
                if (cell.name() == cellName)
                {
                    cellIdx = idx;
                    for (IndexType jdx = 0; jdx < cell.numPinIdx(); ++jdx)
                    {
                        const auto &pin = this->pin(cell.pinIdx(jdx));
                        if (pin.name() == pinName)
                        {
                            pinIdx = cell.pinIdx(jdx);
                            break;
                        }
                    }
                    break;
                }
            }
            AssertMsg(cellIdx != INDEX_TYPE_MAX, "Unknown pin! cellname: %s, pinname %s \n", cellName.c_str(), pinName.c_str());
            if (pinIdx  == INDEX_TYPE_MAX)
            {
                WRN("%s unknown pin name. cell name %s pin name %s \n", __FUNCTION__, cellName.c_str(), pinName.c_str());
            }
            sig.addPinIdx(pinIdx);
        }
        /*------------------------------*/ 
        /* Technology-dependent         */
        /*------------------------------*/ 
        /// @brief get the spacing requirement between two cells
        /// @param index for cell 1
        /// @param index for cell 2
        /// @return A box object representing the spacing requirements. xyLo for cell 1 is left and bottm. xyHi for ... 2...
        Box<LocType> cellSpacing(IndexType cellIdx1, IndexType cellIdx2) const;
        /*------------------------------*/ 
        /* Supporting functions         */
        /*------------------------------*/ 
        /// @brief calcuate the total cell area
        /// @return the total cell area
        RealType calculateTotalCellArea() const;
        /// @brief calculate and return the HPWL
        /// @return HPWL
        LocType hpwl() const;
        LocType hpwlWithVitualPins() const;
        void expandCellToGridSize(LocType gridSize)
        {
           for (auto &cell : _cellArray)
           {
               cell.forceExtendToGrid(gridSize);
           }
        }
        bool checkSym();
        /*------------------------------*/ 
        /* Debug functions              */
        /*------------------------------*/ 
#ifdef DEBUG_DRAW
        /// @brief draw the cell blocks
        /// @param the filename for saving
        void drawCellBlocks(const std::string &name);
#endif //DEBUG_DRAW

    private:
        std::vector<Cell> _cellArray; ///< The cells of the placement problem
        std::vector<Net> _netArray; ///< The nets of the placement problem
        std::vector<Pin> _pinArray; ///< The pins of the placement problem
        std::vector<SymGroup> _symGroups; ///< The symmetric groups
        std::vector<ProximityGroup> _proximityGrps; ///< The proximity group constraints
        std::vector<SignalPath> _signalPaths; ///< The signal/current paths
        Tech _tech; ///< The tech information
        Parameters _para; ///< The parameters for the placement engine
};

inline RealType Database::calculateTotalCellArea() const
{
    RealType area = 0;
    for (const auto &cell : _cellArray)
    {
        area += cell.cellBBox().area();
    }
    return area;
}

inline Box<LocType> Database::cellSpacing(IndexType cellIdx1, IndexType cellIdx2) const
{
    const auto &cell1 = this->cell(cellIdx1);
    const auto &cell2 = this->cell(cellIdx2);
    Box<LocType> spacings = Box<LocType>(0, 0, 0, 0);
    for (IndexType layerIdx = 0; layerIdx < _tech.numLayers(); ++layerIdx)
    {
        bool cell1HasLayer = cell1.layerHasShape(layerIdx);
        bool cell2HasLayer = cell2.layerHasShape(layerIdx);
        if (! ( cell1HasLayer && cell2HasLayer))
        {
            continue;
        }
        bool layerHasSpacingRule = _tech.hasSpacingRule(layerIdx);
        if (!layerHasSpacingRule)
        {
            continue;
        }
        LocType spacingRule = _tech.spacingRule(layerIdx);
        // Calculate the layer shape to the cell boundry
        LocType cell1ToBoundXLo = cell1.bbox(layerIdx).xLo() - cell1.cellBBox().xLo();
        LocType cell1ToBoundXHi = cell1.cellBBox().xHi() - cell1.bbox(layerIdx).xHi();
        LocType cell1ToBoundYLo = cell1.bbox(layerIdx).yLo() - cell1.cellBBox().yLo();
        LocType cell1ToBoundYHi = cell1.cellBBox().yHi() - cell1.bbox(layerIdx).yHi();
        LocType cell2ToBoundXLo = cell2.bbox(layerIdx).xLo() - cell2.cellBBox().xLo();
        LocType cell2ToBoundXHi = cell2.cellBBox().xHi() - cell2.bbox(layerIdx).xHi();
        LocType cell2ToBoundYLo = cell2.bbox(layerIdx).yLo() - cell2.cellBBox().yLo();
        LocType cell2ToBoundYHi = cell2.cellBBox().yHi() - cell2.bbox(layerIdx).yHi();
        // cell 1 is left
        LocType xLo = cell1ToBoundXHi + spacingRule + cell2ToBoundXLo;
        // cell 1 is lower
        LocType yLo = cell1ToBoundYHi + spacingRule + cell2ToBoundYLo;
        // cell 1 is right
        LocType xHi = cell1ToBoundXLo + spacingRule + cell2ToBoundXHi;
        // cell 1 is higher
        LocType yHi = cell1ToBoundYLo + spacingRule + cell2ToBoundYHi;
        // Update the cell-wise spacing
        spacings.setXLo(std::max(spacings.xLo(), xLo));
        spacings.setYLo(std::max(spacings.yLo(), yLo));
        spacings.setXHi(std::max(spacings.xHi(), xHi));
        spacings.setYHi(std::max(spacings.yHi(), yHi));
    }
    return spacings;
}

PROJECT_NAMESPACE_END

#endif /// IDEAPLACE_DATABASE_H_
