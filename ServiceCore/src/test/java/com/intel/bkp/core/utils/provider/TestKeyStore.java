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

package com.intel.bkp.core.utils.provider;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.security.Key;
import java.security.KeyStoreException;
import java.security.KeyStoreSpi;
import java.security.PublicKey;
import java.security.UnrecoverableKeyException;
import java.security.cert.Certificate;
import java.util.Collections;
import java.util.Date;
import java.util.Enumeration;
import java.util.HashMap;

public class TestKeyStore extends KeyStoreSpi {

    private final HashMap<String, Object> store = new HashMap<>();
    public static final String password = "password";
    private static final String testKeyAliasPositive = "testAlias";
    private static final String testKeyAliasWrong = "testKeyAliasWrong";
    private static final String testKeyAliasWrongNullPubKey = "testKeyAliasWrongNullPubKey";
    private static final String testKeyAliasWrongNullChain = "wrongAliasNullChain";
    private static final String testKeyAliasWrongNullCertificateInChain = "testKeyAliasWrongNullCertificateInChain";
    private static final String testKeyAliasWrongEmptyCertificateChain = "testKeyAliasWrongEmptyCertificateChain";

    private static final Certificate certificate = new Certificate("certType") {
        @Override
        public byte[] getEncoded() {
            return new byte[0];
        }

        @Override
        public void verify(PublicKey key) {

        }

        @Override
        public void verify(PublicKey key, String sigProvider) {

        }

        @Override
        public String toString() {
            return null;
        }

        @Override
        public PublicKey getPublicKey() {
            return new PublicKey() {
                @Override
                public String getAlgorithm() {
                    return null;
                }

                @Override
                public String getFormat() {
                    return null;
                }

                @Override
                public byte[] getEncoded() {
                    return testKeyAliasPositive.getBytes();
                }
            };
        }
    };
    private static final Certificate certificateNullPubKey = new Certificate("certType") {
        @Override
        public byte[] getEncoded() {
            return new byte[0];
        }

        @Override
        public void verify(PublicKey key) {

        }

        @Override
        public void verify(PublicKey key, String sigProvider) {

        }

        @Override
        public String toString() {
            return null;
        }

        @Override
        public PublicKey getPublicKey() {
            return null;
        }
    };

    private Certificate[] prepareCertificateChain(String alias) {
        return switch (alias) {
            case testKeyAliasPositive -> new Certificate[]{certificate};
            case testKeyAliasWrongNullPubKey -> new Certificate[]{certificateNullPubKey};
            case testKeyAliasWrongNullChain -> null;
            case testKeyAliasWrongNullCertificateInChain -> new Certificate[]{null};
            case testKeyAliasWrongEmptyCertificateChain -> new Certificate[]{};
            default -> null;
        };
    }

    @Override
    public Key engineGetKey(String alias, char[] password) throws UnrecoverableKeyException {
        if (alias.equals(testKeyAliasPositive)) {
            return (Key) this.store.get(alias);
        } else {
            throw new UnrecoverableKeyException();
        }
    }

    @Override
    public Certificate[] engineGetCertificateChain(String alias) {
        return prepareCertificateChain(alias);
    }

    @Override
    public Certificate engineGetCertificate(String alias) {
        return null;
    }

    @Override
    public Date engineGetCreationDate(String alias) {
        return null;
    }

    @Override
    public void engineSetKeyEntry(String alias, Key key, char[] password,
                                  Certificate[] chain) throws KeyStoreException {
        if (alias.equals(testKeyAliasWrong)) {
            throw new KeyStoreException("Failed to set key entry");
        }

        this.store.put(alias, key);
    }

    @Override
    public void engineSetKeyEntry(String alias, byte[] key, Certificate[] chain) {

    }

    @Override
    public void engineSetCertificateEntry(String alias, Certificate cert) {

    }

    @Override
    public void engineDeleteEntry(String alias) throws KeyStoreException {
        if (alias.equals(testKeyAliasWrong)) {
            throw new KeyStoreException("Failed to remove key object");
        }

        this.store.remove(alias);
    }

    @Override
    public Enumeration<String> engineAliases() {
        return Collections.enumeration(this.store.keySet());
    }

    @Override
    public boolean engineContainsAlias(String alias) {
        return false;
    }

    @Override
    public int engineSize() {
        return 0;
    }

    @Override
    public boolean engineIsKeyEntry(String alias) {
        return true;
    }

    @Override
    public boolean engineIsCertificateEntry(String alias) {
        return false;
    }

    @Override
    public String engineGetCertificateAlias(Certificate cert) {
        return null;
    }

    @Override
    public void engineStore(OutputStream stream, char[] password) {

    }

    @Override
    public void engineLoad(InputStream stream, char[] password) throws IOException {

        String passwordString = new String(password);
        if (!passwordString.equals(TestKeyStore.password)) {
            throw new IOException("Wrong password");
        }
    }
}
