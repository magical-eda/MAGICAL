//
// Created by anoynmous on 09/04/2018
//

#ifndef AROUTER_TECHDATABASE_H_
#define AROUTER_TECHDATABASE_H_

#include <unordered_map>
#include "db/techDB/LefTechComponents.h"

// TechDatabase.h
// 
// Databse for technology information. Design rule, macro etc.
//

PROJECT_NAMESPACE_BEGIN


class TechDatabase
{
    public:
        explicit TechDatabase() = default;

        // Getters
        const std::string &                                     version() const                                          { return _version; }
        std::string &                                           version()                                                { return _version; }
        const std::string &                                     busbitChars() const                                      { return _busBitChars; }
        std::string &                                           busbitChars()                                            { return _busBitChars; }
        const std::string &                                     dividChar() const                                        { return _dividChar; }
        std::string &                                           dividChar()                                              { return _dividChar; }
        const std::vector<LefPropDef> &                         propDefs() const                                         { return _propDefs; }
        std::vector<LefPropDef> &                               propDefs()                                               { return _propDefs; }
        RealType                                                manufacturingGird() const                                { return _manufacturingGrid; }
        const std::string &                                     clearanceMeasure() const                                 { return _clearanceMeasure; }
        std::string &                                           clearanceMeasure()                                       { return _clearanceMeasure; }
        const LefUnit &                                         units() const                                            { return _units; }
        LefUnit &                                               units()                                                  { return _units; }
        const LefSite &                                         siteConfig() const                                       { return _siteConfig; }
        LefSite &                                               siteConfig()                                             { return _siteConfig; }
        const std::vector<std::pair<IndexType, IndexType>> &    layerTypeVec() const                                     { return _layerTypeVec; }
        std::vector<std::pair<IndexType, IndexType>> &          layerTypeVec()                                           { return _layerTypeVec; }
        const std::vector<std::pair<IndexType, IndexType>> &    accessLayerTypeVec() const                               { return _accessLayerTypeVec; }
        std::vector<std::pair<IndexType, IndexType>> &          accessLayerTypeVec()                                     { return _accessLayerTypeVec; }
        const std::vector<LefLayerCut> &                        cutLayers() const                                        { return _cutLayers; }
        std::vector<LefLayerCut> &                              cutLayers()                                              { return _cutLayers; }
        const std::vector<LefLayerRouting> &                    routingLayers() const                                    { return _routingLayers; }
        std::vector<LefLayerRouting> &                          routingLayers()                                          { return _routingLayers; }
        const std::vector<LefLayerMasterslice> &                mastersliceLayers() const                                { return _mastersliceLayers; }
        std::vector<LefLayerMasterslice> &                      mastersliceLayers()                                      { return _mastersliceLayers; }

        const std::unordered_map<std::string, IntType> &        mapLayerStr2LayerMaskId() const                              { return _mapLayerStr2LayerMaskId; }
        std::unordered_map<std::string, IntType> &              mapLayerStr2LayerMaskId()                                    { return _mapLayerStr2LayerMaskId; }
        const std::unordered_map<std::string, IntType> &        mapLayerStr2Idx() const                                  { return _mapLayerStr2Idx; }
        std::unordered_map<std::string, IntType> &              mapLayerStr2Idx()                                        { return _mapLayerStr2Idx; }
        const std::unordered_map<std::string, IntType> &        mapLayerStr2AccessIdx() const                            { return _mapLayerStr2AcessIdx; }
        std::unordered_map<std::string, IntType> &              mapLayerStr2AccessIdx()                                  { return _mapLayerStr2AcessIdx; } 

        // Setters
        void                                        setVersion(const std::string &version)                   { _version = version; }
        void                                        setBusBitChars(const std::string &busBitChars)           { _busBitChars = busBitChars; }
        void                                        setDividChar(const std::string &dividChar)               { _dividChar = dividChar; }
        void                                        setManufacturingGrid(RealType manufacturingGrid)         { _manufacturingGrid = manufacturingGrid; }
        void                                        setClearanceMeasure(const std::string &clearanceMeasure) { _clearanceMeasure = clearanceMeasure; }

        /*------------------------------*/ 
        /* Helpful query                */
        /*------------------------------*/ 
        IntType micro2DBU(RealType micro) const
        {
            RealType realDBU = micro * _units.databaseNumber;
            IntType dbu = static_cast<IntType>(realDBU);
            Assert(::klib::AlmostEquals<RealType>(dbu, realDBU));
            return dbu;
        }
        // Misc.
        /// @brief Get the mask id of the (masterslice + routing) layer idx
        IntType accessLayerMaskId(IndexType accessLayerIdx) 
        {
            const auto &pair = _accessLayerTypeVec.at(accessLayerIdx);
            if (pair.first == 1) { return _mapLayerStr2LayerMaskId.at(_routingLayers.at(pair.second).name); }
            else if (pair.first == 2) { return _mapLayerStr2LayerMaskId.at(_mastersliceLayers.at(pair.second).name); }
            else { Assert(false); return false; }
        }

        std::string accessLayerName(IndexType accessIdx) const 
        {
            const auto &pair = _accessLayerTypeVec.at(accessIdx);
            if (pair.first == 1) { return _routingLayers.at(pair.second).name; }
            else if (pair.first == 2) { return _mastersliceLayers.at(pair.second).name; }
            else { Assert(false); return ""; }
        }

        IntType accessLayer2LayerIdx(IndexType accessLayerIdx)
        {
            const auto &pair = _accessLayerTypeVec.at(accessLayerIdx);
            if (pair.first == 1) { return _mapLayerStr2Idx[_routingLayers.at(pair.second).name]; }
            else if (pair.first == 2) {  return _mapLayerStr2Idx[_mastersliceLayers.at(pair.second).name]; }
            else { Assert(false); return false; }
        }

        IntType layerIdx2MaskID(IntType layerIdx)
        {
            const auto &pair = _layerTypeVec.at(layerIdx);
            if (pair.first == 0) { return _mapLayerStr2LayerMaskId.at(_cutLayers.at(pair.second).name); }
            else if (pair.first == 1) { return _mapLayerStr2LayerMaskId.at(_routingLayers.at(pair.second).name); }
            else if (pair.first == 2) { return _mapLayerStr2LayerMaskId.at(_mastersliceLayers.at(pair.second).name); }
            else { AssertMsg(false, "type %d \n", pair.first); return false; }
        }

        /// @brief whether the layer in layetype is some layer cared in router (masterslice, routing, cut)
        bool isRouterLayer(IndexType layerIdx)
        {
            auto layertype = _layerTypeVec.at(layerIdx).first;
            if (layertype == 0 || layertype == 1 || layertype == 2)
            {
                return true;
            }
            return false;
        }


        // Unit conversions
        /// @brief database unit to micro meter
        RealType                                    du2um(IntType data) const { return data / static_cast<IntType>( _units.databaseNumber); }
        //IntType                                     um2du(RealType um) const  { return static_cast<IntType>(um * _units.databaseNumber); } 
    private:
        std::string _version = "";
        std::string _busBitChars = "";
        std::string _dividChar = "";
        std::vector<LefPropDef> _propDefs;
        RealType _manufacturingGrid = 0.0;
        std::string _clearanceMeasure = "";
        // bool _useMinSpacing = false; // whether the minimum spacing applies to obstruction geometries
        LefUnit _units;
        LefSite _siteConfig;
        std::vector<std::pair<IndexType, IndexType>> _layerTypeVec; // The order of layers. first = 0 CUT, 1 ROUTING, 2 MASTERSLICE, 3 OVERLAP second = idx of layers in the corresponding vectors  
        std::vector<std::pair<IndexType, IndexType>> _accessLayerTypeVec; // masterslices or routing layers, pins and metals can be on. no CUT, others similar to _layerTypeVec
        std::vector<LefLayerCut> _cutLayers;
        std::vector<LefLayerRouting> _routingLayers;
        std::vector<LefLayerMasterslice> _mastersliceLayers;


        // Map
        std::unordered_map<std::string, IntType> _mapLayerStr2LayerMaskId; ///< Map the name of the layer to the mask id
        std::unordered_map<std::string, IntType> _mapLayerStr2Idx; ///< Map the name of the layer to the index in _layerTypeVec
        std::unordered_map<std::string, IntType> _mapLayerStr2AcessIdx; ///< Map the name of the layer to the index in _accessLayerTypeVec
};


PROJECT_NAMESPACE_END
#endif ///AROUTER_TECHDATABASE_H_
