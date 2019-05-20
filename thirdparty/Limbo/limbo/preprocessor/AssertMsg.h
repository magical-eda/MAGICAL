/**
 * @file   AssertMsg.h
 * @brief  assertion with message 
 *
 * macro: assert_msg (deprecated)
 *
 * attribute: assertion with message. 
 *            if defined NO_LIMBO_ASSERTION, call assert in STL; 
 *            else call custom assertion
 *  
 * macro: limboAssertMsg, limboAssert
 *
 * attribute: assertion with or without message. 
 *            compared with assert in STL, it provides detailed information on the failures and positions 
 *
 * @author Yibo Lin
 * @date   Nov 2014
 */

#ifndef LIMBO_PREPROCESSOR_ASSERTMSG_H
#define LIMBO_PREPROCESSOR_ASSERTMSG_H

#include <iostream>
#include <limbo/preprocessor/Msg.h>

/// deprecated
/// @def assert_msg(condition, message)
/// @brief assertion with message 
/// 
/// I leave it here for backward compatibility. 
/// custom assertion with message. 
/// example usage: assert_msg(condition, "this is " << value << " for test");
#ifndef NO_LIMBO_ASSERTION
#define assert_msg(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << __FILE__ << ":" << __LINE__ << ": " << __PRETTY_FUNCTION__ << ": Assertion `" << #condition << "' failed: " << message << std::endl; \
            abort(); \
        } \
    } while (false)
#else
#define assert_msg(condition, message) \
	do { \
		assert(condition); \
	} while (false)
#endif

/// @def limboAssertMsg(condition, args...)
/// @brief custom assertion with message 
/// 
/// example usage: limboAssertMsg(a == 1, "this is %s", name);
#define limboAssertMsg(condition, args...) do {\
    if (!(condition)) \
    {\
        ::limbo::limboPrintAssertMsg(#condition, __FILE__, __LINE__, __PRETTY_FUNCTION__, args); \
        abort(); \
    }\
} while (false)

/// @def limboAssert(condition)
/// @brief custom assertion without message 
///
/// example usage: limboAssert(a == 1);
#define limboAssert(condition) do {\
    if (!(condition)) \
    {\
        ::limbo::limboPrintAssertMsg(#condition, __FILE__, __LINE__, __PRETTY_FUNCTION__); \
        abort(); \
    }\
} while (false)

/// namespace for Limbo
namespace limbo 
{

/// @brief static assertion 
template <bool>
struct StaticAssert;
/// @brief template specialization 
template <>
struct StaticAssert<true> {};

} // namespace limbo

/// @def limboStaticAssert(condition)
/// @brief compile time assertion 
#define limboStaticAssert(condition) {\
    limbo::StaticAssert<(condition) != 0>(); \
}

#endif 
