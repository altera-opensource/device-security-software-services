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

package com.intel.bkp.crypto.x509.utils;

import com.intel.bkp.test.FileUtils;
import org.bouncycastle.asn1.x509.AuthorityKeyIdentifier;
import org.bouncycastle.asn1.x509.SubjectKeyIdentifier;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;

import java.security.PublicKey;
import java.security.cert.X509Certificate;

import static com.intel.bkp.utils.HexConverter.toHex;
import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertNull;

class KeyIdentifierUtilsTest {

    /* Below certs were generated using OpenSSL with command:
        openssl req -newkey rsa:2048 -nodes -keyout test.pem -x509 -days 365 -out cert.pem
        The fact if cert contains AKI and SKI extensions depends on content of openssl.cnf
        Value of KEY_IDENTIFIER is equal to both AKI and SKI (cert is self-signed) and is taken from output of command:
        openssl x509 --text -in cert.pem
     */
    private static final String CERT_WITHOUT_AKI_AND_SKI = "cert_withoutAKIandSKI.pem";
    private static final String CERT_WITH_AKI_AND_SKI = "cert_withAKIandSKI.pem";
    private static final String KEY_IDENTIFIER = "3C7A2451CC347770F6A0A13F5F697218A5481434";
    // https://tsci.intel.com/content/IPCS/certs/IPCS_agilex.cer
    private static final String CERT_WITH_SKI_METHOD_2_RFC7093 = "IPCS_agilex.cer";

    private static X509Certificate certWithoutAKIandSKI;
    private static X509Certificate certWithAKIandSKI;
    private static X509Certificate certWithSkiMethod2Rfc7093;

    @BeforeAll
    static void init() {
        certWithoutAKIandSKI = FileUtils.loadCertificate(CERT_WITHOUT_AKI_AND_SKI);
        certWithAKIandSKI = FileUtils.loadCertificate(CERT_WITH_AKI_AND_SKI);
        certWithSkiMethod2Rfc7093 = FileUtils.loadCertificate(CERT_WITH_SKI_METHOD_2_RFC7093);
    }

    @Test
    void createAuthorityKeyIdentifier_SKIExists_Success() throws Exception {
        // given
        final byte[] ski = KeyIdentifierUtils.getSubjectKeyIdentifier(certWithAKIandSKI);

        // when
        final AuthorityKeyIdentifier aki = KeyIdentifierUtils.createAuthorityKeyIdentifier(certWithAKIandSKI);

        // then
        assertNotNull(aki);
        assertArrayEquals(ski, aki.getKeyIdentifier());
    }

    @Test
    void createAuthorityKeyIdentifier_SKIDoesNotExist_Success() throws Exception {
        // when
        final AuthorityKeyIdentifier aki = KeyIdentifierUtils.createAuthorityKeyIdentifier(certWithoutAKIandSKI);

        // then
        assertNotNull(aki);
    }

    @Test
    void getAuthorityKeyIdentifier_Success() {
        // when
        final byte[] aki = KeyIdentifierUtils.getAuthorityKeyIdentifier(certWithAKIandSKI);

        // then
        assertEquals(KEY_IDENTIFIER, toHex(aki));
    }

    @Test
    void getAuthorityKeyIdentifier_AKIDoesNotExist_ReturnsNull() {
        // when
        final byte[] aki = KeyIdentifierUtils.getAuthorityKeyIdentifier(certWithoutAKIandSKI);

        // then
        assertNull(aki);
    }

    @Test
    void getSubjectKeyIdentifier_Success() {
        // when
        final byte[] ski = KeyIdentifierUtils.getSubjectKeyIdentifier(certWithAKIandSKI);

        // then
        assertEquals(KEY_IDENTIFIER, toHex(ski));
    }

    @Test
    void createSubjectKeyIdentifier_Success() {
        // given
        final PublicKey publicKey = certWithSkiMethod2Rfc7093.getPublicKey();
        final SubjectKeyIdentifier expectedSki =
            new SubjectKeyIdentifier(KeyIdentifierUtils.getSubjectKeyIdentifier(certWithSkiMethod2Rfc7093));

        // when
        final SubjectKeyIdentifier ski = KeyIdentifierUtils.createSubjectKeyIdentifier(publicKey);

        // then
        assertEquals(expectedSki, ski);
    }

    @Test
    void calculateSubjectKeyIdentifierUsingMethod2FromRfc7093_Success() {
        // given
        final PublicKey publicKey = certWithSkiMethod2Rfc7093.getPublicKey();
        final byte[] expectedSki = KeyIdentifierUtils.getSubjectKeyIdentifier(certWithSkiMethod2Rfc7093);

        // when
        final byte[] ski = KeyIdentifierUtils.calculateSubjectKeyIdentifierUsingMethod2FromRfc7093(publicKey);

        // then
        assertEquals(toHex(expectedSki), toHex(ski));
    }

    @Test
    void getSubjectKeyIdentifier_SKIDoesNotExist_ReturnsNull() {
        // when
        final byte[] ski = KeyIdentifierUtils.getSubjectKeyIdentifier(certWithoutAKIandSKI);

        // then
        assertNull(ski);
    }

}
