#ifndef ZKUTIL_BASIC_LIB_H_
#define ZKUTIL_BASIC_LIB_H_
// ================================================================================ 
// Basic and commonly used function lib
// by Keren Zhu
// Data: 04/28/2018
// ================================================================================ 

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <cstdio>
#include <cmath>            // ceilf(), floorf()
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include "MsgPrinter.h"
#include "Assert.h"
#include "XYZ.h"
#include "util/thirdparty/AlmostEquals.h"
#include "global/namespace.h"
#include "global/type.h"

namespace klib 
{
    using IndexType  = PROJECT_NAMESPACE::IndexType;
    using IntType    = PROJECT_NAMESPACE::IntType;
    using RealType   = PROJECT_NAMESPACE::RealType;
    using MsgPrinter = PROJECT_NAMESPACE::MsgPrinter;
    // ================================================================================ 
    // STL extension
    // ================================================================================ 
    /// @breif Erase an element in the vector by swapping with the last element and resize
    /// @brief Ordering is not kept
    template<typename T>
    inline bool eraseElementInVec(std::vector<T> &vec, IndexType idx) 
    {
        IndexType vectorSize = vec.size();
        if (idx >= vectorSize) 
        {
            MsgPrinter::err("eraseElementInVec error: idx out of bound \n");
            Assert(false);
            return false;
        }
        // idx < vectorSize
        vec[idx] = std::move(vec.back());
        vec.resize(vectorSize - 1);
        return true;
    }

/// @breif Erase an element in the vector by swapping with the last element and resize
/// @brief Ordering is not kept
    template<typename T>
    inline bool eraseElementInVec(std::vector<T> &vec, IntType idx) 
    {
        if (idx < 0) 
        {
            MsgPrinter::err("eraseElementInVec error: idx out of bound \n");
            Assert(false);
            return false;
        }
        return eraseElementInVec(vec, static_cast<IndexType>(idx));
    }


    /// @brief Swap t1 and t2 if t1 > t2. Ie. put t1 and t2 in order
    template<typename T>
    inline void putTwoInOrder(T &t1, T &t2) 
    {
        if (t1 > t2) 
        {
            std::swap(t1, t2);
        }
    }


    template<typename T>
    inline void putTwoInOrder(T &t1, T &t2, std::function<bool(T&, T&)> const compare) 
    {
        if (compare(t1, t2)) 
        {
            std::swap(t1, t2);
        }
    }

    /// Calculate two usigned int difference
    inline IndexType idxDif(IndexType i1, IndexType i2) 
    {
        return i1 > i2? i1 - i2 : i2 - i1;
    }

    template<typename T>
    inline T absDif(T t1, T t2) 
    {
        return t1 > t2? t1 - t2 : t2 - t1;
    }

    inline bool idxAddOverflow(IndexType idx1, IndexType idx2)
    {
        IndexType sum = idx1 + idx2;
        return sum < idx1 || sum < idx2;
    }

    /// @brief move v1 elements into v2, and erase v1
    /// @param two reference to two vectors containing the same type of elements
    template<typename T>
    inline void moveVector(std::vector<T> &vFrom, std::vector<T> &vTo)
    {
        vTo.insert(vTo.end(), std::make_move_iterator(vFrom.begin()),
                              std::make_move_iterator(vFrom.end()));
        vFrom.clear();
    }

    /// @brief Stuct for null deleter
    namespace createSharedPtrDetails
    {
        struct null_deleter
        {
            void operator()(void const *) const
            {
            }
        };
    }

    /// @brief create a shared_ptr to a existing target
    template<typename T>
    inline std::shared_ptr<T> createSharedPtr(T &t)
    {
        return std::shared_ptr<T>(&t, createSharedPtrDetails::null_deleter());
    }

    // ================================================================================ 
    // Numerical functions
    // ================================================================================ 

    /// @breif Automatically decide whether to round a float/double. depending on the returning type.
    template<typename T>
    inline T autoRound(RealType rValue) 
    {
        return static_cast<T>(std::round(rValue));
    }

    template<>
    inline float autoRound(RealType rValue)
    {
        return rValue;
    }

    template<>
    inline double autoRound(RealType rValue)
    {
        return rValue;
    }


    template<typename T>
    inline T manhattanDistance(const PROJECT_NAMESPACE::XY<T> &t1, const PROJECT_NAMESPACE::XY<T> &t2)
    {
        return absDif(t1.x(), t2.x()) + absDif(t1.y(), t2.y());
    }

    template<typename T>
    inline T manhattanDistance(const PROJECT_NAMESPACE::XYZ<T> &t1, PROJECT_NAMESPACE::XYZ<T> &t2)
    {
        return absDif(t1.z(), t2.z()) + manhattanDistance(t1.xy(), t2.xy());
    }

    template<typename T>
    inline T manhattanDistance(const PROJECT_NAMESPACE::XYZ<T> t1, PROJECT_NAMESPACE::XYZ<T> t2)
    {
        return absDif(t1.z(), t2.z()) + manhattanDistance(t1.xy(), t2.xy());
    }

    /// @brief determine whether another XYZ is adjacent to this
    /// @param another XYZ
    /// @return true if two are adjacent to each other, false if not
    template<typename T>
    bool    xyzAdjacent(const PROJECT_NAMESPACE::XYZ<T> &lhs, const PROJECT_NAMESPACE::XYZ<T> &rhs) 
    {
        return (klib::absDif<T>(lhs.x(), rhs.x()) + klib::absDif<T>(lhs.y(), rhs.y()) + klib::absDif<T>(lhs.z(), rhs.z())) == 1;
    }
    // ================================================================================ 
    // Parser string
    // ================================================================================ 
    /// @brief trim left space of a string
    inline std::string & ltrim(std::string &s) 
    {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
    }

    /// @brief trim right space of a string
    inline std::string & rtrim(std::string &s) 
    {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
    }

    /// @brief trim spaces from both ends
    inline std::string & trim(std::string &s) 
    {
        return ltrim(rtrim(s));
    }

    /// @brief given path in str_path, output the design name
    /// @brief for example: with input "path1/path2/sparc_exu_alu", output "sparc_exu_alu"
    inline std::string parsePath2Name(std::string str_path) 
    {
        size_t pos = str_path.rfind('/');
        if (pos == std::string::npos) return str_path;

        return str_path.substr(pos+1);
    }

    /// @brief given path in str_path, output the design name
    /// @brief for example: with input "path1/path2/sparc_exu_alu", output "path1/path2"
    inline std::string parseFile2Path(std::string str_path) 
    {
        size_t pos = str_path.rfind('/');
        if (pos == std::string::npos) return str_path;

        return str_path.substr(0, pos);
    }

    /// @brief parse filename suffix
    inline std::string parseSuffix(std::string str) 
    {
        size_t pos = str.rfind('.');
        if (pos == std::string::npos) return str;
        return str.substr(pos+1);
    }

    /// @brief trim filename suffix
    inline std::string trimSuffix(std::string str) 
    {
        size_t pos = str.rfind('.');
        return str.substr(0, pos);
    }

    // ================================================================================ 
    // File-reading functions
    // ================================================================================ 

    /// @brief Check whether the file exists
    inline bool isFileExist(const char *fileName) 
    {
        std::ifstream infile(fileName);
        return infile.good();
    }

    /// @brief read from ifstream in, search target1 & target2,
    /// @brief if found, return true, and the whole line is stored in "str_line"
    /// @brief if reach EOF, return false
    inline bool readSearchUntil(std::ifstream &in, std::string &str_line, const std::string &target1, const std::string &target2="") 
    {
        bool skipTarget2 = target2.length() <= 0;
        while (!in.eof())
        {
            getline( in, str_line );  if (in.eof())          return false;
            if (std::string::npos != str_line.find(target1)) return true;
            if (skipTarget2) continue;
            if (std::string::npos != str_line.find(target2)) return true;
        } // while
        return false;
    }

} // namespce klib
#endif // ZKUTIL_BASIC_LIB_H_
