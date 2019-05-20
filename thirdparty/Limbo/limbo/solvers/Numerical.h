/**
 * @file   Numerical.h
 * @author Yibo Lin
 * @date   Mar 2017
 * @brief  numerical functions for linear algebra 
 */
#ifndef LIMBO_SOLVERS_NUMERICAL_H
#define LIMBO_SOLVERS_NUMERICAL_H

#include <limbo/solvers/Solvers.h>
#if MKL == 1
#include <mkl.h>
#endif

/// namespace for Limbo 
namespace limbo 
{
/// namespace for Limbo.Solvers 
namespace solvers 
{

#if MKL == 1
/// @brief \f$ y = a \cdot x+y \f$ 
/// @tparam T data type 
/// @param n dimension 
/// @param a constant 
/// @param x vector 
/// @param y output vector 
template <typename T>
inline void mklaxpy(unsigned int n, T a, T const* x, T* y);
/// @brief template specialization for float 
/// @param n dimension 
/// @param a constant 
/// @param x vector 
/// @param y output vector 
template <>
inline void mklaxpy<float>(unsigned int n, float a, float const* x, float* y) 
{
    cblas_saxpy(n, a, x, 1, y, 1);
}
/// @brief template specialization for double 
/// @param n dimension 
/// @param a constant 
/// @param x vector 
/// @param y output vector 
template <>
inline void mklaxpy<double>(unsigned int n, double a, double const* x, double* y) 
{
    cblas_daxpy(n, a, x, 1, y, 1);
}

/// @brief \f$ y = a A x + b y \f$
/// @tparam T data type 
/// @tparam MatrixType sparse matrix type in CSR format 
/// @param a constant 
/// @param A matrix 
/// @param x vector 
/// @param b constant 
/// @param y output vector 
template <typename T, typename MatrixType>
inline void mklAxPlusy(T a, MatrixType const& A, T const* x, T b, T* y); 
/// @brief template specialization for float 
/// @param a constant 
/// @param A matrix 
/// @param x vector 
/// @param b constant 
/// @param y output vector 
template <>
inline void mklAxPlusy<float, MatrixCSR<float, int, 1> >(float a, MatrixCSR<float, int, 1> const& A, float const* x, float b, float* y)
{
    mkl_scsrmv("N", &A.numRows, &A.numColumns, &a, "G", A.vElement, A.vColumn, A.vRowBeginIndex, A.vRowBeginIndex+1, x, &b, y);
}
/// @brief template specialization for double 
/// @param a constant 
/// @param A matrix 
/// @param x vector 
/// @param b constant 
/// @param y output vector 
template <>
inline void mklAxPlusy<double, MatrixCSR<double, int, 1> >(double a, MatrixCSR<double, int, 1> const& A, double const* x, double b, double* y)
{
    mkl_dcsrmv("N", &A.numRows, &A.numColumns, &a, "G", A.vElement, A.vColumn, A.vRowBeginIndex, A.vRowBeginIndex+1, x, &b, y);
}

/// @brief \f$ y = a A^T x + b y \f$
/// @tparam T data type 
/// @tparam MatrixType sparse matrix type in CSR format 
/// @param a constant 
/// @param A matrix 
/// @param x vector 
/// @param b constant 
/// @param y output vector 
template <typename T, typename MatrixType>
inline void mklATxPlusy(T a, MatrixType const& A, T const* x, T b, T* y);
/// @brief template specialization for float 
/// @param a constant 
/// @param A matrix 
/// @param x vector 
/// @param y output vector 
template <>
inline void mklATxPlusy<float, MatrixCSR<float, int, 1> >(float a, MatrixCSR<float, int, 1> const& A, float const* x, float b, float* y)
{
    mkl_scsrmv("T", &A.numRows, &A.numColumns, &a, "G", A.vElement, A.vColumn, A.vRowBeginIndex, A.vRowBeginIndex+1, x, &b, y);
}
/// @brief template specialization for double 
/// @param a constant 
/// @param A matrix 
/// @param x vector 
/// @param y output vector 
template <>
inline void mklATxPlusy<double, MatrixCSR<double, int, 1> >(double a, MatrixCSR<double, int, 1> const& A, double const* x, double b, double* y)
{
    mkl_dcsrmv("T", &A.numRows, &A.numColumns, &a, "G", A.vElement, A.vColumn, A.vRowBeginIndex, A.vRowBeginIndex+1, x, &b, y);
}
/// @brief compute dot product \f$ x^T y \f$
/// @tparam T data type 
/// @param n dimension 
/// @param x vector 
/// @param y vector 
/// @return dot product 
template <typename T>
inline T mkldot(unsigned int n, T const* x, T const* y);
/// @brief template specialization for float 
/// @param n dimension 
/// @param x vector 
/// @param y vector 
/// @return dot product 
template <>
inline float mkldot(unsigned int n, float const* x, float const* y)
{
    return cblas_sdot(n, x, 1, y, 1);
}
/// @brief template specialization for double 
/// @param n dimension 
/// @param x vector 
/// @param y vector 
/// @return dot product 
template <>
inline double mkldot(unsigned int n, double const* x, double const* y)
{
    return cblas_ddot(n, x, 1, y, 1);
}

/// @brief copy vector 
/// @param T data type 
/// @param n dimension 
/// @param x source vector 
/// @param y target vector 
template <typename T>
inline void mklcopy(unsigned int n, T const* x, T* y);
/// @brief template specialization for float 
/// @param n dimension 
/// @param x source vector 
/// @param y target vector 
template <>
inline void mklcopy(unsigned int n, float const* x, float* y)
{
    cblas_scopy(n, x, 1, y, 1);
}
/// @brief template specialization for double 
/// @param n dimension 
/// @param x source vector 
/// @param y target vector 
template <>
inline void mklcopy(unsigned int n, double const* x, double* y)
{
    cblas_dcopy(n, x, 1, y, 1);
}
#endif

/// @brief \f$ y = a \cdot x+y \f$ 
/// @tparam T data type of \a a, \a y
/// @tparam V data type of \a x
/// @param n dimension 
/// @param a constant 
/// @param x vector 
/// @param y output vector 
template <typename T, typename V>
inline void axpy(unsigned int n, T a, V const* x, T* y) 
{
#if MKL == 1
    mklaxpy(n, a, x, y);
#else 
    for (unsigned int i = 0; i < n; ++i)
        y[i] += a*x[i];
#endif
}

/// @brief \f$ y = a A x + y \f$
/// @tparam T data type of \a a, \a y
/// @tparam V data type of \a x
/// @tparam MatrixType sparse matrix type in CSR format 
/// @param a constant 
/// @param A matrix 
/// @param x vector 
/// @param y output vector 
template <typename T, typename V, typename MatrixType>
inline void AxPlusy(T a, MatrixType const& A, V const* x, T* y) 
{
    // y = a A x + y
#if MKL == 1
    mklAxPlusy(a, A, x, (T)1, y);
#else 
    for (typename MatrixType::index_type i = 0; i < A.numRows; ++i)
    {
        for (typename MatrixType::index_type k = A.vRowBeginIndex[i]; k < A.vRowBeginIndex[i+1]; ++k)
        {
            typename MatrixType::index_type kk = k-MatrixType::s_startingIndex;
#ifdef DEBUG_NUMERICAL
            limboAssert(A.vColumn[kk]-MatrixType::s_startingIndex < A.numColumns && kk < A.numElements); 
#endif
            y[i] += a*A.vElement[kk]*x[A.vColumn[kk]-MatrixType::s_startingIndex];
        }
    }
#endif
}
/// @brief \f$ y = a A^T x + y \f$
/// @tparam T data type of \a a, \a y
/// @tparam V data type of \a x
/// @tparam MatrixType sparse matrix type in CSR format 
/// @param a constant 
/// @param A matrix 
/// @param x vector 
/// @param y output vector 
template <typename T, typename V, typename MatrixType>
inline void ATxPlusy(T a, MatrixType const& A, V const* x, T* y) 
{
    // y = a A^T x + y
#if MKL == 1
    mklATxPlusy(a, A, x, (T)1, y);
#else 
    for (typename MatrixType::index_type i = 0; i < A.numRows; ++i)
    {
        for (typename MatrixType::index_type k = A.vRowBeginIndex[i]; k < A.vRowBeginIndex[i+1]; ++k)
        {
            typename MatrixType::index_type kk = k-MatrixType::s_startingIndex;
#ifdef DEBUG_NUMERICAL
            limboAssert(A.vColumn[kk]-MatrixType::s_startingIndex < A.numColumns && kk < A.numElements); 
#endif
            y[A.vColumn[kk]-MatrixType::s_startingIndex] += a*A.vElement[kk]*x[i];
        }
    }
#endif
}
/// @brief compute dot product \f$ x^T y \f$
/// @tparam T data type 
/// @param n dimension 
/// @param x vector 
/// @param y vector 
/// @return dot product 
template <typename T>
inline T dot(unsigned int n, T const* x, T const* y) 
{
    T result = 0; 
#if MKL == 1
    result = mkldot(n, x, y);
#else 
    for (unsigned int i = 0; i < n; ++i)
        result += x[i]*y[i];
#endif 
    return result; 
}
/// @brief copy vector 
/// @tparam T data type 
/// @param n dimension 
/// @param x source vector 
/// @param y target vector 
template <typename T>
inline void vcopy(unsigned int n, T const* x, T* y) 
{
    // y = x 
#if MKL == 1
    mklcopy(n, x, y);
#else 
    std::copy(x, x+n, y); 
#endif
}

} // namespace solvers 
} // namespace limbo 

#endif
