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

package com.intel.bkp.core.psgcertificate.model;

import com.intel.bkp.core.exceptions.ParseStructureException;
import lombok.AllArgsConstructor;
import lombok.Getter;

import java.util.Arrays;
import java.util.stream.Collectors;

import static com.intel.bkp.utils.HexConverter.toFormattedHex;

@Getter
@AllArgsConstructor
public enum PsgSignatureMagic {
    STANDARD(0x74881520),
    CANCELLABLE_BLOCK0_ENTRY(0x71050792);

    private final int value;

    public static String getAllowedMagics() {
        return Arrays.stream(values())
            .map(item -> toFormattedHex(item.getValue()))
            .collect(Collectors.joining(", "));
    }

    public static PsgSignatureMagic from(int magic) throws ParseStructureException {
        return Arrays.stream(values())
            .filter(item -> item.getValue() == magic)
            .findAny()
            .orElseThrow(() -> new ParseStructureException(
                "Invalid signature magic. Expected any of: %s, Actual: %s."
                    .formatted(PsgPublicKeyMagic.getAllowedMagics(), toFormattedHex(magic))));
    }
}
