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
#include "gtest/gtest.h"
#include "FcsCommunicationFcsLib.h"
#include "FcsSimulator.h"
class FcsCommunicationFcsLibUT : public ::testing::Test {
public:
    FcsCommunication* fcsCommunication = nullptr;
    virtual ~FcsCommunicationFcsLibUT() {
    }

    virtual void TearDown() {
        delete fcsCommunication;
        fcsCommunication = nullptr;
    }

};

TEST_F(FcsCommunicationFcsLibUT, loadLibrary)
{
    fcsCommunication = FcsCommunication::getFcsCommunication();
    assert(fcsCommunication);
}

TEST_F(FcsCommunicationFcsLibUT, getChipIdTest)
{
    std::vector<uint32_t> expectedPayload {0x00002000, 0xA36F424D, 0x448D6710};
    fcsCommunication = FcsCommunication::getFcsCommunication();
    int32_t statusReturnedFromFcs = -1;
    VerifierProtocol verifierProtocol;
    verifierProtocol.setCommandCode(SDM_COMMAND_CODE::GET_CHIPID);
    std::vector<uint8_t> responseBuffer(0);
    EXPECT_TRUE(fcsCommunication->runCommandCode(verifierProtocol, responseBuffer, statusReturnedFromFcs));
    EXPECT_EQ(0, statusReturnedFromFcs);
    EXPECT_EQ(Utils::byteBufferFromWordPointer(expectedPayload.data(), expectedPayload.size()), responseBuffer);
}

TEST_F(FcsCommunicationFcsLibUT, getIdCodeTest)
{
    std::vector<uint32_t> expectedPayload {0x00001000, 0x0364F0DD};
    fcsCommunication = FcsCommunication::getFcsCommunication();
    int32_t statusReturnedFromFcs = -1;
    VerifierProtocol verifierProtocol;
    verifierProtocol.setCommandCode(SDM_COMMAND_CODE::GET_JTAG_IDCODE);
    std::vector<uint8_t> responseBuffer(0);
    EXPECT_TRUE(fcsCommunication->runCommandCode(verifierProtocol, responseBuffer, statusReturnedFromFcs));
    EXPECT_EQ(0, statusReturnedFromFcs);
    EXPECT_EQ(Utils::byteBufferFromWordPointer(expectedPayload.data(), expectedPayload.size()), responseBuffer);
}

TEST_F(FcsCommunicationFcsLibUT, getAttestationCertTest)
{
    std::vector<uint32_t> certRequest {0x00000004};
    fcsCommunication = FcsCommunication::getFcsCommunication();
    int32_t statusReturnedFromFcs = -1;
    VerifierProtocol verifierProtocol;
    verifierProtocol.setCommandCode(SDM_COMMAND_CODE::GET_ATTESTATION_CERTIFICATE);
    verifierProtocol.setIncomingPayload(Utils::byteBufferFromWordPointer(certRequest.data(), certRequest.size()));
    std::vector<uint8_t> responseBuffer;
    EXPECT_TRUE(fcsCommunication->runCommandCode(verifierProtocol, responseBuffer, statusReturnedFromFcs));
    EXPECT_EQ(0, statusReturnedFromFcs);
    std::vector<uint8_t> expectedPayload(ATTESTATION_SUBKEY_CMD_MAX_SZ);
    uint32_t expectedPayloadSize;
    FcsSimulator::fcs_get_response("GET_ATTESTATION_CERTIFICATE", expectedPayload.data(), &expectedPayloadSize);
    expectedPayload.resize(expectedPayloadSize);
    uint32_t header = ((expectedPayloadSize / WORD_SIZE) << 12);
    std::vector<uint8_t> headerBuffer = Utils::byteBufferFromWordPointer(&header, 1);
    std::copy(expectedPayload.begin(), expectedPayload.end(), std::back_inserter(headerBuffer));
    EXPECT_EQ(headerBuffer.size(), responseBuffer.size());
    EXPECT_EQ(headerBuffer, responseBuffer);
}

TEST_F(FcsCommunicationFcsLibUT, MctpGetVersionTest)
{
    std::vector<uint32_t> payload {0x05130000, 0x00008410};
    fcsCommunication = FcsCommunication::getFcsCommunication();
    int32_t statusReturnedFromFcs = -1;
    VerifierProtocol verifierProtocol;
    verifierProtocol.setCommandCode(SDM_COMMAND_CODE::MCTP);
    verifierProtocol.setIncomingPayload(Utils::byteBufferFromWordPointer(payload.data(), payload.size()));
    std::vector<uint8_t> responseBuffer;
    EXPECT_TRUE(fcsCommunication->runCommandCode(verifierProtocol, responseBuffer, statusReturnedFromFcs));
    EXPECT_EQ(0, statusReturnedFromFcs);
    std::vector<uint8_t> expectedPayload(MBOX_SEND_RSP_MAX_SZ);
    uint32_t expectedPayloadSize;
    FcsSimulator::fcs_get_response("MCTP", expectedPayload.data(), &expectedPayloadSize);
    expectedPayload.resize(expectedPayloadSize);
    uint32_t header = ((expectedPayloadSize / WORD_SIZE) << 12);
    std::vector<uint8_t> headerBuffer = Utils::byteBufferFromWordPointer(&header, 1);
    std::copy(expectedPayload.begin(), expectedPayload.end(), std::back_inserter(headerBuffer));
    EXPECT_EQ(headerBuffer.size(), responseBuffer.size());
    EXPECT_EQ(headerBuffer, responseBuffer);
}

TEST_F(FcsCommunicationFcsLibUT, GetDeviceIdentityTest)
{
    fcsCommunication = FcsCommunication::getFcsCommunication();
    int32_t statusReturnedFromFcs = -1;
    VerifierProtocol verifierProtocol;
    verifierProtocol.setCommandCode(SDM_COMMAND_CODE::GET_DEVICE_IDENTITY);
    std::vector<uint8_t> responseBuffer;
    EXPECT_TRUE(fcsCommunication->runCommandCode(verifierProtocol, responseBuffer, statusReturnedFromFcs));
    EXPECT_EQ(0, statusReturnedFromFcs);
    std::vector<uint8_t> expectedPayload(MBOX_SEND_RSP_MAX_SZ);
    uint32_t expectedPayloadSize;
    FcsSimulator::fcs_get_response("GET_DEVICE_IDENTITY", expectedPayload.data(), &expectedPayloadSize);
    expectedPayload.resize(expectedPayloadSize);
    uint32_t header = ((expectedPayloadSize / WORD_SIZE) << 12);
    std::vector<uint8_t> headerBuffer = Utils::byteBufferFromWordPointer(&header, 1);
    std::copy(expectedPayload.begin(), expectedPayload.end(), std::back_inserter(headerBuffer));
    EXPECT_EQ(headerBuffer.size(), responseBuffer.size());
    EXPECT_EQ(headerBuffer, responseBuffer);
}

TEST_F(FcsCommunicationFcsLibUT, QspiOpenTest)
{
    fcsCommunication = FcsCommunication::getFcsCommunication();
    int32_t statusReturnedFromFcs = -1;
    VerifierProtocol verifierProtocol;
    verifierProtocol.setCommandCode(SDM_COMMAND_CODE::QSPI_OPEN);
    std::vector<uint8_t> responseBuffer;
    EXPECT_TRUE(fcsCommunication->runCommandCode(verifierProtocol, responseBuffer, statusReturnedFromFcs));
    EXPECT_EQ(0, statusReturnedFromFcs);
}

TEST_F(FcsCommunicationFcsLibUT, QspiCloseTest)
{
    fcsCommunication = FcsCommunication::getFcsCommunication();
    int32_t statusReturnedFromFcs = -1;
    VerifierProtocol verifierProtocol;
    verifierProtocol.setCommandCode(SDM_COMMAND_CODE::QSPI_CLOSE);
    std::vector<uint8_t> responseBuffer;
    EXPECT_TRUE(fcsCommunication->runCommandCode(verifierProtocol, responseBuffer, statusReturnedFromFcs));
    EXPECT_EQ(0, statusReturnedFromFcs);
}

TEST_F(FcsCommunicationFcsLibUT, QspiSetCSTest)
{
    std::vector<uint32_t> payload {0};
    fcsCommunication = FcsCommunication::getFcsCommunication();
    int32_t statusReturnedFromFcs = -1;
    VerifierProtocol verifierProtocol;
    verifierProtocol.setCommandCode(SDM_COMMAND_CODE::QSPI_SET_CS);
    verifierProtocol.setIncomingPayload(Utils::byteBufferFromWordPointer(payload.data(), payload.size()));
    std::vector<uint8_t> responseBuffer;
    EXPECT_TRUE(fcsCommunication->runCommandCode(verifierProtocol, responseBuffer, statusReturnedFromFcs));
    EXPECT_EQ(0, statusReturnedFromFcs);
}

TEST_F(FcsCommunicationFcsLibUT, QspiEraseTest)
{
    uint32_t numWordsToErase = 0x200;
    std::vector<uint32_t> payload {0, numWordsToErase};
    fcsCommunication = FcsCommunication::getFcsCommunication();
    int32_t statusReturnedFromFcs = -1;
    VerifierProtocol verifierProtocol;
    verifierProtocol.setCommandCode(SDM_COMMAND_CODE::QSPI_ERASE);
    verifierProtocol.setIncomingPayload(Utils::byteBufferFromWordPointer(payload.data(), payload.size()));
    std::vector<uint8_t> responseBuffer;
    EXPECT_TRUE(fcsCommunication->runCommandCode(verifierProtocol, responseBuffer, statusReturnedFromFcs));
    EXPECT_EQ(0, statusReturnedFromFcs);
}

TEST_F(FcsCommunicationFcsLibUT, qspiReadTest)
{
    uint32_t numWordsToRead = 0x200;
    std::vector<uint32_t> payload = {0, numWordsToRead};
    fcsCommunication = FcsCommunication::getFcsCommunication();
    int32_t statusReturnedFromFcs = -1;
    VerifierProtocol verifierProtocol;
    verifierProtocol.setCommandCode(SDM_COMMAND_CODE::QSPI_READ);
    verifierProtocol.setIncomingPayload(Utils::byteBufferFromWordPointer(payload.data(), payload.size()));
    std::vector<uint8_t> responseBuffer;
    EXPECT_TRUE(fcsCommunication->runCommandCode(verifierProtocol, responseBuffer, statusReturnedFromFcs));
    EXPECT_EQ(0, statusReturnedFromFcs);
    EXPECT_EQ((size_t)numWordsToRead * WORD_SIZE, responseBuffer.size());
    std::vector<uint8_t> expectedPayload(MBOX_SEND_RSP_MAX_SZ);
    uint32_t expectedPayloadSize;
    FcsSimulator::fcs_get_response("QSPI_READ", expectedPayload.data(), &expectedPayloadSize);
    expectedPayload.resize(expectedPayloadSize * WORD_SIZE);
    EXPECT_EQ(expectedPayload, responseBuffer);
}

TEST_F(FcsCommunicationFcsLibUT, QspiWriteTest)
{
    uint32_t numWordsToWrite = 0x200;
    std::vector<uint32_t> payload = {0, numWordsToWrite};
    std::vector<uint8_t> dataToWrite(numWordsToWrite * WORD_SIZE);
    FcsSimulator::fcs_get_response("QSPI_READ", dataToWrite.data(), new uint32_t);
    std::vector<uint32_t> dataToWriteU32 = Utils::wordBufferFromByteBuffer(dataToWrite);
    std::copy(dataToWriteU32.begin(), dataToWriteU32.end(), std::back_inserter(payload));
    fcsCommunication = FcsCommunication::getFcsCommunication();
    int32_t statusReturnedFromFcs = -1;
    VerifierProtocol verifierProtocol;
    verifierProtocol.setCommandCode(SDM_COMMAND_CODE::QSPI_WRITE);
    verifierProtocol.setIncomingPayload(Utils::byteBufferFromWordPointer(payload.data(), payload.size()));
    std::vector<uint8_t> responseBuffer;
    EXPECT_TRUE(fcsCommunication->runCommandCode(verifierProtocol, responseBuffer, statusReturnedFromFcs));
    EXPECT_EQ(0, statusReturnedFromFcs);
}
