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


#include "quartus/jtag_helper_test.h"

/**
 * JtagHelper tests
 */

void prepare_response(unsigned int **outBuf, size_t &outBufSizeIn4ByteWords)
{
    auto *response = new uint8_t[sampleResponseLength];
    for (size_t i = 0; i < sampleResponseLength; i++)
        response[i] = sampleResponse[i];
    memcpy(*outBuf, response, sampleResponseLength);
    outBufSizeIn4ByteWords = sampleResponseLengthIn4ByteWords;
    delete[] response;
}

Status_t mock_send_message(Unused, Unused, Unused, unsigned int **outBuf, size_t &outBufSizeIn4ByteWords)
{
    prepare_response(outBuf, outBufSizeIn4ByteWords);
    return ST_OK;
}

Status_t mock_send_message_too_big_bufOutSize(Unused, Unused, Unused, unsigned int **outBuf, size_t &outBufSizeIn4ByteWords)
{
    prepare_response(outBuf, outBufSizeIn4ByteWords);
    outBufSizeIn4ByteWords = 10000;
    return ST_OK;
}

Status_t mock_send_message_error(Unused, Unused, Unused, unsigned int **outBuf, size_t &outBufSizeIn4ByteWords)
{
    prepare_response(outBuf, outBufSizeIn4ByteWords);
    return ST_GENERIC_ERROR;
}

void JtagHelperTest::mockQuartusCallToSendMessage() const {
    EXPECT_CALL(*mockQuartusContext, send_message(_, _, _, _, _)).WillOnce(Invoke(mock_send_message));
}

void JtagHelperTest::mockQuartusCallToSendMessageError() const {
    EXPECT_CALL(*mockQuartusContext, send_message(_, _, _, _, _)).WillOnce(Invoke(mock_send_message_error));
}

void JtagHelperTest::mockQuartusCallToSendMessageTooBigBufOutSize() const {
    EXPECT_CALL(*mockQuartusContext, send_message(_, _, _, _, _)).WillOnce(Invoke(mock_send_message_too_big_bufOutSize));
}

void JtagHelperTest::mockLoggerCallsSendingToQuartus(const std::string &message, const size_t messageLenIn4ByteWords) const {
    EXPECT_CALL(*mockLoggerPtr, log(_, GTEST_MATCH_SUBSTR(message))).Times(1);
    EXPECT_CALL(*mockLoggerPtr, log(_, GTEST_MATCH_SUBSTR("Decoded command length in 4-byte words: " + std::to_string(messageLenIn4ByteWords)))).Times(1);
    EXPECT_CALL(*mockLoggerPtr, log(_, GTEST_MATCH_SUBSTR("Sending JTAG command to device..."))).Times(1);
}

void JtagHelperTest::mockLoggerCallsJtagResponsesFromQuartus() const {
    EXPECT_CALL(*mockLoggerPtr, log(_, GTEST_MATCH_SUBSTR("JtagResponses from device: "))).Times(1);
}

void JtagHelperTest::mockLoggerCallsErrorResponseFromQuartus() const {
    EXPECT_CALL(*mockLoggerPtr, log(_, GTEST_MATCH_SUBSTR("Device responded with status error."))).Times(1);
}

void JtagHelperTest::mockLoggerCallsQuartusMemoryAccess() const {
    EXPECT_CALL(*mockLoggerPtr, log(_, GTEST_MATCH_SUBSTR("Device tried to write response beyond allocated memory."))).Times(1);
}

TEST_F(JtagHelperTest, exchange_jtag_cmd_Success)
{
    // given
    JtagHelperImpl jtagHelper(mockQuartusContext.get(), mockLoggerPtr);

    mockLoggerCallsSendingToQuartus(sampleMessageBase64Encoded, sampleMessageLengthIn4ByteWords);
    mockLoggerCallsJtagResponsesFromQuartus();
    mockQuartusCallToSendMessage();
    std::string outResponse;

    // when
    const Status_t result = jtagHelper.exchange_jtag_cmd(sampleCommandBase64Encoded, outResponse);

    // then
    ASSERT_EQ(sampleResponseBase64Encoded, outResponse);
    ASSERT_EQ(ST_OK, result);
}

TEST_F(JtagHelperTest, exchange_jtag_cmd_WithSigmaTeardownCommand_Success)
{
    // given
    std::string message = "1TAAEAAAAACk4lK4AAAAAA=="; // It can be anything compliant to Base64
    const Command command = {
            .type = SEND_PACKET,
            .value = message
    };
    size_t messageLenIn4ByteWords = 4;
    JtagHelperImpl jtagHelper(mockQuartusContext.get(), mockLoggerPtr);

    mockLoggerCallsSendingToQuartus(message, messageLenIn4ByteWords);
    mockLoggerCallsJtagResponsesFromQuartus();
    mockQuartusCallToSendMessage();
    std::string outResponse;

    // when
    const Status_t result = jtagHelper.exchange_jtag_cmd(command, outResponse);

    // then
    ASSERT_EQ(sampleResponseBase64Encoded, outResponse);
    ASSERT_EQ(ST_OK, result);
}

TEST_F(JtagHelperTest, exchange_jtag_cmd_With4ByteLenCommand_Success)
{
    // given
    std::string message = "QUJDRA==";
    const Command command = {
            .type = SEND_PACKET,
            .value = message
    };
    size_t messageLenIn4ByteWords = 1;
    JtagHelperImpl jtagHelper(mockQuartusContext.get(), mockLoggerPtr);

    mockLoggerCallsSendingToQuartus(message, messageLenIn4ByteWords);
    mockLoggerCallsJtagResponsesFromQuartus();
    mockQuartusCallToSendMessage();
    std::string outResponse;

    // when
    const Status_t result = jtagHelper.exchange_jtag_cmd(command, outResponse);

    // then
    ASSERT_EQ(sampleResponseBase64Encoded, outResponse);
    ASSERT_EQ(ST_OK, result);
}

TEST_F(JtagHelperTest, exchange_jtag_cmd_With3ByteLenCommand_Success)
{
    // given
    std::string message = "QUJD";
    const Command command = {
            .type = SEND_PACKET,
            .value = message
    };
    size_t messageLenIn4ByteWords = 1;
    JtagHelperImpl jtagHelper(mockQuartusContext.get(), mockLoggerPtr);

    mockLoggerCallsSendingToQuartus(message, messageLenIn4ByteWords);
    mockLoggerCallsJtagResponsesFromQuartus();
    mockQuartusCallToSendMessage();
    std::string outResponse;

    // when
    const Status_t result = jtagHelper.exchange_jtag_cmd(command, outResponse);

    // then
    ASSERT_EQ(sampleResponseBase64Encoded, outResponse);
    ASSERT_EQ(ST_OK, result);
}

TEST_F(JtagHelperTest, exchange_jtag_cmd_With2ByteLenCommand_Success)
{
    // given
    std::string message = "QUI=";
    const Command command = {
            .type = SEND_PACKET,
            .value = message
    };
    size_t messageLenIn4ByteWords = 1;
    JtagHelperImpl jtagHelper(mockQuartusContext.get(), mockLoggerPtr);

    mockLoggerCallsSendingToQuartus(message, messageLenIn4ByteWords);
    mockLoggerCallsJtagResponsesFromQuartus();
    mockQuartusCallToSendMessage();
    std::string outResponse;

    // when
    const Status_t result = jtagHelper.exchange_jtag_cmd(command, outResponse);

    // then
    ASSERT_EQ(sampleResponseBase64Encoded, outResponse);
    ASSERT_EQ(ST_OK, result);
}

TEST_F(JtagHelperTest, exchange_jtag_cmd_With1ByteLenCommand_Success)
{
    // given
    std::string message = "QQ==";
    const Command command = {
            .type = SEND_PACKET,
            .value = message
    };
    size_t messageLenIn4ByteWords = 1;
    JtagHelperImpl jtagHelper(mockQuartusContext.get(), mockLoggerPtr);

    mockLoggerCallsSendingToQuartus(message, messageLenIn4ByteWords);
    mockLoggerCallsJtagResponsesFromQuartus();
    mockQuartusCallToSendMessage();
    std::string outResponse;

    // when
    const Status_t result = jtagHelper.exchange_jtag_cmd(command, outResponse);

    // then
    ASSERT_EQ(sampleResponseBase64Encoded, outResponse);
    ASSERT_EQ(ST_OK, result);
}

TEST_F(JtagHelperTest, exchange_jtag_cmd_Quartus_error_Returns_error)
{
    // given
    JtagHelperImpl jtagHelper(mockQuartusContext.get(), mockLoggerPtr);

    mockLoggerCallsSendingToQuartus(sampleMessageBase64Encoded, sampleMessageLengthIn4ByteWords);
    mockLoggerCallsErrorResponseFromQuartus();
    mockQuartusCallToSendMessageError();
    std::string outResponse;

    // when
    const Status_t result = jtagHelper.exchange_jtag_cmd(sampleCommandBase64Encoded, outResponse);

    // then
    ASSERT_EQ("", outResponse);
    ASSERT_EQ(ST_GENERIC_ERROR, result);
}

TEST_F(JtagHelperTest, exchange_jtag_cmd_Quartus_response_with_too_big_bufOutSize)
{
    // given
    JtagHelperImpl jtagHelper(mockQuartusContext.get(), mockLoggerPtr);

    mockLoggerCallsSendingToQuartus(sampleMessageBase64Encoded, sampleMessageLengthIn4ByteWords);
    mockLoggerCallsQuartusMemoryAccess();
    mockQuartusCallToSendMessageTooBigBufOutSize();
    std::string outResponse;

    // when
    const Status_t result = jtagHelper.exchange_jtag_cmd(sampleCommandBase64Encoded, outResponse);

    // then
    ASSERT_EQ("", outResponse);
    ASSERT_EQ(ST_GENERIC_ERROR, result);
}

TEST_F(JtagHelperTest, exchange_jtag_cmd_empty_command_Success)
{
    // given
    JtagHelperImpl jtagHelper(mockQuartusContext.get(), mockLoggerPtr);

    mockLoggerCallsSendingToQuartus(messageEmpty, 0);
    mockLoggerCallsJtagResponsesFromQuartus();
    mockQuartusCallToSendMessage();
    std::string outResponse;

    // when
    const Status_t result = jtagHelper.exchange_jtag_cmd(commandEmpty, outResponse);

    // then
    ASSERT_EQ(sampleResponseBase64Encoded, outResponse);
    ASSERT_EQ(ST_OK, result);
}

TEST_F(JtagHelperTest, exchange_jtag_cmd_not_base64_command_throws)
{
    // given
    JtagHelperImpl jtagHelper(mockQuartusContext.get(), mockLoggerPtr);
    EXPECT_CALL(*mockLoggerPtr, log(_, GTEST_MATCH_SUBSTR(messageNotBase64))).Times(1);
    std::string outResponse;

    // when
    try {
        jtagHelper.exchange_jtag_cmd(commandNotBase64, outResponse);
        FAIL() << "Expected BkpBase64Exception.";
    } catch(const BkpBase64Exception &ex) {
        EXPECT_TRUE(std::string(ex.what()).find(messageNotBase64) != std::string::npos);
    } catch(...) {
        FAIL() << "Expected BkpBase64Exception.";
    }
}
