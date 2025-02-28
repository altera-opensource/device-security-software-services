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

package com.intel.bkp.fpgacerts.utils;

import com.intel.bkp.test.CertificateUtils;
import org.apache.commons.lang3.StringUtils;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;

import java.security.PublicKey;
import java.security.cert.X509Certificate;

import static com.intel.bkp.utils.HexConverter.fromHex;
import static org.junit.jupiter.api.Assertions.assertEquals;

class SkiHelperTest {

    private static final String TEST_FOLDER = "certs/dice/";
    private static final String CERT_WITH_SKI_SHA384 = "deviceid_0123456789abcdef_H-e6TM4R12mufV0ZECGRv-fzc-I.cer";
    private static final String EXPECTED_SKI_BASE64URL = "H-e6TM4R12mufV0ZECGRv-fzc-I";
    private static final String PUBLIC_KEY_XY_BYTES = "BC372F488A1AE307CC9EE542DE03C67EF88E223D6A52780A2BA247DDCFC78360"
        + "403DA175921F300D8BCB7F06CC48B5C686B55E2FCC0A47EBEA182CBD6E5AACA40C074CD55355412BD9972E6FD22C8FC98C50BDAE7F06"
        + "CAC1FEF97AADDC3B66F1";

    private static X509Certificate certificate;

    @BeforeAll
    static void setUpClass() {
        certificate = CertificateUtils.readCertificate(TEST_FOLDER, CERT_WITH_SKI_SHA384);
    }

    @Test
    void getSkiInBase64UrlForDiceSubject_WithPublicKey_Success() {
        // given
        final PublicKey publicKey = certificate.getPublicKey();

        // when
        final String result = SkiHelper.getSkiInBase64UrlForDiceSubject(publicKey);

        // then
        assertEquals(StringUtils.left(EXPECTED_SKI_BASE64URL, 16), result);
    }

    @Test
    void getSkiInBase64UrlForUrl_WithXyBytes_Success() {
        // when
        final String result = SkiHelper.getSkiInBase64UrlForUrl(fromHex(PUBLIC_KEY_XY_BYTES));

        // then
        assertEquals(EXPECTED_SKI_BASE64URL, result);
    }

    @Test
    void getFwIdInBase64UrlForUrl_Success() {
        // given
        final String FW_ID_DIGEST = "32883E2526F54EA21FBF99642A8F56E787A0319D1D0E2AF84C36352E9A760EE80EA6C427098D17D26F"
            + "65723C0C1C66EA";
        final String EXPECTED_FW_ID_BASE64URL = "Mog-JSb1TqIfv5lkKo9W54egMZ0d";

        // when
        final String result = SkiHelper.getFwIdInBase64UrlForUrl(fromHex(FW_ID_DIGEST));

        // then
        assertEquals(EXPECTED_FW_ID_BASE64URL, result);
    }

    @Test
    void getPdiForUrlFrom_Success() {
        // given
        final String deviceIdentityHex = "A8E6A021035207CA82BA15FCFA11476F17A7836976A2046A030C06F816BF6751";
        final String expectedPdiUrlEncoded = "qOagIQNSB8qCuhX8-hFHbxeng2k";

        // when
        final String result = SkiHelper.getPdiForUrlFrom(fromHex(deviceIdentityHex));

        // then
        assertEquals(expectedPdiUrlEncoded, result);
    }
}
