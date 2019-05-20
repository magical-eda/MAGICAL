//
// Created by Keren on 04/12/2018
//


#ifndef ZKUTIL_MSGPRINTER_H_
#define ZKUTIL_MSGPRINTER_H_

#include <cstdio> // FILE *, stderr
#include <string>
#include <ctime>
#include <cstdarg>
#include "global/namespace.h"

PROJECT_NAMESPACE_BEGIN

/// ================================================================================ 
/// MsgPrinter, used to manage printing message into the log or on the screen
/// Copied and implemented based on Wuxi's
/// ================================================================================ 


/// Enum type for message printing
enum class MsgType 
{
    INF,
    WRN,
    ERR,
    DBG
};

/// Function converting enum type to std::string
std::string msgTypeToStr(MsgType msgType);

/// Message printing class
class MsgPrinter 
{
    public:
        static void startTimer() { _startTime = std::time(nullptr); } // Cache start time
        static void screenOn()   { _screenOutStream = stderr; }       // Turn on screen printing
        static void screenOff()  { _screenOutStream = nullptr; }      // Turn off screen printing

        static void openLogFile(const std::string &file);
        static void closeLogFile();
        static void inf(const char *rawFormat, ...);
        static void wrn(const char *rawFormat, ...);
        static void err(const char *rawFormat, ...);
        static void dbg(const char *rawFormat, ...);

    private:
        static void print(MsgType msgType, const char *rawFormat, va_list args);

    private:
        static std::time_t   _startTime;
        static FILE *        _screenOutStream;  // Out stream for screen printing
        static FILE *        _logOutStream;     // Out stream for log printing
        static std::string   _logFileName;      // Current log file name
};

PROJECT_NAMESPACE_END

#endif // SHROUTER_MSGPRINTER_H_
 
