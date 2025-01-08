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

#include <stdint.h>

#include "CommandHeader.h"
#include "utils.h"

void CommandHeader::parse(std::vector<uint8_t> &buffer)
{
    uint32_t header = Utils::decodeFromLittleEndianBuffer(buffer);
    fromUint32(header);
}

void CommandHeader::encode(std::vector<uint8_t> &buffer)
{
    uint32_t header = toUint32();
    Utils::encodeToLittleEndianBuffer(header, buffer);
}

void CommandHeader::fromUint32(uint32_t input)
{
    code = readBits(input, 11);
    res2 = readBits(input, 1);
    length = readBits(input, 11);
    res1 = readBits(input, 1);
    id = readBits(input, 4);
    client = readBits(input, 4);
}

uint32_t CommandHeader::toUint32()
{
    uint32_t output = 0;
    writeBits(output, client, 4);
    writeBits(output, id, 4);
    writeBits(output, res1, 1);
    writeBits(output, length, 11);
    writeBits(output, res2, 1);
    writeBits(output, code, 11);
    return output;
}

uint32_t CommandHeader::readBits(uint32_t &source, uint8_t numberOfBits)
{
    uint32_t mask = (1 << numberOfBits) - 1;
    uint32_t result = source & mask;
    source >>= numberOfBits;
    return result;
}

void CommandHeader::writeBits(
    uint32_t &destination,
    uint32_t source,
    uint8_t numberOfBits)
{
    destination <<= numberOfBits;
    uint32_t mask = (1 << numberOfBits) - 1;
    destination |= (source & mask);
}
