#ifndef ZKUTIL_ASSERT_H_
#define ZKUTIL_ASSERT_H_


#ifndef NDEBUG

#include <string>
#include <cstdio>
#include "MsgPrinter.h"
#include <cassert>


// Assert without message
#define Assert(cond) \
        do { \
            if (!(cond)) \
            { \
                char format[1024]; \
                sprintf(format, "Assertion failed at file %s line %d. \n                                    assert: %s\n", __FILE__, __LINE__, #cond); \
                MsgPrinter::err(format); \
                assert(0); \
                } \
            } while(false)

// Assert with message
#define AssertMsg(cond, msg, ...) \
        do { \
            if (!(cond)) \
            { \
                char prefix[1024]; \
                sprintf(prefix, "Assertion failed at file %s line %d.\n                                          assert: %s\n", __FILE__, __LINE__, #cond); \
                std::string format = std::string(prefix) + "                                          message: " + std::string(msg); \
                MsgPrinter::err(format.c_str(), ##__VA_ARGS__); \
                assert(0); \
            } \
        } while (false)
#else // NDEBUG
#define Assert(cond) \
        do { \
        } while(false)
#define AssertMsg(cond, msg, ...) \
        do { \
        } while (false)
#endif // NDEBUG
#endif // ZKTUIL_ASSERT_H_
