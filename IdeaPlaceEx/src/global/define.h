/**
 * @file define.h
 * @brief Define the flags
 * @author Keren Zhu
 * @date 09/30/2019
 */

#ifndef IDEAPLACE_DEFINE_H_
#define IDEAPLACE_DEFINE_H_

//#define NODEBUG

//#define DEBUG_DRAW
//#define DEBUG_GR
//#define DEBUG_LEGALIZE

#define MULTI_SYM_GROUP

#ifdef NODEBUG
#define AT(vec, idx) vec[idx]
#else
#define AT(vec, idx) vec.at(idx)
#endif /// NODEBUG

#endif /// IDEAPLACE_DEFINE_H
