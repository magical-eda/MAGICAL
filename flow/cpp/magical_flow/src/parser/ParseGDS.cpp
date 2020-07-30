#include "ParseGDS.h"
#include <iostream>

PROJECT_NAMESPACE_BEGIN

bool Parser::read(const std::string &fileName)
{
    GdsParser::GdsDB::GdsReader reader (_db);
    if (!reader(fileName))
        return false;
    std::string topCell = _db.cells().back().name();
    GdsCell top = _db.extractCell(topCell);
    // _layer.clear();
    for (const auto &object: top.objects())
    {
        ::GdsParser::GdsDB::GdsObjectHelpers()(object.first, object.second, ParseLayout(_layer, _techDB));
    }
    return true;
}

PROJECT_NAMESPACE_END
