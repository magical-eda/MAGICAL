/**
 * @file   pqueue.hpp
 * @brief  Datastructure - Priority queue
 * @author Hao Chen
 * @date   09/30/2019
 *
 **/

#ifndef _DS_PQUEUE_HPP_
#define _DS_PQUEUE_HPP_

#include <ext/pb_ds/priority_queue.hpp>

#include "src/global/global.hpp"

PROJECT_NAMESPACE_START

// !!!!!!!! MAX HEAP

template<typename T, typename Cmp = std::less<T> >
using PQueue = __gnu_pbds::priority_queue<T, Cmp>;

template<typename T, typename Cmp = std::less<T> >
using BinaryHeap = __gnu_pbds::priority_queue<T, Cmp, __gnu_pbds::binary_heap_tag>;

template<typename T, typename Cmp = std::less<T> >
using BinomialHeap = __gnu_pbds::priority_queue<T, Cmp, __gnu_pbds::binomial_heap_tag>;

template<typename T, typename Cmp = std::less<T> >
using RCBinomialHeap = __gnu_pbds::priority_queue<T, Cmp, __gnu_pbds::rc_binomial_heap_tag>;

template<typename T, typename Cmp = std::less<T> >
using PairingHeap = __gnu_pbds::priority_queue<T, Cmp, __gnu_pbds::pairing_heap_tag>;

template<typename T, typename Cmp = std::less<T> >
using ThinHeap = __gnu_pbds::priority_queue<T, Cmp, __gnu_pbds::thin_heap_tag>;

PROJECT_NAMESPACE_END

#endif /// _DS_PQUEUE_HPP_
