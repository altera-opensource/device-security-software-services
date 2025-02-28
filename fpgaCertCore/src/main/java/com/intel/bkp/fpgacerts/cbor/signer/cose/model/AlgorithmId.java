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

package com.intel.bkp.fpgacerts.cbor.signer.cose.model;

import com.intel.bkp.fpgacerts.cbor.signer.cose.exception.CoseException;
import com.upokecenter.cbor.CBORObject;
import lombok.Getter;

import java.util.stream.Stream;

import static com.intel.bkp.crypto.constants.CryptoConstants.SHA256_LEN;
import static com.intel.bkp.crypto.constants.CryptoConstants.SHA256_WITH_ECDSA;
import static com.intel.bkp.crypto.constants.CryptoConstants.SHA384_LEN;
import static com.intel.bkp.crypto.constants.CryptoConstants.SHA384_WITH_ECDSA;
import static com.intel.bkp.crypto.constants.CryptoConstants.SHA512_LEN;
import static com.intel.bkp.crypto.constants.CryptoConstants.SHA512_WITH_ECDSA;

@Getter
public enum AlgorithmId {
    ECDSA_256(-7, SHA256_WITH_ECDSA, SHA256_LEN),
    ECDSA_384(-35, SHA384_WITH_ECDSA, SHA384_LEN),
    ECDSA_521(-36, SHA512_WITH_ECDSA, SHA512_LEN);

    private final int cborValue;
    private final String shaAlgorithmName;
    private final int signatureLength;

    AlgorithmId(int cborValue, String shaAlgorithmName, int signatureLength) {
        this.cborValue = cborValue;
        this.shaAlgorithmName = shaAlgorithmName;
        this.signatureLength = signatureLength;
    }

    public CBORObject getCbor() {
        return CBORObject.FromObject(cborValue);
    }

    public static AlgorithmId fromCbor(CBORObject obj) throws CoseException {
        if (obj == null) {
            throw new CoseException("No Algorithm Specified");
        }
        return Stream.of(values())
            .filter(algorithm -> obj.equals(algorithm.getCbor()))
            .findAny()
            .orElseThrow(() -> new CoseException("Unknown Algorithm Specified"));
    }

    public static AlgorithmId fromValue(Integer value) throws CoseException {
        if (value == null) {
            throw new CoseException("No Algorithm Specified");
        }
        return Stream.of(values())
            .filter(algorithm -> value.equals(algorithm.getCbor().AsInt32Value()))
            .findAny()
            .orElseThrow(() -> new CoseException("Unknown Algorithm Specified"));
    }
}
