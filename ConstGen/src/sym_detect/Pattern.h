/*! @file sym_detect/Pattern.h
    @brief Mosfet pair patterns.

    This class has been augmented also to handle passive
    device matching and self symmetry mosfets. The name
    remains as legacy.

    @author Mingjie Liu
    @date 11/24/2018
*/
#ifndef __PATTERN_H__
#define __PATTERN_H__

#include "db/Netlist.h"
#include "global/type.h"

PROJECT_NAMESPACE_BEGIN
/*! @class Pattern
    @brief Pattern class
*/
class Pattern
{
public:
/*! @brief Constructor 
    @param netlist Netlist for pattern search.
*/
    explicit Pattern() = default;
    explicit Pattern(Netlist & netlist) 
        : _netlist(netlist)
    {}

/*! @brief Set netlist database. */
    void    setNetlist(const Netlist & netlist) { _netlist = netlist; }
/*! @brief Return pattern for pair of mosfets.

    Valid patterns have same InstType.
    Currently they also have same size attribute.

    TODO Add ratio pair detection in future.
    @see MosPattern.
    @param mosId1 Id for mosfet.
    @param mosId2 Id for mosfet.
*/
    MosPattern          pattern(IndexType mosId1, IndexType mosId2) const;

private:
    Netlist &     _netlist;

private:
/*! @brief Return true if Inst pair have same InstType. */
    bool                matchedType(IndexType mosId1, IndexType mosId2) const;
/*! @brief Return true if Inst pair have same size attributes. */
    bool                matchedSize(IndexType mosId1, IndexType mosId2) const;
/*! @brief Return true if fits MosPattern::DIFF_SOURCE */
    bool                diffPairInput(IndexType mosId1, IndexType mosId2) const;
/*! @brief Return true if fits MosPattern::DIFF_CASCODE */
    bool                diffPairCascode(IndexType mosId1, IndexType mosId2) const;
/*! @brief Return true if fits MosPattern::CASCODE */
    bool                validPairCascode(IndexType mosId1, IndexType mosId2) const;
/*! @brief Return true if fits MosPattern::LOAD */
    bool                validPairLoad(IndexType mosId1, IndexType mosId2) const;
/*! @brief Return true if fits MosPattern::CROSS_CASCODE */
    bool                crossPairCascode(IndexType mosId1, IndexType mosId2) const;
/*! @brief Return true if fits MosPattern::CROSS_LOAD */
    bool                crossPairLoad(IndexType mosId1, IndexType mosId2) const;
/*! @brief Return true if Mosfets are forming matched cap. */
    bool                capMos(IndexType mosId1, IndexType mosId2) const;
};

PROJECT_NAMESPACE_END

#endif
