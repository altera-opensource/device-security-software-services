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

package com.intel.bkp.fpgacerts.measurements;

import com.intel.bkp.fpgacerts.dice.tcbinfo.TcbInfoMeasurement;
import com.intel.bkp.fpgacerts.measurements.mapping.IMeasurementResponseToTcbInfoMapper;
import com.intel.bkp.fpgacerts.measurements.mapping.SpdmMeasurementResponseToTcbInfoMapper;
import com.intel.bkp.protocol.spdm.exceptions.SpdmCommandFailedException;
import com.intel.bkp.protocol.spdm.jna.model.SpdmProtocol;
import com.intel.bkp.protocol.spdm.service.SpdmGetMeasurementsMessageSender;
import lombok.RequiredArgsConstructor;

import java.util.List;

@RequiredArgsConstructor
public class SpdmDeviceMeasurementsProvider implements IDeviceMeasurementsProvider<SpdmDeviceMeasurementsRequest> {

    private final SpdmGetMeasurementsMessageSender spdmGetMeasurementsMessageSender;
    private final IMeasurementResponseToTcbInfoMapper<SpdmMeasurementResponseProvider> measurementResponseMapper;

    public SpdmDeviceMeasurementsProvider(SpdmProtocol spdmProtocol) {
        this(new SpdmGetMeasurementsMessageSender(spdmProtocol), new SpdmMeasurementResponseToTcbInfoMapper());
    }

    @Override
    public List<TcbInfoMeasurement> getMeasurementsFromDevice(
        SpdmDeviceMeasurementsRequest request) throws SpdmCommandFailedException {
        return measurementResponseMapper.map(getMeasurementResponseFromDevice(request));
    }

    private SpdmMeasurementResponseProvider getMeasurementResponseFromDevice(
        SpdmDeviceMeasurementsRequest request) throws SpdmCommandFailedException {
        return new SpdmMeasurementResponseProvider(spdmGetMeasurementsMessageSender.send(request.slotId()));
    }
}
