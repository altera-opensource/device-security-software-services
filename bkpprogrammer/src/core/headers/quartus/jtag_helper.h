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
#ifndef PROGRAMMER_JTAG_HELPER_H
#define PROGRAMMER_JTAG_HELPER_H

#include <string>
#include <utility>

#include "pgm_plugin_bkp_export.h"
#include "utils/base64_utils.h"
#include "utils/bkp_exception.h"
#include "utils/structures.h"
#include "logger.h"

#ifndef _WIN32
#include "quartus/protected_memory.h"
#endif // !_WIN32

static const size_t DWORD_SIZE_IN_BYTES = 4;
static const size_t BUF_OUT_SIZE_MAX_IN_BYTES = 4096; //4kB
static const size_t BUF_OUT_SIZE_MAX_IN_4BYTE_WORDS = BUF_OUT_SIZE_MAX_IN_BYTES / DWORD_SIZE_IN_BYTES;

//Interface
class JtagHelper {
protected:
    JtagHelper() = default;

public:
    virtual ~JtagHelper() = default;

    virtual Status_t exchange_jtag_cmd(const Command &command, std::string &string) = 0;
};

class JtagHelperImpl : public JtagHelper
{
private:
    QuartusContext *qc;
    std::shared_ptr<Logger> logger;

    static size_t bytesTo4ByteWords(size_t sizeBytes);
    static size_t fourByteWordsToBytes(size_t size4ByteWords);
#ifndef _WIN32
    static bool is_page_guarded(ProtectedMemory::PageState state);
#endif // !_WIN32
    static bool is_result_ok(Status_t result);
    static bool is_buffer_size_in_range(size_t bufOutSizeIn4ByteWords);
    std::string encode_command(const std::string &decodedCmd);
    std::string decode_command(const std::string &encodedCmd);
    Status_t exchange_jtag_cmd_internal(const Message_t messageType, const std::string &encodedCommand,
                                        std::string &outResponse);

public:
    explicit JtagHelperImpl(QuartusContext *qc_, std::shared_ptr<Logger> logger_)  :
        qc(qc_), logger(std::move(logger_)) {};

    ~JtagHelperImpl() override = default;

    JtagHelperImpl(const JtagHelperImpl &L) = delete;

    JtagHelperImpl &operator=(const JtagHelperImpl &L) = delete;

    Status_t exchange_jtag_cmd(const Command &command, std::string &outResponse) override;

    std::string prepareResponse(const uint32_t *outBuffer, const size_t bufOutSizeIn4ByteWords);
};

#endif //PROGRAMMER_JTAG_HELPER_H
