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

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

class SpdmHeaderParserTest {

    private static final byte SPDM_VERSION = 1;
    private static final byte REQUEST_RESPONSE_CODE = 2;
    private static final byte PARAM1 = 3;
    private static final byte PARAM2 = 4;
    private static final SpdmHeader HEADER = new SpdmHeader(SPDM_VERSION, REQUEST_RESPONSE_CODE, PARAM1, PARAM2);

    @Test
    void parse_Success() {
        // given
        // when
        final SpdmHeader result = new SpdmHeaderParser().parse(HEADER.array());

        // then
        assertEquals(HEADER, result);
    }

    @Test
    void print_WithFoundSpdmCode() {
        // when
        final String result = HEADER.toString();

        // then
        assertEquals("SpdmHeader(SPDMVersion = 0x01, "
            + "RequestResponseCode = 0x02 (CERTIFICATE), Param1 = 0x03, Param2 = 0x04)", result);
    }
}
