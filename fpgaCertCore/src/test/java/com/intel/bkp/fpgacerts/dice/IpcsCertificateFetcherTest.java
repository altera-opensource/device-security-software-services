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

package com.intel.bkp.fpgacerts.dice;

import com.intel.bkp.fpgacerts.chain.DistributionPointCertificate;
import com.intel.bkp.fpgacerts.chain.ICertificateFetcher;
import com.intel.bkp.fpgacerts.exceptions.IpcsCertificateFetcherNotInitializedException;
import com.intel.bkp.fpgacerts.url.DistributionPointAddressProvider;
import com.intel.bkp.fpgacerts.url.params.DiceEnrollmentParams;
import com.intel.bkp.fpgacerts.url.params.DiceParams;
import com.intel.bkp.fpgacerts.url.params.parsing.DiceEnrollmentParamsIssuerParser;
import com.intel.bkp.fpgacerts.url.params.parsing.DiceParamsIssuerParser;
import com.intel.bkp.fpgacerts.url.params.parsing.DiceParamsSubjectParser;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;
import org.mockito.Mock;
import org.mockito.junit.jupiter.MockitoExtension;

import javax.security.auth.x500.X500Principal;
import java.security.cert.X509Certificate;
import java.util.Optional;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

@ExtendWith(MockitoExtension.class)
class IpcsCertificateFetcherTest {

    private static final String URL = "url";
    private static final DiceParams DICE_PARAMS = new DiceParams("SKI", "UID");
    private static final DiceEnrollmentParams DICE_ENROLLMENT_PARAMS = new DiceEnrollmentParams("SKIER", "SVN", "UID");
    private static final String IPCS_SIGNING_CA_COMMON_NAME = "CN=Intel:Agilex:IPCS";
    private static final String IPCS_ENROLLMENT_CERT_COMMON_NAME = "CN=Intel:Agilex:ER:01:1122334455667788";

    @Mock
    private X509Certificate firmwareCert;
    @Mock
    private X509Certificate deviceIdEnrollmentCert;
    @Mock
    private X509Certificate ipcsDeviceIdCert;
    @Mock
    private X500Principal deviceIdL0Issuer;
    @Mock
    private X509Certificate fetchedCert;
    @Mock
    private ICertificateFetcher<X509Certificate> certificateFetcher;
    @Mock
    private DiceParamsSubjectParser diceParamsSubjectParser;
    @Mock
    private DiceParamsIssuerParser diceParamsIssuerParser;
    @Mock
    private DiceEnrollmentParamsIssuerParser diceEnrollmentParamsIssuerParser;
    @Mock
    private DistributionPointAddressProvider addressProvider;

    private IpcsCertificateFetcher sut;

    private DistributionPointCertificate fetchedDpCert;

    @BeforeEach
    void initSut() {
        sut = new IpcsCertificateFetcher(certificateFetcher, diceParamsSubjectParser, diceParamsIssuerParser,
            diceEnrollmentParamsIssuerParser, addressProvider);
        sut.clear();
        fetchedDpCert = new DistributionPointCertificate(URL, fetchedCert);
    }

    @Test
    void fetchDeviceIdCert_NoCerts_Throws() {
        // when-then
        assertThrows(IpcsCertificateFetcherNotInitializedException.class, () -> sut.fetchIpcsDeviceIdCert());
    }

    @Test
    void fetchDeviceIdCert_OnlyFirmwareCert_Success() {
        // given
        sut.setFirmwareCert(firmwareCert);
        mockParsingParamsFromFirmwareCert();
        mockDeviceIdUrl();
        mockFetchingCertificate();

        // when
        final var result = sut.fetchIpcsDeviceIdCert();

        // then
        assertEquals(fetchedDpCert, result.orElseThrow());
    }

    @Test
    void fetchDeviceIdCert_BothCerts_UsesFirmwareCert() {
        // given
        sut.setFirmwareCert(firmwareCert);
        sut.setDeviceIdL0Cert(deviceIdEnrollmentCert);
        mockParsingParamsFromFirmwareCert();
        mockDeviceIdUrl();
        mockFetchingCertificate();

        // when
        final var result = sut.fetchIpcsDeviceIdCert();

        // then
        assertEquals(fetchedDpCert, result.orElseThrow());
    }

    @Test
    void fetchDeviceIdCert_OnlyDeviceIdEnrollmentCert_Success() {
        // given
        sut.setDeviceIdL0Cert(deviceIdEnrollmentCert);
        mockParsingParamsFromDeviceIdEnrollmentCertSubject();
        mockDeviceIdUrl();
        mockFetchingCertificate();

        // when
        final var result = sut.fetchIpcsDeviceIdCert();

        // then
        assertEquals(fetchedDpCert, result.orElseThrow());
    }

    @Test
    void fetchDeviceIdCert_SecondInvocation_ReturnsPreviouslyFetchedCert() {
        // given
        sut.setFirmwareCert(firmwareCert);
        mockParsingParamsFromFirmwareCert();
        mockDeviceIdUrl();
        mockFetchingCertificate();

        // when
        sut.fetchIpcsDeviceIdCert();
        sut.fetchIpcsDeviceIdCert();

        // then
        verify(diceParamsIssuerParser).parse(any());
        verify(certificateFetcher).fetchCertificate(any());
    }

    @Test
    void fetchDeviceIdCert_NoCertFetched_ReturnsEmptyOptional() {
        // given
        sut.setFirmwareCert(firmwareCert);
        mockParsingParamsFromFirmwareCert();
        mockDeviceIdUrl();
        mockFetchingCertificateDoesNotExist();

        // when
        final var result = sut.fetchIpcsDeviceIdCert();

        // then
        assertTrue(result.isEmpty());
    }

    @Test
    void fetchEnrollmentCert_NoCerts_Throws() {
        // when-then
        assertThrows(IpcsCertificateFetcherNotInitializedException.class,
            () -> sut.fetchIpcsEnrollmentCert());
    }

    @Test
    void fetchEnrollmentCert_OnlyFirmwareCert_Throws() {
        // given
        sut.setFirmwareCert(firmwareCert);

        // when-then
        assertThrows(IpcsCertificateFetcherNotInitializedException.class,
            () -> sut.fetchIpcsEnrollmentCert());
    }

    @Test
    void fetchEnrollmentCert_OnlyDeviceIdEnrollmentCert_Success() {
        // given
        sut.setDeviceIdL0Cert(deviceIdEnrollmentCert);
        mockDeviceIdL0Issuer(deviceIdEnrollmentCert, IPCS_ENROLLMENT_CERT_COMMON_NAME);
        mockParsingEnrollmentParamsFromDeviceIdEnrollmentCert();
        mockEnrollmentUrl();
        mockFetchingCertificate();

        // when
        final var result = sut.fetchIpcsEnrollmentCert();

        // then
        assertEquals(fetchedDpCert, result.orElseThrow());
    }

    @Test
    void fetchEnrollmentCert_OnlyDeviceIdL0CertOtherThanEnrollment_Throws() {
        // given
        sut.setDeviceIdL0Cert(ipcsDeviceIdCert);
        mockDeviceIdL0Issuer(ipcsDeviceIdCert, IPCS_SIGNING_CA_COMMON_NAME);

        // when-then
        final IpcsCertificateFetcherNotInitializedException ex =
            assertThrows(IpcsCertificateFetcherNotInitializedException.class,
                () -> sut.fetchIpcsEnrollmentCert());

        assertEquals("DeviceIdEnrollment certificate was not provided - failed to determine URL params.",
            ex.getMessage());
    }

    @Test
    void fetchEnrollmentCert_SecondInvocation_ReturnsPreviouslyFetchedCert() {
        // given
        sut.setDeviceIdL0Cert(deviceIdEnrollmentCert);
        mockDeviceIdL0Issuer(deviceIdEnrollmentCert, IPCS_ENROLLMENT_CERT_COMMON_NAME);
        mockParsingEnrollmentParamsFromDeviceIdEnrollmentCert();
        mockEnrollmentUrl();
        mockFetchingCertificate();

        // when
        sut.fetchIpcsEnrollmentCert();
        sut.fetchIpcsEnrollmentCert();

        // then
        verify(diceEnrollmentParamsIssuerParser).parse(any());
        verify(certificateFetcher).fetchCertificate(any());
    }

    @Test
    void fetchEnrollmentCert_NoCertFetched_ReturnsEmptyOptional() {
        // given
        sut.setDeviceIdL0Cert(deviceIdEnrollmentCert);
        mockDeviceIdL0Issuer(deviceIdEnrollmentCert, IPCS_ENROLLMENT_CERT_COMMON_NAME);
        mockParsingEnrollmentParamsFromDeviceIdEnrollmentCert();
        mockEnrollmentUrl();
        mockFetchingCertificateDoesNotExist();

        // when
        final var result = sut.fetchIpcsEnrollmentCert();

        // then
        assertTrue(result.isEmpty());
    }

    @Test
    void fetchIidUdsCert_NoCerts_Throws() {
        // when-then
        assertThrows(IpcsCertificateFetcherNotInitializedException.class, () -> sut.fetchIpcsIidUdsCert());
    }

    @Test
    void fetchIidUdsCert_OnlyFirmwareCert_Throws() {
        // given
        sut.setFirmwareCert(firmwareCert);

        // when-then
        assertThrows(IpcsCertificateFetcherNotInitializedException.class, () -> sut.fetchIpcsIidUdsCert());
    }

    @Test
    void fetchIidUdsCert_OnlyDeviceIdEnrollmentCert_Success() {
        // given
        sut.setDeviceIdL0Cert(deviceIdEnrollmentCert);
        mockDeviceIdL0Issuer(deviceIdEnrollmentCert, IPCS_ENROLLMENT_CERT_COMMON_NAME);
        mockParsingParamsFromDeviceIdEnrollmentCertIssuer();
        mockIidUdsUrl();
        mockFetchingCertificate();

        // when
        final var result = sut.fetchIpcsIidUdsCert();

        // then
        assertEquals(fetchedDpCert, result.orElseThrow());
    }

    @Test
    void fetchIidUdsCert_OnlyDeviceIdL0CertOtherThanEnrollment_Throws() {
        // given
        sut.setDeviceIdL0Cert(ipcsDeviceIdCert);
        mockDeviceIdL0Issuer(ipcsDeviceIdCert, IPCS_SIGNING_CA_COMMON_NAME);

        // when-then
        final IpcsCertificateFetcherNotInitializedException ex =
            assertThrows(IpcsCertificateFetcherNotInitializedException.class,
                () -> sut.fetchIpcsIidUdsCert());

        assertEquals("DeviceIdEnrollment certificate was not provided - failed to determine URL params.",
            ex.getMessage());
    }

    @Test
    void fetchIidUdsCert_SecondInvocation_ReturnsPreviouslyFetchedCert() {
        // given
        sut.setDeviceIdL0Cert(deviceIdEnrollmentCert);
        mockDeviceIdL0Issuer(deviceIdEnrollmentCert, IPCS_ENROLLMENT_CERT_COMMON_NAME);
        mockParsingParamsFromDeviceIdEnrollmentCertIssuer();
        mockIidUdsUrl();
        mockFetchingCertificate();

        // when
        sut.fetchIpcsIidUdsCert();
        sut.fetchIpcsIidUdsCert();

        // then
        verify(diceParamsIssuerParser).parse(any());
        verify(certificateFetcher).fetchCertificate(any());
    }

    @Test
    void fetchIidUdsCert_NoCertFetched_ReturnsEmptyOptional() {
        // given
        sut.setDeviceIdL0Cert(deviceIdEnrollmentCert);
        mockDeviceIdL0Issuer(deviceIdEnrollmentCert, IPCS_ENROLLMENT_CERT_COMMON_NAME);
        mockParsingParamsFromDeviceIdEnrollmentCertIssuer();
        mockIidUdsUrl();
        mockFetchingCertificateDoesNotExist();

        // when
        final var result = sut.fetchIpcsIidUdsCert();

        // then
        assertTrue(result.isEmpty());
    }

    private void mockParsingParamsFromFirmwareCert() {
        when(diceParamsIssuerParser.parse(firmwareCert)).thenReturn(DICE_PARAMS);
    }

    private void mockParsingParamsFromDeviceIdEnrollmentCertSubject() {
        when(diceParamsSubjectParser.parse(deviceIdEnrollmentCert)).thenReturn(DICE_PARAMS);
    }

    private void mockParsingParamsFromDeviceIdEnrollmentCertIssuer() {
        when(diceParamsIssuerParser.parse(deviceIdEnrollmentCert)).thenReturn(DICE_PARAMS);
    }

    private void mockParsingEnrollmentParamsFromDeviceIdEnrollmentCert() {
        when(diceEnrollmentParamsIssuerParser.parse(deviceIdEnrollmentCert)).thenReturn(DICE_ENROLLMENT_PARAMS);
    }

    private void mockDeviceIdUrl() {
        when(addressProvider.getDeviceIdCertUrl(DICE_PARAMS)).thenReturn(URL);
    }

    private void mockIidUdsUrl() {
        when(addressProvider.getIidUdsCertUrl(DICE_PARAMS)).thenReturn(URL);
    }

    private void mockEnrollmentUrl() {
        when(addressProvider.getEnrollmentCertUrl(DICE_ENROLLMENT_PARAMS)).thenReturn(URL);
    }

    private void mockFetchingCertificate() {
        when(certificateFetcher.fetchCertificate(URL)).thenReturn(Optional.of(fetchedCert));
    }

    private void mockFetchingCertificateDoesNotExist() {
        when(certificateFetcher.fetchCertificate(URL)).thenReturn(Optional.empty());
    }

    private void mockDeviceIdL0Issuer(X509Certificate deviceIdL0Cert, String value) {
        when(deviceIdL0Cert.getIssuerX500Principal()).thenReturn(deviceIdL0Issuer);
        when(deviceIdL0Issuer.getName()).thenReturn(value);
    }
}
