//
// Created by Keren on 04/12/2018
//

#include "MsgPrinter.h"
#include "Assert.h"

PROJECT_NAMESPACE_BEGIN

std::time_t MsgPrinter::_startTime = std::time(nullptr);
FILE* MsgPrinter::_screenOutStream = stderr;
FILE* MsgPrinter::_logOutStream = nullptr;
std::string MsgPrinter::_logFileName = "";

/// Converting enum type to std::string
std::string msgTypeToStr(MsgType msgType) 
{
    switch (msgType) 
    {
        case MsgType::INF:  return "INF"; break;
        case MsgType::WRN:  return "WRN"; break;
        case MsgType::ERR:  return "ERR"; break;
        case MsgType::DBG:  return "DBG"; break;
    }
    AssertMsg(false, "Unknown MsgType. \n");
}

/// Open a log file, all output will be stored in the log
void MsgPrinter::openLogFile(const std::string &logFileName) 
{
    if (_logOutStream != nullptr) 
    {
        fclose(_logOutStream);
        wrn("Current log file %s is forcibly closed\n", _logFileName.c_str());
    }

    _logFileName = logFileName;
    _logOutStream = fopen(logFileName.c_str(), "w");

    if (_logOutStream == nullptr) 
    {
        err("Cannot open log file %s\n", logFileName.c_str());
    }
    else {
        inf("Open log file %s\n", logFileName.c_str());
    }
}

/// Close current log file
void MsgPrinter::closeLogFile() 
{
    if (_logOutStream == nullptr) 
    {
        wrn("No log file is opened. Call to %s is ignored.\n", __func__);
    }
    else 
    {
        inf("Close log file %s.\n", _logFileName.c_str());
    }
}

/// Print information
void MsgPrinter::inf(const char* rawFormat, ...) 
{
    va_list args;
    va_start(args, rawFormat);
    print(MsgType::INF, rawFormat, args);
    va_end(args);
}

/// Print Warnings
void MsgPrinter::wrn(const char* rawFormat, ...) 
{
    va_list args;
    va_start(args, rawFormat);
    print(MsgType::WRN, rawFormat, args);
    va_end(args);
}

///Print errors
void MsgPrinter::err(const char* rawFormat, ...) 
{
    va_list args;
    va_start(args, rawFormat);
    print(MsgType::ERR, rawFormat, args);
    va_end(args);
}

/// Print debugging information
void MsgPrinter::dbg(const char* rawFormat, ...) 
{
    va_list args;
    va_start(args, rawFormat);
    print(MsgType::DBG, rawFormat, args);
    va_end(args);
}

/// Message printing kernel
void MsgPrinter::print(MsgType msgType, const char* rawFormat, va_list args) 
{
    /// Get the message type
    std::string type = "[" + msgTypeToStr(msgType);

    /// Get local time and elapsed time
    struct tm* timeInfo;
    std::time_t now = std::time(nullptr);
    timeInfo = localtime(&now);
    double elapsed = difftime(now, _startTime);

    /// Local time
    char locTime[32];
    strftime(locTime, 32, " %F %T ", timeInfo);

    /// Elapsed time
    char elpTime[32];
    sprintf(elpTime, "%5.0lf sec]  ", elapsed);

    // Combine all the strings together
    std::string format = type + std::string(locTime) + std::string(elpTime) + std::string(rawFormat);

    // print to log
    if (_logOutStream)
    {
        va_list args_copy;
        va_copy(args_copy, args);
        vfprintf(_logOutStream, format.c_str(), args_copy);
        va_end(args_copy);
        fflush(_logOutStream);
    }

    // print to screen
    if (_screenOutStream)
    {
        vfprintf(_screenOutStream, format.c_str(), args);
        fflush(_screenOutStream);
    }
}
PROJECT_NAMESPACE_END
