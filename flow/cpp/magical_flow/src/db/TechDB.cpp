#include "TechDB.h"
#include "parser/ParseSimpleTech.h"

PROJECT_NAMESPACE_BEGIN

namespace PARSE
{
    /// @brief parser layer ID
    /// @param the input file name for layers(simple techfile)
    bool parseSimpleTechFile(const std::string &file, TechDB &techDB)
    {
        if (!ParseSimpleTech(techDB).read(file))
        {
            return false;
        }
        return true;
    }
}
PROJECT_NAMESPACE_END
