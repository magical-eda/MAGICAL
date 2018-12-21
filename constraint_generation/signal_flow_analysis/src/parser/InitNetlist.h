/*! @file parser/InitNetlist.h
    @brief Parser to initialize netlist.
    @author Mingjie Liu
    @date 11/24/2018
    
    Input file should follow same format generated
    through scripts/create_init_obj.py. The python scripts
    take standardized hspice/spectre netlist files as
    inputs. Sample input files for c++ are under benchmarks. 
*/
#ifndef __INITNETLIST_H__
#define __INITNETLIST_H__

#include "db/Netlist.h"

PROJECT_NAMESPACE_BEGIN
/*! @class InitNetlist
    @brief InitNetlist class
*/
class InitNetlist
{
public:
/*! @brief Default Constructor */
    explicit InitNetlist() = default;
/*! @brief Constructor with initialization */
    explicit InitNetlist(Netlist & netlist) : _netlistDB(netlist) {}
    
/*! @brief Parse file and build netlist.

    Input files should follow same format generated
    through scripts/create_init_obj.py. Sample input 
    files for c++ are under benchmarks. The python 
    scripts take standardized hspice/spectre netlist 
    files as inputs. 

    @param filename Input file to parse.
*/
    bool read(const std::string &filename);
     
private:
    Netlist & _netlistDB; 
};


PROJECT_NAMESPACE_END

#endif
