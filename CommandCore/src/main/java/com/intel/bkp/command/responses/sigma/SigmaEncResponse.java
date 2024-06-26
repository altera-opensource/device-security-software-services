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

package com.intel.bkp.command.responses.sigma;

import com.intel.bkp.command.logger.ILogger;
import com.intel.bkp.core.interfaces.IStructure;
import com.intel.bkp.utils.interfaces.BytesConvertible;
import lombok.Getter;
import lombok.Setter;

import java.nio.ByteBuffer;

@Getter
@Setter
public class SigmaEncResponse implements BytesConvertible, ILogger, IStructure {

    private SigmaEncFlowType flowType = SigmaEncFlowType.HEADER_ONLY;

    private byte[] reservedHeader = new byte[0];
    private byte[] magic = new byte[0];
    private byte[] sdmSessionId = new byte[0];
    private byte[] messageResponseCounter = new byte[0];
    private byte[] reserved1 = new byte[0];
    private int payloadLen = 0;
    private byte[] initialIv = new byte[0];
    private byte numberOfPaddingBytes = 0;
    private byte[] reserved2 = new byte[0];
    private byte[] encryptedPayload = new byte[0];
    private byte[] mac = new byte[0];

    public boolean hasEncryptedResponse() {
        return SigmaEncFlowType.WITH_ENCRYPTED_RESPONSE == flowType;
    }

    @Override
    public byte[] array() {
        return switch (flowType) {
            default -> arrayHeaderOnly();
            case WITH_ENCRYPTED_RESPONSE -> arrayWithEncryptedResponse();
        };
    }

    private byte[] arrayHeaderOnly() {
        return new byte[0];
    }

    private byte[] arrayWithEncryptedResponse() {
        final int capacity = reservedHeader.length + magic.length + sdmSessionId.length
            + messageResponseCounter.length + reserved1.length + Integer.BYTES + initialIv.length
            + Byte.BYTES + reserved2.length + encryptedPayload.length + mac.length;

        return ByteBuffer.allocate(capacity)
            .put(reservedHeader)
            .put(magic)
            .put(sdmSessionId)
            .put(messageResponseCounter)
            .put(reserved1)
            .putInt(payloadLen)
            .put(initialIv)
            .put(numberOfPaddingBytes)
            .put(reserved2)
            .put(encryptedPayload)
            .put(mac)
            .array();
    }
}
