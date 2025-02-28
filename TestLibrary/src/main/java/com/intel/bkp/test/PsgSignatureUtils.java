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

package com.intel.bkp.test;

import com.intel.bkp.core.psgcertificate.PsgSignatureBuilder;
import com.intel.bkp.core.psgcertificate.model.PsgSignatureCurveType;
import org.apache.commons.lang3.StringUtils;

import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.security.PrivateKey;
import java.security.Signature;
import java.security.SignatureException;

import static com.intel.bkp.crypto.CryptoUtils.getBouncyCastleProvider;
import static com.intel.bkp.crypto.constants.CryptoConstants.SHA384_WITH_ECDSA;

public class PsgSignatureUtils {

    private static byte[] signEcData(byte[] msg, PrivateKey priv, String sigSpec) {
        if (StringUtils.isEmpty(sigSpec)) {
            sigSpec = SHA384_WITH_ECDSA;
        }
        try {
            Signature ecdsa = Signature.getInstance(sigSpec, getBouncyCastleProvider());
            ecdsa.initSign(priv);
            ecdsa.update(msg);
            return ecdsa.sign();
        } catch (SignatureException | NoSuchAlgorithmException | InvalidKeyException e) {
            throw new RuntimeException("Failed to sign data");
        }
    }

    public static byte[] signDataInPsgFormat(byte[] msg, PrivateKey priv, String sigSpec) {
        final byte[] signed = signEcData(msg, priv, sigSpec);
        return new PsgSignatureBuilder()
            .signature(signed, PsgSignatureCurveType.SECP384R1)
            .build()
            .array();
    }
}
