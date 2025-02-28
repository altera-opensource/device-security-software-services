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

import com.intel.bkp.bkps.config.ApplicationProperties;
import com.intel.bkp.bkps.exception.PrefetchingFailedToDownloadException;
import com.intel.bkp.bkps.exception.PrefetchingGenericException;
import com.intel.bkp.bkps.rest.onboarding.model.ZipDiceParamsDTO;
import com.intel.bkp.bkps.rest.prefetching.model.PrefetchZipDataDTO;
import com.intel.bkp.bkps.rest.provisioning.utils.ZipUtil;
import com.intel.bkp.crypto.exceptions.X509CertificateParsingException;
import com.intel.bkp.crypto.x509.parsing.X509CertificateParser;
import com.intel.bkp.fpgacerts.chain.DistributionPointCertificate;
import com.intel.bkp.fpgacerts.chain.DistributionPointZip;
import com.intel.bkp.fpgacerts.dice.IpcsZipFetcher;
import com.intel.bkp.fpgacerts.url.params.ZipDiceParams;
import com.intel.bkp.utils.PathUtils;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Service;

import java.security.cert.X509Certificate;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

@Service
@Slf4j
public class ZipDiceFetchService extends FetchServiceBase<ZipDiceParamsDTO, PrefetchZipDataDTO> {

    private final IpcsZipFetcher ipcsZipFetcher;

    protected ZipDiceFetchService(ApplicationProperties applicationProperties,
                                  CacheZipFetcher zipFetcher,
                                  CacheChainFetcher chainFetcher,
                                  CacheCrlMapFetcher crlFetcher) {
        super(applicationProperties.getDistributionPoint(), chainFetcher, crlFetcher);
        final var dp = applicationProperties.getDistributionPoint();
        this.ipcsZipFetcher = new IpcsZipFetcher(zipFetcher,
            PathUtils.buildPath(dp.getMainPath(), dp.getZipBasePath()));
    }

    public PrefetchZipDataDTO fetch(ZipDiceParamsDTO data) {
        final ZipDiceParams zipDiceParams = new ZipDiceParams(data.id(), data.deviceIdHex(), data.family());
        final DistributionPointZip fetchedZip = ipcsZipFetcher.fetchZip(zipDiceParams)
            .orElseThrow(() -> new PrefetchingFailedToDownloadException(
                "ZIP cannot be retrieved for params: %s".formatted(data)));

        final byte[] zipContent = fetchedZip.getZipContent();
        final Map<String, byte[]> certs = ZipUtil.extractFilesFromZip(zipContent, ".cer");

        final List<DistributionPointCertificate> dpCerts = new ArrayList<>();
        for (Map.Entry<String, byte[]> entry : certs.entrySet()) {
            log.debug("Parsing certificate from ZIP: {}", entry.getKey());
            final byte[] certContent = entry.getValue();
            final X509Certificate x509Certificate = parseCertificate(entry.getKey(), certContent);
            dpCerts.add(new DistributionPointCertificate(entry.getKey(), x509Certificate));
        }
        log.debug("Parsed {} certificates.", dpCerts.size());

        final var fetchedCrls = fetchCrls(dpCerts);
        log.debug("Fetched {} CRLs.", fetchedCrls.size());

        return new PrefetchZipDataDTO(fetchedZip, fetchedCrls);
    }

    private X509Certificate parseCertificate(String name, byte[] certContent) {
        try {
            return X509CertificateParser.toX509Certificate(certContent);
        } catch (X509CertificateParsingException e) {
            throw new PrefetchingGenericException("Failed to parse X509 certificate from ZIP: %s".formatted(name));
        }
    }
}
