/**
 * @file   hash.hpp
 * @brief  Datastructure - Hash
 * @author Hao Chen
 * @date   09/30/2019
 *
 **/

#ifndef _DS_HASH_HPP_
#define _DS_HASH_HPP_

#include <ext/pb_ds/assoc_container.hpp>
#include <sparsehash/dense_hash_set>
#include <sparsehash/dense_hash_map>
#include <sparsehash/sparse_hash_set>
#include <sparsehash/sparse_hash_map>

#include "src/global/global.hpp"

PROJECT_NAMESPACE_START

// pbds
template<typename Key, typename Value, typename Hash_Fn = typename __gnu_pbds::detail::default_hash_fn<Key>::type>
using CCHashTable = __gnu_pbds::cc_hash_table<Key, Value, Hash_Fn>;

template<typename Key, typename Value, typename Hash_Fn = typename __gnu_pbds::detail::default_hash_fn<Key>::type>
using GPHashTable = __gnu_pbds::gp_hash_table<Key, Value, Hash_Fn>;

// google
template<typename Key, typename Hash_Fn = typename __gnu_pbds::detail::default_hash_fn<Key>::type>
using DenseHashSet = google::dense_hash_set<Key, Hash_Fn>;

template<typename Key, typename Value, typename Hash_Fn = typename __gnu_pbds::detail::default_hash_fn<Key>::type>
using DenseHashMap = google::dense_hash_map<Key, Value, Hash_Fn>;

template<typename Key, typename Hash_Fn = typename __gnu_pbds::detail::default_hash_fn<Key>::type>
using SparseHashSet = google::sparse_hash_set<Key, Hash_Fn>;

template<typename Key, typename Value, typename Hash_Fn = typename __gnu_pbds::detail::default_hash_fn<Key>::type>
using SparseHashMap = google::sparse_hash_map<Key, Value, Hash_Fn>;

PROJECT_NAMESPACE_END

#endif /// _DS_HASH_HPP_
