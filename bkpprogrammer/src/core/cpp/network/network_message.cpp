/*
 * This project is licensed as below.
 *
 * **************************************************************************
 *
 * Copyright 2020-2025 Altera Corporation. All Rights Reserved.
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

#include <boost/json/src.hpp>
#include <boost/algorithm/string.hpp>
#include "network/network_message.h"

// conversion functions between custom structs (Response, Command) and boost::json:value
// They are needed to convert e.g. from vector<Response> to JSON array with objects
void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, Response const& response)
{
    jv =
    {
        {fields::jtag_responses_status, static_cast<int>(response.status)},
        {fields::jtag_responses_value, response.value}
    };
}

Command tag_invoke(boost::json::value_to_tag<Command>, boost::json::value const& jv)
{
    Command command;
    command.type = static_cast<Message_t>(jv.at(fields::message_type).as_int64());
    command.value = jv.at(fields::command_value).as_string();
    return command;
}

void NetworkMessage::parseFromResponseJsonCommon(boost::json::value jv)
{
    NetworkResponse &response = getResponse();
    response.status = jv.at(fields::status).as_string();
    boost::algorithm::to_lower(response.status);
    response.apiVersion = static_cast<uint32_t>(jv.at(fields::api_version).as_int64());
    response.jtagCommands = boost::json::value_to<std::vector<Command>>(jv.at(fields::jtag_commands));
}

boost::json::object NetworkMessage::getRequestJsonCommon()
{
    NetworkRequest &request = getRequest();
    return {
        {fields::api_version, request.apiVersion},
        {fields::jtag_responses, boost::json::value_from(request.jtagResponses)},
        {fields::supported_commands, request.supportedCommands}
    };
}
