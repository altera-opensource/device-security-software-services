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

package com.intel.bkp.command.model;

import org.junit.jupiter.api.Test;

import static com.intel.bkp.command.model.CertificateRequestType.FIRMWARE;
import static com.intel.bkp.command.model.CertificateRequestType.findByType;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

class CertificateRequestTypeTest {

    @Test
    void findByType_Success() {
        // given
        final CertificateRequestType expectedType = FIRMWARE;
        byte[] data = new byte[]{Integer.valueOf(expectedType.getType()).byteValue()};

        // when
        final CertificateRequestType result = findByType(data);

        // then
        assertEquals(expectedType, result);
    }

    @Test
    void findByType_WithNullRequestType_Throws() {
        // when-then
        assertThrows(IllegalArgumentException.class, () -> findByType(null));
    }

    @Test
    void findByType_WithEmptyArrayRequestType_Throws() {
        // given
        final byte[] data = new byte[]{};

        // when-then
        assertThrows(IllegalArgumentException.class, () -> findByType(data));
    }

    @Test
    void findByType_WithNoMatchingRequestType_Throws() {
        // given
        final byte[] data = new byte[]{0x00};

        // when-then
        assertThrows(IllegalArgumentException.class, () -> findByType(data));
    }
}

