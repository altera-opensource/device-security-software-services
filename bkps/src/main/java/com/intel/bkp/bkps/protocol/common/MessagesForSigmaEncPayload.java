/*
 * This project is licensed as below.
 *
 * **************************************************************************
 *
 * Copyright 2020-2024 Intel Corporation. All Rights Reserved.
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

package com.intel.bkp.bkps.protocol.common;

import com.intel.bkp.bkps.command.CommandLayerService;
import com.intel.bkp.bkps.crypto.aesgcm.AesGcmSealingKeyProviderImpl;
import com.intel.bkp.bkps.crypto.sealingkey.SealingKeyManager;
import com.intel.bkp.bkps.domain.AesKey;
import com.intel.bkp.bkps.domain.ServiceConfiguration;
import com.intel.bkp.bkps.exception.ProvisioningGenericException;
import com.intel.bkp.command.logger.CommandLogger;
import com.intel.bkp.command.messages.common.CertificateBuilder;
import com.intel.bkp.command.model.CommandIdentifier;
import com.intel.bkp.core.endianness.EndiannessActor;
import com.intel.bkp.core.exceptions.ParseStructureException;
import com.intel.bkp.core.psgcertificate.PsgAesKeyBuilderFactory;
import com.intel.bkp.crypto.exceptions.EncryptionProviderException;
import lombok.AccessLevel;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Component;

import static com.intel.bkp.command.logger.CommandLoggerValues.CERTIFICATE_MESSAGE;
import static com.intel.bkp.utils.HexConverter.fromHex;

@Component
@RequiredArgsConstructor(access = AccessLevel.PACKAGE)
@Slf4j
public class MessagesForSigmaEncPayload {

    private final AesGcmSealingKeyProviderImpl aesGcmSealingKeyProvider;
    private final SealingKeyManager sealingKeyManager;
    private final CommandLayerService commandLayer;
    private final PsgAesKeyBuilderFactory psgAesKeyBuilderFactory = new PsgAesKeyBuilderFactory();

    public byte[] prepareFrom(ServiceConfiguration configuration) {
        final var confidentialData = configuration.getConfidentialData();
        final var aesKey = confidentialData.getAesKey();
        final var aesKeyBytes = decryptConfidentialData(aesKey);
        verifyAesKeyCanBeParsed(aesKeyBytes);

        final var certificate = new CertificateBuilder(aesKeyBytes)
            .testProgram(aesKey.getTestProgram())
            .build();

        final byte[] certificateBytes = commandLayer.create(certificate, CommandIdentifier.CERTIFICATE);
        CommandLogger.log(certificate, CERTIFICATE_MESSAGE, this.getClass());

        return certificateBytes;
    }

    private byte[] decryptConfidentialData(AesKey aesKey) {
        aesGcmSealingKeyProvider.initialize(sealingKeyManager.getActiveKey());
        final var customerAesKey = fromHex(aesKey.getValue());
        try {
            return aesGcmSealingKeyProvider.decrypt(customerAesKey);
        } catch (EncryptionProviderException e) {
            throw new ProvisioningGenericException("Failed to decrypt sensitive data with sealing key.", e);
        }
    }

    private void verifyAesKeyCanBeParsed(byte[] aesKeyBytes) {
        try {
            psgAesKeyBuilderFactory
                .withActor(EndiannessActor.FIRMWARE)
                .getPsgAesKeyBuilder(aesKeyBytes)
                .withActor(EndiannessActor.FIRMWARE)
                .parse(aesKeyBytes);
        } catch (ParseStructureException e) {
            throw new ProvisioningGenericException("Failed to get AES Key SDM version or parse Customer AES Key.", e);
        }
    }
}
