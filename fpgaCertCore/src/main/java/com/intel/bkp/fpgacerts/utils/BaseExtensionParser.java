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

package com.intel.bkp.fpgacerts.utils;

import lombok.Getter;
import lombok.NonNull;
import lombok.extern.slf4j.Slf4j;
import org.bouncycastle.asn1.ASN1Encodable;
import org.bouncycastle.asn1.ASN1Primitive;

import java.io.IOException;
import java.security.cert.X509Extension;
import java.util.Optional;

import static com.intel.bkp.crypto.x509.utils.X509ExtensionUtils.getExtensionBytes;
import static com.intel.bkp.crypto.x509.utils.X509ExtensionUtils.getObjDescription;

@Slf4j
@Getter
public abstract class BaseExtensionParser<T> {

    private static final String START_LOG_MESSAGE = "Parsing {} extension from {}";
    private static final String ERROR_MESSAGE = "Failed to parse %s extension from %s";

    private final String extensionName;

    protected BaseExtensionParser(String extensionName) {
        this.extensionName = extensionName;
    }

    protected abstract T parse(@NonNull final X509Extension x509Obj);

    protected void logExtensionParsingStart(final X509Extension x509Obj, final String extensionName) {
        log.trace(START_LOG_MESSAGE, extensionName, getObjDescription(x509Obj));
    }

    protected Optional<ASN1Encodable> getExtension(@NonNull final X509Extension x509Obj,
                                                   final String extensionOid) {
        return getExtensionBytes(x509Obj, extensionOid)
            .map(bytes -> {
                try {
                    return ASN1Primitive.fromByteArray(bytes);
                } catch (IOException e) {
                    throw new IllegalArgumentException(getExtensionParsingError(x509Obj));
                }
            });
    }

    protected String getExtensionParsingError(final X509Extension x509Obj) {
        return String.format(ERROR_MESSAGE, extensionName, getObjDescription(x509Obj));
    }
}
