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

package com.intel.bkp.crypto.aesgcm;

import com.intel.bkp.crypto.exceptions.EncryptionProviderException;
import com.intel.bkp.crypto.exceptions.KeystoreGenericException;
import com.intel.bkp.test.KeyGenUtils;
import org.bouncycastle.jce.provider.BouncyCastleProvider;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;
import org.mockito.junit.jupiter.MockitoExtension;

import java.nio.ByteOrder;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.mockito.Mockito.CALLS_REAL_METHODS;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

@ExtendWith(MockitoExtension.class)
public class AesGcmProviderTest {

    private final AesGcmProvider sut = mock(AesGcmProvider.class, CALLS_REAL_METHODS);

    @Test
    void encrypt_WithEmptySecretKey_ThrowsException() {
        // given
        prepareCipher();
        prepareProvider();
        prepareByteOrder();

        // when-then
        final EncryptionProviderException exception = assertThrows(EncryptionProviderException.class,
            () -> sut.encrypt(new byte[]{1})
        );

        // then
        assertEquals("Context Key is not set.", exception.getMessage());
    }

    @Test
    void encrypt_WithEmptyProvider_ThrowsException() throws Exception {
        // given
        prepareSecretKey();
        prepareCipher();
        prepareByteOrder();

        // when-then
        final EncryptionProviderException exception = assertThrows(EncryptionProviderException.class,
            () -> sut.encrypt(new byte[]{1})
        );

        // then
        assertEquals("Provider is not set.", exception.getMessage());
    }

    @Test
    void encrypt_WithEmptyCipher_ThrowsException() throws Exception {
        // given
        prepareSecretKey();
        prepareProvider();
        prepareByteOrder();

        // when-then
        final EncryptionProviderException exception = assertThrows(EncryptionProviderException.class,
            () -> sut.encrypt(new byte[]{1})
        );

        // then
        assertEquals("Cipher type is not set.", exception.getMessage());
    }

    @Test
    void encrypt_WithEmptyByteOrder_ThrowsException() throws Exception {
        // given
        prepareSecretKey();
        prepareProvider();
        prepareCipher();

        // when-then
        final EncryptionProviderException exception = assertThrows(EncryptionProviderException.class,
            () -> sut.encrypt(new byte[]{1})
        );

        // then
        assertEquals("ByteOrder is not set.", exception.getMessage());
    }

    @Test
    void decrypt_DataTooShort_ThrowsException() throws Exception {
        // given
        prepareAll();

        // when-then
        final EncryptionProviderException exception = assertThrows(EncryptionProviderException.class,
            () -> sut.decrypt(new byte[]{0, 0, 0, 12, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1}) // no data here
        );

        // then
        assertEquals("AES decryption failed.", exception.getMessage());
    }

    @Test
    void encrypt_decrypt_NoData_Success() throws Exception {
        // given
        prepareAll();
        byte[] expected = {};

        // when
        byte[] decrypt = sut.decrypt(sut.encrypt(expected));

        // then
        assertArrayEquals(expected, decrypt);
    }

    @Test
    void encrypt_decrypt_OneEmptyByte_Success() throws Exception {
        // given
        prepareAll();
        byte[] expected = new byte[1];

        // when
        byte[] decrypt = sut.decrypt(sut.encrypt(expected));

        // then
        assertArrayEquals(expected, decrypt);
    }

    @Test
    void encrypt_decrypt_OneByte_Success() throws Exception {
        // given
        prepareAll();
        byte[] expected = new byte[]{2};

        // when
        byte[] decrypt = sut.decrypt(sut.encrypt(expected));

        // then
        assertArrayEquals(expected, decrypt);
    }

    @Test
    void encrypt_decrypt_ManyBytes_Success() throws Exception {
        // given
        prepareAll();
        byte[] expected = new byte[]{1, 2, 3, 4, 5, 6, 7, 8, 9};

        // when
        byte[] decrypt = sut.decrypt(sut.encrypt(expected));

        // then
        assertArrayEquals(expected, decrypt);
    }

    @Test
    void encrypt_NullDataThrows() throws Exception {
        // given
        prepareAll();

        // when-then
        final EncryptionProviderException exception = assertThrows(EncryptionProviderException.class,
            () -> sut.encrypt(null)
        );

        // then
        assertEquals("AES encryption failed.", exception.getMessage());
    }

    @Test
    void decrypt_ThrowsNotValidIvException() throws Exception {
        // given
        prepareAll();

        // when-then
        final EncryptionProviderException exception = assertThrows(EncryptionProviderException.class,
            () -> sut.decrypt(new byte[]{1, 1})
        );

        // then
        assertEquals("Data to decrypt for AesGcm is incorrect.", exception.getMessage());
    }

    @Test
    void decrypt_ThrowsNotValidIvLengthException() throws Exception {
        // given
        prepareAll();

        // when-then
        final EncryptionProviderException exception = assertThrows(EncryptionProviderException.class,
            () -> sut.decrypt(new byte[]{0, 0, 0, 1, 0}) // iv len is 1, but should be 12
        );

        // then
        assertEquals("Invalid iv length.", exception.getMessage());
    }

    @Test
    void decrypt_ThrowsInvalidDataIvException() throws Exception {
        // given
        prepareAll();

        // when-then
        final EncryptionProviderException exception = assertThrows(EncryptionProviderException.class,
            () -> sut.decrypt(new byte[]{0, 0, 0, 12, 0})
        );

        // then
        assertEquals("Data to decrypt for AesGcm is incorrect.", exception.getMessage());
    }

    private void prepareSecretKey() throws KeystoreGenericException {
        when(sut.getSecretKey()).thenReturn(KeyGenUtils.genAes256());
    }

    private void prepareProvider() {
        when(sut.getProvider()).thenReturn(new BouncyCastleProvider());
    }

    private void prepareCipher() {
        when(sut.getCipherType()).thenReturn("GCM");
    }

    private void prepareByteOrder() {
        when(sut.getByteOrder()).thenReturn(ByteOrder.BIG_ENDIAN);
    }

    private void prepareAll() throws KeystoreGenericException {
        prepareSecretKey();
        prepareProvider();
        prepareCipher();
        prepareByteOrder();
    }
}
