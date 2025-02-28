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

package com.intel.bkp.core.psgcertificate;

import com.intel.bkp.core.endianness.EndiannessActor;
import com.intel.bkp.core.exceptions.ParseStructureException;
import com.intel.bkp.core.psgcertificate.enumerations.KeyWrappingType;
import com.intel.bkp.core.psgcertificate.enumerations.StorageType;
import com.intel.bkp.test.FileUtils;
import com.intel.bkp.test.enumeration.ResourceDir;
import org.junit.jupiter.api.Test;

import static com.intel.bkp.utils.HexConverter.toHex;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertThrows;

class PsgAesKeyBuilderSDM12Test {

    private static final String AES_KEY_E2E = "3022E09098452FF5521674B193D0FB50D64D92D8977461F403161D86CEDEFE12";
    private static final String AES_KEY_CUSTOM = "9F4AC374FFE4226BFDF36F52B9B85603468634BD21A20E50AF8BBE9B1C233226";

    @Test
    void parse_WithActualData_Success() {
        // given
        final byte[] aesContent = FileUtils.loadBinary(ResourceDir.ROOT, ("signed_iid_aes.ccert"));

        // when
        PsgAesKeyBuilderSDM12 builder = new PsgAesKeyBuilderSDM12()
            .withActor(EndiannessActor.FIRMWARE)
            .parse(aesContent);

        // then
        commonAssert(aesContent, builder, StorageType.PUFSS, KeyWrappingType.USER_IID_PUF);
        assertEquals(AES_KEY_E2E, toHex(builder.getUserAesRootKey()));
    }

    @Test
    void parse_WithActualData_withBBram_Success() {
        // given
        final byte[] aesContent = FileUtils.loadBinary(ResourceDir.ROOT, "signed_bbram_aes.ccert");

        // when
        PsgAesKeyBuilderSDM12 builder = new PsgAesKeyBuilderSDM12()
            .withActor(EndiannessActor.FIRMWARE)
            .parse(aesContent);

        // then
        commonAssert(aesContent, builder, StorageType.BBRAM, KeyWrappingType.INTERNAL);
        assertEquals(AES_KEY_E2E, toHex(builder.getUserAesRootKey()));
    }

    @Test
    void parse_WithActualData_withEfuse_Success() {
        // given
        final byte[] aesContent = FileUtils.loadBinary(ResourceDir.ROOT,
            "signed_efuse_aes.ccert");

        // when
        PsgAesKeyBuilderSDM12 builder = new PsgAesKeyBuilderSDM12()
            .withActor(EndiannessActor.FIRMWARE)
            .parse(aesContent);

        // then
        commonAssert(aesContent, builder, StorageType.EFUSES, KeyWrappingType.INTERNAL);
        assertEquals(AES_KEY_E2E, toHex(builder.getUserAesRootKey()));
    }

    @Test
    void parse_WithActualData_withEfuseNotFromE2eTests_Success() {
        // given
        final byte[] aesContent = FileUtils.loadBinary(ResourceDir.ROOT,
            "signed_efuse_aes_wrong.ccert");

        // when
        PsgAesKeyBuilderSDM12 builder = new PsgAesKeyBuilderSDM12()
            .withActor(EndiannessActor.FIRMWARE)
            .parse(aesContent);

        // then
        commonAssert(aesContent, builder, StorageType.EFUSES, KeyWrappingType.INTERNAL);

        assertEquals(AES_KEY_CUSTOM, toHex(builder.getUserAesRootKey()));
    }

    @Test
    void parse_WithSDM15Certificate_Throws() {
        // given
        final byte[] aesContent = FileUtils.loadBinary(ResourceDir.ROOT, ("signed_UDS_intelpuf_wrapped_aes_testmode1.ccert"));

        // when - then
        final var exception = assertThrows(ParseStructureException.class, () -> new PsgAesKeyBuilderSDM12()
            .withActor(EndiannessActor.FIRMWARE)
            .parse(aesContent));

        // then
        assertEquals("Mismatch between certificate SDM version and Builder version. "
            + "Certificate SDM version: 2. Builder/parser version: 0", exception.getMessage());
    }

    private void commonAssert(byte[] content, PsgAesKeyBuilderSDM12 builder, StorageType storage, KeyWrappingType keyType) {
        assertNotNull(builder);
        assertEquals(storage, builder.getStorageType());
        assertEquals(keyType, builder.getKeyWrappingType());

        final String expected = toHex(content);
        final String actual = toHex(builder.build().array());
        assertEquals(expected, actual);
    }
}
