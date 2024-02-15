/**
 * @file ParserSyNet.h
 * @brief Parser for .symnet file
 * @author Keren Zhu
 * @date 03/09/2020
 */

#ifndef IDEAPLACE_PARSER_SYMNET_H_
#define IDEAPLACE_PARSER_SYMNET_H_

#include "db/Database.h"

PROJECT_NAMESPACE_BEGIN


/// @class IDEAPLACE::PaserSymNet
/// @brief Parsing in the .symnet files. This file is simply the name of nets that are symmetric or self-symmetric
class ParserSymNet
{
    public:
        /// @brief constructor
        /// @param Reference to the placement engine database
        explicit ParserSymNet(Database &db) : _db(db) {}
        /// @brief parsing in the file
        /// @param input file name
        /// @return if the parsing is successful
        bool read(const std::string &filename);
    private:
        /// @brief process the pairs read from the file, and save the result in the database
        bool processNamePair();
    private:
        Database &_db; ///< The placement engine database
        std::vector<std::pair<std::string, std::string>> _pairs; ///< The pairs for the input symmetric net
        std::vector<std::string> _selfs; ///< The self symmetry pairs
};
PROJECT_NAMESPACE_END

#endif
