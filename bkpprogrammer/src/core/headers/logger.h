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

#ifndef PROGRAMMER_LOGGER_H
#define PROGRAMMER_LOGGER_H

#include <iostream>
#include <memory>
#include <utility>

#include "pgm_plugin_bkp_export.h"
#include "utils/transaction_id_manager.h"
#include "utils/string_utils.h"

#ifndef LOG
    #define LOG(...) logger->log(__VA_ARGS__)
#endif

//Interface
class Logger {
protected:
    Logger() = default;

public:
    virtual ~Logger() = default;

    virtual void log(LogLevel_t level, std::string message) const = 0;
};

class LoggerImpl : public Logger {
private:
    QuartusContext *qc;
    std::shared_ptr<TransactionIdManager> txIdMgr;

public:
    explicit LoggerImpl(QuartusContext *qc_, std::shared_ptr<TransactionIdManager> txIdMgr_) :
            qc(qc_), txIdMgr(std::move(txIdMgr_)) {};

    ~LoggerImpl() override = default;
    LoggerImpl(const LoggerImpl &L) = delete;
    LoggerImpl &operator=(const LoggerImpl &L) = delete;

    void log(LogLevel_t level, std::string message) const override;
    std::string getTransactionId() const;
};

namespace logMessages {
    namespace proxy {
        const std::string set_to = "Proxy is set to: ";
        const std::string no_proxy = "No proxy is provided.";
        const std::string no_proxy_auth = "No proxy authentication is provided.";
    }
    namespace cert {
        const std::string load_root = "Using server root certificate for MTLS: ";
        const std::string load_client = "Using client certificate for MTLS: ";
        const std::string load_key = "Using client private key for MTLS: ";
    }
    namespace password {
        const std::string looking_for_password = "Looking for password for MTLS private key in ";
    }
}

#endif //PROGRAMMER_LOGGER_H
