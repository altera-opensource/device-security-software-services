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

#include <mutex>

#include "pgm_plugin_bkp_export.h"
#include "bkp_master.h"
#include "version.h"

std::mutex bkp_mutex;

const char* get_version()
{
    return BKP_PROGRAMMER_VERSION;
}

void printVersion(QuartusContext &qc)
{
    qc.log(L_INFO, "BKP Plugin version: " + std::string(get_version()));
}

void printVersionWithMessage(QuartusContext &qc, std::string message)
{
    printVersion(qc);
    qc.log(L_INFO, message);
}

Status_t bkp_provision(QuartusContext &qc)
{
    std::lock_guard<std::mutex> m(bkp_mutex);
    printVersionWithMessage(qc, "Calling bkp_provision");
    BkpMaster bkpMaster(&qc);
    return bkpMaster.provision();
}

Status_t bkp_prefetch(QuartusContext &qc)
{
    std::lock_guard<std::mutex> m(bkp_mutex);
    printVersionWithMessage(qc, "Calling bkp_prefetch");
    BkpMaster bkpMaster(&qc);
    return bkpMaster.prefetch();
}

Status_t bkp_puf_activate(QuartusContext &qc, PufType_t puf_type)
{
    std::lock_guard<std::mutex> m(bkp_mutex);
    printVersionWithMessage(qc, "Calling bkp_puf_activate");
    BkpMaster bkpMaster(&qc);
    return bkpMaster.pufActivate(puf_type);
}

Status_t bkp_set_authority(QuartusContext &qc, PufType_t puf_type, uint8_t slot_id, bool force_enrollment)
{
    std::lock_guard<std::mutex> m(bkp_mutex);
    printVersionWithMessage(qc, "Calling bkp_set_authority");
    BkpMaster bkpMaster(&qc);
    return bkpMaster.setAuthority(puf_type, slot_id, force_enrollment);
}
