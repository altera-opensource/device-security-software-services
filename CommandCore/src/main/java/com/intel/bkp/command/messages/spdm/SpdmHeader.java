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

package com.intel.bkp.command.messages.spdm;

import lombok.AllArgsConstructor;
import lombok.EqualsAndHashCode;
import lombok.Getter;

import java.nio.ByteBuffer;
import java.util.HexFormat;


@Getter
@EqualsAndHashCode
@AllArgsConstructor
public class SpdmHeader {

    private static final byte SPDM_HEADER_SIZE = Integer.BYTES;

    private final byte spdmVersion;
    private final byte requestResponseCode;
    private final byte param1;
    private final byte param2;

    public byte[] array() {
        return ByteBuffer.allocate(SPDM_HEADER_SIZE)
            .put(spdmVersion)
            .put(requestResponseCode)
            .put(param1)
            .put(param2)
            .rewind()
            .array();
    }

    public int getLength() {
        return SPDM_HEADER_SIZE;
    }

    @Override
    public String toString() {
        final HexFormat hex = HexFormat.of();
        return String.format(
            "%s(SPDMVersion = 0x%s, RequestResponseCode = 0x%s (%s), Param1 = 0x%s, Param2 = 0x%s)",
            this.getClass().getSimpleName(),
            hex.toHexDigits(spdmVersion),
            hex.toHexDigits(requestResponseCode),
            SpdmRequestResponseCode.print(requestResponseCode),
            hex.toHexDigits(param1),
            hex.toHexDigits(param2)
        );
    }

}

