/*! @file main/ConstGen.cpp
    @brief ConstGen.cpp
    @author Mingjie LIu
    @date 03/18/2020

    Reimplementing interface for python.
*/
#ifndef __DEFPARSE__
#define __DEFPARSE__

#include <string>
#include <limbo/parsers/def/adapt/DefDriver.h>
#include "parser/DefDataBase.h"

PROJECT_NAMESPACE_BEGIN
/*! @class DefParse 
    @brief DefParse class
*/
class DefParse
{
public:
/*! @brief Default Constructor */
    explicit DefParse() = default;

/*! @brief Read def file throught parser*/
    void        readDefFile(const std::string & fileName) { DefParser::read(_db, fileName); }
    
/*! @brief Return number of pins. */
    IndexType   numPins() { return _db.Pins.size(); }

/*! @brief Return number of pin. */
    std::string pinName(IndexType id) { return _db.Pins.at(id).name; }

/*! @brief Return layer of pin. */
    IntType     pinLayer(IndexType id) { return _db.Pins.at(id).bbox[0]; }

/*! @brief Return bounding box of pin. */
    IntType     pinXLo(IndexType id) { return _db.Pins.at(id).bbox[1]; }
    IntType     pinYLo(IndexType id) { return _db.Pins.at(id).bbox[2]; }
    IntType     pinXHi(IndexType id) { return _db.Pins.at(id).bbox[3]; }
    IntType     pinYHi(IndexType id) { return _db.Pins.at(id).bbox[4]; }

private:
    DefDataBase _db;
};

PROJECT_NAMESPACE_END

#endif
