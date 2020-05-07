/**
 * @file ParserConnection.h
 * @brief Parser for .connection file
 * @author Keren Zhu
 * @date 10/04/2019
 */

#ifndef IDEAPLACE_PARSER_CONNECTION_H_
#define IDEAPLACE_PARSER_CONNECTION_H_

#include "db/Database.h"

PROJECT_NAMESPACE_BEGIN

/// @class IDEAPLACE::PaserConnection
/// @brief parsing in the .connection file
class ParserConnection
{
    public:
        /// @brief constructor
        /// @param Reference to the placement engine database
        explicit ParserConnection(Database &db) : _db(db) {}
        /// @brief parsing in the file
        /// @param input file name
        /// @return if the parsing is successful
        bool read(const std::string &filename);
    private:
        Database &_db; ///< The placement engine database
};

PROJECT_NAMESPACE_END

#endif ///IDEAPLACE_PARSER_CONNECTION_H_
