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

package com.intel.bkp.fpgacerts.dice.tcbinfo.verification;

import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;

import static com.intel.bkp.fpgacerts.dice.tcbinfo.verification.TcbInfoTestUtil.parseTcbInfo;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class ModelVerifierTest {

    private static final String AGILEX_FAMILY_NAME = "Agilex";

    private static final ModelVerifier SUT = new ModelVerifier();

    @BeforeAll
    static void init() {
        SUT.withFamilyName(AGILEX_FAMILY_NAME);
    }

    @Test
    void verify_ValidModelValue_Success() {
        // given
        final String tcbInfoWithModelValueAgilex =
            "305D8009696E74656C2E636F6D81064167696C6578830105840100850100A63F303D06096086480165030402020430FF0102030405"
                + "060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F";
        final var tcbInfo = parseTcbInfo(tcbInfoWithModelValueAgilex);

        // when
        final boolean result = SUT.verify(tcbInfo);

        // then
        assertTrue(result);
    }

    @Test
    void verify_ValidModelValueButDifferentLetterSize_Fails() {
        // given
        final String tcbInfoWithModelValueAgilexInLowercase =
            "305D8009696E74656C2E636F6D81066167696C6578830105840100850100A63F303D06096086480165030402020430FF0102030405"
                + "060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F";
        final var tcbInfo = parseTcbInfo(tcbInfoWithModelValueAgilexInLowercase);

        // when
        final boolean result = SUT.verify(tcbInfo);

        // then
        assertFalse(result);
    }

    @Test
    void verify_InvalidModelValue_Fails() {
        // given
        final String tcbInfoWithModelValueRandomString =
            "30638009696E74656C2E636F6D810C52616E646F6D537472696E67830105840100850100A63F303D06096086480165030402020430"
                + "FF0102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F";
        final var tcbInfo = parseTcbInfo(tcbInfoWithModelValueRandomString);

        // when
        final boolean result = SUT.verify(tcbInfo);

        // then
        assertFalse(result);
    }

    @Test
    void verify_NoModelField_Success() {
        // given
        final String tcbInfoWithoutModelField =
            "30558009696E74656C2E636F6D830105840100850100A63F303D06096086480165030402020430FF0102030405060708090A0B0C0D"
                + "0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F";
        final var tcbInfo = parseTcbInfo(tcbInfoWithoutModelField);

        // when
        final boolean result = SUT.verify(tcbInfo);

        // then
        assertTrue(result);
    }
}
