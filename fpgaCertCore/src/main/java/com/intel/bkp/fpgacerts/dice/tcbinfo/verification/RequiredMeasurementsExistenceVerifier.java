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

package com.intel.bkp.fpgacerts.dice.tcbinfo.verification;

import com.intel.bkp.fpgacerts.dice.tcbinfo.TcbInfoKey;
import com.intel.bkp.fpgacerts.dice.tcbinfo.TcbInfoValue;
import lombok.extern.slf4j.Slf4j;

import java.util.Map;

import static com.intel.bkp.fpgacerts.dice.tcbinfo.MeasurementType.CMF;
import static com.intel.bkp.fpgacerts.dice.tcbinfo.MeasurementType.ROM_EXTENSION;

@Slf4j
public class RequiredMeasurementsExistenceVerifier {

    private String familyName;

    public RequiredMeasurementsExistenceVerifier withFamilyName(String familyName) {
        this.familyName = familyName;
        return this;
    }

    public boolean verify(Map<TcbInfoKey, TcbInfoValue> map) {
        final var existenceVerifier = MeasurementExistenceVerifier.instance(map);
        final boolean romExtensionMeasurementPresent = isRomExtensionMeasurementPresent(existenceVerifier);
        final boolean cmfMeasurementPresent = isCmfMeasurementPresent(existenceVerifier);
        final boolean allRequiredMeasurementsExist = romExtensionMeasurementPresent && cmfMeasurementPresent;

        if (!allRequiredMeasurementsExist) {
            log.error("""
                    Chain does not contain all required measurements.
                    Is Rom extension measurement present: {}
                    Is CMF measurement present: {}""",
                romExtensionMeasurementPresent, cmfMeasurementPresent);
        }

        return allRequiredMeasurementsExist;
    }

    private boolean isRomExtensionMeasurementPresent(MeasurementExistenceVerifier existenceVerifier) {
        return existenceVerifier.isMeasurementPresent(familyName, ROM_EXTENSION);
    }

    private boolean isCmfMeasurementPresent(MeasurementExistenceVerifier existenceVerifier) {
        return existenceVerifier.isMeasurementPresent(familyName, CMF);
    }
}
