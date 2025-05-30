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

package com.intel.bkp.utils;

import org.junit.jupiter.api.AfterAll;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;
import org.mockito.MockedStatic;

import java.time.Instant;
import java.time.LocalDateTime;
import java.time.ZoneOffset;
import java.util.Date;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.mockito.Mockito.CALLS_REAL_METHODS;
import static org.mockito.Mockito.mockStatic;
import static org.mockito.Mockito.when;

class X509DateBuilderHelperTest {
    private static final Instant NOW_INSTANT = toInstant("2021-09-28T11:21:58");

    private static final String NOW_DATE = "2021-09-28";
    private static final String NOW_PLUS_5_YEARS_DATE = "2026-09-28";
    private static final String NOW_PLUS_5_HOURS = "2021-09-28T16:21:58";
    private static final Date NOW_PLUS_5_YEARS = Date.from(toInstant(NOW_PLUS_5_YEARS_DATE + "T11:21:58"));
    private static final Integer PLUS_YEARS = 5;

    private static final String NOW_PLUS_12_HOURS_DATE_TIME = "2021-09-28 23:21:58";
    private static final String NOW_PLUS_24_HOURS_DATE_TIME = "2021-09-29 11:21:58";
    private static final String INFINITE_DATE = "9999-12-31";
    private static final Integer PLUS_12_HOURS = 12;
    private static final Integer PLUS_24_HOURS = 24;

    private static MockedStatic<Instant> instantMockStatic;

    @BeforeAll
    static void prepareStaticMock() {
        instantMockStatic = mockStatic(Instant.class, CALLS_REAL_METHODS);
        when(Instant.now()).thenReturn(NOW_INSTANT);
    }

    @AfterAll
    static void closeStaticMock() {
        instantMockStatic.close();
    }

    private static Instant toInstant(String dateInput) {
        return LocalDateTime.parse(dateInput).toInstant(ZoneOffset.UTC);
    }

    @Test
    void notBefore_Success() {
        // given
        final Date expected = Date.from(NOW_INSTANT);

        // when
        final Date result = X509DateBuilderHelper.notBefore();

        // then
        assertEquals(expected, result);
    }

    @Test
    void notBeforeDate_Success() {
        // when
        final String result = X509DateBuilderHelper.notBeforeDate();

        // then
        assertEquals(NOW_DATE, result);
    }

    @Test
    void notAfter_WithAddedYears_Success() {
        // when
        Date actualDate = X509DateBuilderHelper.notAfter(5);

        // then
        assertEquals(NOW_PLUS_5_YEARS, actualDate);
    }

    @Test
    void notAfter_WithStartDateAndAddedHours_Success() {
        //given
        Date startDateNow = DateBuilder.now().build();
        Date expectedDate = Date.from(toInstant(NOW_PLUS_5_HOURS));

        // when
        Date actualDate = X509DateBuilderHelper.notAfter(startDateNow, 5);

        // then
        assertEquals(expectedDate, actualDate);
    }

    @Test
    void notAfter_Success() {
        //given
        Date expectedDate = Date.from(toInstant(INFINITE_DATE + "T23:59:59"));

        // when
        Date actualDate = X509DateBuilderHelper.notAfter();

        // then
        assertEquals(expectedDate, actualDate);
    }

    @Test
    void notAfterDate_Plus5_Success() {
        // when
        final String result = X509DateBuilderHelper.notAfterDate(PLUS_YEARS);

        // then
        assertEquals(NOW_PLUS_5_YEARS_DATE, result);
    }

    @Test
    void notAfterDateTime_Plus12Hours_Success() {
        // when
        final String result = X509DateBuilderHelper.notAfterDateTime(PLUS_12_HOURS);

        // then
        assertEquals(NOW_PLUS_12_HOURS_DATE_TIME, result);
    }

    @Test
    void notAfterDateTime_Plus24Hours_Success() {
        // when
        final String result = X509DateBuilderHelper.notAfterDateTime(PLUS_24_HOURS);

        // then
        assertEquals(NOW_PLUS_24_HOURS_DATE_TIME, result);
    }

    @Test
    void notAfterDate_Success() {
        // when
        String actualDateString = X509DateBuilderHelper.notAfterDate();

        // then
        assertEquals(INFINITE_DATE, actualDateString);
    }
}
