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

package com.intel.bkp.core.endianness.maps;

import com.intel.bkp.core.endianness.EndiannessActor;
import org.junit.jupiter.api.Test;

import static com.intel.bkp.core.endianness.StructureField.PSG_SIG_HASH_MAGIC;
import static com.intel.bkp.core.endianness.StructureField.PSG_SIG_MAGIC;
import static com.intel.bkp.core.endianness.StructureField.PSG_SIG_SIZE_R;
import static com.intel.bkp.core.endianness.StructureField.PSG_SIG_SIZE_S;
import static com.intel.bkp.utils.ByteSwapOrder.CONVERT;
import static org.junit.jupiter.api.Assertions.assertEquals;

class PsgSignatureEndiannessMapImplTest {

    @Test
    void populateServiceMap_Success() {
        // when
        PsgSignatureEndiannessMapImpl sut = new PsgSignatureEndiannessMapImpl(EndiannessActor.SERVICE);

        // then
        assertEquals(0, sut.getSize());
    }

    @Test
    void populateFirmwareMap_Success() {
        // when
        PsgSignatureEndiannessMapImpl sut = new PsgSignatureEndiannessMapImpl(EndiannessActor.FIRMWARE);

        // then
        assertEquals(CONVERT, sut.get(PSG_SIG_MAGIC));
        assertEquals(CONVERT, sut.get(PSG_SIG_SIZE_R));
        assertEquals(CONVERT, sut.get(PSG_SIG_SIZE_S));
        assertEquals(CONVERT, sut.get(PSG_SIG_HASH_MAGIC));
        assertEquals(4, sut.getSize());
    }
}
