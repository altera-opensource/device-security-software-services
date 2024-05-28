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

#include "bkp_master_test.h"

/**
 * BKP Master tests
 */

//region ProvisioningAndNetworkErrorsTest

TEST_F(BkpMasterTest, provision_GetNextNetworkFails_LogsAndReturnsError)
{
    // given
    mockNetworkInitProvisioning();
    mockNetworkThrowHttpException();
    mockLoggerInitializationMessages();
    mockLoggerSendingData();
    mockLoggerFailWithMessage(testMessage);

    BkpMaster bkpMaster(&qc, mockTxIdMgrPtr, mockLoggerPtr, mockNetworkPtr, mockJtagPtr);

    // when
    Status_t result = bkpMaster.provision();

    // then
    ASSERT_EQ(ST_GENERIC_ERROR, result);
}

TEST_F(BkpMasterTest, provision_StatusEmpty_ReportsError)
{
    // given
    mockNetworkInitProvisioning();
    mockEmptyNetworkCalls();
    mockLoggerInitializationMessages();
    mockLoggerSendingData();
    mockLoggerReceivedData();
    mockLoggerFailWithMessage("[3500] Json parsing BKP Service response failed");

    BkpMaster bkpMaster(&qc, mockTxIdMgrPtr, mockLoggerPtr, mockNetworkPtr, mockJtagPtr);

    // when
    Status_t result = bkpMaster.provision();

    // then
    ASSERT_EQ(ST_GENERIC_ERROR, result);
}

TEST_F(BkpMasterTest, provision_IsDone)
{
    // given
    mockNetworkInitProvisioning();
    mockNetworkDone();
    mockLoggerInitializationMessages();
    mockLoggerSendingData();
    mockLoggerReceivedData();
    mockLoggerProvisioningSuccess();

    BkpMaster bkpMaster(&qc, mockTxIdMgrPtr, mockLoggerPtr, mockNetworkPtr, mockJtagPtr);

    // when
    Status_t result = bkpMaster.provision();

    // then
    ASSERT_EQ(ST_OK, result);
}

TEST_F(BkpMasterTest, provision_IsContinue)
{
    // given
    mockNetworkInitProvisioning();
    mockNetworkContinueThenDone();
    mockLoggerInitializationMessages();
    mockLoggerSendingData(2);
    mockLoggerReceivedData(2);
    mockLoggerProvisioningSuccess();

    BkpMaster bkpMaster(&qc, mockTxIdMgrPtr, mockLoggerPtr, mockNetworkPtr, mockJtagPtr);

    // when
    Status_t result = bkpMaster.provision();

    // then
    ASSERT_EQ(ST_OK, result);
}

TEST_F(BkpMasterTest, provision_IsContinueEmptyJtagCmd)
{
    // given
    mockNetworkInitProvisioning();
    mockNetworkContinueWithEmptyJtagCmd();
    mockLoggerInitializationMessages();
    mockLoggerReceivedData(3);
    mockLoggerCalls();
    mockLoggerProvisioningSuccess();

    BkpMaster bkpMaster(&qc, mockTxIdMgrPtr, mockLoggerPtr, mockNetworkPtr, mockJtagPtr);

    // when
    Status_t result = bkpMaster.provision();

    // then
    ASSERT_EQ(ST_OK, result);
}

TEST_F(BkpMasterTest, provision_CanIterateMaxIterationCount)
{
    // given
    {
        testing::InSequence s;
        mockNetworkContinueManyTimes(BkpMaster::MAX_ITERATION_COUNT - 1);
        mockNetworkDone();
    }
    mockNetworkInitProvisioning();
    mockLoggerInitializationMessages();
    mockLoggerSendingData(BkpMaster::MAX_ITERATION_COUNT);
    mockLoggerReceivedData(BkpMaster::MAX_ITERATION_COUNT);
    mockLoggerProvisioningSuccess();

    BkpMaster bkpMaster(&qc, mockTxIdMgrPtr, mockLoggerPtr, mockNetworkPtr, mockJtagPtr);

    // when
    Status_t result = bkpMaster.provision();

    // then
    ASSERT_EQ(ST_OK, result);
}

TEST_F(BkpMasterTest, provision_CannotExceedMaxIterationCount)
{
    // given
    mockNetworkInitProvisioning();
    mockNetworkContinueManyTimes(BkpMaster::MAX_ITERATION_COUNT);
    mockLoggerInitializationMessages();
    mockLoggerSendingData(BkpMaster::MAX_ITERATION_COUNT);
    mockLoggerReceivedData(BkpMaster::MAX_ITERATION_COUNT);
    mockLoggerFailWithMessage("Exceeded maximum requests counter.");

    BkpMaster bkpMaster(&qc, mockTxIdMgrPtr, mockLoggerPtr, mockNetworkPtr, mockJtagPtr);

    // when
    Status_t result = bkpMaster.provision();

    // then
    ASSERT_EQ(ST_GENERIC_ERROR, result);
}

TEST_F(BkpMasterTest, provision_ReceivedJtagCommands)
{
    // given
    mockNetworkInitProvisioning();
    mockNetwork2JtagCommandsAndDone();
    mockLoggerInitializationMessages();
    mockLoggerSendingData();
    mockLoggerReceivedData();
    mockLoggerCalls();
    mockLoggerProvisioningSuccess();

    BkpMaster bkpMaster(&qc, mockTxIdMgrPtr, mockLoggerPtr, mockNetworkPtr, mockJtagPtr);

    // when
    Status_t result = bkpMaster.provision();

    // then
    ASSERT_EQ(ST_OK, result);
}

TEST_F(BkpMasterTest, prefetch_ReceivedJtagCommands)
{
    // given
    mockNetworkInitPrefetch();
    mockNetwork2JtagCommandsAndDone();
    mockLoggerInitializationMessages();
    mockLoggerSendingData();
    mockLoggerReceivedData();
    mockLoggerCalls();
    mockLoggerPrefetchingSuccess();

    BkpMaster bkpMaster(&qc, mockTxIdMgrPtr, mockLoggerPtr, mockNetworkPtr, mockJtagPtr);

    // when
    Status_t result = bkpMaster.prefetch();

    // then
    ASSERT_EQ(ST_OK, result);
}

//endregion ProvisioningAndNetworkErrorsTest

//region ExceptionHandlingTest


TEST_F(BkpMasterTest, provision_GetNextThrowsUnknownError)
{
    // given
    mockNetworkInitProvisioning();
    EXPECT_CALL(*mockNetworkPtr, perform).WillOnce(Throw(BkpHttpException(unknownError)));
    mockLoggerInitializationMessages();
    mockLoggerSendingData();
    mockLoggerFailWithMessage(unknownError);

    BkpMaster bkpMaster(&qc, mockTxIdMgrPtr, mockLoggerPtr, mockNetworkPtr, mockJtagPtr);

    // when
    Status_t result = bkpMaster.provision();

    // then
    ASSERT_EQ(ST_GENERIC_ERROR, result);
}

TEST_F(BkpMasterTest, prefetch_GetNextThrowsUnknownError)
{
    // given
    mockNetworkInitPrefetch();
    EXPECT_CALL(*mockNetworkPtr, perform).WillOnce(Throw(BkpHttpException(unknownError)));
    EXPECT_CALL(*mockLoggerPtr, log(L_ERROR, GTEST_MATCH_SUBSTR(unknownError)));
    mockLoggerInitializationMessages();
    mockLoggerSendingData();
    mockLoggerPrefetchingFail();

    BkpMaster bkpMaster(&qc, mockTxIdMgrPtr, mockLoggerPtr, mockNetworkPtr, mockJtagPtr);

    // when
    Status_t result = bkpMaster.prefetch();

    // then
    ASSERT_EQ(ST_GENERIC_ERROR, result);
}

TEST_F(BkpMasterTest, provision_GetNextThrowsGenericException)
{
    // given
    mockNetworkInitProvisioning();
    EXPECT_CALL(*mockNetworkPtr, perform).WillOnce(Throw(MockException(testMessage)));
    mockLoggerInitializationMessages();
    mockLoggerSendingData();
    mockLoggerFailWithMessage(testMessage, error_messages::service_unavailable);

    BkpMaster bkpMaster(&qc, mockTxIdMgrPtr, mockLoggerPtr, mockNetworkPtr, mockJtagPtr);

    // when
    Status_t result = bkpMaster.provision();

    // then
    ASSERT_EQ(ST_GENERIC_ERROR, result);
}

TEST_F(BkpMasterTest, provision_GetNextThrowsHttpException)
{
    // given
    mockNetworkInitProvisioning();
    EXPECT_CALL(*mockNetworkPtr, perform).WillOnce(Throw(BkpHttpException(testMessage)));
    mockLoggerInitializationMessages();
    mockLoggerSendingData();
    mockLoggerFailWithMessage(testMessage, error_messages::http_error);

    BkpMaster bkpMaster(&qc, mockTxIdMgrPtr, mockLoggerPtr, mockNetworkPtr, mockJtagPtr);

    // when
    Status_t result = bkpMaster.provision();

    // then
    ASSERT_EQ(ST_GENERIC_ERROR, result);
}

TEST_F(BkpMasterTest, provision_GetNextThrowsServiceResponseException)
{
    // given
    mockNetworkInitProvisioning();
    EXPECT_CALL(*mockNetworkPtr, perform).WillOnce(Throw(BkpServiceResponseException(testMessage)));
    mockLoggerInitializationMessages();
    mockLoggerSendingData();
    mockLoggerFailWithMessage(testMessage, error_messages::provisioning_cancelled);

    BkpMaster bkpMaster(&qc, mockTxIdMgrPtr, mockLoggerPtr, mockNetworkPtr, mockJtagPtr);

    // when
    Status_t result = bkpMaster.provision();

    // then
    ASSERT_EQ(ST_GENERIC_ERROR, result);
}

TEST_F(BkpMasterTest, provision_GetNextThrowsBkpBase64Exception)
{
    // given
    mockNetworkInitProvisioning();
    EXPECT_CALL(*mockJtagPtr, exchange_jtag_cmd).WillOnce(Throw(BkpBase64Exception(testMessage)));
    //invalid base64 in commands - doesn't matter, as the base64 decode exception is mocked to occur anyway
    std::string jsonFromBkps = R"({"context":{"value":"","empty":true},"status":"done","apiVersion":1,"jtagCommands":[{"type":0,"value":"A"},{"type":0,"value":"!!!!"}]})";
    mockNetworkCalls(jsonFromBkps);
    mockLoggerInitializationMessages();
    mockLoggerSendingData();
    mockLoggerReceivedData();
    EXPECT_CALL(*mockLoggerPtr, log(L_INFO, provisioning_stage::received_from_bkps));
    mockLoggerFailWithMessage(error_messages::base64_error, testMessage);

    BkpMaster bkpMaster(&qc, mockTxIdMgrPtr, mockLoggerPtr, mockNetworkPtr, mockJtagPtr);

    // when
    Status_t result = bkpMaster.provision();

    // then
    ASSERT_EQ(ST_GENERIC_ERROR, result);
}

TEST_F(BkpMasterTest, provision_GetNextThrowsUnknownBkpException)
{
    // given
    mockNetworkInitProvisioning();
    EXPECT_CALL(*mockNetworkPtr, perform).WillOnce(Throw(MockBkpUnknownException(testMessage)));
    mockLoggerInitializationMessages();
    mockLoggerSendingData();
    mockLoggerFailWithMessage(testMessage, error_messages::unknown_error);

    BkpMaster bkpMaster(&qc, mockTxIdMgrPtr, mockLoggerPtr, mockNetworkPtr, mockJtagPtr);

    // when
    Status_t result = bkpMaster.provision();

    // then
    ASSERT_EQ(ST_GENERIC_ERROR, result);
}

TEST_F(BkpMasterTest, provision_GetNextThrowsInteger)
{
    // given
    mockNetworkInitProvisioning();
    EXPECT_CALL(*mockNetworkPtr, perform).WillOnce(Throw(5));
    mockLoggerInitializationMessages();
    mockLoggerSendingData();
    mockLoggerProvisioningFail();

    BkpMaster bkpMaster(&qc, mockTxIdMgrPtr, mockLoggerPtr, mockNetworkPtr, mockJtagPtr);

    // when
    Status_t result = bkpMaster.provision();

    // then
    ASSERT_EQ(ST_GENERIC_ERROR, result);
}

//endregion ExceptionHandlingTest

void BkpMasterTest::mockNetworkInitProvisioning()
{
    EXPECT_CALL(*mockNetworkPtr, init(_, "/prov/v1/get_next")).Times(1);
}

void BkpMasterTest::mockNetworkInitPrefetch()
{
    EXPECT_CALL(*mockNetworkPtr, init(_, "/prov/v1/prefetch/next")).Times(1);
}

void BkpMasterTest::mockEmptyNetworkCalls()
{
    EXPECT_CALL(*mockNetworkPtr, perform).Times(1);
}

void BkpMasterTest::mockNetworkCalls(std::string jsonFromBkps)
{
    EXPECT_CALL(*mockNetworkPtr, perform).WillOnce(testing::SetArgReferee<1>(jsonFromBkps));
}

void BkpMasterTest::mockNetworkDone()
{
    mockNetworkCalls(R"({"context":{"value":"","empty":true},"status":"done","apiVersion":1,"jtagCommands":[]})");
}

void BkpMasterTest::mockNetworkContinueThenDone()
{
    std::string jsonContinue = R"({"context":{"value":"","empty":true},"status":"continue","apiVersion":1,"jtagCommands":[]})";
    std::string jsonDone = R"({"context":{"value":"","empty":true},"status":"done","apiVersion":1,"jtagCommands":[]})";
    EXPECT_CALL(*mockNetworkPtr, perform)
            .WillOnce(testing::SetArgReferee<1>(jsonContinue))
            .WillOnce(testing::SetArgReferee<1>(jsonDone));
}

void BkpMasterTest::mockNetworkContinueWithEmptyJtagCmd()
{
    std::string jsonContinue1 = R"({"context":{"value":"","empty":true},"status":"continue","apiVersion":1,"jtagCommands":[{"type":0,"value":"cmd"}]})";
    std::string jsonContinue2 = R"({"context":{"value":"","empty":true},"status":"continue","apiVersion":1,"jtagCommands":[]})";
    std::string jsonDone = R"({"context":{"value":"","empty":true},"status":"done","apiVersion":1,"jtagCommands":[]})";
    EXPECT_CALL(*mockNetworkPtr, perform)
            .WillOnce(testing::SetArgReferee<1>(jsonContinue1))
            .WillOnce(testing::SetArgReferee<1>(jsonContinue2))
            .WillOnce(testing::SetArgReferee<1>(jsonDone));
    mockJtagCalls(1);
    std::string jsonRsp1 = R"("jtagResponses":[])";
    std::string jsonRsp2 = R"("jtagResponses":[{"status":0,"value":""}])";
    EXPECT_CALL(*mockLoggerPtr, log(L_DEBUG, GTEST_MATCH_SUBSTR2("Prepared payload:", jsonRsp1))).Times(2);
    EXPECT_CALL(*mockLoggerPtr, log(L_DEBUG, GTEST_MATCH_SUBSTR2("Prepared payload:", jsonRsp2))).Times(1);
}

void BkpMasterTest::mockNetworkContinueManyTimes(int times)
{
    std::string jsonContinue = R"({"context":{"value":"","empty":true},"status":"continue","apiVersion":1,"jtagCommands":[]})";
    EXPECT_CALL(*mockNetworkPtr, perform)
        .Times(times)
        .WillRepeatedly(testing::SetArgReferee<1>(jsonContinue));
}

void BkpMasterTest::mockNetworkThrowHttpException() {
    EXPECT_CALL(*mockNetworkPtr, perform).WillOnce(Throw(BkpHttpException(testMessage)));
}

void BkpMasterTest::mockNetwork2JtagCommandsAndDone()
{
    std::string jsonFromBkps = R"({"context":{"value":"","empty":true},"status":"done","apiVersion":1,"jtagCommands":[{"type":0,"value":"cmd1"},{"type":0,"value":"cmd2"}]})";
    mockNetworkCalls(jsonFromBkps);
    mockJtagCalls(2);
}

void BkpMasterTest::mockJtagCalls(size_t numberOfCommands)
{
    EXPECT_CALL(*mockJtagPtr, exchange_jtag_cmd).Times(numberOfCommands);
}

void BkpMasterTest::mockLoggerSendingData(int times)
{
    EXPECT_CALL(*mockLoggerPtr, log(L_DEBUG, GTEST_MATCH_SUBSTR("Prepared payload:"))).Times(times);
}

void BkpMasterTest::mockLoggerReceivedData(int times)
{
    EXPECT_CALL(*mockLoggerPtr, log(L_DEBUG, GTEST_MATCH_SUBSTR("Received data from BKPS:"))).Times(times);
}

void BkpMasterTest::mockLoggerInitializationMessages()
{
    EXPECT_CALL(*mockLoggerPtr, log(L_DEBUG, GTEST_MATCH_SUBSTR(logMessages::cert::load_root))).Times(1);
    EXPECT_CALL(*mockLoggerPtr, log(L_DEBUG, GTEST_MATCH_SUBSTR(logMessages::cert::load_client))).Times(1);
    EXPECT_CALL(*mockLoggerPtr, log(L_DEBUG, GTEST_MATCH_SUBSTR(logMessages::cert::load_key))).Times(1);
    EXPECT_CALL(*mockLoggerPtr, log(L_DEBUG, GTEST_MATCH_SUBSTR(logMessages::password::looking_for_password))).Times(1);
    EXPECT_CALL(*mockLoggerPtr, log(L_INFO, GTEST_MATCH_SUBSTR(logMessages::proxy::set_to))).Times(1);
}

void BkpMasterTest::mockLoggerCalls()
{
    EXPECT_CALL(*mockLoggerPtr, log(L_INFO, provisioning_stage::received_from_bkps)).Times(1);
    EXPECT_CALL(*mockLoggerPtr, log(L_INFO, provisioning_stage::received_from_quartus)).Times(1);
}

void BkpMasterTest::mockLoggerPrefetchingSuccess()
{
    EXPECT_CALL(*mockLoggerPtr, log(L_INFO, "Prefetching COMPLETED."));
}

void BkpMasterTest::mockLoggerProvisioningSuccess()
{
    EXPECT_CALL(*mockLoggerPtr, log(L_INFO, "Provisioning COMPLETED."));
}
void BkpMasterTest::mockLoggerPrefetchingFail()
{
    EXPECT_CALL(*mockLoggerPtr, log(L_ERROR, "Prefetching FAILED."));
}

void BkpMasterTest::mockLoggerProvisioningFail()
{
    EXPECT_CALL(*mockLoggerPtr, log(L_ERROR, "Provisioning FAILED."));
}

void BkpMasterTest::mockLoggerFailWithMessage(const std::string &message)
{
    EXPECT_CALL(*mockLoggerPtr, log(L_ERROR, GTEST_MATCH_SUBSTR(message)));
    mockLoggerProvisioningFail();
}

void BkpMasterTest::mockLoggerFailWithMessage(const std::string &message, const std::string &error_msg)
{
    EXPECT_CALL(*mockLoggerPtr, log(L_ERROR, GTEST_MATCH_SUBSTR2(message, error_msg)));
    mockLoggerProvisioningFail();
}
