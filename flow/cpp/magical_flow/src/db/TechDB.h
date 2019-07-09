/**
 * @file TechDB.h
 * @brief Database for the needed technology information for hierarchical design
 * @author Keren Zhu
 * @date 07/05/2019
 */

#ifndef MAGICAL_FLOW_TECHDB_H_
#define MAGICAL_FLOW_TECHDB_H_

#include <unordered_map>
#include "global/global.h"

PROJECT_NAMESPACE_BEGIN

class TechUnit
{
    public:
        /// @brief default constrcutor
        explicit TechUnit() = default; 
        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get the dbu
        /// @return the dbu
        IntType dbu() const { return _dbu; }
        /// @brief get the GDSII user unit, 1nm per bit
        /// @return the GDSII user unit, 1nm per bit
        RealType dbuUU() const { return 1 / static_cast<RealType>(_dbu); }
        /// @brief get the GDSII database unit in meter, usually 1e-9
        /// @return the GDSII database unit
        RealType dbuM() const { return 1 / static_cast<RealType>(_dbu * 1000000); }
        /// @brief get the GDSII header
        /// @return the GDSII header
        IntType gdsHeader() const { return _gdsHeader; }
        /*------------------------------*/ 
        /* Setters                      */
        /*------------------------------*/ 
        /// @brief set the dbu
        /// @param the dbu
        void setDbu(IntType dbu) { _dbu = dbu; }
    private:
        IntType _dbu = 1000; ///< database unit / um. ie. 1 dbu imply 1 unit = 1nm
        IntType _gdsHeader = 600; ///< GDSII format header. usually 600 see: http://boolean.klaasholwerda.nl/interface/bnf/gdsformat.html#recordheader
};

/// @class MAGICAL_FLOW::TechDB
/// @brief The database for needed technology information
class TechDB
{
    public:
        /// @brief constructor
        explicit TechDB() { _pdkLayerToDbLayer.resize(RESERVED_LAYERS_NUMBER, INDEX_TYPE_MAX); }
        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get the units 
        /// @return the units 
        TechUnit & units() { return _units; }
        /// @brief get the number of layers
        /// @return the number of layers
        IndexType numLayers() const { return _dbLayerToPdkLayer.size(); }
        /// @brief convert db layer to techLayer
        /// @param the index of layer in db
        /// @return the layer ID in PDK
        IndexType dbLayerToPdk(IndexType dbLayerIdx) const { return _dbLayerToPdkLayer.at(dbLayerIdx); }
        /// @brief convert pdk layer to db layer
        /// @param the pdk layer
        /// @return the db layer
        IndexType pdkLayerToDb(IndexType pdkLayer) const { return _pdkLayerToDbLayer.at(pdkLayer); }
        /// @brief convert layer name to db layer
        /// @param the name of the layer
        /// @return the corresponding layer index in the db
        IndexType layerNameToIdx(const std::string &name) const { return _layerNameToDbLayer.at(name); }
        /*------------------------------*/ 
        /* Setters                      */
        /*------------------------------*/ 
        /*------------------------------*/ 
        /* Building the db              */
        /*------------------------------*/ 
        /// @brief push back a new layer
        /// @param first: the tech layer ID
        /// @param second: the name of the layer
        /// @return the index of the layer
        IndexType addNewLayer(IndexType techID, const std::string &name)
        {
            if (!_dbLayerToPdkLayer.empty())
            {
                AssertMsg(_dbLayerToPdkLayer.back() < techID, "TechDB::addNewLayer: the order of adding layers is wrong.");
            }
            IndexType index = _dbLayerToPdkLayer.size(); 
            _dbLayerToPdkLayer.emplace_back(techID);
            _pdkLayerToDbLayer.at(techID) = index;
            _layerNameToDbLayer[name] = index;
            return index;
        }
    private:
        TechUnit _units; ///< Units 
        std::vector<IndexType> _dbLayerToPdkLayer; ///< _dbLayerToLayerId[the index of layer in this project] = the layer ID in the PDK
        std::vector<IndexType> _pdkLayerToDbLayer; ///< _pdkLayerToDbLayer[PDK layer ID] = the index of layer in this project. The size of the vector is const defined in "global/constant.h"
        std::unordered_map<std::string, IndexType> _layerNameToDbLayer; ///< _layerNameToDbLayer["name of the layer"] = index of layer in db
};

namespace PARSE
{
    /// @brief parser layer ID
    /// @param the input file name for layers(simple techfile)
    bool parseSimpleTechFile(const std::string &file, TechDB &techDB);
}
PROJECT_NAMESPACE_END

#endif //MAGICAL_FLOW_TECHDB_H_
