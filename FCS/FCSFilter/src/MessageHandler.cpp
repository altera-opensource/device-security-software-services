
/*
This project, FPGA Crypto Service Server, is licensed as below

***************************************************************************

Copyright 2020-2024 Intel Corporation. All Rights Reserved.

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

#include "MessageHandler.h"
#include "FcsCommunication.h"
#include "Logger.h"
#include "VerifierProtocol.h"

void handleIncomingMessage(
    std::vector<uint8_t> &messageBuffer,
    std::vector<uint8_t> &responseBuffer)
{
    VerifierProtocol verifierProtocol;
    if (!verifierProtocol.parseMessage(messageBuffer))
    {
        Logger::log("Couldn't parse incoming message", Error);
        verifierProtocol.prepareResponseMessage(
            std::vector<uint8_t>(), responseBuffer, verifierProtocol.getErrorCode());
        return;
    }

    std::vector<uint8_t> payloadFromFcs;
    bool fcsCallSucceeded = false;
    int32_t statusReturnedFromFcs;
    switch (verifierProtocol.getCommandCode())
    {
        case getChipId:
        {
            fcsCallSucceeded = FcsCommunication::getChipId(
                payloadFromFcs, statusReturnedFromFcs);
        }
        break;
        case sigmaTeardown:
        {
            fcsCallSucceeded = FcsCommunication::sigmaTeardown(
                verifierProtocol.getSigmaTeardownSessionId(),
                statusReturnedFromFcs);
        }
        break;
        case createAttestationSubKey:
        {
            fcsCallSucceeded = FcsCommunication::createAttestationSubkey(
                verifierProtocol.getIncomingPayload(),
                payloadFromFcs,
                statusReturnedFromFcs);
        }
        break;
        case getMeasurement:
        {
            fcsCallSucceeded = FcsCommunication::getMeasurement(
                verifierProtocol.getIncomingPayload(),
                payloadFromFcs,
                statusReturnedFromFcs);
        }
        break;
        case getAttestationCertificate:
        {
            fcsCallSucceeded = FcsCommunication::getAttestationCertificate(
                verifierProtocol.getCertificateRequest(),
                payloadFromFcs,
                statusReturnedFromFcs);
            if (statusReturnedFromFcs == -1)
            {
                Logger::log("GET_ATTESTATION_CERTIFICATE not supported by the driver. Returning unknown command.");
                verifierProtocol.prepareEmptyResponseMessage(
                responseBuffer, unknownCommand);
                return;
            }
        }
        break;
        case sigmaM1:
        case sigmaM3:
        case sigmaEnc:
        case mctp:
        case getIdCode:
        case getDeviceIdentity:
        {
            fcsCallSucceeded = FcsCommunication::mailboxGeneric(
                verifierProtocol.getCommandCode(),
                verifierProtocol.getIncomingPayload(),
                payloadFromFcs,
                statusReturnedFromFcs);
        }
        break;
        default:
        {
            Logger::log("Command code not recognized: "
                + std::to_string(verifierProtocol.getCommandCode()));
            verifierProtocol.prepareEmptyResponseMessage(
                responseBuffer, unknownCommand);
            return;
        }
        break;
    }
    if (!fcsCallSucceeded)
    {
        /*
        don't prepare any message, server should disconnect
        - emulates system console behavior
        */
        return;
    }
    verifierProtocol.prepareResponseMessage(
        payloadFromFcs, responseBuffer, statusReturnedFromFcs);
}
