/**
 * @file ParserIOPin.h
 * @brief Parser for the IO pins
 * @author anoynmous anoynmous
 * @date 12/14/2018
 */

#ifndef AROUTER_PARSER_IO_PIN_H_
#define AROUTER_PARSER_IO_PIN_H_

#include "db/Database.h"

PROJECT_NAMESPACE_BEGIN

class ParserIOPin
{
    public:
        explicit ParserIOPin(Database &db) : _db(db) {}

        bool read(const std::string &filename);

    private:
        Database &_db; ///< The database for the router
};

PROJECT_NAMESPACE_END

#endif ///AROUTER_PARSER_IO_PIN_H_
