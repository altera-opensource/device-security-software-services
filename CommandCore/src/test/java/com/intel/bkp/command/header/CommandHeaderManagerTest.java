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

package com.intel.bkp.command.header;

import com.intel.bkp.command.exception.CommandHeaderValidationException;
import com.intel.bkp.command.exception.JtagResponseException;
import com.intel.bkp.command.exception.JtagUnknownCommandResponseException;
import org.junit.jupiter.api.Test;

import java.util.Base64;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertThrows;

public class CommandHeaderManagerTest {

    private static final int ERROR_CODE_SUCCESS = 0;
    private static final int ERROR_CODE_FAIL = 55; // just != 0
    private static final String REAL_RESPONSE_FROM_QUARTUS = "AAUAEA==";

    @Test
    void build_ReturnValidObject() {
        // given
        final byte[] expectedHeader = {17, 11, 64, 0};
        final CommandHeader commandHeader = new CommandHeader(0, 180, 1, 1);

        // when
        final byte[] result = CommandHeaderManager.build(commandHeader);

        // then
        assertNotNull(result);
        assertEquals(4, result.length);
        assertArrayEquals(expectedHeader, result);
    }

    @Test
    void build_parse_ReturnValidObject() throws CommandHeaderValidationException {
        // given
        final int expectedCode = 10;
        final int expectedArgSize = 180;
        final int expectedId = 7;
        final int expectedClient = 7;
        final CommandHeader commandHeader =
            new CommandHeader(expectedCode, expectedArgSize, expectedClient, expectedId);

        // when
        final byte[] buildHeader = CommandHeaderManager.build(commandHeader);
        CommandHeader result = CommandHeaderManager.parse(buildHeader);

        // then
        assertEquals(expectedCode, result.getCode());
        assertEquals(expectedArgSize, result.getArgumentsSize());
        assertEquals(expectedId, result.getId());
        assertEquals(expectedClient, result.getClient());
    }

    @Test
    void buildForFw_parseFromFw_ReturnValidObject() throws CommandHeaderValidationException {
        // given
        final int expectedCode = 10;
        final int expectedArgSize = 180;
        final int expectedId = 7;
        final int expectedClient = 7;
        final CommandHeader commandHeader =
            new CommandHeader(expectedCode, expectedArgSize, expectedClient, expectedId);

        // when
        final byte[] buildHeader = CommandHeaderManager.buildForFw(commandHeader);
        CommandHeader result = CommandHeaderManager.parseFromFw(buildHeader);

        // then
        assertEquals(expectedCode, result.getCode());
        assertEquals(expectedArgSize, result.getArgumentsSize());
        assertEquals(expectedId, result.getId());
        assertEquals(expectedClient, result.getClient());
    }

    @Test
    void parse_NullArray_ThrowException() {
        // given
        final byte[] commandHeader = null;

        // when
        assertThrows(CommandHeaderValidationException.class, () -> CommandHeaderManager.parse(commandHeader));
    }

    @Test
    void parse_TooSmallArray_ThrowException() {
        // given
        final byte[] commandHeader = { 1, 1 };

        // when
        assertThrows(CommandHeaderValidationException.class, () -> CommandHeaderManager.parse(commandHeader));
    }

    @Test
    void parse_TooBigArray_ThrowException() {
        // given
        final byte[] commandHeader = { 1, 1, 1, 1, 1 };

        // when
        assertThrows(CommandHeaderValidationException.class, () -> CommandHeaderManager.parse(commandHeader));
    }

    @Test
    void parse_ReturnValidObject() throws CommandHeaderValidationException {
        // given
        final int expectedCode = 0;
        final int expectedArgSize = 180;
        final int expectedId = 1;
        final int expectedClient = 1;
        final byte[] commandHeader = {17, 11, 64, 0};

        // when
        final CommandHeader result = CommandHeaderManager.parse(commandHeader);

        // then
        assertNotNull(result);
        assertEquals(expectedCode, result.getCode());
        assertEquals(expectedArgSize, result.getArgumentsSize());
        assertEquals(expectedId, result.getId());
        assertEquals(expectedClient, result.getClient());
    }

    @Test
    void validateCommandHeaderCode_ErrorCodeEqualsZero_Success() {
        // given
        CommandHeader header = new CommandHeader(ERROR_CODE_SUCCESS, 1, 2, 3);
        byte[] command = CommandHeaderManager.buildForFw(header);

        // when
        CommandHeaderManager.validateCommandHeaderCode(command, "TestResponse");
    }

    @Test
    void validateCommandHeaderCode_ErrorCodeEqualsNotZero_Throws() {
        // given
        CommandHeader header = new CommandHeader(ERROR_CODE_FAIL, 1, 2, 3);
        byte[] command = CommandHeaderManager.buildForFw(header);

        // when
        assertThrows(JtagResponseException.class,
            () -> CommandHeaderManager.validateCommandHeaderCode(command, "TestResponse"));
    }

    @Test
    void validateCommandHeaderCode_WithRealExample_Throws() {
        // given
        final byte[] receivedResponseHeader = Base64.getDecoder().decode(REAL_RESPONSE_FROM_QUARTUS);

        // when-then
        assertThrows(JtagResponseException.class,
            () -> CommandHeaderManager.validateCommandHeaderCode(receivedResponseHeader, "TEST"),
            "Command [TEST] of id '0' and client '1' failed with code: '1280' (0x500).");
    }

    @Test
    void validateCommandHeaderCode_WithUnknownCommand_Throws() {
        // given
        final CommandHeader commandHeader = new CommandHeader(FwErrorCodes.UNKNOWN_COMMAND.getCode(), 0, 1, 1);

        // when
        final byte[] result = CommandHeaderManager.buildForFw(commandHeader);

        // when-then
        assertThrows(JtagUnknownCommandResponseException.class,
            () -> CommandHeaderManager.validateCommandHeaderCode(result, "TEST"),
            "Command [TEST] of id '1' and client '1' failed with code: '3' (0x3).");
    }

    @Test
    void validateCommandHeaderCode_TooSmallData_Throws() {
        // given
        byte[] command = new byte[1];

        // when
        assertThrows(JtagResponseException.class,
            () -> CommandHeaderManager.validateCommandHeaderCode(command, "TestResponse"));
    }
}
