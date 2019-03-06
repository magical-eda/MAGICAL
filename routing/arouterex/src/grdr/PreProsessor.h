/**
 * @file ProcessTerminal.h
 * @brief Parse and process the terminal-related information
 * @author anoynmous anoynmous
 * @date 12/10/2018
 */

#ifndef AROUTER_PROCESS_TERMINAL_H_
#define AROUTER_PROCESS_TERMINAL_H_

#include "parser/ParseTerminalFile.h"
#include "db/Database.h"
#include "grdr/IntraCellRouter.h"

PROJECT_NAMESPACE_BEGIN

/// @class AROUTER::ProcessTerminal
/// @brief process the terminal-related stuff
class ProcessTerminal
{
    public:
        /// @brief constructor
        /// @param The database for the router
        explicit ProcessTerminal(Database &db) : _db(db), _intra(db), _parser(db.grDB().phyPinArray().size()) {} 
        /// @brief process the terminals
        /// @return if successful
        bool process();
    private:
        /// @brief process if no terminal file is given
        /// @return if successful
        bool processNoTerminalFile();
        /// @brief process with terminal file given
        /// @return if successful
        bool processTerminalFile();

    private:
        Database &_db; ///< The whole database for the router
        IntraCellRouter _intra; ///< Process the terminal and connecting the intracell connection
        ParseTerminalFile _parser; ///< The parser for the terminal file
};

/// @class AROUTER::ProcessSymNet
/// @brief process the symmetry nets. Find the corresponding pins for the pair of SymNet. NOTE: should be called after the terminals have been processed
class ProcessSymNet
{
    public:
        /// @brief Constructor
        /// @param The database for the router
        explicit ProcessSymNet(Database &db) : _db(db) {}
        /// @brief process the symmetry nets and find their corresponding pairs of pins
        /// @return if successful
        bool process();
    private:
        /// @brief process one single symmnet
        /// @param the index of the symmetry net
        /// @return if successful
        bool processOneSymNet(IndexType symNetIdx);
        /// @brief find the horizontal symmetric axis of a pair of symnet
        /// @param the index of the symmetry net
        /// @return if the symmetry is found
        bool configureHorizontalSym(IndexType symNetIdx);
        /// @brief find the vertical symmetric axis of a pair of symnet
        /// @param the index of the symmetry net
        /// @return if the symmetry is found
        bool configureVerticalSym(IndexType symNetIdx);
        /// @brief set the related parameters to the symmetry net
        /// @param first: the index of the symmetry net
        /// @param second: the type of symmetry
        /// @param third: the symmetry axis of the pair
        /// @param fourth: indices of terminals for net 1
        /// @param fifth: indices of terminals for net 2
        void configureSymNet(IndexType symNetIdx, SymType symType, LocType symAxis, const std::vector<IndexType> &terminals1, const std::vector<IndexType> &terminals2);
        /// @brief determine whether two terminal can be handled symmetricly with respect to a given symmetry axis
        /// @param first, second: two terminals. And the first one is regarded as major
        /// @param third: the symmtric axis
        /// @param fourth: the type of symmetry
        bool isTwoTerminalsSymmetric(const Terminal &term1, const Terminal &term2, LocType symAxis, SymType symType);

    private:
        Database &_db; ///< The database for the router
};

/// @class AROUTER::ProcessSelfSymNet
/// @brief process the symmetry nets. Find the corresponding pins for the pair of SymNet. NOTE: should be called after the terminals have been processed
class ProcessSelfSymNet
{
    public:
        /// @brief Constructor
        /// @param The database for the router
        explicit ProcessSelfSymNet(Database &db) : _db(db) {}
        /// @brief process the symmetry nets and find their corresponding pairs of pins
        /// @return if successful
        bool process();
    private:
        /// @brief process one single symmnet
        /// @param the index of the symmetry net
        /// @return if successful
        bool processOneSelfSymNet(IndexType symNetIdx);
        /// @brief find the horizontal symmetric axis of a pair of symnet
        /// @param the index of the symmetry net
        /// @return if the symmetry is found
        bool configureHorizontalSym(IndexType symNetIdx);
        /// @brief find the vertical symmetric axis of a pair of symnet
        /// @param the index of the symmetry net
        /// @return if the symmetry is found
        bool configureVerticalSym(IndexType symNetIdx);
        /// @brief set the related parameters to the symmetry net
        /// @param first: the index of the symmetry net
        /// @param second: the type of symmetry
        /// @param third: the symmetry axis of the pair
        /// @param fourth: indices of terminals for net 1
        /// @param fifth: indices of terminals for net 2
        void configureSelfSymNet(IndexType selfSymNetIdx, SymType symType, LocType symAxis, const std::vector<IndexType> &terminals);
        /// @brief determine whether two terminal can be handled symmetricly with respect to a given symmetry axis
        /// @param first, second: two terminals. And the first one is regarded as major
        /// @param third: the symmtric axis
        /// @param fourth: the type of symmetry
        bool isTwoTerminalsSymmetric(const Terminal &term1, const Terminal &term2, LocType symAxis, SymType symType);

    private:
        Database &_db; ///< The database for the router
};
PROJECT_NAMESPACE_END

#endif ///AROUTER_PROCESS_TERMINAL_H_
