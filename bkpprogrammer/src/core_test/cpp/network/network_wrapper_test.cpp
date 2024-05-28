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

#include <gmock/gmock.h>

#include "mocks/mocked_logger.h"
#include "mocks/mocked_transaction_id_manager.h"
#include "network/network_wrapper.h"

TEST(CurlWrapperTest, writeMemoryCallback_consumesData)
{
    std::vector<char> inputData = {'a', 'b', 'c'};
    std::string buffer;

    size_t result = CurlWrapper::writeMemoryCallback(inputData.data(), inputData.size(), 1, (void *)&buffer);
    ASSERT_EQ(inputData.size(), result); //check if the method consumed all available data
    ASSERT_EQ("abc", buffer);

    inputData = {'d', 'e', 'f', 'g', 'h'};
    result = CurlWrapper::writeMemoryCallback(inputData.data(), inputData.size(), 1, (void *)&buffer);
    ASSERT_EQ(inputData.size(), result); //check if the method consumed all available data
    ASSERT_EQ("abcdefgh", buffer);
}

TEST(CurlWrapperTest, findTransactionIdInHeader_success)
{
    std::string input = "X-Request-TransactionId: aaabbbccc";
    std::string output;

    size_t result = CurlWrapper::findTransactionIdInHeaders(input.data(), input.size(), 1, (void *)&output);
    ASSERT_EQ(input.size(), result);
    ASSERT_EQ("aaabbbccc", output);

    // check that old value is overwritten
    input = "X-Request-TransactionId: ddeeff";
    result = CurlWrapper::findTransactionIdInHeaders(input.data(), input.size(), 1, (void *)&output);
    ASSERT_EQ(input.size(), result);
    ASSERT_EQ("ddeeff", output);

    // empty txId should be processed and ignored
    input = "X-Request-TransactionId: ";
    result = CurlWrapper::findTransactionIdInHeaders(input.data(), input.size(), 1, (void *)&output);
    ASSERT_EQ(input.size(), result);
    ASSERT_EQ("ddeeff", output);

    // header other than tx id should be processed and ignored
    input = "Content-Type: application/json";
    result = CurlWrapper::findTransactionIdInHeaders(input.data(), input.size(), 1, (void *)&output);
    ASSERT_EQ(input.size(), result);
    ASSERT_EQ("ddeeff", output);

    // edge case is processed and don't throw/fail etc.
    input = "X-Request-TransactionId: X-Request-TransactionId: aaaa";
    result = CurlWrapper::findTransactionIdInHeaders(input.data(), input.size(), 1, (void *)&output);
    ASSERT_EQ(input.size(), result);
}

TEST(CurlWrapperTest, getOutgoingHeaders_success)
{
    //given
    CurlWrapper curlWrapper(std::make_shared<MockLogger>(), std::make_shared<MockTransactionIdManager>());
    curlWrapper.transactionId = "abcd";

    //when
    curl_slist* headerList = curlWrapper.getOutgoingHeaders();

    //then
    std::string output;
    curl_slist *currentElement = headerList;
    while (currentElement != nullptr)
    {
        output += std::string(currentElement->data) + "|";
        currentElement = currentElement->next;
    }
    ASSERT_EQ("Accept: application/json|Content-Type: application/json|charset: utf-8|X-Request-TransactionId: abcd|", output);
    curl_slist_free_all(headerList);
}
