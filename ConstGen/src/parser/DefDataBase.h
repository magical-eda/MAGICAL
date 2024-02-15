/*! @file parser/defparser.h
    @brief Parser to def file.
    @author Mingjie Liu
    @date 01/05/2020
    
    Adapted from limbo package
*/
#ifndef __DEFDATABASE_H__
#define __DEFDATABASE_H__

#include <iostream>
#include <fstream>
#include <limbo/parsers/def/adapt/DefDriver.h>
#include <global/type.h>

PROJECT_NAMESPACE_BEGIN
/*! @struct pins Class
    @brief personalized pin with assumptions
*/
struct pins
{
    std::string name;
    std::vector<IntType> bbox;
    // Assume that pin orient W and unit 2000
    // Dirty implementation
    pins(DefParser::Pin p)
    {
        name = p.pin_name;
        bbox.push_back((IntType)p.vLayer[0][1] - '0');
        if(p.orient == "W")
        {
            bbox.push_back((p.vBbox[0][1]+p.origin[0])/2);
            bbox.push_back((p.vBbox[0][0]+p.origin[1])/2);
            bbox.push_back((p.vBbox[0][3]+p.origin[0])/2);
            bbox.push_back((p.vBbox[0][2]+p.origin[1])/2);
        }
        else if(p.orient == "N")
        {
            bbox.push_back((p.vBbox[0][0]+p.origin[0])/2);
            bbox.push_back((p.vBbox[0][1]+p.origin[1])/2);
            bbox.push_back((p.vBbox[0][2]+p.origin[0])/2);
            bbox.push_back((p.vBbox[0][3]+p.origin[1])/2);
        }
        else if(p.orient == "S")
        {
            bbox.push_back((p.vBbox[0][0]+p.origin[0])/2);
            bbox.push_back((p.origin[1]-p.vBbox[0][3])/2);
            bbox.push_back((p.vBbox[0][2]+p.origin[0])/2);
            bbox.push_back((p.origin[1]-p.vBbox[0][1])/2);
        }
        else if(p.orient == "E")
        {
            bbox.push_back((p.vBbox[0][1]+p.origin[0])/2);
            bbox.push_back((p.origin[1]-p.vBbox[0][2])/2);
            bbox.push_back((p.vBbox[0][3]+p.origin[0])/2);
            bbox.push_back((p.origin[1]-p.vBbox[0][0])/2);
        }
    }
    void print()
    {
        std::cout << name << std::endl;
        for(IntType i : bbox)
            std::cout << i << " ";
        std::cout << std::endl;
    }
};

/*! @class DefDataBase Class
    @brief DefDataBase class
*/
class DefDataBase : public DefParser::DefDataBase
{
    public:
        std::vector<pins> Pins;
        DefDataBase() {}
        virtual void set_def_dividerchar(std::string const& token) {}
        virtual void set_def_busbitchars(std::string const& token) {}
        virtual void set_def_version(std::string const& token) {}
        virtual void set_def_design(std::string const& token) {}
        virtual void set_def_unit(int token) {}
        virtual void set_def_diearea(int t1, int t2, int t3, int t4) {}
        virtual void add_def_row(DefParser::Row const&) {}
        virtual void add_def_component(DefParser::Component const& c) {}
        virtual void resize_def_component(int token) {}
        virtual void add_def_pin(DefParser::Pin const& p) 
        {
            pins p_cur(p);
            Pins.push_back(p);
        }
        virtual void resize_def_pin(int token) {}
        virtual void add_def_net(DefParser::Net const& n) {}
        virtual void resize_def_net(int token) {}
        virtual void resize_def_blockage(int n) {}
        virtual void add_def_placement_blockage(std::vector<std::vector<int> > const& vBbox) {}
        virtual void end_def_design() {}
        void print()
        {
            for(pins p: Pins)
                p.print();
        }
};


PROJECT_NAMESPACE_END

#endif
