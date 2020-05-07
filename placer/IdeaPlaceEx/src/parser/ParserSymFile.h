/**
 * @file ParserSymFile.h
 * @brief Parser for .sym file
 * @author Keren Zhu
 * @date 12/16/2019
 */

#ifndef IDEAPLACE_PARSER_SYMFILE_H_
#define IDEAPLACE_PARSER_SYMFILE_H_

#include "db/Database.h"

PROJECT_NAMESPACE_BEGIN

/// @class IDEAPLACE::PaserSymFile
/// @brief parsing in the .sym file
class ParserSymFile
{
    public:
        /// @brief constructor
        /// @param Reference to the placement engine database
        explicit ParserSymFile(Database &db) : _db(db) {}
        /// @brief parsing in the file
        /// @param input file name
        /// @return if the parsing is successful
        bool read(const std::string &filename);
    private:
        /// @brief find cell index from name
        IndexType cellIdx(const std::string &cellName);
    private:
        Database &_db; ///< The placement engine database
};

PROJECT_NAMESPACE_END

#endif //IDEAPLACE_PARSER_SYMFILE_H_
