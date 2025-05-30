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

package com.intel.bkp.protocol.spdm.jna;

import com.intel.bkp.protocol.spdm.exceptions.SpdmCommandFailedException;
import com.intel.bkp.protocol.spdm.exceptions.SpdmNotSupportedException;
import com.intel.bkp.protocol.spdm.exceptions.SpdmRuntimeException;
import com.intel.bkp.protocol.spdm.jna.model.LibSpdmReturn;
import com.sun.jna.Pointer;
import com.sun.jna.ptr.PointerByReference;

import java.nio.ByteBuffer;

import static com.intel.bkp.protocol.spdm.jna.model.LibSpdmReturn.LIBSPDM_STATUS_SPDM_INTERNAL_EXCEPTION;
import static com.intel.bkp.protocol.spdm.jna.model.LibSpdmReturn.LIBSPDM_STATUS_SPDM_NOT_SUPPORTED;
import static com.intel.bkp.protocol.spdm.jna.model.LibSpdmReturn.LIBSPDM_STATUS_SUCCESS;
import static com.intel.bkp.utils.HexConverter.toHex;

public class SpdmUtils {

    public static void copyBuffer(ByteBuffer srcBuffer, PointerByReference dstBufferP, Pointer dstLenP) {
        final Pointer dstBuffer = dstBufferP.getValue();
        final long srcLen = srcBuffer.remaining();
        final long dstLen = dstLenP.getLong(0);

        verifyDestinationBufferLen(dstLen, srcLen);

        for (int i = 0; i < srcLen; i++) {
            dstBuffer.setByte(i, srcBuffer.get(i));
        }

        dstLenP.setLong(0, srcLen);
    }

    public static void copyBuffer(byte[] srcBuffer, Pointer dstBuffer, long dstLen) {
        final long srcLen = srcBuffer.length;

        verifyDestinationBufferLen(dstLen, srcLen);

        for (int i = 0; i < srcLen; i++) {
            dstBuffer.setByte(i, srcBuffer[i]);
        }
    }

    static byte[] getBytes(Pointer data, Pointer dataLenP) {
        final int dataLen = dataLenP.getInt(0);
        return getBytes(data, dataLen);
    }

    static byte[] getBytes(Pointer data, int dataLen) {
        final ByteBuffer buffer = data.getByteBuffer(0, dataLen);

        final byte[] dataBytes = new byte[dataLen];
        buffer.get(dataBytes);
        return dataBytes;
    }

    public static void throwOnError(LibSpdmReturn status) throws SpdmCommandFailedException {
        if (LIBSPDM_STATUS_SPDM_NOT_SUPPORTED.equals(status)) {
            throw new SpdmNotSupportedException();
        }

        if (LIBSPDM_STATUS_SPDM_INTERNAL_EXCEPTION.equals(status)) {
            throw new SpdmRuntimeException(
                "SPDM exception due to internal error with status: 0x%s".formatted(toHex(status.asLong())));
        }

        if (!LIBSPDM_STATUS_SUCCESS.equals(status)) {
            throw new SpdmCommandFailedException(status);
        }
    }

    private static void verifyDestinationBufferLen(long dstLen, long srcLen) {
        if (dstLen < srcLen) {
            throw new SpdmRuntimeException("Destination buffer is too small. "
                + "Destination size = %d, Source size = %d".formatted(dstLen, srcLen));
        }
    }
}
