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

package com.intel.bkp.bkps.attestation;

import com.intel.bkp.bkps.exception.PrefetchingFailedToDownloadException;
import com.intel.bkp.bkps.exception.PrefetchingGenericException;
import com.intel.bkp.fpgacerts.chain.ChainFetcherBase;
import com.intel.bkp.fpgacerts.chain.DistributionPointCertificate;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Component;

import java.security.cert.X509Certificate;
import java.util.List;

@Slf4j
@Component
public class CacheChainFetcher extends ChainFetcherBase {

    public CacheChainFetcher(CacheCertificateFetcher certificateFetcher) {
        super(certificateFetcher);
    }

    @Override
    public List<DistributionPointCertificate> fetchCertificateChain(String url) {
        return super.fetchCertificateChain(url);
    }

    @Override
    public List<DistributionPointCertificate> fetchCertificateChain(X509Certificate cert) {
        return super.fetchCertificateChain(cert);
    }

    @Override
    protected RuntimeException getFetchingFailureException(String url) {
        return new PrefetchingFailedToDownloadException("Failed to download certificate: " + url);
    }

    @Override
    protected RuntimeException getNoIssuerCertUrlException(String certificateSubject) {
        return new PrefetchingGenericException(
            "Fetched certificate doesn't have issuer url set and it's not self-signed: " + certificateSubject);
    }
}
