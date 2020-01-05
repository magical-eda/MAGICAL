/**
 * @file IdeaPlaceEx.h
 * @brief wrapper of Everything
 * @author Keren Zhu
 * @date 10/02/2019
 */

#ifndef IDEAPLACE_IDEAPLACEEX_H_
#define IDEAPLACE_IDEAPLACEEX_H_

#include "db/Database.h"
/* Solver */
#include "place/CGLegalizer.h"
#include "place/NlpWnconj.h" //< This stupid package must be included here

PROJECT_NAMESPACE_BEGIN


/// @class IDEAPLACE::IdeaPlaceEx
/// @brief the main wrapper for the placement engine
class IdeaPlaceEx
{
    public:
        /// @brief default constructor
        explicit IdeaPlaceEx() = default;
        /// @brief the file-based input
        /// @param the system arguments
        /// @return if the parsing is successful
        bool parseFileBased(int argc, char** argv);
        /// @brief run the placement algorithm
        /// @return whether the placement is successful
        bool solve();
        /// @brief the file-based output
        /// @param the system arguments
        /// @return if the writing is successful
        bool outputFileBased(int argc, char** argv);
        /*------------------------------*/ 
        /* File-based input interface   */
        /*------------------------------*/ 
        /// @brief read in tech simple file
        /// @param the filename for the tech simple file 
        void readTechSimpleFile(const std::string &techFileSimple);
        /// @brief read pin file
        /// @param the filename for the pin file
        void readPinFile(const std::string &pinFile);
        /// @brief read connection file
        /// @param the connection file
        void readConnectionFile(const std::string &connectionFile);
        /// @brief read net weight file
        /// @param net weight file
        void readNetWgtFile(const std::string &netWgtFile);
        /// @brief read sym file
        /// @param sym file
        void readSymFile(const std::string &symFile);
        /// @brief read a gds for a cell
        /// @param first: the filename for gds layout file
        /// @param second: the cell index. if INDEX_TYPE_MAX, then will try to match the gds cellname
        void readGdsLayout(const std::string &gdsFile, IndexType cellIdx=INDEX_TYPE_MAX);
        /*------------------------------*/ 
        /* tech input interface         */
        /*------------------------------*/ 
        /// @brief add gds layer
        /// @param gds layer index
        /// @return the placement layer index
        IndexType addGdsLayer(IndexType gdsLayer)
        {
            return _db.tech().addGdsLayer(gdsLayer);
        }
        /// @brief finish adding gds layers
        void finishAddingGdsLayer()
        {
            _db.tech().initRuleDataStructure();
        }
        /*------------------------------*/ 
        /* Standard input interface     */
        /*------------------------------*/ 
        /// @brief add a new cell
        /// @return the index for that cell
        IndexType allocateCell()
        {
            IndexType cellIdx =  _db.allocateCell();
            _db.initCell(cellIdx);
            return cellIdx;
        }
        /// @brief add a new pin
        /// @param the index of cell this pin belonging to
        /// @return the index of the pin allocated
        IndexType allocatePin(IndexType cellIdx)
        {
            auto pinIdx =  _db.allocatePin();
            _db.pin(pinIdx).setCellIdx(cellIdx);
            _db.cell(cellIdx).addPin(pinIdx);
            return pinIdx;
        }
        /// @brief add pin shape for a pin
        /// @param the index of the pin
        /// @param xLo
        /// @param yLo
        /// @param xHi
        /// @param yHi
        void addPinShape(IndexType pinIdx, LocType xLo, LocType yLo, LocType xHi, LocType yHi)
        {
            _db.pin(pinIdx).shape().unionBox(Box<LocType>(xLo, yLo, xHi, yHi));
        }
        /// @brief allocate a new net
        /// @return the index for the net
        IndexType allocateNet()
        {
            return _db.allocateNet();
        }
        /// @brief add pin to net
        /// @param first: the pin index
        /// @param second: the net index
        void addPinToNet(IndexType pinIdx, IndexType netIdx)
        {
            _db.net(netIdx).addPin(pinIdx);
            _db.pin(pinIdx).addNetIdx(netIdx);
        }
        /// @brief set the net weight
        /// @param the index of the net
        /// @param the weight for the net
        void setNetWgt(IndexType netIdx, IntType weight)
        {
            _db.net(netIdx).setWeight(weight);
        }
        /// @brief allocate a symmetric group
        /// @return the index of the symmetric group
        IndexType allocateSymGrp()
        {
            return _db.allocateSymGrp();
        }
        /// @brief add a symmetric pair to a symmetric group
        /// @param first: symmetric group index
        /// @param second: the index of the first cell. The order of the cell indices does not matter.
        /// @param third: the index of the second cell. The order of the cell indices does not matter.
        void addSymPair(IndexType symGrpIdx, IndexType cellIdx1, IndexType cellIdx2)
        {
            _db.symGroup(symGrpIdx).addSymPair(cellIdx1, cellIdx2);
        }
        /// @brief add a self symmetric cell to a symmetric group
        /// @param first: the symmetric group index
        /// @param second: the cell index
        void addSelfSym(IndexType symGrpIdx, IndexType selfSymCellIdx)
        {
            _db.symGroup(symGrpIdx).addSelfSym(selfSymCellIdx);
        }
        /// @brief add cell layout shape 
        /// @param the index of the cell
        /// @param the index of the placer layer
        /// @param xLo
        /// @param yLo
        /// @param xHi
        /// @param yHi
        void addCellShape(IndexType cellIdx, IndexType placerLayer, LocType xLo, LocType yLo, LocType xHi, LocType yHi)
        {
            _db.cell(cellIdx).unionBBox(placerLayer, Box<LocType>(xLo, yLo, xHi, yHi));
        }
        /*------------------------------*/ 
        /* Standard output interface    */
        /*------------------------------*/ 
        /// @brief get the x coordinate for a cell
        /// @param  the cell index
        /// @return the x coordinate
        LocType xCellLoc(IndexType cellIdx)
        {
            return _db.cell(cellIdx).xLoc();
        }
        /// @brief get the y coordinate for a cell
        /// @param  the cell index
        /// @return the y coordinate
        LocType yCellLoc(IndexType cellIdx)
        {
            return _db.cell(cellIdx).yLoc();
        }
        /// @brief get the index of the cell based on name
        /// @param cell name  
        /// @return the cellIdx
        IndexType cellIdxName(const std::string name);
        /// @brief set cell name
        /// @param first cellIdx
        /// @param second cell name
        void setCellName(IndexType cellIdx, const std::string name) { _db.setCellName(cellIdx, name); }
        /// @brief return cell name
        /// @param first cellIdx
        /// @return cell name
        std::string cellName(IndexType cellIdx) { return _db.cell(cellIdx).name(); }
        /// @brief return number of cells
        /// @return number of cells
        IndexType numCells() { return _db.numCells(); }
        /// @brief get the index pin 
        /// @param cell index
        /// @param the pin index of the cell
        /// @return the pinIdx in database
        IndexType pinIdx(IndexType cellIdx, IndexType pinCellIdx) 
        { 
            if (pinCellIdx >= _db.cell(cellIdx).numPinIdx())
                return INDEX_TYPE_MAX;
            return _db.cell(cellIdx).pinIdx(pinCellIdx); 
        }
    protected:
        Database _db; ///< The placement engine database 
};

PROJECT_NAMESPACE_END

#endif ///IDEAPLACE_IDEAPLACEEX_H_
