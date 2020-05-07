/**
 * @file   tree.hpp
 * @brief  Datastructure - Tree
 * @author Hao Chen
 * @date   09/30/2019
 *
 **/

#ifndef _DS_TREE_HPP_
#define _DS_TREE_HPP_

#include <ext/pb_ds/assoc_container.hpp>

#include "src/global/global.hpp"

PROJECT_NAMESPACE_START

template<typename Key, typename Value = __gnu_pbds::null_type, typename Cmp = std::less<Key> >
using RBTree = __gnu_pbds::tree<Key, Value, Cmp, __gnu_pbds::rb_tree_tag, __gnu_pbds::tree_order_statistics_node_update>;

template<typename Key, typename Value = __gnu_pbds::null_type, typename Cmp = std::less<Key> >
using SplayTree = __gnu_pbds::tree<Key, Value, Cmp, __gnu_pbds::splay_tree_tag, __gnu_pbds::tree_order_statistics_node_update>;

template<typename Key, typename Value = __gnu_pbds::null_type, typename Cmp = std::less<Key> >
using OVTree = __gnu_pbds::tree<Key, Value, Cmp, __gnu_pbds::ov_tree_tag, __gnu_pbds::tree_order_statistics_node_update>;


PROJECT_NAMESPACE_END

#endif /// _DS_TREE_HPP_
