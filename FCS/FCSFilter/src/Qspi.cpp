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

#include "Logger.h"
#include "Qspi.h"
#include "CommandHeader.h"
#include "utils.h"
#include <stdexcept>

void Qspi::callFcs(uint32_t commandCode,
             std::vector<uint8_t> &inBuffer,
             std::vector<uint8_t> &outBuffer,
             std::string functionName)
{
    int32_t statusReturnedFromFcs = -1;
    std::string payloadHex;
    for (auto byteValue : inBuffer)
    {
        payloadHex += Utils::toHexString(byteValue) + " ";
    }
    Logger::log("Calling FCS. Payload: " + payloadHex, Debug);
    if (!FcsCommunication::mailboxGeneric(commandCode, inBuffer, outBuffer, statusReturnedFromFcs))
    {
        throw std::runtime_error(functionName + ": FCS call failed.");
    }
    if (statusReturnedFromFcs != 0)
    {
        throw std::runtime_error(functionName + ": Error code: " + Utils::toHexString(statusReturnedFromFcs));
    }
    std::string outputHex;
    for (auto byteValue : outBuffer)
    {
        outputHex += Utils::toHexString(byteValue) + " ";
    }
    Logger::log("Output from FCS: " + outputHex, Debug);
}

void Qspi::qspiOpen()
{
    Logger::log("QSPI Open", Debug);
    std::vector<uint8_t> request;
    std::vector<uint8_t> response;
    callFcs(CommandCodes::QSPI_OPEN, request, response, "qspiOpen");
}

void Qspi::qspiClose()
{
    Logger::log("QSPI Close", Debug);
    std::vector<uint8_t> request;
    std::vector<uint8_t> response;
    callFcs(CommandCodes::QSPI_CLOSE, request, response, "qspiClose");
}

void Qspi::qspiSetCs(uint8_t cs, bool mode, bool ca)
{
    Logger::log("QSPI Set CS", Debug);
    if (cs > 0xF)
    {
        throw std::invalid_argument("CS should fit on 4 bits");
    }

    uint32_t payload = 0;
    payload |= cs << 28;
    payload |= mode << 27;
    payload |= ca << 26;
    std::vector<uint8_t> request(sizeof(uint32_t), 0);
    Utils::encodeToLittleEndianBuffer(payload, request);

    std::vector<uint8_t> response;

    callFcs(CommandCodes::QSPI_SET_CS, request, response, "qspiSetCs");
}

void Qspi::qspiErase(uint32_t address, uint32_t sizeInWords)
{
    if (!is4kAligned(address))
    {
        throw std::invalid_argument("qspiErase: address should be aligned to 4KB");
    }
    if (!is4kAligned(sizeInWords))
    {
        throw std::invalid_argument("qspiErase: size should be aligned to 4KB");
    }
    std::vector<uint8_t> request(2 * sizeof(uint32_t), 0);
    Utils::encodeToLittleEndianBuffer(address, request);
    Utils::encodeToLittleEndianBuffer(sizeInWords, request, sizeof(uint32_t));
    std::vector<uint8_t> response;
    callFcs(CommandCodes::QSPI_ERASE, request, response, "qspiErase");
}

void Qspi::qspiReadMultiple(uint32_t address, uint32_t sizeInWords, std::vector<uint8_t> &outBuffer)
{
    uint32_t wordsLeft = sizeInWords;
    do
    {
        uint32_t wordsToRead = wordsLeft > maxTransferSizeInWords ? maxTransferSizeInWords : wordsLeft;
        std::vector<uint8_t> readBuffer(wordsToRead * WORD_SIZE);
        uint32_t byteOffset = (sizeInWords - wordsLeft) * WORD_SIZE;
        qspiRead(address + byteOffset, wordsToRead, readBuffer);
        std::copy(
                readBuffer.begin(),
                readBuffer.end(),
                std::back_inserter(outBuffer));
        wordsLeft -= wordsToRead;
    } while (wordsLeft > 0);
}

void Qspi::qspiWriteMultiple(uint32_t address, uint32_t sizeInWords, std::vector<uint8_t> &inBuffer)
{
    uint32_t wordsLeft = sizeInWords;
    do
    {
        uint32_t wordsToWrite = wordsLeft > maxTransferSizeInWords ? maxTransferSizeInWords : wordsLeft;
        std::vector<uint8_t> writeBuffer;
        uint32_t byteOffset = (sizeInWords - wordsLeft) * WORD_SIZE;
        std::copy(
                inBuffer.begin() + byteOffset,
                inBuffer.begin() + byteOffset + (wordsToWrite * WORD_SIZE),
                std::back_inserter(writeBuffer));
        Qspi::qspiWrite(address + byteOffset, wordsToWrite, writeBuffer);
        wordsLeft -= wordsToWrite;
    } while (wordsLeft > 0);
}

void Qspi::qspiRead(uint32_t address, uint32_t sizeInWords, std::vector<uint8_t> &outBuffer)
{
    std::vector<uint8_t> request(2 * sizeof(uint32_t));
    Utils::encodeToLittleEndianBuffer(address, request);
    Utils::encodeToLittleEndianBuffer(sizeInWords, request, sizeof(uint32_t));
    Logger::log("qspiRead called. Address: " + Utils::toHexString(address) +
                " Length in words: " + Utils::toHexString(sizeInWords), Debug);
    callFcs(CommandCodes::QSPI_READ, request, outBuffer, "qspiRead");
}

void Qspi::qspiWrite(uint32_t address, uint32_t sizeInWords, std::vector<uint8_t> &inBuffer)
{
    std::vector<uint8_t> request(2 * sizeof(uint32_t));
    Utils::encodeToLittleEndianBuffer(address, request);
    Utils::encodeToLittleEndianBuffer(sizeInWords, request, sizeof(uint32_t));
    std::copy (inBuffer.begin(), inBuffer.end(), std::back_inserter(request));
    std::vector<uint8_t> response;
    callFcs(CommandCodes::QSPI_WRITE, request, response, "qspiWrite");
}

void Qspi::qspiAssert(bool condition, std::string exceptionMessage) 
{
     if (!condition)
     {
        throw std::invalid_argument(exceptionMessage);
     }
 
}

bool Qspi::qspiVerify(uint32_t address, uint32_t sizeInWords, std::vector<uint8_t> inBuffer)
{
    qspiAssert(((address % 4) == 0), "qspiVerify: start address must be word aligned");
    qspiAssert(((sizeInWords % 16) == 0), "qspiVerify: start address must be word aligned");
    qspiAssert(!inBuffer.empty(), "qspiVerify: input buffer should not be empty");
    Logger::log("qspiVerify called. start address = " + Utils::toHexString(address), Debug);
    std::vector<uint8_t> respBuf;
    qspiReadMultiple(address, sizeInWords, respBuf);
    bool status = !respBuf.empty();
    if (status)
    {
        uint32_t offset = 0;
        for (offset = 0; status && offset < (sizeInWords * 4); offset++)
        {
            status = (inBuffer[offset] == respBuf[offset]);
        }

        if (!status)
        {
            Logger::log("Verification failed at address " + Utils::toHexString(address + offset), Error);
        }
    }

    return status;
}
