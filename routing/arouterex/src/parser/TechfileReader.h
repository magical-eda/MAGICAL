//
// Created by Keren on 09/13/2018
//

#ifndef AROUTER_TECHFILEREADER_H_
#define AROUTER_TECHFILEREADER_H_

#include "db/techDB/TechDatabase.h"


/// ================================================================================ 
/// TechfileReader.h
///
/// Read the techfile for Cadence virtuoso
///
/// Basically, for now, I need to read the techlayer in this file.
///
/// ================================================================================ 

PROJECT_NAMESPACE_BEGIN
namespace TechfileReader {
class TechfileReader
{
    public:
        explicit TechfileReader(TechDatabase &techDB) : _techDB(techDB) {}

        bool read(const std::string &fileName);

    private:
        bool processTechLayers(std::ifstream &inf);
    private:
        TechDatabase &_techDB;
};

bool readTechfile(const std::string &fileName, TechDatabase &techDB);
}
PROJECT_NAMESPACE_END

#endif /// AROUTER_TECHFILEREADER_H_
