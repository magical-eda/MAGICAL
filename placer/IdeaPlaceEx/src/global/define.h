/**
 * @file define.h
 * @brief Define the flags
 * @author Keren Zhu
 * @date 09/30/2019
 */

#ifndef IDEAPLACE_DEFINE_H_
#define IDEAPLACE_DEFINE_H_

//#define NODEBUG

#define DEBUG_DRAW
//#define DEBUG_GR
//#define DEBUG_LEGALIZE

#define MULTI_SYM_GROUP

//#define NODEBUG

#ifdef NODEBUG
#define AT(vec, idx) vec[idx]
#else
#define AT(vec, idx) vec.at(idx)
#endif /// NODEBUG

#define PIN_ASSIGN_IN_GP
#define PIN_ASSIGN_IN_LG

#endif /// IDEAPLACE_DEFINE_H
