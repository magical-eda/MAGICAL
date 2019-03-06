/**
 * @file ParserIspd08.h
 * @brief Parser for ispd08 benchmarks
 * @author anoynmous anoynmous
 * @date 10/07/2018
 */

#ifndef AROUTER_PARSERISPD08_H_
#define AROUTER_PARSERISPD08_H_

#include "db/Database.h"

PROJECT_NAMESPACE_BEGIN

/// @class AROUTER::ISPDRawDb
/// @brief the raw data read from input ISPD'08 benchmarks
class ISPDRawDb 
{

    public:
        struct ispdPin 
        {
            ispdPin() = default;
            ispdPin(LocType x, LocType y, IndexType layer): _x(x), _y(y), _layer(layer) {}
            LocType               _x = 0;
            LocType               _y = 0;
            IndexType               _layer = 0;
            std::vector<XY<LocType>> _polygonPts;
        };
        struct ispdNet 
        {
            ispdNet() = default;
            ispdNet(const std::string &netName, IndexType netIdx, IndexType pinSize, IndexType minimumWidth)
                : _netName(netName), _netIdx(netIdx), _pinSize(pinSize), _minimumWidth(minimumWidth)
            { _pinArray.reserve(pinSize); }
            //ispdNet(ispdNet &&) = default;
            std::string             _netName = "";
            IndexType               _netIdx = 0;
            IndexType               _pinSize = 0;
            IndexType               _minimumWidth = 0;
            std::vector<ispdPin>    _pinArray;
            bool                    _isSymmetry = false;
            IndexType               _symNetIdx = 0;

        };
        struct ispdSymNet 
        {
            ispdSymNet() = default;
            ispdSymNet(std::string net1, std::string net2) :_net1(net1), _net2(net2) {}
            std::string             _net1 = "";
            std::string             _net2 = "";
        };
        struct ispdCapAdjust 
        {
            ispdCapAdjust() = default; 
            ispdCapAdjust(LocType x1, LocType y1, IndexType layer1, LocType x2, LocType y2, IndexType layer2, IntType adjustment)
                : _x1(x1), _y1(y1), _layer1(layer1), _x2(x2), _y2(y2), _layer2(layer2), _adjustment(adjustment) {}
            LocType               _x1 = 0;
            LocType               _y1 = 0;
            IndexType               _layer1 = 0;
            LocType               _x2 = 0;
            LocType               _y2 = 0;
            IndexType               _layer2 = 0;
            IndexType               _adjustment = 0;
        };
    public:
        explicit ISPDRawDb() = default;

        // Getters
        IndexType                               xGridSize() const                   { return _xGridSize; }
        IndexType                               xGridSize()                         { return _xGridSize; }
        IndexType                               yGridSize() const                   { return _yGridSize; }
        IndexType                               yGridSize()                         { return _yGridSize; }
        IndexType                               layerSize() const                   { return _layerSize;  }
        IndexType                               layerSize()                         { return _layerSize; }

        const std::vector<IndexType> &          verticalCapacity() const            { return _verticalCapacity; }
        std::vector<IndexType> &                verticalCapacity()                  { return _verticalCapacity; }
        const std::vector<IndexType> &          horizontalCapacity() const          { return _horizontalCapacity; }
        std::vector<IndexType> &                horizontalCapacity()                { return _horizontalCapacity; }
        const std::vector<IndexType> &          minimumWidth() const                { return _minimumWidth; }
        std::vector<IndexType> &                minimumWidth()                      { return _minimumWidth; }
        const std::vector<IndexType> &          minimumSpacing() const              { return _minimumSpacing; }
        std::vector<IndexType> &                minimumSpacing()                    { return _minimumSpacing; }
        const std::vector<IndexType> &          viaSpacing() const                  { return _viaSpacing; }
        std::vector<IndexType> &                viaSpacing()                        { return _viaSpacing; }

        LocType                               xLF() const                         { return _xLF; }
        LocType                               yLF() const                         { return _yLF; }
        LocType                               xHi() const                         { return _xHi; }
        LocType                               yHi() const                         { return _yHi; }
        LocType                               tileWidth() const                   { return _tileWidth; }
        LocType                               tileWidth()                         { return _tileWidth; }
        LocType                               tileHeight() const                  { return _tileHeight; }
        LocType                               tileHeight()                        { return _tileHeight; }

        IndexType                               netSize() const                     { return _netSize; }
        IndexType                               netSize()                           { return _netSize; }
        const std::vector<ispdNet> &            netArray() const                    { return _netArray; }
        std::vector<ispdNet> &                  netArray()                          { return _netArray; }

        IndexType                               symSize() const                     { return _symSize; }
        IndexType                               symSize()                           { return _symSize; }
        const std::vector<ispdSymNet> &         symNetArray() const                 { return _symNetArray; }
        std::vector<ispdSymNet> &               symNetArray()                       { return _symNetArray; }

        IndexType                               capAdjustSize() const               { return _capAdjustSize; }
        IndexType                               capAdjustSize()                     { return _capAdjustSize; }
        const std::vector<ispdCapAdjust> &      capAdjustArray() const              { return _capAdjustArray; }
        std::vector<ispdCapAdjust> &            capAdjustArray()                    { return _capAdjustArray; }

        

        // Setters
        void                                    setXGridSize(IndexType xGridSize)   { _xGridSize = xGridSize; }
        void                                    setYGridSize(IndexType yGridSize)   { _yGridSize = yGridSize; }
        void                                    setLayerSize(IndexType layerSize)   { _layerSize = layerSize; reserveLayerSize(layerSize); }
        void                                    setXLF(LocType xLF)               { _xLF = xLF; }
        void                                    setYLF(LocType yLF)               { _yLF = yLF; }
        void                                    setXHi(LocType xHi)               { _xHi = xHi; }
        void                                    setYHi(LocType yHi)               { _yHi = yHi; }
        void                                    setTileWidth(LocType tileWidth)   { _tileWidth = tileWidth; }
        void                                    setTileHeight(LocType tileHeight) { _tileHeight= tileHeight; }
        void                                    setNetSize(IndexType netSize)       { _netSize = netSize; _netArray.reserve(netSize); }
        void                                    setSymSize(IndexType symSize)       { _symSize = symSize; _symNetArray.reserve(symSize); }
        void                                    setCapSize(IndexType capSize)       { _capAdjustSize = capSize; _capAdjustArray.reserve(capSize); }

        // Vector access and operations
        void                                    reserveLayerSize(IndexType layerSize); // Reserve the vectors depending on the layer size
        void                                    addVerticalCapacity(IndexType cap)  { _verticalCapacity.emplace_back(cap); }
        void                                    addHorizontalCapacity(IndexType cp) { _horizontalCapacity.emplace_back(cp); }
        void                                    addMinimumWidth(IndexType width)    { _minimumWidth.emplace_back(width); }
        void                                    addMinimumSpacing(IndexType sp)     { _minimumSpacing.emplace_back(sp); }
        void                                    addViaSpacing(IndexType viaSp)      { _viaSpacing.emplace_back(viaSp); }
        void                                    addNet(const ispdNet &net)               { _netArray.emplace_back(net); }
        void                                    addSymNet(const ispdSymNet &sym)         { _symNetArray.emplace_back(sym); }
        void                                    addCapAdjust(ispdCapAdjust cap)     { _capAdjustArray.emplace_back(cap); }

    private:
        IndexType _xGridSize = 0; /// How many grids in horizontal direction
        IndexType _yGridSize = 0; ///...................vertical .......
        IndexType _layerSize = 0; /// number of routing layers

        std::vector<IndexType> _verticalCapacity;
        std::vector<IndexType> _horizontalCapacity;

        std::vector<IndexType> _minimumWidth;
        std::vector<IndexType> _minimumSpacing;
        std::vector<IndexType> _viaSpacing;
        
        IndexType _xLF = 0; ///  lower left corner (Minimum X)
        IndexType _yLF = 0; /// ............................Y
        IndexType _tileWidth = 0; /// The width of each tile
        IndexType _tileHeight = 0; /// The height of each tile
        LocType _xHi = 0; /// xHi for the design
        LocType _yHi = 0; /// yHi for the design

        IndexType _netSize = 0; /// number of nets
        std::vector<ispdNet> _netArray; /// An array of the nets

        ///Sym nets
        IndexType _symSize = 0; /// Number of sym nets
        std::vector<ispdSymNet> _symNetArray; /// Pairs of sym nets, containing the string of net name 

        /// Capacity adjustment
        IndexType _capAdjustSize = 0;
        std::vector<ispdCapAdjust> _capAdjustArray;


};

inanoynmouse void ISPDRawDb::reserveLayerSize(IndexType layerSize) 
{
    _verticalCapacity.reserve(layerSize);
    _horizontalCapacity.reserve(layerSize);
    _minimumWidth.reserve(layerSize);
    _minimumSpacing.reserve(layerSize);
    _viaSpacing.reserve(layerSize);
}

/// @class AROUTER::ParserIspd08
/// @brief parser for ISPD' 08 format benchmarks
class ParserIspd08
{
    public:
        /// @brief constructor
        /// @param db: reference to the database. fileName: the file name of the input benchmark
        explicit ParserIspd08(Database &db, const std::string fileName, IntType dbu) : _db(db), _fileName(fileName) 
        {
            Assert(dbu >= 1000);
            Assert(dbu % 1000 == 0);
            _scale = dbu / 1000;
        }
        /// @brief read the benchmark and store the data in the Database
        /// @return if the read successes
        bool read(); 
        /// @brief read the modified pin shape version of benchmark and store the data in the database
        /// @return if the read succuesses
        bool readPinShapeBenchmark();
    private:
        /// @brief read the file and store the data in _rawDB
        /// @return if the read sucesses
        bool readFile();
        /// @brief read the pin-shape version file and store the data in _rawDB
        /// @return if the read is successful
        bool readFilePinShapeBenchmark();
        /// @brief translate the _rawDB to _db
        /// @return if the translation sucesses
        bool trans2DB();
        Database &_db; ///< the reference to the Database
        ISPDRawDb _rawDB; ///< the unprocessed rawDatabase
        std::string _fileName; ///< the input benchmark
        IntType _benchmark = 0; ///< The type of benchmark being read
        IntType _scale = 1; ///< How much need to the scale for the unit. The benechmark assume the unit is in nm. Assume dbu > 1000, and is the multiplier of 1000
};
PROJECT_NAMESPACE_END

#endif ///AROUTER_PARSERISPD08_H_
