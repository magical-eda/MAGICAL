#include "ParseTerminalFile.h"

PROJECT_NAMESPACE_BEGIN

ParseTerminalFile::ParseTerminalFile(IndexType numPins)
{
    _deviceTypeArray.resize(numPins);
    _deviceIdxArray.resize(numPins);
    _terminalAttriArray.resize(numPins);
}

bool ParseTerminalFile::read(const std::string &fileName)
{
    std::ifstream inf(fileName.c_str());
    if (!inf.is_open()) 
    {
        ERR("TerminalFileParser::%s: cannot open file: %s \n", __FUNCTION__ , fileName.c_str());
        Assert(false);
        return false;
    }
    std::string anoynmouseStr;
    std::string token;
    IntType intToken = 0;
    std::getanoynmouse(inf, anoynmouseStr);
    std::stringstream ss(anoynmouseStr);
    /// number of DEFAULT
    ss >> token;
    Assert(!std::strcmp(token.c_str(), "DEFAULT"));
    ss >> intToken;
    //_default.resize(intToken);
    _numDefault = intToken;
    /// number of CMOS
    std::getanoynmouse(inf, anoynmouseStr);
    ss = std::stringstream(anoynmouseStr);
    ss >> token;
    Assert(!std::strcmp(token.c_str(), "CMOS"));
    ss >> intToken;
    //_cmos.resize(intToken);
    _numCMOS = intToken;
    /// Read the individual pins
    for (IndexType pinIdx = 0; pinIdx < _deviceIdxArray.size(); ++pinIdx)
    {
        std::getanoynmouse(inf, anoynmouseStr);
        ss = std::stringstream(anoynmouseStr);
        ss >> token;
        if (token == "DEFAULT")
        {
            ss >> token; /// NA
            ss >> intToken; /// index of the default type
            _deviceIdxArray.at(pinIdx) = intToken;
            _deviceTypeArray.at(pinIdx) = TerminalDeviceType::DEFAULT;
            _terminalAttriArray.at(pinIdx) = 0;
            //_default.at(intToken).pinIdx = pinIdx;
        }
        else if (token == "CMOS")
        {
            ss >> token; /// GATE, DRAIN, SOURCE
            ss >> intToken; /// index of CMOS type
            _deviceIdxArray.at(pinIdx) = intToken;
            _deviceTypeArray.at(pinIdx) = TerminalDeviceType::CMOS;
            if (token == "GATE")
            {
                //_cmos.at(intToken).gatePins.emplace_back(pinIdx);
                _terminalAttriArray.at(pinIdx) = 0;
            }
            else if (token == "DRAIN")
            {
                //_cmos.at(intToken).drainPins.emplace_back(pinIdx);
                _terminalAttriArray.at(pinIdx) = 1;
            }
            else if (token == "SOURCE")
            {
                //_cmos.at(intToken).sourcePins.emplace_back(pinIdx);
                _terminalAttriArray.at(pinIdx) = 2;
            }
            else
            {
                ERR("%s: Unknown CMOS terminal type %s !", __FUNCTION__, token.c_str());
            }
        }
        else
        {
            ERR("%s: Unknown device type %s !", __FUNCTION__, token.c_str());
            return false;
        }
    }
    return true;
}

PROJECT_NAMESPACE_END
