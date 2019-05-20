#include "GridCellGR.h"

PROJECT_NAMESPACE_BEGIN


bool GridCellGR::dropBlockage()
{
    if (!_blocked)
    {
        _blocked = true;
        return true;
    }
    else 
    {
        return false;
    }
}

bool GridCellGR::eraseBlockage()
{
    if (_blocked)
    {
        _blocked = false;
        return true;
    }
    else
    {
        return false;
    }
}

PROJECT_NAMESPACE_END
