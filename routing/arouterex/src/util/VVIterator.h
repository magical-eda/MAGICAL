//
// Created by anoynmous anoynmous on 04/07/2018
//

#ifndef ZKUTIL_VVITERATOR_H_
#define ZKUTIL_VVITERATOR_H_
#include <iterator>
#include <vector>
#include "../global/namespace.h"

/// ==============================================
/// vector<vector<foo>> iterator
///
/// a generic class for providing a 1-D iterator for a 2-D vector, in other words, vector<vector<>>. 
/// Implementation based on https://stackoverflow.com/questions/1784573/iterator-for-2d-vector
/// ==============================================


//An iterator over a vector of vectors
template<typename T>
class VVIterator : public std::iterator<std::bidirectional_iterator_tag, T> {
    public:
        VVIterator() = default;

        static VVIterator<T> begin(std::vector<std::vector<T>>& vv) {
            return VVIterator(&vv, 0, 0);
        }
        static VVIterator<T> end(std::vector<std::vector<T>>& vv) {
            return VVIterator(&vv, vv.size(), 0);
        }
        // ++prefix operator
        VVIterator& operator++() {
            // If we haven't reached the end of this sub-vector
            if (idxInner + 1 < (*vv)[idxOuter].size()) {
                // Go the next element
                ++idxInner;
            }
            else {
                // Otherwise skip to the next sub-vector, and keep skipping over empty ones until a non-empty one or the end
                // In other words, if vector[i] is empty, it will not been acessed
                do {
                    ++idxOuter;
                } while (idxOuter < (*vv).size() && (*vv)[idxOuter].empty());
                
                // Point to the beginning of sub-vector
                idxInner = 0;
            }
            return *this;
        }
        
        // --prefix operator
        VVIterator& operator--() {
            //If we haven't reached the beginning of the current sub-vector.
            if (idxInner > 0) {
                // Previous element in the sub-vector
                --idxInner;
            }
            else {
                // Otherwise skip to the previous sub-vector, and skip the empty ones
                // Empty sub-vector will not be iterated through
                do  {
                    --idxOuter;
                } while ((*vv)[idxOuter].empty());

                // Go to the end of the current sub-vector
                idxInner = (*vv)[idxOuter].size() - 1;
            }
            return *this;
        }
        
        // postfix ++ operator
        VVIterator operator++(int) {
            T retval = *this;
            ++(*this);
            return retval;
        }
        
        // postfix -- operator
        VVIterator operator--(int) {
            T retval = *this;
            --(*this);
            return retval;
        }

        bool operator==(const VVIterator& other) const
        {
            return other.vv == vv && other.idxOuter == idxOuter && other.idxInner == idxInner;
        }
        bool operator!=(const VVIterator&other) const {
          return !(*this == other);
        }
        const T& operator*() const {
          return *this;
        }
        T& operator*() {
          return (*vv)[idxOuter][idxInner];
        }
        const T& operator->() const {
          return *this;
        }
        T& operator->() {
          return *this;
        }

    private:
        VVIterator( std::vector<std::vector<T>>* _vv,
                    std::size_t _idxOuter,
                    std::size_t _idxInner )
            : vv(_vv), idxOuter(_idxOuter), idxInner(_idxInner) {}
        std::vector<std::vector<int>>* vv = nullptr;
        std::size_t idxOuter = 0;
        std::size_t idxInner = 0;
};
#endif //ZKUTIL_VVITERATOR_H_
