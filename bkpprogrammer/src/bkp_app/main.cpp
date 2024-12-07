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

#include "pgm_plugin_bkp_export.h"
#include "utils/structures.h"
#include "Context.h"
#include "Logger.h"

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#define MAX_SLOT_ID 7
using namespace boost::algorithm;

const std::map<std::string, PufType_t> PufTypeMap
{
    { "UDS_EFUSE", UDS_EFUSE },
    { "UDS_IID", UDS_IID },
    { "UDS_INTEL", UDS_INTEL }
};

po::options_description commandlineDesc("If none of bkp_prefetch|bkp_puf_activate|bkp_set_authority is specified then bkp_provision will run as a default.\nOptions:");
po::variables_map vm;

bool getPufTypeFromString(std::string pufTypeString, PufType_t &outPufType)
{
    auto itr = PufTypeMap.find(pufTypeString);
    if (itr == PufTypeMap.end())
    {
        Logger::log("Unrecognized PUF type", Error);
        return false;
    }
    outPufType = itr->second;
    return true;
}

void exitWithMessage(std::string message, uint32_t exitCode=1)
{
    std::stringstream helpStream;
    commandlineDesc.print(helpStream);
    Logger::log("ERROR: " + message, Fatal);
    Logger::log(helpStream.str(), Fatal);
    exit(exitCode);
}

PufType_t extractPufType(std::vector<PufType_t> allowedTypes)
{
    if (!vm.count("puf_type"))
    {
        exitWithMessage("puf_type parameter is required");
    }
    PufType_t pufType;
    if (!getPufTypeFromString(vm["puf_type"].as<std::string>(), pufType)
        || std::find(allowedTypes.begin(), allowedTypes.end(), pufType) == allowedTypes.end())
    {
        exitWithMessage("Incorrect PUF type.");
    }
    return pufType;
}

uint8_t extractSlotId()
{
    if (!vm.count("slot_id"))
    {
        exitWithMessage("slot_id parameter is required");
    }
    int slotId = vm["slot_id"].as<int>();
    if (slotId < 0 || slotId > MAX_SLOT_ID)
    {
        exitWithMessage("Incorrect slot_id. Allowed: 0..7");
    }
    return slotId;
};

bool extractForceEnrollment()
{
    return vm.count("force_enrollment");
}

void checkUserInputs(po::options_description opts, int argc, char* argv[])
{
    auto opts_vec = opts.options();
    std::string token;
    auto search_arg_func = [&token](boost::shared_ptr<boost::program_options::option_description> opt)
    {
        bool status = false;
        if (token.find("=") != std::string::npos)
        {
            token = token.substr(0, token.find("="));
        }
        if (opt.get()->match(token, false, false, false) == boost::program_options::option_description::full_match)
        {
            return true;
        }
        return status;
    };

    std::vector<std::string> args(argv, argv + argc);
    for (auto& arg : args)
    {
        token = arg;
        if (starts_with(token, "--"))
        {
            token = token.substr(2);
            if (std::find_if(opts_vec.begin(), opts_vec.end(), search_arg_func) == opts_vec.end())
            {
                exitWithMessage("Command line has invalid argument " + arg + ". Please cross-check with --help message.");
                break;
            }
        }
    }
}

int main(int argc, char* argv[]) try
{
    commandlineDesc.add_options()
        ("help,h", "Print help.\n")
        ("log_level,l", po::value<std::string>()->default_value("Info"), "Don't show logs below this level. Allowed: Debug|Info|Warning|Error|Fatal\n")
        ("bkp_prefetch", "Request Black Key Provisioning Service to cache certificate chain for the device.\n")
        ("bkp_puf_activate", "Request Black Key Provisioning Service to Activate PUF on the device using PUF Helper Data.\n")
        ("bkp_set_authority", "Request Black Key Provisioning Service to set Intel authority chain on the device to selected SPDM slot with chain of certificates issued by Intel per requested PUF type.\n")
        ("puf_type", po::value<std::string>(), "For bkp_puf_activate (required): UDS_IID|UDS_INTEL\nFor bkp_set_authority (required): UDS_EFUSE|UDS_IID|UDS_INTEL\n")
        ("slot_id", po::value<int>(), "For bkp_set_authority (required): <0..7>.\n")
        ("force_enrollment", "Force provisioning of chain with Enrollment#0 certificate instead of DeviceID certificate\n")
        ("bkp_options", po::value<std::string>()->default_value("bkp_options.txt"), "Config file name.\n")
        ("ini", po::value<std::string>(), "INI configuration string.\n");

    try
    {
        checkUserInputs(commandlineDesc, argc, argv);
        po::store(po::parse_command_line(argc, argv, commandlineDesc), vm);
        po::notify(vm);
    }
    catch (boost::program_options::error &e)
    {
        exitWithMessage(e.what());
    }

    if (vm.count("help"))
    {
        exitWithMessage("BKP App version: " + std::string(get_version()), 0);
    }
    if (!Logger::setCurrentLogLevel(vm["log_level"].as<std::string>()))
    {
        exitWithMessage("Incorrect log level passed in parameters: " + vm["log_level"].as<std::string>());
    }

    Context context;
    context.readConfig(vm["bkp_options"].as<std::string>());

    if (vm.count("ini"))
    {
        std::string iniContent = vm["ini"].as<std::string>();
        boost::algorithm::to_lower(iniContent);
        std::stringstream ss;
        ss << iniContent;
        boost::property_tree::ptree pt;
        boost::property_tree::ini_parser::read_ini(ss, pt);
        context.setShouldSaveWkeyToMachine(pt.get<bool>("bkp_save_wkey_to_machine"));
    }

    if (vm.count("bkp_prefetch"))
    {
        return bkp_prefetch(context);
    }

    if (vm.count("bkp_puf_activate"))
    {
        return bkp_puf_activate(context,
                                extractPufType({UDS_IID, UDS_INTEL}));
    }

    if (vm.count("bkp_set_authority"))
    {
        return bkp_set_authority(context,
                                 extractPufType({UDS_EFUSE, UDS_IID, UDS_INTEL}),
                                 extractSlotId(),
                                 extractForceEnrollment());
    }

    return bkp_provision(context);
}
catch (const std::exception& ex)
{
    Logger::log(ex.what(), Fatal);
    exit(1);
}
