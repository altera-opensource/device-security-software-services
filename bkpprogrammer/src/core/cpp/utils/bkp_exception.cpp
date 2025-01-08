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

#include "utils/bkp_exception.h"

namespace {

    const std::string BKP_TAG = "BKP";

    struct BkpErrcCategory : std::error_category
    {
        const char* name() const noexcept override;
        std::string message(int event) const override;
    };

    const char *BkpErrcCategory::name() const noexcept {
        return BKP_TAG.c_str();
    }

    std::string BkpErrcCategory::message(int event) const {
        switch (static_cast<BkpErrc>(event))
        {
            case BkpErrc::HTTP_ERROR:
                return error_messages::http_error;

            case BkpErrc::SERVICE_UNAVAILABLE:
                return error_messages::service_unavailable;

            case BkpErrc::PROVISIONING_CANCELLED:
                return error_messages::provisioning_cancelled;

            case BkpErrc::JSON_ERROR:
                return error_messages::json_error;

            case BkpErrc::BASE64_ERROR:
                return error_messages::base64_error;

            case BkpErrc::INTERNAL_ERROR:
                return error_messages::internal_error;

            default:
                return error_messages::unknown_error;
        }
    }

    const BkpErrcCategory bkpErrcCategory {};
}

std::error_code make_error_code(BkpErrc e) {
    return {static_cast<int>(e), bkpErrcCategory};
}

