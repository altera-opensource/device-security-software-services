/*
 * This project is licensed as below.
 *
 * **************************************************************************
 *
 * Copyright 2020-2024 Intel Corporation. All Rights Reserved.
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
#include "Logger.h"

std::vector<BYTE> PufHandlerSdm1_5::convert_word_to_bytes(std::vector<DWORD> data)
{
    std::vector<BYTE> outData;
    outData.assign(data.size() * 4, 0);
    for (DWORD i = 0; i < (data.size() * 4); i++)
    {
        outData[i] = ((data[i / 4] >> ((i % 4) * 8)) & 0xFF);
    }

    return outData;
}

bool PufHandlerSdm1_5::verify_bos_partition_header(const DWORD* bos_partition_data, DWORD& address_block0, DWORD& address_block1, DWORD& size_in_word)
{
    bool status = false;

    // Magic Number
    status = (bos_partition_data[0] == static_cast<DWORD>(BOS_PARTITION_DATA_BLOCK_SECTIONS::BOS_OBJECT_DIRECTORY));

    // Start address
    status = status && (bos_partition_data[1] == BOS_PARTITION_START_ADDRESS);

    // Partition size
    DWORD partition_size = bos_partition_data[2];
    status = status && (partition_size != 0 && partition_size != 0xFFFFFFFF);

    // CRC32 checksum default to 0xFFFFFFFF
    status = status && (bos_partition_data[3] == 0xFFFFFFFF);

    if (status)
    {
        address_block0 = BOS_PARTITION_START_ADDRESS;
        address_block1 = BOS_PARTITION_START_ADDRESS + partition_size;
        size_in_word = partition_size/4;
    }

    return status;
}

std::list<Puf_Object> PufHandlerSdm1_5::get_bos_partition_content
(
    const DWORD* respBuf
)
{
    // create object and push back the element
    std::list<Puf_Object> pufObjectList;

    // directory block is 512 bytes
    // first 4 words is BOS directory
    // subsequent words is OTN
    DWORD bos_partition_size_in_words = respBuf[2];
    for (DWORD puf_offset = 4; puf_offset < BOS_PARTITION_BLOCK_SIZE_IN_WORD; puf_offset += 4)
    {
        // offset0. puf_type
        // offset1. start_block
        // offset2. file size
        // offset3. CRC32
        if (respBuf[puf_offset] == 0x0 || respBuf[puf_offset] == 0xFFFFFFFF)
        {
            continue;
        }
        DWORD id = respBuf[puf_offset];
        DWORD start_block = respBuf[puf_offset + 1];
        DWORD file_size = respBuf[puf_offset + 2];
        DWORD crc32 = respBuf[puf_offset + 3];
        DWORD ptr_start_addr = start_block * BOS_PARTITION_BLOCK_SIZE_IN_WORD;
        DWORD total_block_needed = (file_size + BOS_PARTITION_BLOCK_SIZE - 1)/ BOS_PARTITION_BLOCK_SIZE;
        DWORD bos_data_size_in_word = total_block_needed * BOS_PARTITION_BLOCK_SIZE_IN_WORD;
        DWORD end_address = start_block * BOS_PARTITION_BLOCK_SIZE_IN_WORD + bos_data_size_in_word;
        if (end_address < bos_partition_size_in_words)
        {
            DWORD* bos_data_content = new DWORD[bos_data_size_in_word];
            memset(bos_data_content, 0, bos_data_size_in_word);
            std::copy(respBuf + ptr_start_addr, respBuf + ptr_start_addr + bos_data_size_in_word, bos_data_content);
            Puf_Object puf_object(id, start_block, file_size, total_block_needed, crc32, reinterpret_cast<BYTE*>(bos_data_content));
            pufObjectList.push_back(puf_object);
        }

    }

    return pufObjectList;
}

DWORD PufHandlerSdm1_5::msb_calculate_crc32
(
    const BYTE* base_address,
    DWORD start_bit_index,
    DWORD num_bits,
    DWORD lfsr_init,
    const DWORD CRC_POLYNOMIAL,
    bool reverse_bit
)
{
    DWORD crc32 = lfsr_init;
    DWORD bit_index = 0;
    DWORD bit = 0;
    bool xor_crc = false;
    for (DWORD i = 0; i < num_bits; i++, start_bit_index++)
    {
        bit_index = start_bit_index & 7;
        if (reverse_bit)
        {
            // We seldom do this -- but this is what nadder is using
            bit_index = 7 - bit_index;
        }
        bit = (base_address != nullptr && (base_address[start_bit_index >> 3] & (1 << bit_index))) ? 0x80000000 : 0;
        xor_crc = ((crc32 ^ bit) & 0x80000000) != 0;
        crc32 <<= 1;
        if (xor_crc)
        {
            crc32 ^= CRC_POLYNOMIAL;
        }
    }
    return crc32;
}

void PufHandlerSdm1_5::construct_bos_partition_content
(
    DWORD* inBufData,
    DWORD inBufDataSize,
    BOS_PARTITION_DATA_BLOCK_SECTIONS section,
    const BYTE* inSectionData,
    DWORD inSectionDataSize,
    const DWORD* respBuf
)
{
    DWORD table_offset = 0;
    // Set BOS partition header (descriptor)
    //--------------------------------------
    inBufData[table_offset] = static_cast<DWORD>(BOS_PARTITION_DATA_BLOCK_SECTIONS::BOS_OBJECT_DIRECTORY);
    table_offset++;

    inBufData[table_offset] = BOS_PARTITION_START_ADDRESS;
    table_offset++;

    inBufData[table_offset] = inBufDataSize * 4;
    table_offset++;

    inBufData[table_offset] = 0xFFFFFFFF;
    table_offset++;

    if (section != BOS_PARTITION_DATA_BLOCK_SECTIONS::INVALID_SECTION)
    {
        // New section data
        DWORD new_data_block = (inSectionDataSize / sizeof(DWORD) + BOS_PARTITION_BLOCK_SIZE_IN_WORD - 1) / BOS_PARTITION_BLOCK_SIZE_IN_WORD;
        DWORD block_size = new_data_block * BOS_PARTITION_BLOCK_SIZE_IN_WORD * 4;
        BYTE* new_data_content = new BYTE[block_size];
        memset(new_data_content, 0, block_size);
        std::copy(inSectionData, inSectionData + inSectionDataSize, new_data_content);
        DWORD new_data_crc32 = msb_calculate_crc32(new_data_content, 0, (inSectionDataSize * 8), 0xFFFFFFFF, 0x04C11DB7, true);
        new_data_crc32 ^= 0xFFFFFFFF;

        DWORD start_block = 0x1;
        std::list<Puf_Object> puf_object_list = get_bos_partition_content(respBuf);
        bool section_found = false;

        for (Puf_Object& obj : puf_object_list)
        {
            if (obj.pufID == static_cast<DWORD>(section))
            {
                // only process if size is more than 0
                // erase operation : size is 0 and data is null, can be ignored
                if (inSectionData && inSectionDataSize > 0)
                {
                    // remove the data
                    if (obj.pufPtr)
                    {
                        delete [] obj.pufPtr;
                        obj.pufPtr = nullptr;
                    }
                    // set the file size
                    obj.pufFileSize = inSectionDataSize;

                    // set the start block
                    obj.pufStartBlock = start_block;

                    // set the  number of block
                    obj.pufNumberOfBlock = new_data_block;

                    // set new CRC32
                    obj.pufCRC32 = new_data_crc32;

                    // set new bos data content
                    obj.pufPtr = new_data_content;
                    start_block = start_block + obj.pufNumberOfBlock;
                }
                section_found = true;
            }
            else
            {
                // set the new start block
                obj.pufStartBlock = start_block;
                start_block = start_block + obj.pufNumberOfBlock;
            }
        }

        // add new puf type into the list if not found
        if (!section_found && inSectionData && inSectionDataSize > 0)
        {
            Puf_Object puf_object(
                static_cast<DWORD>(section),
                start_block,
                inSectionDataSize,
                new_data_block,
                new_data_crc32,
                new_data_content);

            puf_object_list.push_back(puf_object);
        }

        // generate data
        for (Puf_Object& obj : puf_object_list)
        {
            //skip if content is empty
            if (obj.pufID == static_cast<DWORD>(section) && inSectionData == nullptr && inSectionDataSize == 0)
            {
                continue;
            }
            inBufData[table_offset] = obj.pufID;
            table_offset++;

            inBufData[table_offset] = obj.pufStartBlock;
            table_offset++;

            inBufData[table_offset] = obj.pufFileSize;
            table_offset++;

            inBufData[table_offset] = obj.pufCRC32;
            table_offset++;

            int index = obj.pufStartBlock * BOS_PARTITION_BLOCK_SIZE / 4;
            for (DWORD i = 0; i < obj.pufFileSize; i++)
            {
                inBufData[index + i / 4] |= (obj.pufPtr[i] << ( (i % 4) * 8));
            }
        }

        //clear list
        if (puf_object_list.size() > 0)
        {
            for (Puf_Object& obj : puf_object_list)
            {
                if (!obj.pufPtr)
                {
                    delete [] obj.pufPtr;
                    obj.pufPtr = nullptr;
                }
            }
            puf_object_list.clear();
        }

        if (new_data_content)
        {
            delete [] new_data_content;
        }
    }
}

bool PufHandlerSdm1_5::updateBosPartition(std::vector<BYTE> data, BOS_PARTITION_DATA_BLOCK_SECTIONS section)
{
    DWORD current_address = BOS_PARTITION_START_ADDRESS;
    std::vector<BYTE> bos_partition_data;
    DWORD* bos_partition = nullptr;
    DWORD address_block0 = 0;
    DWORD address_block1 = 0;
    DWORD size_in_word = 0;
    bool status = true;
    Logger::log("Update BOS partition", Debug);
    for (size_t retry = 1; retry <= 8; retry++)
    {
        Qspi::qspiReadMultiple(current_address, BOS_PARTITION_HEADER_SIZE, bos_partition_data);
        status = (bos_partition_data.size() == (BOS_PARTITION_HEADER_SIZE * 4));
        if (status)
        {
            status = verify_bos_partition_header(reinterpret_cast<DWORD*>(bos_partition_data.data()), address_block0, address_block1, size_in_word);
            if (status)
            {
                Logger::log("BOS Partition data block address0 = " + Utils::toHexString(address_block0), Debug);
                Logger::log("BOS Partition data block address1 = " + Utils::toHexString(address_block1), Debug);
                break;
            }
            else
            {
                current_address += BOS_PARTITION_MIN_SIZE;
            }
            bos_partition_data.clear();
        }
        else
        {
            Logger::log("Failed to read QSPI flash at address " + Utils::toHexString(current_address), Error);
            break;
        }
    }

    // Read BOS partition and update with new data
    if (status)
    {
        // get the BOS partition content
        Logger::log("Read BOS partition data content at address " + Utils::toHexString(current_address), Debug);
        bos_partition_data.clear();
        Qspi::qspiReadMultiple(current_address, size_in_word, bos_partition_data);
        status = (bos_partition_data.size() == (size_in_word * 4));
        if (status)
        {
            bos_partition = new DWORD[size_in_word];
            memset(bos_partition, 0, size_in_word * sizeof(DWORD));
            // Construct BOS data content
            construct_bos_partition_content(
                bos_partition,
                size_in_word,
                section,
                data.data(),
                data.size(),
                reinterpret_cast<DWORD*>(bos_partition_data.data())
            );
        }
        else
        {
            Logger::log("Failed to read QSPI flash at address " + Utils::toHexString(current_address), Error);
        }

    }

    // Update and verify BOS partition block0 and block1 data
    if (status)
    {
        int block_index = 0;
        current_address = address_block0;
        do
        {
            Logger::log("Update BOS partition data block" + std::to_string(block_index) + " at address " + Utils::toHexString(current_address), Debug);
            Qspi::qspiErase(current_address, size_in_word);
            std::vector<DWORD> bos_partition_vec(bos_partition, bos_partition + size_in_word);
            std::vector<BYTE> writeData = convert_word_to_bytes(std::move(bos_partition_vec));
            Qspi::qspiWriteMultiple(current_address, size_in_word, writeData); 
            status = status && Qspi::qspiVerify(current_address, size_in_word, std::move(writeData));
            if (status && block_index == 0)
            {
                // Update for block 1
                current_address = address_block1;
            }
        } while (status && ++block_index < 2);
    }

    if (bos_partition)
    {
        delete [] bos_partition;
    }

    return status;
}
