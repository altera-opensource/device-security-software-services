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

#ifndef BKP_APP_CONTEXT_H
#define BKP_APP_CONTEXT_H

#include "pgm_plugin_bkp_export.h"
#include "Puf.h"
#include <boost/program_options.hpp>
#include "string.h"
#include <fstream>

namespace po = boost::program_options;

class Context : public QuartusContext
{
public:
    void log(LogLevel_t level, const std::string& msg) override;
    Status_t send_message(Message_t messageType, const uint32_t *inBuf, size_t inBufSize, uint32_t **outBuf,
                          size_t &outBufSize) override;
    std::string get_config(const std::string& key) override;
    unsigned int get_supported_commands() override;

    void readConfig(std::string fileName);

    void setShouldSaveWkeyToMachine(bool param) { shouldSaveWkeyToMachine = param; }
private:
    void writeWkeyToFile(std::vector<uint8_t> wkeyData);
    void writeWkeyToFlash(std::vector<uint8_t> wkeyData, PufType_t pufType);
    void writePufHelpDataToFlash(std::vector<uint8_t> pufHelpData, PufType_t pufType);
    static const inline std::string wkeyFilename = "bkp_auto.wkey";
    po::variables_map parsedConfig;
    bool shouldSaveWkeyToMachine = false;
};


#endif //BKP_APP_CONTEXT_H
