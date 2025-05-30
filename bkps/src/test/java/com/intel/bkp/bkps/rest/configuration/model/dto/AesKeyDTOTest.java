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

package com.intel.bkp.bkps.rest.configuration.model.dto;

import com.intel.bkp.core.psgcertificate.enumerations.StorageType;
import jakarta.validation.ConstraintViolation;
import jakarta.validation.Validation;
import jakarta.validation.Validator;
import jakarta.validation.ValidatorFactory;
import org.apache.commons.lang3.StringUtils;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import java.util.Set;

import static com.intel.bkp.core.utils.ApplicationConstants.REQUEST_BODY_STRING_MAX_SIZE;
import static org.junit.jupiter.api.Assertions.assertEquals;

public class AesKeyDTOTest {

    private static final String VALID_AES_KEY = "0102";
    private static final String INVALID_AES_KEY_NOT_HEX = "*";
    private static final String INVALID_AES_KEY_TOO_LONG = StringUtils.repeat("0", REQUEST_BODY_STRING_MAX_SIZE + 2);

    private Validator validator;

    @BeforeEach
    void setUp() {
        ValidatorFactory validatorFactory = Validation.buildDefaultValidatorFactory();
        validator = validatorFactory.getValidator();
    }

    @Test
    void validation_WithValidSize_Success() {
        //given
        AesKeyDTO sut = prepareAesKeyDTO_WithValidStringSize();

        //when
        Set<ConstraintViolation<AesKeyDTO>> violations = validator.validate(sut);

        //then
        assertEquals(0, violations.size());
    }

    @Test
    void validation_WithInvalidSize_ReportViolation() {
        //given
        AesKeyDTO sut = prepareAesKeyDTO_WithInvalidStringSize();

        //when
        Set<ConstraintViolation<AesKeyDTO>> violations = validator.validate(sut);

        //then
        assertEquals(1, violations.size());
    }

    @Test
    void validation_WithNotHexString_ReportViolation() {
        //given
        AesKeyDTO sut = prepareAesKeyDTO_WithNotHexString();

        //when
        Set<ConstraintViolation<AesKeyDTO>> violations = validator.validate(sut);

        //then
        assertEquals(1, violations.size());
    }

    private AesKeyDTO prepareAesKeyDTO_WithValidStringSize() {
        return new AesKeyDTO(StorageType.PUFSS, VALID_AES_KEY);
    }

    private AesKeyDTO prepareAesKeyDTO_WithInvalidStringSize() {
        return new AesKeyDTO(StorageType.PUFSS, INVALID_AES_KEY_TOO_LONG);
    }

    private AesKeyDTO prepareAesKeyDTO_WithNotHexString() {
        return new AesKeyDTO(StorageType.PUFSS, INVALID_AES_KEY_NOT_HEX);
    }
}
