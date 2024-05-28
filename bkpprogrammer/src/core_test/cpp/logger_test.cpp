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

#include "logger_test.h"

/**
 * Logger tests
 */

void LoggerTest::prepare_transaction_id(bool is_transaction_id)
{
    if (is_transaction_id) {
        EXPECT_CALL(*mockTransactionIdManagerPtr, is_transaction_id()).WillOnce(Return(true));
        EXPECT_CALL(*mockTransactionIdManagerPtr, get_transaction_id()).WillOnce(Return(fakeTxId));
    } else {
        EXPECT_CALL(*mockTransactionIdManagerPtr, is_transaction_id()).WillOnce(Return(false));
        EXPECT_CALL(*mockTransactionIdManagerPtr, get_transaction_id()).Times(0);
    }
}

TEST_F(LoggerTest, log_WithOneParam_WithTransactionId_Success)
{
    // given
    LoggerImpl logger(&mockQuartusContext, mockTransactionIdManagerPtr);
    LogLevel_t expectedLogLevel = L_DEBUG;
    prepare_transaction_id(true);

    // then
    EXPECT_CALL(mockQuartusContext, log(L_DEBUG,
            AllOf(HasSubstr(sampleMessage1), HasSubstr(fakeTxId)))).Times(1);

    // when
    logger.log(expectedLogLevel, sampleMessage1);
}

TEST_F(LoggerTest, log_WithOneParam_NoTransactionId_Success)
{
    // given
    LoggerImpl logger(&mockQuartusContext, mockTransactionIdManagerPtr);
    LogLevel_t expectedLogLevel = L_DEBUG;
    prepare_transaction_id(false);

    // then
    EXPECT_CALL(mockQuartusContext, log(expectedLogLevel, HasSubstr(sampleMessage1)));

    // when
    logger.log(expectedLogLevel, sampleMessage1);
}


