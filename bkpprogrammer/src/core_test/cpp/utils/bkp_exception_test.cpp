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

#include "utils/bkp_exception_test.h"

/**
 * BKP Exception tests
 */

TEST_F(BkpExceptionTest, BkpServiceResponseException_what_returnsCorrectMessage)
{
    // given
    BkpServiceResponseException bkpException(expected);

    // when
    std::string result(bkpException.what());

    // then
    ASSERT_TRUE(result.find(expected)!= std::string::npos);
}

TEST_F(BkpExceptionTest, BkpJsonParsingException_what_returnsCorrectMessage)
{
    // given
    BkpJsonParsingException bkpException(expected);

    // when
    std::string result(bkpException.what());

    // then
    ASSERT_TRUE(result.find(expected)!= std::string::npos);
}

TEST_F(BkpExceptionTest, BkpBase64Exception_what_returnsCorrectMessage)
{
    // given
    BkpBase64Exception bkpException(expected);

    // when
    std::string result(bkpException.what());

    // then
    ASSERT_TRUE(result.find(expected) != std::string::npos);
}

TEST_F(BkpExceptionTest, BkpServiceUnavailableException_what_returnsCorrectMessage)
{
    // given
    BkpServiceUnavailableException bkpException(expected);

    // when
    std::string result(bkpException.what());

    // then
    ASSERT_TRUE(result.find(expected) != std::string::npos);
}

TEST_F(BkpExceptionTest, BkpHttpException_what_returnsCorrectMessage)
{
    // given
    BkpHttpException bkpException(expected);

    // when
    std::string result(bkpException.what());

    // then
    ASSERT_TRUE(result.find(expected) != std::string::npos);
}

TEST_F(BkpExceptionTest, UnknownException_what_returnsCorrectMessage)
{
    // given
    BkpUnknownException bkpException(expected);

    // when
    std::string result(bkpException.what());

    // then
    ASSERT_TRUE(result.find(expected) != std::string::npos);
}

TEST_F(BkpExceptionTest, UnknownException_getCategoryName_returnsCorrectCategory)
{
    // given
    BkpUnknownException bkpException(expected);

    // when
    std::string result(bkpException.error_code.category().name());

    // then
    ASSERT_TRUE(result.find("BKP") != std::string::npos);
}
