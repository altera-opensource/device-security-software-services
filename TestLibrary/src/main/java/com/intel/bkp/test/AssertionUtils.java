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

package com.intel.bkp.test;

import com.intel.bkp.core.exceptions.BKPRuntimeException;
import com.intel.bkp.core.interfaces.IErrorCode;
import org.apache.commons.lang3.ArrayUtils;

import java.util.Arrays;
import java.util.Collections;
import java.util.List;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

public class AssertionUtils {

    public static void assertThatArrayIsSubarrayOfAnotherArray(byte[] array, byte[] subarrayToVerify) {
        List<Byte> outerArray = Arrays.asList(ArrayUtils.toObject(array));
        List<Byte> innerArray = Arrays.asList(ArrayUtils.toObject(subarrayToVerify));
        assertTrue(Collections.indexOfSubList(outerArray, innerArray) != -1);
    }

    public static void verifyExpectedErrorCode(BKPRuntimeException ex, IErrorCode codeMap) {
        assertEquals(codeMap.getCode(), ex.getErrorCode().getCode());
        assertEquals(codeMap.getExternalMessage(), ex.getErrorCode().getExternalMessage());
    }

    public static void verifyExpectedErrorCode(BKPRuntimeException ex, IErrorCode codeMap, String externalMessage) {
        assertEquals(codeMap.getCode(), ex.getErrorCode().getCode());
        assertEquals(externalMessage, ex.getErrorCode().getExternalMessage());
    }

    public static void verifyExpectedErrorCodeOnly(BKPRuntimeException ex, IErrorCode codeMap) {
        assertEquals(codeMap.getCode(), ex.getErrorCode().getCode());
    }

    public static void verifyExpectedMessage(BKPRuntimeException ex, String expectedMessage) {
        assertEquals(expectedMessage, ex.getMessage());
    }

    public static void verifyContainsExpectedMessage(BKPRuntimeException ex, String expectedMessage) {
        assertTrue(ex.getMessage().contains(expectedMessage));
    }
}
