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

package com.intel.bkp.verifier.protocol.sigma.service;

import com.intel.bkp.core.manufacturing.model.PufType;
import com.intel.bkp.fpgacerts.dice.tcbinfo.TcbInfoMeasurement;
import com.intel.bkp.fpgacerts.dice.tcbinfo.TcbInfoMeasurementsAggregator;
import com.intel.bkp.fpgacerts.verification.EvidenceVerifier;
import com.intel.bkp.verifier.database.model.S10CacheEntity;
import com.intel.bkp.verifier.exceptions.CacheEntityDoesNotExistException;
import com.intel.bkp.verifier.model.VerifierExchangeResponse;
import com.intel.bkp.verifier.service.certificate.AppContext;
import com.intel.bkp.verifier.service.measurements.RimHandlersProvider;
import lombok.AccessLevel;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;

import java.util.List;
import java.util.Optional;

@Slf4j
@RequiredArgsConstructor(access = AccessLevel.PACKAGE)
public class GpS10AttestationComponent {

    private final EvidenceVerifier evidenceVerifier;
    private final S10AttestationRevocationService s10AttestationRevocationService;
    private final TcbInfoMeasurementsAggregator tcbInfoMeasurementsAggregator;
    private final GpDeviceMeasurementsProvider gpDeviceMeasurementsProvider;

    public GpS10AttestationComponent() {
        this(new EvidenceVerifier(new RimHandlersProvider()), new S10AttestationRevocationService(),
            new TcbInfoMeasurementsAggregator(), new GpDeviceMeasurementsProvider());
    }

    public VerifierExchangeResponse perform(String refMeasurementHex, byte[] deviceId) {
        return perform(AppContext.instance(), refMeasurementHex, deviceId);
    }

    VerifierExchangeResponse perform(AppContext appContext, String refMeasurementHex, byte[] deviceId) {
        final S10CacheEntity entity = readEntityFromDatabase(appContext, deviceId);

        s10AttestationRevocationService.checkAndRetrieve(deviceId, PufType.getPufTypeHex(entity.getPufType()));

        tcbInfoMeasurementsAggregator.add(getMeasurementsFromDevice(entity, deviceId));

        final var verificationResult = evidenceVerifier.verify(tcbInfoMeasurementsAggregator, refMeasurementHex);
        return VerifierExchangeResponse.from(verificationResult);
    }

    private List<TcbInfoMeasurement> getMeasurementsFromDevice(S10CacheEntity entity, byte[] deviceId) {
        final var measurementsRequest = GpDeviceMeasurementsRequest.forS10(deviceId, entity);
        return gpDeviceMeasurementsProvider.getMeasurementsFromDevice(measurementsRequest);
    }

    private S10CacheEntity readEntityFromDatabase(AppContext appContext, byte[] deviceId) {
        final Optional<S10CacheEntity> entity = appContext
            .getSqLiteHelper()
            .getS10CacheEntityService()
            .read(deviceId);

        return entity.orElseThrow(CacheEntityDoesNotExistException::new);
    }
}
