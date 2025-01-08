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

#include "utils/base64_utils_test.h"

/**
 * Base64 utils tests
 */

 //region Positive

TEST_F(Base64UtilsTest, encode64_SuccessfulEncode)
{
    // given
    std::string test_str = R"({"cfgId":1,"chipId":"Test","overbuildMax":10})";
    std::string expected = "eyJjZmdJZCI6MSwiY2hpcElkIjoiVGVzdCIsIm92ZXJidWlsZE1heCI6MTB9";

    // when
    std::string output = b64_utils::encode64(test_str);

    // then
    ASSERT_EQ(expected, output);
}

TEST_F(Base64UtilsTest, decode64_SuccessfulDecode)
{
    // given
    std::string test_str = "eyJjZmdJZCI6MSwiY2hpcElkIjoiVGVzdCIsIm92ZXJidWlsZE1heCI6MTB9";
    std::string expected = R"({"cfgId":1,"chipId":"Test","overbuildMax":10})";

    // when
    std::string output = b64_utils::decode64(test_str);

    // then
    ASSERT_EQ(expected, output);
}

 //endregion Positive

 //region Negative

TEST_F(Base64UtilsTest, encode64_UnmatchingBase64)
{
    // given
    std::string test_str = "test str";
    std::string expected = "eyJjZmdJZCI6MSwiY2hpcElkIjoiVGVzdCIsIm92ZXJidWlsZE1heCI6MTB9";

    // when
    std::string output = b64_utils::encode64(test_str);

    // then
    ASSERT_NE(expected, output);
}

TEST_F(Base64UtilsTest, decode64_WrongBase64String_Error)
{
    // given
    std::string test_str = "eyJjZmdJZCI6";
    std::string expected = R"({"cfgId":1,"chipId":"Test","overbuildMax":10})";

    // when
    std::string output = b64_utils::decode64(test_str);

    // then
    ASSERT_NE(expected, output);
}

TEST_F(Base64UtilsTest, decode64_NotBase64_Error)
{
    // given
    std::string test_str = "eyJjZmdJZCI6...**";

    // then
    EXPECT_ANY_THROW({
        // when
        b64_utils::decode64(test_str);
    });
}

 //endregion Negative
