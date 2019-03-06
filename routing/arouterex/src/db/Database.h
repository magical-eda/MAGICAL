//
// Created by anoynmous on 09/12/2018
//

#ifndef AROUTER_DATABASE_H_
#define AROUTER_DATABASE_H_

#include "ProgArgs.h"
#include "db/drDB/DatabaseDR.h"
#include "db/techDB/TechDatabase.h"
#include "db/macroDB/MacroDatabase.h"
#include "db/grDB/DatabaseGR.h"
#include "db/SelfSymNet.h"
//#include "db/Terminal.h"
#include "dr/SearchInstruct.h"
#include <unordered_set>

/// ================================================================================ 
/// Database.h
///
/// Wrapped Database for tech, DR and GR
///
/// ================================================================================  

PROJECT_NAMESPACE_BEGIN

/// @brief AROUTER::Blockage
/// @brief the blockage in the design
class Blockage
{
    public:
        /// @brief default constructor
        explicit Blockage() = default;
        /// @brief constructor with layer and shape
        explicit Blockage(const Box<LocType> & rect, IndexType layer) : _rect(rect), _layer(layer) {}
        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get the blockage shape
        /// @return the shape of this blockage
        const Box<LocType> & rect() const { return _rect; }
        /// @brief get the blockage shape
        /// @return the shape of this blockage
        Box<LocType> & rect() { return _rect; }
        /// @brief get the layer of this blockage
        /// @return the layer of this blockage: masterslice + metal + cut
        IndexType layer() const { return _layer; }
        /*------------------------------*/ 
        /* Setters                      */
        /*------------------------------*/ 
        /// @brief set the layer for the blockage
        /// @param the index of the layer of the blockage (routing + masterslice + cut)
        void setLayer(IndexType layer) { _layer = layer; }
    private:
        Box<LocType> _rect; ///< THe rectangle shape of this blockage
        IndexType _layer = INDEX_TYPE_MAX; ///< layer of this blockage. layer: alllayer / access layer. metal + masterslice + cut
};

class Database
{
    public:
        explicit Database() = default;

        /*------------------------------*/ 
        /* Init                         */
        /*------------------------------*/ 
        /// @brief initialization of the SearchInstruct
        void initInstr()
        {
            _instr.init(*this);
        }

        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get _grDB
        /// @return the global routing database
        const DatabaseGR & grDB() const {  return _grDB; }
        /// @brief get _grDB
        /// @return the global routing database
        DatabaseGR & grDB() { return _grDB; }
        const DatabaseDR & drDB() const { return _drDB; }
        DatabaseDR & drDB() { return _drDB; }

        const TechDatabase & techDB() const { return _techDB; }
        TechDatabase & techDB() { return _techDB; }

        const MacroDatabase & macroDB() const { return _macroDB; }
        MacroDatabase & macroDB() { return _macroDB; }

        const ProgArgs &progArgs() const { return _progArgs; }

        const SearchInstruct & searchInstruct() const { return _instr; }

        SearchInstruct & searchInstruct() { return _instr; }

        /// @brief get the terminals
        /// @return the terminals/pin collections
        const std::vector<Terminal> & terminalArray() const { return _terminalArray; }
        /// @brief get the terminals
        /// @return the terminals/pin collections
        std::vector<Terminal> & terminalArray() { return _terminalArray; }
        /// @brief get one terminal
        /// @param the index of the terminal
        /// @return one terminal
        const Terminal & terminal(IndexType termIdx) const { return _terminalArray.at(termIdx); }
        /// @brief get one terminal
        /// @param the index of the terminal
        /// @return one terminal
        Terminal & terminal(IndexType termIdx) { return _terminalArray.at(termIdx); }
        /// @brief get number of terminals
        /// @return number of terminals
        IndexType numTerminals() const { return _terminalArray.size(); }
        /// @brief get the blockages
        /// @return the blockages
        const std::vector<Blockage> & blockageArray() const { return _blockageArray; }
        /// @brief get the blockages
        /// @return the blockages
        std::vector<Blockage> & blockageArray() { return _blockageArray; }
        /// @brief get one blockage
        /// @param the index of an blockage
        /// @return one blockage
        const Blockage & blockage(IndexType blockIdx) const { return _blockageArray.at(blockIdx); }
        /// @brief get one blockage
        /// @param the index of an blockage
        /// @return one blockage
        Blockage & blockage(IndexType blockIdx) { return _blockageArray.at(blockIdx); }
        /// @brief get the number of blockages
        /// @return the number of blockage
        IndexType numBlockages() const { return _blockageArray.size(); }
        /// @brief get the hpwls for the nets
        /// @return the hpwl for the nets
        IndexType  hpwl(IndexType netIdx) const { return _hpwls.at(netIdx); } 
        /// @brief get _symNetArray
        /// @return the vector of SymNet objects, which each contain the symmetry information of a symmetry net pair
        const std::vector<SymNet> &             symNetArray() const       { return _symNetArray; }
        /// @brief get _symNetArray
        /// @return the vector of SymNet objects, which each contain the symmetry information of a symmetry net pair
        std::vector<SymNet> &                   symNetArray()             { return _symNetArray; }
        /// @brief get the number of symnets
        /// @return the number symnets
        IndexType numSymNets() const { return _symNetArray.size(); }
        /// @brief get _selfSymNetArray
        /// @return the vector of SymNet objects, which each contain the symmetry information of a symmetry net pair
        const std::vector<SelfSymNet> &             selfSymNetArray() const       { return _selfSymNetArray; }
        /// @brief get _selfSymNetArray
        /// @return the vector of SymNet objects, which each contain the symmetry information of a symmetry net pair
        std::vector<SelfSymNet> &                   selfSymNetArray()             { return _selfSymNetArray; }
        /// @brief get the number of self symnets
        /// @return the number symnets
        IndexType numSelfSymNets() const { return _selfSymNetArray.size(); }
        /// @brief get a self symmetry net
        /// @param the index of the self symmetry net
        /// @return a self symmetry net
        const SelfSymNet & selfSymNet(IndexType selfSymNetIdx) const { return _selfSymNetArray.at(selfSymNetIdx); }
        /// @brief get a self symmetry net
        /// @param the index of the self symmetry net
        /// @return a self symmetry net
        SelfSymNet & selfSymNet(IndexType selfSymNetIdx) { return _selfSymNetArray.at(selfSymNetIdx); }
        /// @brief get a symmetic net
        /// @param the index of the symmetric net
        /// @return a symmetric net
        const SymNet & symNet(IndexType symNetIdx) const { return _symNetArray.at(symNetIdx); }
        /// @brief get a symmetic net
        /// @param the index of the symmetric net
        /// @return a symmetric net
        SymNet & symNet(IndexType symNetIdx) { return _symNetArray.at(symNetIdx); }
        /*------------------------------*/ 
        /* Setters                      */
        /*------------------------------*/ 
        /// @brief add a blockage
        /// @param first box defined the rectangle shape
        /// @param second the index of layer (router layer, all layers, masterslice +routing + cut)
        void addBlockage(const Box<LocType> &rect, IndexType layerIdx)
        {
            _blockageArray.emplace_back(Blockage(rect, layerIdx));
        }
        /// @brief calculate the hpwls
        void calculateHpwls();
        /// @brief insert string name for the io ports
        /// @param the string of the io port name
        void insertIOport(const std::string &port) { _iopins.insert(port); }
        /// @brief whether the port is contained in the io ports
        /// @param the name of the ports
        /// @return whether io port
        bool isIOport(const std::string &port) const { if (_iopins.find(port) == _iopins.end()) { return false; } return true; }
        /*------------------------------*/ 
        /*  Parse                       */
        /*------------------------------*/ 
        void parse(IntType argc, char** argv);

        /// @brief Override the routing setting (capacity etc.) with LEF tech
        /// @return if successful
        bool overrideGlobalSettingWithLef();
        /*------------------------------*/ 
        /*  Debug                       */
        /*------------------------------*/ 
        /// @brief draw all pins and terminals to gds
        bool drainAllPins(const std::string &filename);
    private:
        /// @brief decide on the side for the global routing grid width / height
        void overrideGlobalRoutingGrid();
        /// @brief recalculate the gridIdx for each pin (for global router)
        void recalculateGridIdx();


        ///////////////////////////
        /// Macro query
        //////////////////////////
        
        /* Via */
        ///@brief Query the LefFixedVia macro class from the macroDB
        ///@param ViaPlaced class
        ///@return const reference to the LefFixedVia macro class
        //const LefFixedVia & fixedViaMacro(const ViaPlaced &via) const { return  macroDB().fixedViaVec2D().at(via.nodeGridIdxLower().z()).at(via.fixedViaIdx()); }


        //////////////////////////////
        /// Detailed routing wrapper
        //////////////////////////////
    private:
        ProgArgs _progArgs;
        DatabaseDR _drDB;
        TechDatabase _techDB;
        MacroDatabase _macroDB;
        DatabaseGR _grDB;
        std::vector<Terminal> _terminalArray; ///< The terminals (pin collections)
        std::vector<Blockage> _blockageArray; ///< The blockage shapes array
        std::vector<LocType> _hpwls; ///< The hpwls for the nets
        std::vector<SymNet> _symNetArray; ///< The array of SymNets, the symmetry information about net pairs
        std::vector<SelfSymNet> _selfSymNetArray; ///< The array for the self symmetry net.
        std::unordered_set<std::string> _iopins; ///< The IO pins that need to be labeled
        SearchInstruct _instr; ///< The wrappers for the technology information
};


/*
 * -------------------------------------------------------------------------------- 
 * Detailed Routing
 * --------------------------------------------------------------------------------
 */

PROJECT_NAMESPACE_END
#endif ///AROUTER_DATABSE_H_
