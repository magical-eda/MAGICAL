/**
 * @file ParserSymNet.h
 * @brief Parser for the Symmetric net specification
 * @author Keren Zhu
 * @date 12/15/2018
 */

#ifndef AROUTER_PARSER_SYMNET_H_
#define AROUTER_PARSER_SYMNET_H_

#include "db/Database.h"

PROJECT_NAMESPACE_BEGIN

/// @class AROUTER::ParserSymNet
/// @brief the parser for the symmetic net specification
class ParserSymNet
{
    public:
        explicit ParserSymNet(Database &db) : _db(db) {}

        /// @brief parse in the symmetic net specification
        bool read(const std::string &filename);

    private:
        /// @brief process the pairs read from the file, and save the result in the database
        bool processNamePair();

    private:
        Database &_db; ///< The database for the router
        std::vector<std::pair<std::string, std::string>> _pairs; ///< The pairs for the input symmetric net
        std::vector<std::string> _selfs; ///< The self symmetry pairs
};

PROJECT_NAMESPACE_END

#endif ///AROUTER_PARSER_SYMNET_H_
