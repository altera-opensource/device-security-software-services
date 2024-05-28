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

#include "utils/string_utils_test.h"

/**
 * String utils tests
 */

 //region Positive

TEST_F(StringUtilsTest, concat_strings_Empty_Success)
{
    // given
    std::string expected = "";

    // when
    std::string output = str_utils::concat_strings( {} );

    // then
    ASSERT_EQ(expected, output);
}

TEST_F(StringUtilsTest, concat_strings_OneString_Success)
{
    // given
    std::string str1 = "s1";
    std::string expected = "s1";

    // when
    std::string output = str_utils::concat_strings( {str1} );

    // then
    ASSERT_EQ(expected, output);
}

TEST_F(StringUtilsTest, concat_strings_TwoStrings_Success)
{
    // given
    std::string str1 = "s1";
    std::string str2 = "s2";
    std::string expected = "s1s2";

    // when
    std::string output = str_utils::concat_strings( {str1, str2} );

    // then
    ASSERT_EQ(expected, output);
}

TEST_F(StringUtilsTest, concat_strings_FiveStrings_Success)
{
    // given
    std::string str1 = "s1";
    std::string str2 = "s2";
    std::string str3 = "s3";
    std::string str4 = "s4";
    std::string str5 = "s5";
    std::string expected = "s1s2s3s4s5";

    // when
    std::string output = str_utils::concat_strings(
            {str1, str2, str3, str4, str5}
            );

    // then
    ASSERT_EQ(expected, output);
}

 //endregion Positive
