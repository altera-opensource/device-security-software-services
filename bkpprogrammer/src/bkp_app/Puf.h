/*
 * This project is licensed as below.
 *
 * **************************************************************************
 *
 * Copyright 2020-2023 Intel Corporation. All Rights Reserved.
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

#ifndef BKPPROGRAMMER_PUF_H
#define BKPPROGRAMMER_PUF_H

#include "Qspi.h"
#include "pgm_plugin_bkp_export.h"

#include <map>
#include <list>
#include <cstring>
#include <string>

#define BOS_PARTITION_START_ADDRESS 0x200000
#define BOS_PARTITION_MIN_SIZE 0x8000
#define BOS_PARTITION_BLOCK_SIZE 512
#define BOS_PARTITION_BLOCK_SIZE_IN_WORD 128
#define BOS_PARTITION_HEADER_SIZE 16

typedef uint8_t BYTE;
typedef uint32_t DWORD;

struct Puf_Object
{
    DWORD pufID;
    DWORD pufFileSize;
    DWORD pufStartBlock;
    DWORD pufNumberOfBlock;
    DWORD pufCRC32;
    BYTE* pufPtr;

    Puf_Object(DWORD id, DWORD startBlock, DWORD filesize, DWORD NumberOfBlock, DWORD crc32, BYTE* ptr)
    {
        pufID = id;
        pufStartBlock = startBlock;
        pufFileSize = filesize;
        pufNumberOfBlock = NumberOfBlock;
        pufCRC32 = crc32;
        pufPtr = ptr;
    }
};

class PufHandler
{
public:
    virtual ~PufHandler() {}

    enum class PufDataBlockSectionOffset
    {
        // byte offset in the block
        AllocationTable = 0x0,
        UserIidPufHelpData = 0x1000,
        UserIidPufWrappedAesKey = 0x2000,
        UdsIidPufHelpData = 0x3000,
        UdsIidPufWrappedAesKey = 0x4000
    };
    bool writeWkeyToFlash(std::vector<BYTE> wkeyData, PufType_t pufType);
    bool writePufHelpDataToFlash(std::vector<BYTE> pufHelpData, PufType_t pufType);
    bool isMbrSignatureCorrect(std::vector<BYTE> &mbrBuffer);
    DWORD getConfigurationDataPartitionAddressFromMbr(std::vector<BYTE> &mbrBuffer);
    void updatePufDataSection(std::vector<BYTE> &pufData,
                              std::vector<BYTE> newSection,
                              PufDataBlockSectionOffset sectionOffset);
private:
    enum class AllocationTableFields
    {
        // byte offset in the allocation table
        numberOfSections = 0x0,
        reserved = 0x4,
        offsetUserIidPufHelpData = 0x8,
        offsetUserIidPufWrappedAesKey = 0xC,
        offsetUdsIidPufHelpData = 0x10,
        offsetUdsIidPufWrappedAesKey = 0x14
    };

    const DWORD pufHelpDataMagicWord = 0x4CF27941;
    const DWORD pufWrappedAesKeyMagicWord = 0x4E110CCD;
    const DWORD pufDataBlockSizeInWords = 0x2000;
    const DWORD pufDataSectionSizeInWords = 0x400;
    const DWORD pufDataBlock0PointerOffset = 0x1F90;
    const DWORD pufDataBlock1PointerOffset = 0x1F98;
    const DWORD pufHelpDataSizeInWords = 402;
    const DWORD pufWrappedAesKeySizeInWords = 30;

    const DWORD mbrSectorSizeInBytes = 0x200;
    const DWORD mbrPartitionEntryOffset = 0x1BE; // MBR partition entry offset in MBR table
    const DWORD mbrPartitionCount = 4;
    const DWORD mbrPartitionInfoSize = 0x10;
    const DWORD mbrPartitionTypeConfigData = 0xA2;
    const DWORD partitionTypeOffset = 0x4;
    const DWORD lbaStartAddressOffset = 0x8;

    // { address offset , expected value }
    std::map<DWORD, BYTE> expectedMbrSignatureFields = {{0x1FE, 0x55}, {0x1FF, 0xAA}};

    bool getConfigurationDataAddress(DWORD& configurationDataAddress);
    bool getPufDataBlockPointer(DWORD configDataAddress, DWORD pointerOffset, DWORD& pufDataBlockPointerOffset);
    bool updatePufData(std::vector<BYTE> newSection, PufDataBlockSectionOffset sectionOffset);
};

class PufHandlerSdm1_5 : public PufHandler
{
public:
    enum class BOS_PARTITION_DATA_BLOCK_SECTIONS
    {
        INVALID_SECTION         = 0x00000000,
        BOS_OBJECT_DIRECTORY    = 0x00534F42,
        UDS_INTEL_PUF           = 0x00444850,  // UDS INTEL PUF
        USER_IID_PUF            = 0x00444949,  // USER IID PUF
        UDS_IID_PUF             = 0x40444949   // UDS IID PUF
    };

    static DWORD msb_calculate_crc32
    (
        const BYTE* base_address,
        DWORD start_bit_index,
        DWORD num_bits,
        DWORD lfsr_init,
        const DWORD CRC_POLYNOMIAL,
        bool reverse_bit=false
    );
    static void construct_bos_partition_content
    (
        DWORD* inBufData,
        DWORD inBufDataSize,
        BOS_PARTITION_DATA_BLOCK_SECTIONS section,
        const BYTE* inSectionData,
        DWORD inSectionDataSize,
        const DWORD* respBuf
    );
    static bool updateBosPartition(std::vector<BYTE> data, BOS_PARTITION_DATA_BLOCK_SECTIONS section);

private:

    static std::vector<BYTE> convert_word_to_bytes(std::vector<DWORD> data);
    static bool verify_bos_partition_header(const DWORD* bos_partition_data, DWORD& address_block0, DWORD& address_block1, DWORD& size_in_word);
    static std::list<Puf_Object> get_bos_partition_content(const DWORD* respBuf);
};

#endif //BKPPROGRAMMER_PUF_H
