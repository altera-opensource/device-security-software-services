/*
This project, FPGA Crypto Service Server, is licensed as below

***************************************************************************

Copyright 2020-2024 Intel Corporation. All Rights Reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

***************************************************************************
*/

#include <map>

#include "Logger.h"

LogLevel Logger::currentLogLevel=Info;

void Logger::log(std::string message, LogLevel level)
{
    if (level >= currentLogLevel)
    {
        std::cout << getLogLevelString(level) << message << std::endl;
    }
}

void Logger::logWithReturnCode(std::string message, int errorCode, LogLevel level)
{
    if (level >= currentLogLevel)
    {
        std::cout
            << getLogLevelString(level)
            << message << " Return code: " << errorCode << std::endl;
    }
}

std::string Logger::getLogLevelString(LogLevel level)
{
    std::string levelString = "";
    switch (level)
    {
        case Debug:
            levelString = "DBG";
            break;
        case Info:
            levelString = "INF";
            break;
        case Warning:
            levelString = "WRN";
            break;
        case Error:
            levelString = "ERR";
            break;
        case Fatal:
            levelString = "FTL";
            break;
        default:
            //should never happen
            levelString = "N/A";
            break;
    }
    return "[" + levelString + "] ";
}

bool Logger::setCurrentLogLevel(std::string level)
{
    const std::map<std::string, LogLevel> LogLevelMap
    {
            { "Debug", Debug },
            { "Info", Info },
            { "Warning", Warning },
            { "Error", Error },
            { "Fatal", Fatal }
    };

    auto itr = LogLevelMap.find(level);
    if (itr == LogLevelMap.end())
    {
        return false;
    }
    setCurrentLogLevel(itr->second);
    return true;
}
