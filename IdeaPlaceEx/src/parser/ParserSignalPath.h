/**
 * @file ParserSignalPath.h
 * @brief Parser for .sigpath file
 * @author Keren Zhu
 * @date 02/24/2020
 */

#ifndef IDEAPLACE_PARSER_SIGNAL_PATH_H_
#define IDEAPLACE_PARSER_SIGNAL_PATH_H_

#include "db/Database.h"

PROJECT_NAMESPACE_BEGIN


/// @class IDEAPLACE::ParserSignalPath
/// @brief Parsing in the .signalpath file
class ParserSignalPath
{
    public:
        /// @brief constructor
        /// @param Reference to the placement engine database
        explicit ParserSignalPath(Database &db) : _db(db) {}
        /// @brief parsing in the file
        /// @param input file name
        /// @return if the parsing is successful
        bool read(const std::string &filename);
    private:
        Database &_db; ///< The placement engine database
};

PROJECT_NAMESPACE_END

#endif //IDEAPLACE_PARSER_SIGNAL_PATH_H_
