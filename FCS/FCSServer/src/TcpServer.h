/*
This project, FPGA Crypto Service Server, is licensed as below

***************************************************************************

Copyright 2020-2025 Altera Corporation. All Rights Reserved.

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

#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <stddef.h>
#include <stdint.h>
#include <sys/poll.h>
#include <vector>

class TcpServer
{
    public:
        void run(
            uint32_t portNumber,
            void (*onMessage)(
                std::vector<uint8_t>&, std::vector<uint8_t>&));
        void closeSockets();

    private:
        void setup(uint32_t portNumber);
        void dropUnusedConnections();
        void handleEventIfAny(
            pollfd &socket,
            void (*onMessage)(
                std::vector<uint8_t>&, std::vector<uint8_t>&));
        void acceptConnection();
        void closeConnectionAndEnableForReuse(pollfd &socket);

        static const uint32_t kMaxNumberOfConnections = 20;
        static const uint32_t kPollTimeoutInMilliseconds = 60 * 1000;

        // 1 server socket + client sockets
        static const uint32_t kNumberOfSockets = kMaxNumberOfConnections + 1;
        static const uint32_t kMaxMessageSizeInBytes = 10000;

        std::vector<uint8_t> messageBuffer
            = std::vector<uint8_t>(kMaxMessageSizeInBytes);

        pollfd sockets[kNumberOfSockets];
        int serverSocketFd = -1;
};

#endif /* TCPSERVER_H */
