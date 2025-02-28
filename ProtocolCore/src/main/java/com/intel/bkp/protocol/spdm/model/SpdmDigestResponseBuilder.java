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

package com.intel.bkp.protocol.spdm.model;

import com.intel.bkp.protocol.spdm.jna.model.SpdmConstants;
import com.intel.bkp.protocol.spdm.jna.model.SpdmGetDigestResult;
import com.intel.bkp.utils.ByteBufferSafe;

import java.util.HashMap;

import static com.intel.bkp.utils.BitUtils.isSet;

public class SpdmDigestResponseBuilder {

    private final HashMap<Integer, byte[]> digestMap = new HashMap<>();

    public SpdmDigestResponseBuilder parse(SpdmGetDigestResult getDigestResult) {
        final byte[] slotMask = new byte[] {getDigestResult.slotMask()};
        final int hashAlgSize = getDigestResult.hashAlgSize();
        final ByteBufferSafe digestBuffer = ByteBufferSafe.wrap(getDigestResult.digests());

        for (int slotId = 0; slotId < SpdmConstants.MAX_SLOT_COUNT; slotId++) {
            if (isSet(slotId, slotMask)) {
                final byte[] digest = new byte[hashAlgSize];
                digestBuffer.get(digest);
                add(slotId, digest);
            }
        }
        return this;
    }

    public SpdmDigestResponse build() {
        return new SpdmDigestResponse(digestMap);
    }

    private void add(Integer slotId, byte[] digest) {
        digestMap.put(slotId, digest);
    }
}
