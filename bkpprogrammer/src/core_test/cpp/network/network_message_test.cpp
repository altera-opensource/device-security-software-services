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

#include <gmock/gmock.h>

#include "network/network_message.h"
#include "network/messages/get_next.h"
#include "network/messages/prefetch_next.h"
#include "network/messages/puf_activate_next.h"
#include "network/messages/set_authority_next.h"

void runParsingAndCheckForJsonException(std::string input)
{
    GetNextMessage getNextMessage("");
    EXPECT_THROW(getNextMessage.parseFromResponseJson(input), std::exception);
}


TEST(NetworkMessageTest, serialize_get_next_Success)
{
    // given
    std::string expected = R"({"apiVersion":1,"jtagResponses":[{"status":0,"value":"resp1"},{"status":1,"value":"resp2"}],"supportedCommands":1,"cfgId":"1","context":{"value":"context"}})";

    Response resp1 = {"resp1", ST_OK };
    Response resp2 = {"resp2", ST_GENERIC_ERROR };
    BkpsContext ctx = { "context" };
    GetNextMessage getNextMessage("1");
    GetNextRequest& request = static_cast<GetNextRequest&>(getNextMessage.getRequest());
    request.context = ctx;
    request.apiVersion = 1;
    request.supportedCommands = 1;
    request.jtagResponses = { resp1, resp2 };

    // when
    std::string result = getNextMessage.getRequestJson();

    //then
    ASSERT_EQ(expected, result);
}

TEST(NetworkMessageTest, serialize_prefetch_Success)
{
    // given
    std::string expected = R"({"apiVersion":1,"jtagResponses":[{"status":0,"value":"resp1"},{"status":1,"value":"resp2"}],"supportedCommands":1})";

    Response resp1 = {"resp1", ST_OK };
    Response resp2 = {"resp2", ST_GENERIC_ERROR };
    PrefetchNextMessage prefetchNextMessage;
    PrefetchNextRequest& request = static_cast<PrefetchNextRequest&>(prefetchNextMessage.getRequest());
    request.apiVersion = 1;
    request.supportedCommands = 1;
    request.jtagResponses = { resp1, resp2 };

    // when
    std::string result = prefetchNextMessage.getRequestJson();

    //then
    ASSERT_EQ(expected, result);
}

TEST(NetworkMessageTest, serialize_puf_activate_Success)
{
    // given
    std::string expected = R"({"apiVersion":1,"jtagResponses":[{"status":0,"value":"resp1"},{"status":1,"value":"resp2"}],"supportedCommands":1,"puf_type":0,"context":{"value":"context"}})";

    Response resp1 = {"resp1", ST_OK };
    Response resp2 = {"resp2", ST_GENERIC_ERROR };
    BkpsContext ctx = { "context" };
    PufActivateNextMessage pufActivateNextMessage(UDS_IID);
    PufActivateNextRequest& request = static_cast<PufActivateNextRequest&>(pufActivateNextMessage.getRequest());
    request.context = ctx;
    request.apiVersion = 1;
    request.supportedCommands = 1;
    request.jtagResponses = { resp1, resp2 };

    // when
    std::string result = pufActivateNextMessage.getRequestJson();

    //then
    ASSERT_EQ(expected, result);
}

TEST(NetworkMessageTest, serialize_set_authority_Success)
{
    // given
    std::string expected = R"({"apiVersion":1,"jtagResponses":[{"status":0,"value":"resp1"},{"status":1,"value":"resp2"}],"supportedCommands":1,"puf_type":0,"slot_id":5,"force_enrollment":false,"context":{"value":"context"}})";

    Response resp1 = {"resp1", ST_OK };
    Response resp2 = {"resp2", ST_GENERIC_ERROR };
    BkpsContext ctx = { "context" };
    SetAuthorityNextMessage setAuthorityNextMessage(UDS_IID, 5, false);
    SetAuthorityNextRequest& request = static_cast<SetAuthorityNextRequest&>(setAuthorityNextMessage.getRequest());
    request.context = ctx;
    request.apiVersion = 1;
    request.supportedCommands = 1;
    request.jtagResponses = { resp1, resp2 };

    // when
    std::string result = setAuthorityNextMessage.getRequestJson();

    //then
    ASSERT_EQ(expected, result);
}

TEST(NetworkMessageTest, serialize_EmptyFieldJtagResponses)
{
    // given
    std::string expected = R"({"apiVersion":1,"jtagResponses":[],"supportedCommands":1,"cfgId":"1","context":{"value":"context"}})";

    BkpsContext ctx = { "context" };
    GetNextMessage getNextMessage("1");
    GetNextRequest& request = static_cast<GetNextRequest&>(getNextMessage.getRequest());
    request.context = ctx;
    request.apiVersion = 1;
    request.supportedCommands = 1;

    // when
    std::string result = getNextMessage.getRequestJson();

    //then
    ASSERT_EQ(expected, result);
}

TEST(NetworkMessageTest, serialize_EmptyFieldContext)
{
    // given
    std::string expected = R"({"apiVersion":1,"jtagResponses":[{"status":0,"value":"resp1"},{"status":1,"value":"resp2"}],"supportedCommands":1,"cfgId":"1","context":{"value":""}})";

    GetNextMessage getNextMessage("1");
    GetNextRequest& request = static_cast<GetNextRequest&>(getNextMessage.getRequest());
    Response resp1 = {"resp1", ST_OK };
    Response resp2 = {"resp2", ST_GENERIC_ERROR };
    BkpsContext ctx = { "" };
    request.context = ctx;
    request.apiVersion = 1;
    request.supportedCommands = 1;
    request.jtagResponses = { resp1, resp2 };

    // when
    std::string result = getNextMessage.getRequestJson();

    //then
    ASSERT_EQ(expected, result);
}

TEST(NetworkMessageTest, serialize_EmptyFieldContextAndJtagResponses)
{
    // given
    std::string expected = R"({"apiVersion":1,"jtagResponses":[],"supportedCommands":1,"cfgId":"1","context":{"value":""}})";

    GetNextMessage getNextMessage("1");
    GetNextRequest& request = static_cast<GetNextRequest&>(getNextMessage.getRequest());
    BkpsContext ctx = { "" };
    request.context = ctx;
    request.apiVersion = 1;
    request.supportedCommands = 1;

    // when
    std::string result = getNextMessage.getRequestJson();

    //then
    ASSERT_EQ(expected, result);
}

TEST(NetworkMessageTest, serialize_EmptyFieldContextAndJtagResponsesAndCfgId)
{
    // given
    std::string expected = R"({"apiVersion":1,"jtagResponses":[],"supportedCommands":1,"cfgId":"","context":{"value":""}})";

    GetNextMessage getNextMessage("");
    GetNextRequest& request = static_cast<GetNextRequest&>(getNextMessage.getRequest());
    BkpsContext ctx = { "" };
    request.context = ctx;
    request.apiVersion = 1;
    request.supportedCommands = 1;

    // when
    std::string result = getNextMessage.getRequestJson();

    //then
    ASSERT_EQ(expected, result);
}

TEST(NetworkMessageTest, serialize_EmptyResponsesFields)
{
    // given
    std::string expected = R"({"apiVersion":1,"jtagResponses":[{"status":0,"value":""}],"supportedCommands":1,"cfgId":"1","context":{"value":"context"}})";

    GetNextMessage getNextMessage("1");
    GetNextRequest& request = static_cast<GetNextRequest&>(getNextMessage.getRequest());
    Response resp1 = {};
    BkpsContext ctx = { "context" };
    request.context = ctx;
    request.cfgId = "1";
    request.apiVersion = 1;
    request.supportedCommands = 1;
    request.jtagResponses = { resp1 };

    // when
    std::string result = getNextMessage.getRequestJson();

    //then
    ASSERT_EQ(expected, result);
}

TEST(NetworkMessageTest, serialize_AllEmpty)
{
    // given
    std::string expected = R"({"apiVersion":0,"jtagResponses":[],"supportedCommands":0,"cfgId":"","context":{"value":""}})";
    GetNextMessage getNextMessage("");

    // when
    std::string result = getNextMessage.getRequestJson();

    //then
    ASSERT_EQ(expected, result);
}

TEST(NetworkMessageTest, parse_StatusOk_Success)
{
    // given
    std::string commandsJson = R"({"apiVersion":1,"context":{"value":"context"},"jtagCommands":[{"type":2,"value":"comm1"},{"type":3,"value":"comm2"}],"status":"done"})";
    GetNextMessage getNextMessage("1");

    // when
    getNextMessage.parseFromResponseJson(commandsJson);

    // then

    GetNextResponse& response = static_cast<GetNextResponse&>(getNextMessage.getResponse());
    ASSERT_EQ("context", response.context.value);
    ASSERT_EQ("done", response.status);
    ASSERT_EQ(1, response.apiVersion);
    ASSERT_EQ("comm1", response.jtagCommands.at(0).value);
    ASSERT_EQ("comm2", response.jtagCommands.at(1).value);
    ASSERT_EQ(2, response.jtagCommands.at(0).type);
    ASSERT_EQ(3, response.jtagCommands.at(1).type);
}

TEST(NetworkMessageTest, parse_Negative)
{
    runParsingAndCheckForJsonException(R"({"apiVersion":1,"jtagCommands":[],"status":"done"})");
    runParsingAndCheckForJsonException(R"({"apiVersion":1,"context":{"value":"context"},"status":"done"})");
    runParsingAndCheckForJsonException(R"({"apiVersion":1,"context":{"value":"context"},"jtagCommands":["comm1","comm2"]})");
    runParsingAndCheckForJsonException(R"({"context":{"value":"context"},"jtagCommands":["comm1","comm2"],"status":"done"})");
    runParsingAndCheckForJsonException(R"({"status":{"code":"ctx","message":"msg","transactionId":"txId"}})");
    runParsingAndCheckForJsonException(R"({})");
    runParsingAndCheckForJsonException(R"({"status":{}})");
    runParsingAndCheckForJsonException(R"({"apiVersion":0,"context":null,"jtagCommands":[],"status":""})");
    runParsingAndCheckForJsonException(R"({"apiVersion":null,"context":{"value":""},"jtagCommands":[],"status":""})");
    runParsingAndCheckForJsonException(R"({"apiVersion":0,"context":{"value":""},"jtagCommands":null,"status":""})");
    runParsingAndCheckForJsonException(R"({"apiVersion":0,"context":{"value":""},"jtagCommands":[],"status":null})");
    runParsingAndCheckForJsonException("");
    runParsingAndCheckForJsonException(" ");
    runParsingAndCheckForJsonException("\\\\\\");
}
