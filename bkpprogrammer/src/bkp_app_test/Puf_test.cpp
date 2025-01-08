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

#include "Puf.h"
#include "gtest/gtest.h"
#include <cstdlib>

TEST(PufTest, isMbrSignatureCorrectTest)
{
    std::vector<BYTE> mbrBuffer(512, 0);
    mbrBuffer[0x1FE] = 0x55;
    mbrBuffer[0x1FF] = 0xAA;

    PufHandler pufHandler;
    EXPECT_TRUE(pufHandler.isMbrSignatureCorrect(mbrBuffer));
    mbrBuffer[511] = 0xAB;
    EXPECT_FALSE(pufHandler.isMbrSignatureCorrect(mbrBuffer));
}

TEST(PufTest, getConfigurationDataPartitionAddressFromMbrTest)
{
    std::vector<BYTE> mbrBuffer(512, 0);

    // partition info structure
    mbrBuffer[0x1BE + 0x4] = 0x48; // partition type: file_system
    mbrBuffer[0x1CE + 0x4] = 0xA2; // partition type: config_data

    // config data sector number (little endian)
    mbrBuffer[0x1CE + 0x8] = 0x2B;
    mbrBuffer[0x1CE + 0x9] = 0x1A;
    mbrBuffer[0x1CE + 0xA] = 0x09;
    mbrBuffer[0x1CE + 0xB] = 0x00;

    PufHandler pufHandler;
    EXPECT_EQ(0x12345600, pufHandler.getConfigurationDataPartitionAddressFromMbr(mbrBuffer));
}

TEST(PufTest, updatePufDataSectionUpdatesNumberOfSectionsField)
{
    std::vector<BYTE> pufDataBlockBuffer(0x8000, 0);
    std::vector<BYTE> pufDataSectionBuffer(0x1000, 42);

    //PUF wrapped AES key magic word
    pufDataSectionBuffer[3] = 0x4E;
    pufDataSectionBuffer[2] = 0x11;
    pufDataSectionBuffer[1] = 0x0C;
    pufDataSectionBuffer[0] = 0xCD;

    PufHandler pufHandler;
    pufHandler.updatePufDataSection(pufDataBlockBuffer, pufDataSectionBuffer, PufHandler::PufDataBlockSectionOffset::UserIidPufWrappedAesKey);

    EXPECT_EQ(1, pufDataBlockBuffer[0]); // number of sections should increase from 0 to 1
    // Check if PUF Data Section (User IID PUF wrapped key) is correctly placed in PUF Data Block
    for (DWORD i = 0; i < pufDataSectionBuffer.size(); i++)
    {
        EXPECT_EQ(pufDataSectionBuffer[i], pufDataBlockBuffer[i + 0x2000]);
    }

    // Check that allocation table was updated after adding new section
    EXPECT_EQ(pufDataBlockBuffer[0xC + 3], 0x00);
    EXPECT_EQ(pufDataBlockBuffer[0xC + 2], 0x00);
    EXPECT_EQ(pufDataBlockBuffer[0xC + 1], 0x20);
    EXPECT_EQ(pufDataBlockBuffer[0xC + 0], 0x00);
}

TEST(PufTest, updatePufDataSectionUpdatesExistingSection)
{
    std::vector<BYTE> pufDataBlockBuffer(0x8000, 0);
    std::vector<BYTE> pufDataSectionBuffer(0x1000, 42);

    //PUF Help Data magic word
    pufDataSectionBuffer[3] = 0x4C;
    pufDataSectionBuffer[2] = 0xF2;
    pufDataSectionBuffer[1] = 0x79;
    pufDataSectionBuffer[0] = 0x41;

    pufDataBlockBuffer[0] = 2; // Number of sections
    // Existing PUF Help Data
    pufDataBlockBuffer[0x1000 + 3] = 0x4C;
    pufDataBlockBuffer[0x1000 + 2] = 0xF2;
    pufDataBlockBuffer[0x1000 + 1] = 0x79;
    pufDataBlockBuffer[0x1000 + 0] = 0x41;

    PufHandler pufHandler;
    pufHandler.updatePufDataSection(pufDataBlockBuffer, pufDataSectionBuffer, PufHandler::PufDataBlockSectionOffset::UserIidPufHelpData);

    EXPECT_EQ(2, pufDataBlockBuffer[0]); // number of sections should not increase
    // Check if PUF Data Section (User IID PUF Help Data) is correctly placed in PUF Data Block
    for (DWORD i = 0; i < pufDataSectionBuffer.size(); i++)
    {
        EXPECT_EQ(pufDataSectionBuffer[i], pufDataBlockBuffer[i + 0x1000]);
    }
}

TEST(PufSdm1_5Test, construct_bos_partition_content)
{
    // Setting up output buffer
    DWORD start_block = 1;
    DWORD block_puf_data = (5 + BOS_PARTITION_BLOCK_SIZE_IN_WORD -1) / BOS_PARTITION_BLOCK_SIZE_IN_WORD;
    DWORD bos_partition_size_words =  (block_puf_data + start_block) * BOS_PARTITION_BLOCK_SIZE_IN_WORD ; // puf data + BOS object directory block size (128 words)
    DWORD* bos_partition_buffer = new DWORD[bos_partition_size_words];
    memset(bos_partition_buffer, 0, bos_partition_size_words * sizeof(DWORD));
    DWORD puf_size_bytes = 5 * sizeof(DWORD);
    BYTE* new_puf_data = new BYTE[puf_size_bytes];
    memset(new_puf_data, 0, puf_size_bytes);
    // Setting random puf data
    for (size_t i = 0; i < puf_size_bytes; i++)
    {
        new_puf_data[i] = rand() % 100;
    }
    DWORD crc32 = PufHandlerSdm1_5::msb_calculate_crc32(new_puf_data, 0, (puf_size_bytes * 8), 0xFFFFFFFF, 0x04C11DB7, true);
    crc32 ^= 0xFFFFFFFF;

    // Setting up response buffer
    DWORD* respBuf = new DWORD[bos_partition_size_words]; // BOS partition read from flash
    memset(respBuf, 0, bos_partition_size_words * sizeof(DWORD));
    // BOS Object Directory descriptor
    respBuf[0] = static_cast<DWORD>(PufHandlerSdm1_5::BOS_PARTITION_DATA_BLOCK_SECTIONS::BOS_OBJECT_DIRECTORY);
    respBuf[1] = BOS_PARTITION_START_ADDRESS;
    respBuf[2] = bos_partition_size_words * sizeof(DWORD);
    respBuf[3] = 0xFFFFFFFF;

    // Puf block descriptor
    BYTE* old_puf_data = new BYTE[puf_size_bytes];
    // Setting random puf data
    for (size_t i = 0; i < puf_size_bytes; i++)
    {
        old_puf_data[i] = rand() % 100;
    }
    respBuf[4] = static_cast<DWORD>(PufHandlerSdm1_5::BOS_PARTITION_DATA_BLOCK_SECTIONS::UDS_INTEL_PUF);
    respBuf[5] = start_block;
    respBuf[6] = puf_size_bytes;
    respBuf[7] = PufHandlerSdm1_5::msb_calculate_crc32(old_puf_data, 0, (puf_size_bytes * 8), 0xFFFFFFFF, 0x04C11DB7, true);
    memcpy(respBuf + start_block * BOS_PARTITION_BLOCK_SIZE_IN_WORD, old_puf_data, puf_size_bytes);

    // Constructs BOS partition based on response buffer and new puf data
    PufHandlerSdm1_5::construct_bos_partition_content
    (
        bos_partition_buffer,
        bos_partition_size_words,
        PufHandlerSdm1_5::BOS_PARTITION_DATA_BLOCK_SECTIONS::UDS_INTEL_PUF,
        new_puf_data,
        puf_size_bytes,
        respBuf
    );

    // Verify new constructed BOS partition
    EXPECT_EQ(bos_partition_buffer[0], static_cast<DWORD>(PufHandlerSdm1_5::BOS_PARTITION_DATA_BLOCK_SECTIONS::BOS_OBJECT_DIRECTORY));
    EXPECT_EQ(bos_partition_buffer[1], BOS_PARTITION_START_ADDRESS);
    EXPECT_EQ(bos_partition_buffer[2], bos_partition_size_words * sizeof(DWORD));
    EXPECT_EQ(bos_partition_buffer[3], 0xFFFFFFFF);
    EXPECT_EQ(bos_partition_buffer[4], static_cast<DWORD>(PufHandlerSdm1_5::BOS_PARTITION_DATA_BLOCK_SECTIONS::UDS_INTEL_PUF));
    EXPECT_EQ(bos_partition_buffer[5], start_block);
    EXPECT_EQ(bos_partition_buffer[6], puf_size_bytes);
    EXPECT_EQ(bos_partition_buffer[7], crc32);
    DWORD start_address = start_block * BOS_PARTITION_BLOCK_SIZE_IN_WORD;
    for (size_t i = 0; i < puf_size_bytes; i++)
    {
        DWORD puf_word = bos_partition_buffer[start_address + i / 4];
        EXPECT_EQ(((puf_word >> ((i % 4) * 8)) & 0xFF), new_puf_data[i]);
    }

    delete [] bos_partition_buffer;
    delete [] respBuf;
    delete [] new_puf_data;
    delete [] old_puf_data;
}

TEST(PufSdm1_5Test, construct_bos_partition_content_puf_type_not_exists)
{
    // Setting up output buffer
    DWORD start_block = 1;
    DWORD block_puf_data = (5 + BOS_PARTITION_BLOCK_SIZE_IN_WORD -1) / BOS_PARTITION_BLOCK_SIZE_IN_WORD;
    DWORD bos_partition_size_words =  (block_puf_data + start_block) * BOS_PARTITION_BLOCK_SIZE_IN_WORD ; // puf data + BOS object directory block size (128 words)
    DWORD* bos_partition_buffer = new DWORD[bos_partition_size_words];
    memset(bos_partition_buffer, 0, bos_partition_size_words * sizeof(DWORD));
    DWORD puf_size_bytes = 5 * sizeof(DWORD);
    BYTE* new_puf_data = new BYTE[puf_size_bytes];
    memset(new_puf_data, 0, puf_size_bytes);
    // Setting random puf data
    for (size_t i = 0; i < puf_size_bytes; i++)
    {
        new_puf_data[i] = rand() % 100;
    }
    DWORD crc32 = PufHandlerSdm1_5::msb_calculate_crc32(new_puf_data, 0, (puf_size_bytes * 8), 0xFFFFFFFF, 0x04C11DB7, true);
    crc32 ^= 0xFFFFFFFF;

    // Setting up response buffer
    DWORD* respBuf = new DWORD[BOS_PARTITION_BLOCK_SIZE_IN_WORD]; // BOS partition read from flash
    memset(respBuf, 0, BOS_PARTITION_BLOCK_SIZE_IN_WORD * sizeof(DWORD));
    // BOS Object Directory descriptor
    respBuf[0] = static_cast<DWORD>(PufHandlerSdm1_5::BOS_PARTITION_DATA_BLOCK_SECTIONS::BOS_OBJECT_DIRECTORY);
    respBuf[1] = BOS_PARTITION_START_ADDRESS;
    respBuf[2] = BOS_PARTITION_BLOCK_SIZE_IN_WORD * sizeof(DWORD);
    respBuf[3] = 0xFFFFFFFF;

    // Constructs BOS partition based on response buffer and new puf data
    PufHandlerSdm1_5::construct_bos_partition_content
    (
        bos_partition_buffer,
        bos_partition_size_words,
        PufHandlerSdm1_5::BOS_PARTITION_DATA_BLOCK_SECTIONS::UDS_INTEL_PUF,
        new_puf_data,
        puf_size_bytes,
        respBuf
    );

    // Verify new constructed BOS partition
    EXPECT_EQ(bos_partition_buffer[0], static_cast<DWORD>(PufHandlerSdm1_5::BOS_PARTITION_DATA_BLOCK_SECTIONS::BOS_OBJECT_DIRECTORY));
    EXPECT_EQ(bos_partition_buffer[1], BOS_PARTITION_START_ADDRESS);
    EXPECT_EQ(bos_partition_buffer[2], bos_partition_size_words * sizeof(DWORD));
    EXPECT_EQ(bos_partition_buffer[3], 0xFFFFFFFF);
    EXPECT_EQ(bos_partition_buffer[4], static_cast<DWORD>(PufHandlerSdm1_5::BOS_PARTITION_DATA_BLOCK_SECTIONS::UDS_INTEL_PUF));
    EXPECT_EQ(bos_partition_buffer[5], start_block);
    EXPECT_EQ(bos_partition_buffer[6], puf_size_bytes);
    EXPECT_EQ(bos_partition_buffer[7], crc32);
    DWORD start_address = start_block * BOS_PARTITION_BLOCK_SIZE_IN_WORD;
    for (size_t i = 0; i < puf_size_bytes; i++)
    {
        DWORD puf_word = bos_partition_buffer[start_address + i / 4];
        EXPECT_EQ(((puf_word >> ((i % 4) * 8)) & 0xFF), new_puf_data[i]);
    }

    delete [] bos_partition_buffer;
    delete [] respBuf;
    delete [] new_puf_data;
}
