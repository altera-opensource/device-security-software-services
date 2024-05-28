/*
 * This project is licensed as below.
 *
 * **************************************************************************
 *
 * Copyright 2020-2023 Intel Corporation. All Rights Reserved.
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

#ifndef PROGRAMMER_BKP_MASTER_H
#define PROGRAMMER_BKP_MASTER_H

#include <utility>

#include "pgm_plugin_bkp_export.h"
#include "network/network_message.h"
#include "network/messages/get_next.h"
#include "network/messages/prefetch_next.h"
#include "network/messages/puf_activate_next.h"
#include "network/messages/set_authority_next.h"
#include "network/network_config.h"
#include "network/network_wrapper.h"
#include "quartus/jtag_helper.h"
#include "utils/structures.h"
#include "logger.h"

namespace operation_status {
    const std::string completed = " COMPLETED.";
    const std::string failed = " FAILED.";
}

namespace provisioning_stage {
    const std::string received_from_bkps = "Received commands from BKPS.";
    const std::string received_from_quartus = "Received response from Quartus.";
}

namespace communication_status {
    const std::string comm_done = "done";
    const std::string comm_continue = "continue";
}

enum API_VERSION {
    V1 = 1
};

static const API_VERSION CURRENT_API_VERSION = API_VERSION::V1;

class BkpMaster
{
private:
    mutable std::string operationTypeString;
    QuartusContext *qc;
    std::shared_ptr<TransactionIdManager> txIdMgr;
    std::shared_ptr<Logger> logger;
    std::shared_ptr<NetworkWrapper> network;
    std::shared_ptr<JtagHelper> jtag;

    Config getConfigFromQuartusContext() const;
    bool isStatusContinue(NetworkMessage &msg) const;
    bool isStatusDone(NetworkMessage &msg) const;
    std::string getCfgId() const;
    uint32_t getSupportedCommands() const;
    std::vector<Response> send_commands_to_quartus(std::vector<Command> jtagCommands) const;
    void logReceivedBkps() const;
    void logReceivedQuartus() const;
    Status_t reportSuccess() const;
    Status_t reportError() const;
    Status_t reportError(const std::exception &ex) const;
    Status_t reportExceededMaxIterationCount() const;
    Status_t perform_message_exchange(NetworkMessage &networkMessage) const;

public:

    static const int MAX_ITERATION_COUNT = 100;

    explicit BkpMaster(QuartusContext *qc_) :
        qc(qc_),
        txIdMgr(std::make_shared<TransactionIdManagerImpl>()),
        logger(std::make_shared<LoggerImpl>(qc_, txIdMgr)),
        network(std::make_shared<CurlWrapper>(logger, txIdMgr)),
        jtag(std::make_shared<JtagHelperImpl>(qc_, logger)) {};

    explicit BkpMaster(QuartusContext *qc_, std::shared_ptr<TransactionIdManager> txIdMgr_, std::shared_ptr<Logger> logger_,
                std::shared_ptr<NetworkWrapper> network_, std::shared_ptr<JtagHelper> jtag_) :
            qc(qc_),
            txIdMgr(std::move(txIdMgr_)),
            logger(std::move(logger_)),
            network(std::move(network_)),
            jtag(std::move(jtag_)) {};

    ~BkpMaster() = default;

    BkpMaster(const BkpMaster &L) = delete;

    BkpMaster &operator=(const BkpMaster &L) = delete;

    Status_t provision() const;
    Status_t prefetch() const;
    Status_t pufActivate(PufType_t pufType) const;
    Status_t setAuthority(PufType_t pufType, uint8_t slotId, bool force_enrollment) const;
};

#endif //PROGRAMMER_BKP_MASTER_H
