/*
 * This project is licensed as below.
 *
 * **************************************************************************
 *
 * Copyright 2020-2024 Intel Corporation. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * **************************************************************************
 */

#ifndef PROGRAMMER_NETWORK_MESSAGE_H
#define PROGRAMMER_NETWORK_MESSAGE_H

#include <boost/json.hpp>
#include "utils/structures.h"

namespace fields {
    const std::string context = "context";
    const std::string context_value = "value";
    const std::string cfg_id = "cfgId";
    const std::string api_version = "apiVersion";
    const std::string supported_commands = "supportedCommands";
    const std::string status = "status";
    const std::string command_value = "value";
    const std::string message_type = "type";
    const std::string jtag_commands = "jtagCommands";
    const std::string jtag_responses = "jtagResponses";
    const std::string jtag_responses_value = "value";
    const std::string jtag_responses_status = "status";
    const std::string puf_type = "puf_type";
    const std::string slot_id = "slot_id";
    const std::string force_enrollment = "force_enrollment";
}

class NetworkRequest
{
public:
    uint32_t apiVersion = 0;
    uint32_t supportedCommands = 0;
    std::vector <Response> jtagResponses;
    virtual ~NetworkRequest() = default;
};

class NetworkResponse
{
public:
    std::string status;
    uint32_t apiVersion;
    std::vector <Command> jtagCommands;
    virtual ~NetworkResponse() = default;
};

class NetworkMessage
{
public:
    virtual std::string getEndpoint() = 0;
    virtual std::string getRequestJson() = 0;
    virtual void parseFromResponseJson(std::string jsonString) = 0;
    virtual NetworkRequest& getRequest() = 0;
    virtual NetworkResponse& getResponse() = 0;
    virtual ~NetworkMessage() = default;
protected:
    void parseFromResponseJsonCommon(boost::json::value jv);
    boost::json::object getRequestJsonCommon();
};

#endif //PROGRAMMER_NETWORK_MESSAGE_H
