/*
This project, FPGA Crypto Service Server, is licensed as below

***************************************************************************

Copyright 2020-2024 Intel Corporation. All Rights Reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

***************************************************************************
*/

#include "gtest/gtest.h"
#include <vector>

#include "utils.h"

TEST(utilsUT, decodeFromLittleEndianBufferTest)
{
    //all 4 bytes 0 should return uint32_t 0
    std::vector<uint8_t> buffer(4, 0);
    EXPECT_EQ((uint32_t)0, Utils::decodeFromLittleEndianBuffer(buffer));

    buffer[0] = 0x04;
    buffer[1] = 0x03;
    buffer[2] = 0x02;
    buffer[3] = 0x01;
    EXPECT_EQ((uint32_t)0x01020304, Utils::decodeFromLittleEndianBuffer(buffer));

    buffer[0] = 0xFF;
    buffer[1] = 0xFF;
    buffer[2] = 0xFF;
    buffer[3] = 0xFF;
    EXPECT_EQ(UINT32_MAX, Utils::decodeFromLittleEndianBuffer(buffer));

    //adding 2 elements at the end of vector and reading from offset 2
    buffer.push_back(0xAA);
    buffer.push_back(0xBB);
    EXPECT_EQ((uint32_t)0xBBAAFFFF, Utils::decodeFromLittleEndianBuffer(buffer, 2));

    //input vector size 6 and offset 3 - not enough input size (should be >=3+4)
    EXPECT_THROW(Utils::decodeFromLittleEndianBuffer(buffer, 3), std::invalid_argument);

    //input vector size 3 without offset - not enough input size (should be >=4)
    buffer.resize(3);
    EXPECT_THROW(Utils::decodeFromLittleEndianBuffer(buffer), std::invalid_argument);
}

TEST(utilsUT, encodeToLittleEndianBuffer)
{
    //output vector size 0 instead of >=4
    std::vector<uint8_t> output;
    EXPECT_THROW(Utils::encodeToLittleEndianBuffer(2137, output), std::invalid_argument);

    //output vector size 4 without offset
    output.resize(4);
    std::vector<uint8_t> expectedOutput {0x01, 0x02, 0x03, 0x04};
    Utils::encodeToLittleEndianBuffer(0x04030201, output);
    EXPECT_EQ(expectedOutput, output);

    //output vector size 6 with offset 1
    output = std::vector<uint8_t> {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
    expectedOutput = std::vector<uint8_t> {0xAA, 0x11, 0x12, 0x13, 0x14, 0xAA};
    Utils::encodeToLittleEndianBuffer(0x14131211, output, 1);
    EXPECT_EQ(expectedOutput, output);

    //output vector size 6 with offset 3 - not enough space to fit 4 bytes
    EXPECT_THROW(Utils::encodeToLittleEndianBuffer(0x14131211, output, 3), std::invalid_argument);
}
