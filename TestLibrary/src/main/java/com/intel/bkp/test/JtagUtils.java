/*
 * [INTEL CONFIDENTIAL]
 *
 * Copyright 2020-2024 Intel Corporation
 *
 * This software and the related documents are Intel copyrighted materials,
 * and your use of them is governed by the express license under which they
 * were provided to you (License). Unless the License provides otherwise,
 * you may not use, modify, copy, publish, distribute, disclose or transmit
 * this software or the related documents without Intel's prior written
 * permission.
 *
 * This software and the related documents are provided as is, with no
 * express or implied warranties, other than those that are expressly stated
 * in the License.
 */

package com.intel.bkp.test;

import com.intel.bkp.command.header.CommandHeader;
import com.intel.bkp.command.header.CommandHeaderManager;

import java.nio.ByteBuffer;

import static com.intel.bkp.utils.ByteConverter.toBytes;

public class JtagUtils {

    public static byte[] prepareValidHeader(int argSize) {
        CommandHeader header = new CommandHeader(0, argSize, 1, 2);
        int headerInt = ByteBuffer.wrap(CommandHeaderManager.buildForFw(header)).getInt();
        return toBytes(headerInt);
    }
}
