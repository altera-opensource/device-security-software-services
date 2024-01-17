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


#include "TcpServer.h"
#include "MessageHandler.h"
#include "Logger.h"

#include <signal.h>
#include <string>

TcpServer server;

void onSignal(sig_atomic_t s)
{
    if (s == SIGINT || s == SIGTERM)
    {
        Logger::log("Signal caught, terminating");
        server.closeSockets();
        exit(0);
    }
}

void printUsageAndExit()
{
    Logger::log("Usage: <executable name> <port number> optional:<log level> e.g ./fcsServer 50001 Debug", Fatal);
    Logger::log("Possible log levels: Debug, Info (default), Warning, Error, Fatal", Fatal);
    exit(1);
}

int main(int argc, char* argv[])
{
    signal(SIGINT, onSignal);
    signal(SIGTERM, onSignal);
    if (argc == 3)
    {
        if (!Logger::setCurrentLogLevel(std::string(argv[2])))
        {
            printUsageAndExit();
        }
    }
    if (argc < 2 || argc > 3)
    {
        printUsageAndExit();
    }
    int portNumber;
    try
    {
        portNumber = std::stoi(argv[1]);
    }
    catch(const std::exception& e)
    {
        printUsageAndExit();
    }

    try
    {
        Logger::log("FCS Server build on: "
            + std::string(__DATE__) + " " + std::string(__TIME__), Debug);
        server.run(portNumber, &handleIncomingMessage);
    }
    catch(const std::exception& e)
    {
        Logger::log(e.what(), Fatal);
        exit(1);
    }

    return 0;
}
