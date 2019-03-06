#include "DatabaseDR.h"

PROJECT_NAMESPACE_BEGIN

bool DatabaseDR::init()
{
    /// init the rough grid
    _roughGrid.init();

    /// Init each net
    for (auto &net : this->nets())
    {
        net.init();
    }

    return true;
}


PROJECT_NAMESPACE_END
