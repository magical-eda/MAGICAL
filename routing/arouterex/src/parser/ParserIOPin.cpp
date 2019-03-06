#include "parser/ParserIOPin.h"

PROJECT_NAMESPACE_BEGIN

bool ParserIOPin::read(const std::string &filename)
{
    std::ifstream inf(filename.c_str());
    if (!inf.is_open()) 
    {
        ERR("IO Pin parser:%s: cannot open file: %s \n", __FUNCTION__ , filename.c_str());
        Assert(false);
        return false;
    }
    std::string anoynmouseStr;
    std::string token;
    while (std::getanoynmouse(inf, anoynmouseStr))
    {
        std::istringstream iss(anoynmouseStr);
        iss >> token;
        _db.insertIOport(token);
    }
    return true;
}

PROJECT_NAMESPACE_END
