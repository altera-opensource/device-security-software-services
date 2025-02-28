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

package com.intel.bkp.fpgacerts.cbor.rim.comid.mapping;

import com.intel.bkp.fpgacerts.cbor.rim.comid.EnvironmentMap;
import com.intel.bkp.fpgacerts.cbor.rim.comid.MeasurementMap;
import com.intel.bkp.fpgacerts.cbor.rim.comid.ReferenceTriple;
import com.intel.bkp.fpgacerts.dice.tcbinfo.TcbInfoKey;
import com.intel.bkp.fpgacerts.dice.tcbinfo.TcbInfoMeasurement;
import com.intel.bkp.fpgacerts.dice.tcbinfo.TcbInfoValue;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;
import org.mockito.Mock;
import org.mockito.junit.jupiter.MockitoExtension;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.mockito.Mockito.when;

@ExtendWith(MockitoExtension.class)
class ReferenceTripleToTcbInfoMeasurementMapperTest {

    @Mock
    private EnvironmentMap environmentMap;
    @Mock
    private MeasurementMap measurementMap;
    @Mock
    private TcbInfoKey tcbInfoKey;
    @Mock
    private TcbInfoValue tcbInfoValue;
    @Mock
    private EnvironmentMapToTcbInfoKeyMapper environmentMapToTcbInfoKeyMapper;
    @Mock
    private MeasurementMapToTcbInfoValueMapper measurementMapToTcbInfoValueMapper;

    private ReferenceTripleToTcbInfoMeasurementMapper sut;

    @BeforeEach
    void prepareSut() {
        sut = new ReferenceTripleToTcbInfoMeasurementMapper(
            environmentMapToTcbInfoKeyMapper,
            measurementMapToTcbInfoValueMapper
        );
    }

    @Test
    void map_Success() {
        // given
        when(environmentMapToTcbInfoKeyMapper.map(environmentMap)).thenReturn(tcbInfoKey);
        when(measurementMapToTcbInfoValueMapper.map(measurementMap)).thenReturn(tcbInfoValue);
        final var referenceTriple = new ReferenceTriple(environmentMap, measurementMap);

        // when
        final TcbInfoMeasurement result = sut.map(referenceTriple);

        // then
        //assertEquals(tcbInfoKey, result.getKey());
        assertEquals(tcbInfoValue, result.getValue());
    }
}
