/**
 * @file   PrintMsg.h
 * @brief  functions for printing messages 
 * @author Yibo Lin
 * @date   Jan 2017
 */

#include <cstdarg>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifndef LIMBO_PREPROCESSOR_PRINTMSG_H
#define LIMBO_PREPROCESSOR_PRINTMSG_H

/// namespace for Limbo
namespace limbo 
{

/// @enum limbo::MessageType
/// @brief message type for print functions 
enum MessageType {
	kNONE = 0, 
	kINFO = 1, 
	kWARN = 2, 
	kERROR = 3, 
	kDEBUG = 4, 
    kASSERT = 5
};

/// forward declaration
/// @cond
int limboPrint(MessageType m, const char* format, ...);
int limboPrintStream(MessageType m, FILE* stream, const char* format, ...);
int limboVPrintStream(MessageType m, FILE* stream, const char* format, va_list args);
int limboSPrint(MessageType m, char* buf, const char* format, ...);
int limboVSPrint(MessageType m, char* buf, const char* format, va_list args);
int limboSPrintPrefix(MessageType m, char* prefix);
void limboPrintAssertMsg(const char* expr, const char* fileName, unsigned lineNum, const char* funcName, const char* format, ...);
void limboPrintAssertMsg(const char* expr, const char* fileName, unsigned lineNum, const char* funcName);
/// @endcond


/// @brief formatted print with prefix 
/// @param m prefix message 
/// @param format refer to the usage of printf 
/// @return 
inline int limboPrint(MessageType m, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	int ret = limboVPrintStream(m, stdout, format, args);
	va_end(args);

	return ret;
}

/// @brief formatted print with prefix to stream 
/// @param m prefix message 
/// @param stream 
/// @param format refer to the usage of printf 
/// @return 
inline int limboPrintStream(MessageType m, FILE* stream, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	int ret = limboVPrintStream(m, stream, format, args);
	va_end(args);

	return ret;
}

/// @brief formatted print with prefix to stream 
/// @param m prefix message 
/// @param stream 
/// @param format refer to the usage of printf 
/// @param args argument list 
/// @return 
inline int limboVPrintStream(MessageType m, FILE* stream, const char* format, va_list args)
{
	// print prefix 
    char prefix[8];
    limboSPrintPrefix(m, prefix);
    // merge prefix and format 
    char formatBuf[256];
    sprintf(formatBuf, "%s%s", prefix, format);

	// print message 
    // only print once to ensure multi-thread safe 
    int ret = vfprintf(stream, formatBuf, args);
	
	return ret;
}

/// @brief formatted print with prefix to buffer  
/// @param m prefix message 
/// @param buf buffer 
/// @param format refer to the usage of printf 
/// @return 
inline int limboSPrint(MessageType m, char* buf, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	int ret = limboVSPrint(m, buf, format, args);
	va_end(args);

	return ret;
}

/// @brief formatted print with prefix to buffer  
/// @param m prefix message 
/// @param buf buffer 
/// @param format refer to the usage of printf 
/// @param args argument list 
/// @return 
inline int limboVSPrint(MessageType m, char* buf, const char* format, va_list args)
{
	// print prefix 
    char prefix[8];
    limboSPrintPrefix(m, prefix);
	sprintf(buf, "%s", prefix);

	// print message 
	int ret = vsprintf(buf+strlen(prefix), format, args);
	
	return ret;
}

/// @brief print prefix message to buffer 
/// @param m prefix message 
/// @param prefix target buffer 
/// @return 
inline int limboSPrintPrefix(MessageType m, char* prefix)
{
	switch (m)
	{
		case kNONE:
            return sprintf(prefix, "%c", '\0');
		case kINFO:
			return sprintf(prefix, "(I) ");
		case kWARN:
            return sprintf(prefix, "(W) ");
		case kERROR:
            return sprintf(prefix, "(E) ");
		case kDEBUG:
            return sprintf(prefix, "(D) ");
        case kASSERT:
            return sprintf(prefix, "(A) ");
		default:
            return sprintf(prefix, "(?) ");
	}
    return 0;
}

/// @brief print message for assertion failure with additional message, see #limboAssertMsg(condition, args...) 
/// @param expr assertion condition  
/// @param fileName source code of file where the assertion is 
/// @param lineNum line number of the assertion 
/// @param funcName function name where the assertion is 
/// @param format refer to the usage of printf 
/// @return 
inline void limboPrintAssertMsg(const char* expr, const char* fileName, unsigned lineNum, const char* funcName, const char* format, ...)
{
    // construct message 
    char buf[1024];
    va_list args;
	va_start(args, format);
    vsprintf(buf, format, args);
    va_end(args);

    // print message 
    limboPrintStream(kASSERT, stderr, "%s:%u: %s: Assertion `%s' failed: %s\n", fileName, lineNum, funcName, expr, buf);
}

/// @brief print message for assertion failure without additional message, see #limboAssert(condition)
/// @param expr assertion condition  
/// @param fileName source code of file where the assertion is 
/// @param lineNum line number of the assertion 
/// @param funcName function name where the assertion is 
/// @return 
inline void limboPrintAssertMsg(const char* expr, const char* fileName, unsigned lineNum, const char* funcName)
{
    // print message
    limboPrintStream(kASSERT, stderr, "%s:%u: %s: Assertion `%s' failed\n", fileName, lineNum, funcName, expr);
}

} // namespace limbo

#endif
