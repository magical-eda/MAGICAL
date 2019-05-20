/**
 * @file   DisjointSet.h
 * @brief  A disjoint set structure and union-find utilities. 
 *
 * Please refer to [GeeksforGeeks implementation](http://www.geeksforgeeks.org/union-find-algorithm-set-2-union-by-rank/)
 * and Boost.DisjointSets for details
 * the union_set() and find_set() function can be used independently 
 *
 * @author Yibo Lin
 * @date   Sep 2015
 */

#ifndef LIMBO_CONTAINERS_DISJOINTSET_H
#define LIMBO_CONTAINERS_DISJOINTSET_H

#include <vector>
#include <algorithm>

/// namespace for Limbo
namespace limbo 
{ 
/// namespace for Limbo.Containers 
namespace containers 
{

/// @class limbo::containers::DisjointSet 
/// @brief simply used for scope control 
struct DisjointSet 
{
    /// @brief find the subset of an element e 
    /// @tparam SubsetHelperType subset helper that wraps elements and ranks 
    /// @param gp a function object to get parent of an element
    /// @param e current element 
    /// @return subset of element \a e 
    template <typename SubsetHelperType>
    static typename SubsetHelperType::element_type const& find_set(SubsetHelperType const& gp, typename SubsetHelperType::element_type const& e)
    {
        if (gp.get_parent(e) == e) // if e is its own parent, it reaches to the top set 
            return e;
        return find_set(gp, gp.get_parent(e));
    }

    /// @brief union two subsets represented by element e1 and e2 
    /// @tparam SubsetHelperType subset helper that wraps elements and ranks 
    /// @param gp a mutable function object to get and set parent of an element 
    /// @param e1 first element 
    /// @param e2 second element, after union_set() operation, e2 will become e1's parent 
    template <typename SubsetHelperType>
    static void union_set(SubsetHelperType& gp, typename SubsetHelperType::element_type const& e1, typename SubsetHelperType::element_type const& e2)
    {
        typename SubsetHelperType::element_type const& root1 = find_set(gp, e1);
        typename SubsetHelperType::element_type const& root2 = find_set(gp, e2);
        // set parent 
        if (gp.get_rank(root1) < gp.get_rank(root2))
            gp.set_parent(root1, root2);
        else if (gp.get_rank(root1) > gp.get_rank(root2))
            gp.set_parent(root2, root1);
        else 
        {
            gp.set_parent(root2, root1);
            gp.set_rank(root1, gp.get_rank(root1)+1);
        }
    }

    /// count the number of subsets 
    /// @tparam SubsetHelperType subset helper that wraps elements and ranks 
    /// @param gp a function object to get and set parent of an element 
    template <typename SubsetHelperType>
    static std::size_t count_sets(SubsetHelperType const& gp)
    {
        std::size_t count = 0;
        for (typename SubsetHelperType::element_type i = 0, ie = gp.size(); i != ie; ++i)
            if (gp.get_parent(i) == i) // top subset
                ++count;
        return count;
    }

    /// @brief a sample implementation of SubsetHelper that uses std::vector as the container. 
    /// It needs to store parent elements and ranks.  
    /// @tparam ElementType element type 
    /// @tparam RankType rank type 
    template <typename ElementType, typename RankType>
    struct SubsetHelper
    {
        /// @nowarn 
        typedef ElementType element_type;
        typedef RankType rank_type;
        /// @endnowarn

        std::vector<element_type>& vParent; ///< reference to parent array 
        std::vector<rank_type>& vRank; ///< reference to rank array 

        /// @brief constructor 
        /// @param vp array of parants 
        /// @param vr array of ranks 
        SubsetHelper(std::vector<element_type>& vp, std::vector<rank_type>& vr) 
            : vParent(vp)
            , vRank(vr)
        {
            // set initial parent to every vertex itself 
            for (unsigned int i = 0, ie = vParent.size(); i != ie; ++i)
                vParent[i] = i;
            std::fill(vRank.begin(), vRank.end(), 0);
        }
        /// @brief get parent element 
        /// @param e element 
        /// @return parent element 
        inline element_type const& get_parent(element_type const& e) const {return vParent[e];}
        /// @brief set parent element 
        /// @param e element 
        /// @param p parent element 
        inline void set_parent(element_type const& e, element_type const& p) {vParent[e] = p;}
        /// @brief get rank 
        /// @param e element 
        /// @return rank 
        inline rank_type const& get_rank(element_type const& e) const {return vRank[e];}
        /// @brief set rank 
        /// @param e element 
        /// @param r rank 
        inline void set_rank(element_type const& e, rank_type const& r) {vRank[e] = r;}
        /// @return number of elements 
        inline std::size_t size() const {return vParent.size();}
    };
};

}} // namespace limbo // namespace containers

#endif
