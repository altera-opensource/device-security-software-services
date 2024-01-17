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

#ifndef PROGRAMMER_TRANSACTION_ID_MANAGER_H
#define PROGRAMMER_TRANSACTION_ID_MANAGER_H

#include <iostream>

namespace httprequest {
    const std::string header_txid = "X-Request-TransactionId";
    const std::string content_type_json = "application/json";
}

//Interface
class TransactionIdManager {
protected:
    TransactionIdManager() = default;

public:
    virtual ~TransactionIdManager() = default;

    virtual void set_transaction_id(std::string transaction_id) = 0;

    virtual std::string get_transaction_id() const = 0;

    virtual bool is_transaction_id() const = 0;
};

class TransactionIdManagerImpl : public TransactionIdManager {
private:
    std::string transactionId;

public:
    explicit TransactionIdManagerImpl() = default;

    ~TransactionIdManagerImpl() override = default;

    TransactionIdManagerImpl(const TransactionIdManagerImpl &L) = delete;

    TransactionIdManagerImpl &operator=(const TransactionIdManagerImpl &L) = delete;

    void set_transaction_id(std::string txId) override;

    std::string get_transaction_id() const override;

    bool is_transaction_id() const override;

};

#endif //PROGRAMMER_TRANSACTION_ID_MANAGER_H
