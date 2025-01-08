/*
This project, FPGA Crypto Service Server, is licensed as below

***************************************************************************

Copyright 2020-2025 Altera Corporation. All Rights Reserved.

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

#ifndef UTILS_H
#define UTILS_H

#include <sstream>
#include <stdexcept>
#include <stdint.h>
#include <vector>
#include <string>
#include <algorithm>

#define WORD_SIZE sizeof(uint32_t)

class Utils
{
    public:
        static uint32_t decodeFromLittleEndianBuffer(
            const std::vector<uint8_t> &buffer, size_t offset = 0)
        {
            if (buffer.size() < offset + WORD_SIZE)
            {
                throw std::invalid_argument("decodeFromLittleEndianBuffer: buffer too small");
            }
            return static_cast<uint32_t>((buffer[3 + offset] << 24)
                                       + (buffer[2 + offset] << 16)
                                       + (buffer[1 + offset] << 8)
                                       + buffer[0 + offset]);
        }

        static void encodeToLittleEndianBuffer(
            uint32_t uintValue, std::vector<uint8_t> &buffer, size_t offset = 0)
        {
            if (buffer.size() < offset + WORD_SIZE)
            {
                throw std::invalid_argument("encodeToLittleEndianBuffer: buffer too small");
            }
            buffer[3 + offset] = static_cast<unsigned char>(uintValue >> 24);
            buffer[2 + offset] = static_cast<unsigned char>(uintValue >> 16);
            buffer[1 + offset] = static_cast<unsigned char>(uintValue >> 8);
            buffer[0 + offset] = static_cast<unsigned char>(uintValue);
        }

        static std::vector<uint32_t> wordBufferFromByteBuffer(const std::vector<uint8_t> &input)
        {
            if (input.size() % WORD_SIZE)
            {
                throw std::invalid_argument("wordBufferFromByteBuffer: input buffer size not multiple of " + std::to_string(WORD_SIZE));
            }
            std::vector<uint32_t> output;
            output.reserve(input.size() / WORD_SIZE);
            for (size_t i = 0; i < input.size(); i += WORD_SIZE)
            {
                output.push_back(decodeFromLittleEndianBuffer(input, i));
            }
            return output;
        }

        static void byteBufferFromWordBuffer(const std::vector<uint32_t> &input, std::vector<uint8_t> &output)
        {
            output.resize(input.size() * WORD_SIZE);
            for (size_t i = 0; i < input.size(); i++)
            {
                encodeToLittleEndianBuffer(input[i], output, i * WORD_SIZE);
            }
        }

        static std::vector<uint8_t> byteBufferFromWordPointer(const uint32_t *input, size_t inputSize)
        {
            std::vector<uint32_t> inVector;
            inVector.assign(input, input + inputSize);
            std::vector<uint8_t> outVector;
            byteBufferFromWordBuffer(inVector, outVector);
            return outVector;
        }

        static void writeToWordPointerFromByteBuffer(std::vector<uint8_t> &input, uint32_t **output, size_t &outputSize)
        {
            std::vector<uint32_t> wordBuffer = wordBufferFromByteBuffer(input);
            if (outputSize < wordBuffer.size())
            {
                throw std::invalid_argument("writeToWordPointerFromByteBuffer: output size too small");
            }
            std::copy(wordBuffer.begin(), wordBuffer.end(), *output);
            outputSize = wordBuffer.size();
        }

        static std::string toHexString(uint32_t input)
        {
            std::stringstream stream;
            stream << "0x" << std::hex << input;
            return stream.str();
        }
};

#endif /* UTILS_H */
