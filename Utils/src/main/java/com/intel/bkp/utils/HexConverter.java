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

package com.intel.bkp.utils;

import lombok.AccessLevel;
import lombok.NoArgsConstructor;
import org.apache.commons.codec.DecoderException;
import org.apache.commons.codec.binary.Hex;
import org.apache.commons.lang3.StringUtils;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.HexFormat;
import java.util.List;

@NoArgsConstructor(access = AccessLevel.PRIVATE)
public class HexConverter {

    public static byte[] fromHex(String data) {
        try {
            return Hex.decodeHex(data);
        } catch (DecoderException e) {
            throw new IllegalArgumentException("Failed to decode HEX string", e);
        }
    }

    public static byte fromHexSingle(String data) {
        if (StringUtils.isBlank(data)) {
            throw new IllegalArgumentException("Only single byte can be converted. Empty string provided.");
        }
        final String sanitized = data.replaceFirst("0x", "");
        if (sanitized.length() != 2) {
            throw new IllegalArgumentException("Only single byte can be converted. Improper data size provided: %d."
                .formatted(sanitized.length()));
        }
        return fromHex(sanitized)[0];
    }

    public static String toHex(int number) {
        return String.format("%02X", number);
    }

    public static String toHex(long number) {
        return String.format("%02X", number);
    }

    public static String toHex(byte[] data) {
        return HexFormat.of().withUpperCase().formatHex(data);
    }

    public static String toHex(byte singleByte) {
        return toHex(new byte[]{singleByte});
    }

    public static String toFormattedHex(byte singleByte) {
        return String.format("0x%s", toHex(singleByte));
    }

    public static String toFormattedHex(int number) {
        return String.format("0x%s", toHex(number));
    }

    public static String toFormattedHex(long number) {
        return String.format("0x%s", toHex(number));
    }

    // TODO ADD UT
    public static String toFormattedHex(byte[] data) {
        List<String> bytesList = new ArrayList<>();
        ByteBuffer wrap = ByteBuffer.wrap(data);
        while (wrap.hasRemaining()) {
            if (wrap.remaining() >= Integer.BYTES) {
                bytesList.add("0x" + String.format("%08x", wrap.getInt()));
            } else {
                byte[] remainingData = new byte[wrap.remaining()];
                wrap.get(remainingData);
                bytesList.add("0x" + String.format("%08x", ByteConverter.toInt(remainingData)));
            }
        }
        return String.join(" ", bytesList);
    }

    public static String toLowerCaseHex(byte[] data) {
        return HexFormat.of().formatHex(data);
    }
}
