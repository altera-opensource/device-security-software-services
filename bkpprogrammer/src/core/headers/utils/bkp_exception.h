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

#ifndef PROGRAMMER_BKP_EXCEPTION_H
#define PROGRAMMER_BKP_EXCEPTION_H

#include <exception>
#include <system_error>
#include <string>

#include "utils/string_utils.h"

namespace error_messages {
    const std::string http_error = "Http error: ";
    const std::string service_unavailable = "BKP Service is unavailable. ";
    const std::string provisioning_cancelled = "BKP Provisioning was cancelled with following message: ";
    const std::string json_error = "Json parsing BKP Service response failed: ";
    const std::string base64_error = "Base64 exception. ";
    const std::string internal_error = "Internal error. ";
    const std::string unknown_error = "Unknown exception. ";
}

enum class BkpErrc
{
    HTTP_ERROR = 3000,
    SERVICE_UNAVAILABLE  = 3200,
    PROVISIONING_CANCELLED  = 3300,
    JSON_ERROR  = 3500,
    BASE64_ERROR  = 3600,
    INTERNAL_ERROR = 3700,
};

namespace std
{
    template <>
    struct is_error_code_enum<BkpErrc> : true_type {};
}

std::error_code make_error_code(BkpErrc e);


class BkpException : public std::exception {
protected:
    std::string msg;

    std::string gen_error_code_str(std::error_code er)
    {
        return str_utils::concat_strings( {"[", std::to_string(er.value()), "] ", er.message()} );
    }

public:
    const char *what() const noexcept override {
        return msg.c_str();
    }
};

class BkpServiceResponseException : public BkpException {
public:
    std::error_code error_code = BkpErrc::PROVISIONING_CANCELLED;

    explicit BkpServiceResponseException(std::string jsonResponse) {
        msg = str_utils::concat_strings( {gen_error_code_str(error_code), std::move(jsonResponse)} );
    }
};

class BkpJsonParsingException : public BkpException {
public:
    std::error_code error_code = BkpErrc::JSON_ERROR;

    explicit BkpJsonParsingException(std::string errorMsg) {
        msg = str_utils::concat_strings( {gen_error_code_str(error_code), std::move(errorMsg)} );
    }
};

class BkpBase64Exception : public BkpException {
public:
    std::error_code error_code = BkpErrc::BASE64_ERROR;

    explicit BkpBase64Exception(std::string errorMsg) {
        msg = str_utils::concat_strings( {gen_error_code_str(error_code), std::move(errorMsg)} );
    }
};

class BkpServiceUnavailableException : public BkpException {
public:
    std::error_code error_code = BkpErrc::SERVICE_UNAVAILABLE;

    explicit BkpServiceUnavailableException(std::string errorMsg) {
        msg = str_utils::concat_strings( {gen_error_code_str(error_code), std::move(errorMsg)} );
    }
};

class BkpHttpException : public BkpException {
public:
    std::error_code error_code = BkpErrc::HTTP_ERROR;

    explicit BkpHttpException(std::string errorMsg) {
        msg = str_utils::concat_strings( {gen_error_code_str(error_code), std::move(errorMsg)} );
    }
};

class BkpInternalException : public BkpException {
public:
    std::error_code error_code = BkpErrc::INTERNAL_ERROR;

    explicit BkpInternalException(std::string errorMsg) {
        msg = str_utils::concat_strings( {gen_error_code_str(error_code), std::move(errorMsg)} );
    }
};

#endif //PROGRAMMER_BKP_EXCEPTION_H
