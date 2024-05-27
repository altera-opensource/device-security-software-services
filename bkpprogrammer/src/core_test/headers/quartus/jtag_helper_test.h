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

#ifndef JTAG_HELPER_TEST_H
#define JTAG_HELPER_TEST_H

#include <gmock/gmock.h>

#include "quartus/jtag_helper.h"
#include "mocks/mocked_logger.h"
#include "mocks/mocked_quartus.h"

using testing::_;
using testing::Return;
using testing::Invoke;
using testing::Unused;

static const std::string sampleMessage = "ABCD";
static const std::string sampleMessageBase64Encoded = "QUJDRA==";
static const std::string sampleResponse = "EFGH";
static const std::string sampleResponseBase64Encoded = "RUZHSA==";
static const std::string messageNotBase64 = ")))";
static const std::string messageEmpty = "";

static const size_t sampleMessageLength = sampleMessage.length();
static const size_t sampleMessageLengthIn4ByteWords = sampleMessageLength / DWORD_SIZE_IN_BYTES;
static const size_t sampleResponseLength = sampleResponse.length();
static const size_t sampleResponseLengthIn4ByteWords = sampleResponseLength / DWORD_SIZE_IN_BYTES;

static const Command sampleCommandBase64Encoded = {
        .type = SEND_PACKET,
        .value = sampleMessageBase64Encoded
};

static const Command commandEmpty = {
        .type = SEND_PACKET,
        .value = messageEmpty
};

static const Command commandNotBase64 = {
        .type = SEND_PACKET,
        .value = messageNotBase64
};

class JtagHelperTest : public ::testing::Test
{
public:

    std::shared_ptr<MockQuartusContext> mockQuartusContext;
    std::shared_ptr<MockLogger> mockLoggerPtr;

    JtagHelperTest() : mockQuartusContext(std::make_shared<MockQuartusContext>()),
        mockLoggerPtr(std::make_shared<MockLogger>()) {};

    void mockQuartusCallToSendMessage() const;
    void mockQuartusCallToSendMessageError() const;
    void mockQuartusCallToSendMessageTooBigBufOutSize() const;
    void mockLoggerCallsSendingToQuartus(const std::string &message, size_t messageLenIn4ByteWords) const;
    void mockLoggerCallsJtagResponsesFromQuartus() const;
    void mockLoggerCallsErrorResponseFromQuartus() const;
    void mockLoggerCallsQuartusMemoryAccess() const;
};

#endif //JTAG_HELPER_TEST_H