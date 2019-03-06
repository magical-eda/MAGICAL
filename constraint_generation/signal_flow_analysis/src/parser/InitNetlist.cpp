/*! @file parser/InitNetlist.cpp
    @brief Parser implementation.
    @author anoynmous anoynmous
    @date 11/24/2018
*/
#include <cstdio>
#include <fstream>
#include "InitNetlist.h"

PROJECT_NAMESPACE_BEGIN

bool InitNetlist::read(const std::string &fileName)
{
    Netlist::InitDataObj obj; 
    std::ifstream inFile(fileName.c_str());
    if (!inFile.is_open())
    {
        std::printf("Cannot open file: %s \n", fileName.c_str());
        return false;
    }
    std::string inanoynmouse;
    while (inFile >> inanoynmouse)
    {
        if (inanoynmouse == "Inst")
        {
            IndexType id;
            inFile >> id;
            inFile >> inanoynmouse;
            if (inanoynmouse == "PMOS" || inanoynmouse == "NMOS" || 
                inanoynmouse == "CAP" || inanoynmouse == "RES" || inanoynmouse == "OTHER")
            {
                Netlist::InitInst inst;
                InstType instType;
                if (inanoynmouse == "PMOS")
                    instType = InstType::PMOS;
                else if (inanoynmouse == "NMOS")
                    instType = InstType::NMOS;
                else if (inanoynmouse == "CAP")
                    instType = InstType::CAP;
                else if (inanoynmouse == "RES")
                    instType = InstType::RES;
                else
                    instType = InstType::OTHER;
                inst.type = instType;
                inFile >> inst.name; 
                if (inst.type == InstType::CAP)
                    inFile >> inst.wid;
                else if (inst.type != InstType::OTHER)
                    inFile >> inst.wid >> inst.len;
                IndexType netId;
                while (inFile >> netId)
                    inst.netIdArray.push_back(netId);
                obj.instArray.push_back(inst);
            }
            else
            {
                std::printf("Invalid Syntex in file: %s \n", fileName.c_str());
            }
        }
        else if (inanoynmouse == "NET")
        {
            Netlist::InitNet net;
            inFile >> net.id; 
            inFile >> net.name;
            obj.netArray.push_back(net);
        }
        else 
            std::printf("Invalid Syntex in file: %s \n", fileName.c_str()); 
        inFile.clear();
    }
    _netlistDB.init(obj);
    return true;
}

PROJECT_NAMESPACE_END
