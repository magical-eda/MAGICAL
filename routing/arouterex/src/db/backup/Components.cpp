#include "Components.h"

PROJECT_NAMESPACE_BEGIN

myPin::myPin(IntType id, IntType x, IntType y):myNode(id, x, y)
{
    myNode(id, x, y);
    m_isDriverPin = false;
    m_isSteiner   = false;
    m_sink        = false;
    m_downCap     = 0.0;
}


myNet::myNet(IntType id): m_id(id)
{
    m_driver      = NULL;
    m_isSym       = false;
    m_symNet      = NULL;
    m_isRoute     = false;
    m_base        = true;
    m_route       = NULL;
    m_broute      = NULL;
    m_left        = INT_TYPE_MAX;
    m_right       = INT_TYPE_MIN;
    m_lower       = INT_TYPE_MAX;
    m_upper       = INT_TYPE_MIN;
    m_isLegal     = true;
    m_avgX        = 0.0;
    m_avgY        = 0.0;
}

myNet::~myNet()
{
    m_segments.clear();
}

bool
myEdge::modifyCap(const IntType layer, const IntType value)
{
    for (IntType i=0; i < m_layerIDs.size(); i++)
    {
        if (layer != m_layerIDs[i]) continue;
        m_layerCaps[i] = value;
        if (m_layerCaps[i] < 0) return false;
        return true;
    }
    return false;
}

bool
myEdge::increaseCap(const IntType layer, const IntType value)
{
    for (IntType i=0; i < m_layerIDs.size(); i++)
    {
        if (layer != m_layerIDs[i]) continue;
        m_layerCaps[i] += value;
        return true;
    }
    return false;
}

IntType
myEdge::minCap()
{
    IntType min_cap = INT_MAX;
    for (IntType i=0; i < m_layerCaps.size(); i++)
    {
        IntType cap = m_layerCaps[i];
        if (cap < min_cap)
        {
            min_cap = cap;
        }
    }
    return min_cap;
}

IntType
myEdge::getTotalCap()
{
    IntType tot_cap = 0;
    for (IntType i=0; i < m_layerCaps.size(); i++)
    {
        IntType cap = m_layerCaps[i];
        tot_cap += cap ;
    }

    return tot_cap;
}

IntType
myEdge::length()
{
    IntType length = 0;
    if (m_from->x() == m_to->x())
        length = m_from->y() - m_to->y();
    else
        length = m_from->x() - m_to->x();

    if (length < 0)
        length = 0 - length;
    return length;
}


PROJECT_NAMESPACE_END

