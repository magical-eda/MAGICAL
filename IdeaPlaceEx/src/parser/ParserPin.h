/**
 * @file ParserPin.h
 * @brief Parser for .pin file
 * @author Keren Zhu
 * @date 10/03/2019
 */

#ifndef IDEAPLACE_PARSER_PIN_H_
#define IDEAPLACE_PARSER_PIN_H_

#include "db/Database.h"

PROJECT_NAMESPACE_BEGIN

/// @class IDEAPLACE::ParserPin
/// @brief Parsing in the .pin file
class ParserPin
{
    public:
        /// @brief constructor
        /// @param Reference to the placement engine database
        explicit ParserPin(Database &db) : _db(db) {}
        /// @brief parsing in the file
        /// @param input file name
        /// @return if the parsing is successful
        bool read(const std::string &filename);
    private:
        Database &_db; ///< The placement engine database
};

PROJECT_NAMESPACE_END

#endif ///IDEAPLACE_PARSER_PIN_H_
