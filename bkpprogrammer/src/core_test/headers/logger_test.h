/*
 * This project is licensed as below.
 *
 * **************************************************************************
 *
 * Copyright 2020-2023 Intel Corporation. All Rights Reserved.
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

#ifndef LOGGER_TEST_H
#define LOGGER_TEST_H

#include <gmock/gmock.h>

#include "logger.h"
#include "mocks/mocked_quartus.h"
#include "mocks/mocked_transaction_id_manager.h"

using testing::_;
using testing::Return;
using testing::HasSubstr;
using testing::AllOf;

class LoggerTest : public ::testing::Test
{
protected:
    void prepare_transaction_id(bool is_transaction_id);

public:
    const std::string fakeTxId = "fake tx id";
    const std::string sampleMessage1 = "test message 1";
    const std::string sampleMessage2 = "test message 2";

    MockQuartusContext mockQuartusContext;
    std::shared_ptr<MockTransactionIdManager> mockTransactionIdManagerPtr;

    LoggerTest() : mockTransactionIdManagerPtr(std::make_shared<MockTransactionIdManager>()) {};
};

#endif //LOGGER_TEST_H
