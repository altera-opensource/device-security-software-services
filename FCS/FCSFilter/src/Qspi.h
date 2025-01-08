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

#ifndef FCS_QSPI_H
#define FCS_QSPI_H

#include "FcsCommunication.h"
#include "utils.h"
#include <string>

class Qspi
{
public:
    enum CommandCodes
    {
        GET_IDCODE = 0x10,
        QSPI_OPEN = 0x32,
        QSPI_CLOSE = 0x33,
        QSPI_SET_CS = 0x34,
        QSPI_ERASE = 0x38,
        QSPI_WRITE = 0x39,
        QSPI_READ = 0x3A,
        MCTP = 0x194,
        GET_DEVICE_IDENTITY = 0x500
    };

    enum SDM_RESPONSE_CODE
    {
        SDM_RESPONSE_CODE_OK = 0,                               //0
        SDM_RESPONSE_CODE_INVALID_CMD,                          //1
        SDM_RESPONSE_CODE_UNKNOWN_BR,                           //2
        SDM_RESPONSE_CODE_UNKNOWN,                              //3
        SDM_RESPONSE_CODE_INVALID_COMMAND_PARAMS    = 0x04,     //4
        SDM_RESPONSE_CODE_CMD_INVALID_ON_SOURCE     = 0x06,     //6
        SDM_RESPONSE_CODE_CLIENT_ID_NO_MATCH        = 0x08,     //8
        SDM_RESPONSE_CODE_INVALID_ADDRESS           = 0x09,     //9
        SDM_RESPONSE_CODE_AUTHENTICATION_FAIL       = 0x0A,     //10
        SDM_RESPONSE_CODE_TIMEOUT                   = 0x0B,     //11
        SDM_RESPONSE_CODE_HW_NOT_READY              = 0x0C,     //12
        SDM_RESPONSE_CODE_HW_ERROR                  = 0x0D,     //13
        SDM_RESPONSE_CODE_SYNC_LOST                 = 0x0E,     //14
        SDM_RESPONSE_CODE_FUNCTION_NOT_SUPPORT      = 0x0F,     //15

        SDM_RESPONSE_CODE_QSPI_HW_ERROR             = 0x080,    //128
        SDM_RESPONSE_CODE_QSPI_ALREADY_OPEN         = 0x081,    //129
        SDM_RESPONSE_CODE_EFUSE_SYSTEM_FAILURE      = 0x082,    //130

        SDM_RESPONSE_CODE_NOT_CONFIGURED            = 0x100,    //256
        SDM_RESPONSE_CODE_DEVICE_BUSY               = 0x1FF,    //511
        SDM_RESPONSE_CODE_FLASH_ACCESS_DENIED       = 0x2FF,    //767
        SDM_RESPONSE_CODE_NO_VALID_RESP_AVAILABLE   = 0x2FF,    //767
        SDM_RESPONSE_CODE_RESPONSE_ERROR            = 0x3FF,    //1023

        //INTEL PUF
        SDM_RESPONSE_CODE_INTEL_PUF_ACTIVATION_FAILED = 0x510,  //1296
        SDM_RESPONSE_CODE_PUF_HELPER_FILE_READ_ERROR  = 0x511,  //1297
        SDM_RESPONSE_CODE_NOT_PROVISIONED_TO_USE_INTEL_PUF  = 0x512,  //1298
        SDM_RESPONSE_CODE_NOT_FUSED_FOR_INTEL_PUF     = 0x513,  //1299
        SDM_RESPONSE_CODE_UDS_EFUSE_ERROR             = 0x514,  //1300

        // Always add the new enum before here
        SDM_RESPONSE_CODE_LAST
    };

    static bool qspiOpen();
    static bool qspiClose();
    static bool qspiSetCs(uint8_t cs, bool mode, bool ca);
    static bool qspiErase(uint32_t address, uint32_t sizeInWords);
    static bool qspiWriteMultiple(uint32_t address, uint32_t sizeInWords, std::vector<uint8_t> &inBuffer);
    static bool qspiReadMultiple(uint32_t address, uint32_t sizeInWords, std::vector<uint8_t> &outBuffer);
    static bool qspiVerify(uint32_t address, uint32_t sizeInWords, std::vector<uint8_t> inBuffer);

private:
    static bool qspiRead(uint32_t address, uint32_t sizeInWords, std::vector<uint8_t> &outBuffer);
    static bool qspiWrite(uint32_t address, uint32_t sizeInWords, std::vector<uint8_t> &inBuffer);
    static bool callFcs(uint32_t commandCode,
                 std::vector<uint8_t> &inBuffer,
                 std::vector<uint8_t> &outBuffer,
                 std::string functionName);

    static bool is4kAligned(size_t address) { return !(address % (0x400 * WORD_SIZE)); };
    static const uint32_t maxTransferSizeInWords = 0x200; //2048 bytes
    static void qspiAssert(bool condition, std::string exceptionMessage);
    static std::string decode_sdm_response_error_code(uint32_t error_code);
};

#endif //FCS_QSPI_H
