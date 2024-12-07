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

#include "Logger.h"
#include "TcpServer.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>


void TcpServer::run(
    uint32_t portNumber,
    void (*onMessage)(std::vector<uint8_t>&, std::vector<uint8_t>&))
{
    setup(portNumber);
    Logger::log("Server started on port " + std::to_string(portNumber));

    while (true)
    {
        for (unsigned int i = 1; i < kNumberOfSockets; i++)
        {
            sockets[i].events = POLLIN;
            sockets[i].revents = 0;
        }
        int numberOfEvents = poll(
            sockets, kNumberOfSockets, kPollTimeoutInMilliseconds);
        if (numberOfEvents < 0)
        {
            Logger::logWithReturnCode("Poll failed.", errno, Error);
            exit(1);
        }
        else if (numberOfEvents == 0)
        {
            dropUnusedConnections();
            continue;
        }
        for (unsigned int i = 0; i < kNumberOfSockets; i++)
        {
            if (sockets[i].fd != -1)
            {
                handleEventIfAny(sockets[i], onMessage);
            }
        }
    }
}

void TcpServer::setup(uint32_t portNumber)
{
    Logger::log("Setting up TCP server...", Debug);
    serverSocketFd = socket(AF_INET, SOCK_STREAM | O_NONBLOCK, 0);
    if (serverSocketFd == -1)
    {
        Logger::logWithReturnCode("Could not create socket.", errno, Fatal);
        exit(1);
    }

    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(portNumber);
    if (bind(serverSocketFd, (sockaddr*)&server, sizeof(server)) < 0)
    {
        Logger::logWithReturnCode("Bind failed.", errno, Fatal);
        if (errno == EADDRINUSE)
        {
            Logger::log("Port " + std::to_string(portNumber)
                + " already in use", Fatal);
        }
        exit(1);
    }

    if (listen(serverSocketFd, kMaxNumberOfConnections) == -1)
    {
        Logger::logWithReturnCode("Listen failed.", errno, Fatal);
        exit(1);
    }

    sockets[0].fd = serverSocketFd;
    sockets[0].events = POLLIN;
    for (unsigned int i = 1; i < kNumberOfSockets; i++)
    {
        sockets[i].fd = -1;
    }
}

void TcpServer::closeSockets()
{
    for (unsigned int i = 1; i < kNumberOfSockets; i++)
    {
        if (sockets[i].fd != -1)
        {
            close(sockets[i].fd);
        }
    }
    close(serverSocketFd);
}

void TcpServer::dropUnusedConnections()
{
    for (unsigned int i = 1; i < kNumberOfSockets; i++)
    {
        if (sockets[i].fd != -1)
        {
            Logger::log("Dropping unused connection: Socket fd: "
                + std::to_string(i), Debug);
            closeConnectionAndEnableForReuse(sockets[i]);
        }
    }
}

void TcpServer::handleEventIfAny(
    pollfd &socket,
    void (*onMessage)(std::vector<uint8_t>&, std::vector<uint8_t>&))
{
    if (socket.fd == serverSocketFd && socket.revents == POLLIN)
    {
        acceptConnection();
    }
    else if (socket.revents == POLLIN)
    {
        messageBuffer.resize(kMaxMessageSizeInBytes);
        ssize_t messageSize = recv(
            socket.fd, messageBuffer.data(), messageBuffer.size(), 0);
        if (messageSize == -1)
        {
            Logger::logWithReturnCode("Recv failed", errno, Error);
            closeConnectionAndEnableForReuse(socket);
        }
        else if (messageSize == 0)
        {
            closeConnectionAndEnableForReuse(socket);
        }
        else if (messageSize > 0)
        {
            messageBuffer.resize(messageSize);
            Logger::log("Received message: Socket fd: "
                + std::to_string(socket.fd), Info);
            std::vector<uint8_t> responseBuffer;

            onMessage(messageBuffer, responseBuffer);

            if (responseBuffer.size() > 0)
            {
                Logger::log("Sending Response: "
                    + std::to_string(responseBuffer.size()) + " bytes", Info);
                if (send(socket.fd, responseBuffer.data(), responseBuffer.size(), 0) == -1)
                {
                    Logger::logWithReturnCode("Send failed", errno, Error);
                }
            }
            else
            {
                Logger::log("No data to send. Closing connection", Info);
                closeConnectionAndEnableForReuse(socket);
            }
        }
    }
    else if (socket.revents == POLLHUP || socket.revents == POLLERR)
    {
        closeConnectionAndEnableForReuse(socket);
    }
}

void TcpServer::acceptConnection()
{
    for (unsigned int i = 1; i < kNumberOfSockets; i++)
    {
        if (sockets[i].fd == -1)
        {
            sockets[i].fd = accept(serverSocketFd, nullptr, nullptr);
            if (sockets[i].fd == -1)
            {
                if (errno != EWOULDBLOCK)
                {
                    Logger::logWithReturnCode("Accept failed.", errno, Error);
                }
            }
            Logger::log("Incoming connection: Socket fd: "
                + std::to_string(sockets[i].fd), Debug);
            break;
        }
    }
}

void TcpServer::closeConnectionAndEnableForReuse(pollfd &socket)
{
    Logger::log("Connection closed: Socket fd: "
        + std::to_string(socket.fd), Info);
    close(socket.fd);
    socket.fd = -1;
}
