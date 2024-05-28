/*
This project, FPGA Crypto Service Server, is licensed as below

***************************************************************************

Copyright 2023 Intel Corporation. All Rights Reserved.

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

#ifndef FCS_QSPI_H
#define FCS_QSPI_H

#include "FcsCommunication.h"
#include "utils.h"
#include <string>

class Qspi
{
    enum CommandCodes
    {
        QSPI_OPEN = 0x32,
        QSPI_CLOSE = 0x33,
        QSPI_SET_CS = 0x34,
        QSPI_ERASE = 0x38,
        QSPI_WRITE = 0x39,
        QSPI_READ = 0x3A,
    };
public:
    static void qspiOpen();
    static void qspiClose();
    static void qspiSetCs(uint8_t cs, bool mode, bool ca);
    static void qspiErase(uint32_t address, uint32_t sizeInWords);
    static void qspiWriteMultiple(uint32_t address, uint32_t sizeInWords, std::vector<uint8_t> &inBuffer);
    static void qspiReadMultiple(uint32_t address, uint32_t sizeInWords, std::vector<uint8_t> &outBuffer);
    static bool qspiVerify(uint32_t address, uint32_t sizeInWords, std::vector<uint8_t> inBuffer);

private:
    static void qspiRead(uint32_t address, uint32_t sizeInWords, std::vector<uint8_t> &outBuffer);
    static void qspiWrite(uint32_t address, uint32_t sizeInWords, std::vector<uint8_t> &inBuffer);
    static void callFcs(uint32_t commandCode,
                 std::vector<uint8_t> &inBuffer,
                 std::vector<uint8_t> &outBuffer,
                 std::string functionName);

    static bool is4kAligned(size_t address) { return !(address % (0x400 * WORD_SIZE)); };
    static const uint32_t maxTransferSizeInWords = 0x200; //2048 bytes
    static void qspiAssert(bool condition, std::string exceptionMessage);
};

#endif //FCS_QSPI_H
