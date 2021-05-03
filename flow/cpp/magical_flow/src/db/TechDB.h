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

class SameLayerSpacingRule {
  public:
  SameLayerSpacingRule(IndexType layerIdx, LocType spacing)
    : _layerIdx(layerIdx), _spacing(spacing) {}
  IndexType layerIdx() const { return _layerIdx; }
  LocType spacing() const { return _spacing; }
  private:
  IndexType _layerIdx;
  LocType _spacing;
};

class InterLayerSpacingRule {
  public:
    InterLayerSpacingRule(IndexType layer1, IndexType layer2, LocType spacing)
      : _layer1(std::min(layer1, layer2)), _layer2(std::max(layer1, layer2)),
      _spacing(spacing) {}
    IndexType layer1() const { return _layer1; }
    IndexType layer2() const { return _layer2; }
    LocType spacing() const { return _spacing; }
  private:
  IndexType _layer1, _layer2;
  LocType _spacing;
};

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
        /// @brief Get is N-Well layer set
        bool isNwellLayerSet() const { return _nwellLayerIdx != INDEX_TYPE_MAX; }
        /// @brief Get the index of NW layer
        IndexType nwellLayerIdx() const { Assert(isNwellLayerSet()); return _nwellLayerIdx; }
        /// @brief Get the number of same layer spacing rules
        IndexType numSameLayerSpacingRules() const { return _sameLayerSpacingRules.size(); }
        /// @brief Get a same layer spacing rule
        const SameLayerSpacingRule &sameLayerSpacingRule(IndexType idx) const { return _sameLayerSpacingRules.at(idx); }
        /// @brief Get the number of inter layer spacing rules
        IndexType numInterLayerSpacingRules() const { return _interLayerSpacingRules.size(); }
        /// @brief Get a inter layer spacing rule
        const InterLayerSpacingRule &interLayerSpacingRule(IndexType idx) const { return _interLayerSpacingRules.at(idx); }
        /*------------------------------*/ 
        /* Setters                      */
        /*------------------------------*/ 
        /// @brief set the name of n-well layer
        void setNwellLayerName(const std::string &layerName) {
          auto layernameIter = _layerNameToDbLayer.find(layerName);
          Assert(layernameIter != _layerNameToDbLayer.end());
          _nwellLayerIdx = layernameIter->second;
        }
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
        /// @brief Add a same-layer spacing rule
        /// @param first: the layer name
        /// @param second: the required spacing rule
        void addSameLayerSpacingRule(const std::string &layerName, LocType spacing) {
          auto iter = _layerNameToDbLayer.find(layerName);
          AssertMsg(iter != _layerNameToDbLayer.end(), "MagicalFlow::TechDB::%s: Unknown layer %s \n", __FUNCTION__, layerName.c_str());
          _sameLayerSpacingRules.emplace_back(SameLayerSpacingRule(iter->second, spacing));
        }
        /// @brief Add a inter-layer spacing rule
        /// @param first: First layer name. The order of two layers does not matter
        /// @param second: Second layer name. The order of two layers does not matter
        /// @param third: The spacing requirement
        void addInterLayerSpacingRule(const std::string &layerName1, const std::string &layerName2, LocType spacing) {
          auto iter1 = _layerNameToDbLayer.find(layerName1);
          AssertMsg(iter1 != _layerNameToDbLayer.end(), "MagicalFlow::TechDB::%s: Unknown layer %s \n", __FUNCTION__, layerName1.c_str());
          auto iter2 = _layerNameToDbLayer.find(layerName2);
          AssertMsg(iter2 != _layerNameToDbLayer.end(), "MagicalFlow::TechDB::%s: Unknown layer %s \n", __FUNCTION__, layerName2.c_str());
          _interLayerSpacingRules.emplace_back(InterLayerSpacingRule(iter1->second, iter2->second, spacing));
        }
    private:
        TechUnit _units; ///< Units 
        std::vector<IndexType> _dbLayerToPdkLayer; ///< _dbLayerToLayerId[the index of layer in this project] = the layer ID in the PDK
        std::vector<IndexType> _pdkLayerToDbLayer; ///< _pdkLayerToDbLayer[PDK layer ID] = the index of layer in this project. The size of the vector is const defined in "global/constant.h"
        std::unordered_map<std::string, IndexType> _layerNameToDbLayer; ///< _layerNameToDbLayer["name of the layer"] = index of layer in db
        /* Placer rules */
        std::vector<SameLayerSpacingRule> _sameLayerSpacingRules; ///< The set of same layer spacing rules.
        std::vector<InterLayerSpacingRule> _interLayerSpacingRules; ///< The set of inter-layer spacing rules.
        IndexType _nwellLayerIdx = INDEX_TYPE_MAX; ///< The layer index of the N-well layer.
};

namespace PARSE
{
    /// @brief parser layer ID
    /// @param the input file name for layers(simple techfile)
    bool parseSimpleTechFile(const std::string &file, TechDB &techDB);
}
PROJECT_NAMESPACE_END

#endif //MAGICAL_FLOW_TECHDB_H_
