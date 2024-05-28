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

#include <chrono>
#include <thread>


bool PufHandler::isMbrSignatureCorrect(std::vector<BYTE> &mbrBuffer)
{
    for (auto const& [offset, expectedValue] : expectedMbrSignatureFields)
    {
        if (mbrBuffer[offset] != expectedValue) { return false; }
    }
    return true;
}

DWORD PufHandler::getConfigurationDataPartitionAddressFromMbr(std::vector<BYTE> &mbrBuffer)
{
    Logger::log("Getting configuration data partition address from MBR", Debug);
    for (DWORD partitionInfoNumber = 0; partitionInfoNumber < mbrPartitionCount; partitionInfoNumber++)
    {
        DWORD partitionInfoAddress = mbrPartitionEntryOffset + partitionInfoNumber * mbrPartitionInfoSize;
        if (mbrBuffer[partitionInfoAddress + partitionTypeOffset] == mbrPartitionTypeConfigData)
        {
            Logger::log("Found config data partition", Debug);
            DWORD lbaStartSector = Utils::decodeFromLittleEndianBuffer(mbrBuffer,
                                                                          partitionInfoAddress + lbaStartAddressOffset);
            return lbaStartSector * mbrSectorSizeInBytes;
        }
    }
    Logger::log("Configuration data partition not found.", Fatal);
    return 0;
}

bool PufHandler::getConfigurationDataAddress(DWORD& configurationDataAddress)
{
    Logger::log("Geting configuration data address", Debug);
    configurationDataAddress = 0x0;

    // The configuration data doesn't start with offset 0x0 when MBR is used.
    // Read 512 bytes from address 0x0 to check for MBR info
    std::vector<BYTE> mbrBuffer;
    Logger::log("QSPI read from address 0x0", Debug);
    bool status = Qspi::qspiReadMultiple(0x0, mbrSectorSizeInBytes / WORD_SIZE, mbrBuffer);
    if (status)
    {
        if (isMbrSignatureCorrect(mbrBuffer))
        {
            Logger::log("MBR found", Debug);
            configurationDataAddress = getConfigurationDataPartitionAddressFromMbr(mbrBuffer);
            status = (configurationDataAddress != 0);
        }
    }
    return status;
}

bool PufHandler::getPufDataBlockPointer(DWORD configDataAddress, DWORD pointerOffset, DWORD& pufDataBlockPointerOffset)
{
    std::vector<BYTE> pufDataBlockPointerBuffer;
    pufDataBlockPointerOffset = 0;
    bool status = Qspi::qspiReadMultiple(configDataAddress + pointerOffset, 1, pufDataBlockPointerBuffer);
    if (status)
    {
        DWORD pufDataBlockPointer = Utils::decodeFromLittleEndianBuffer(pufDataBlockPointerBuffer);
        if (pufDataBlockPointer == 0x0 || pufDataBlockPointer == 0xFFFFFFFF)
        {
            status = false;
            Logger::log("getPufDataBlockPointer: invalid pointer: " + std::to_string(pufDataBlockPointer), Error);
        }
        pufDataBlockPointerOffset = (configDataAddress + pufDataBlockPointer);
    }
    return status;
}

void PufHandler::updatePufDataSection(std::vector<BYTE> &pufData,
                                      std::vector<BYTE> newSection,
                                      PufDataBlockSectionOffset sectionOffset)
{
    if (pufData.size() != pufDataBlockSizeInWords * WORD_SIZE
        || newSection.size() > pufDataSectionSizeInWords * WORD_SIZE)
    {
        Logger::log("PUF Data Block size: " + std::to_string(pufData.size()), Error);
        Logger::log("PUF Data Block section size: " + std::to_string(newSection.size()), Error);
        throw std::invalid_argument("updatePufDataSection: incorrect size");
    }

    DWORD expectedSectionMagic = 0;
    DWORD sectionAllocationTableOffset = 0;
    switch (sectionOffset)
    {
        case PufDataBlockSectionOffset::UserIidPufHelpData:
            expectedSectionMagic = pufHelpDataMagicWord;
            sectionAllocationTableOffset = static_cast<DWORD>(AllocationTableFields::offsetUserIidPufHelpData);
            break;
        case PufDataBlockSectionOffset::UdsIidPufHelpData:
            expectedSectionMagic = pufHelpDataMagicWord;
            sectionAllocationTableOffset = static_cast<DWORD>(AllocationTableFields::offsetUdsIidPufHelpData);
            break;
        case PufDataBlockSectionOffset::UserIidPufWrappedAesKey:
            expectedSectionMagic = pufWrappedAesKeyMagicWord;
            sectionAllocationTableOffset = static_cast<DWORD>(AllocationTableFields::offsetUserIidPufWrappedAesKey);
            break;
        case PufDataBlockSectionOffset::UdsIidPufWrappedAesKey:
            expectedSectionMagic = pufWrappedAesKeyMagicWord;
            sectionAllocationTableOffset = static_cast<DWORD>(AllocationTableFields::offsetUdsIidPufWrappedAesKey);
            break;
        default:
            throw std::invalid_argument("updatePufDataSection: PufDataBlockSectionOffset is invalid");
    }
    DWORD existingSectionMagic = Utils::decodeFromLittleEndianBuffer(pufData, static_cast<DWORD>(sectionOffset));
    Logger::log("updatePufDataSection: existing section magic: " + std::to_string(existingSectionMagic), Debug);
    if (existingSectionMagic != expectedSectionMagic)
    {
        // if there was no such section before, increment number of sections
        DWORD numberOfSectionsFielsOffset = static_cast<DWORD>(PufDataBlockSectionOffset::AllocationTable) +
                static_cast<DWORD>(AllocationTableFields::numberOfSections);
        DWORD numberOfSections = Utils::decodeFromLittleEndianBuffer(pufData, numberOfSectionsFielsOffset);
        Logger::log("updatePufDataSection: read number of sections: " + std::to_string(numberOfSections), Debug);
        if (numberOfSections == 0xFFFFFFFF)
        {
            numberOfSections = 0;
            Logger::log("No sections are available. Reset number of section to 0", Debug);
        }
        numberOfSections++;
        Utils::encodeToLittleEndianBuffer(numberOfSections, pufData, numberOfSectionsFielsOffset);

        //Update allocation table field
        Utils::encodeToLittleEndianBuffer(static_cast<DWORD>(sectionOffset), pufData, sectionAllocationTableOffset);
    }

    for (DWORD i = 0; i < newSection.size(); i++)
    {
        pufData[i + static_cast<DWORD>(sectionOffset)] = newSection[i];
    }
}

bool PufHandler::updatePufData(std::vector<BYTE> newSection, PufDataBlockSectionOffset sectionOffset)
{
    Logger::log("Updating PUF Data", Debug);
    bool status = true;
    std::vector<BYTE> pufData;
    DWORD configDataAddress = 0;
    DWORD pufDataBlock0Pointer = 0;
    DWORD pufDataBlock1Pointer = 0;
    status = getConfigurationDataAddress(configDataAddress);

    if (status)
    {
        Logger::log("Reading PUF Data block 0 pointer", Debug);
        status = getPufDataBlockPointer(configDataAddress, pufDataBlock0PointerOffset, pufDataBlock0Pointer);

        if (status)
        {
            Logger::log("Reading PUF Data block 1 pointer", Debug);
            status = getPufDataBlockPointer(configDataAddress, pufDataBlock1PointerOffset, pufDataBlock1Pointer);
            if (status && (pufDataBlock1Pointer - pufDataBlock0Pointer != pufDataBlockSizeInWords * WORD_SIZE))
            {
                Logger::log("pufDataBlock0Pointer: " + std::to_string(pufDataBlock0Pointer), Fatal);
                Logger::log("pufDataBlock1Pointer: " + std::to_string(pufDataBlock1Pointer), Fatal);
                Logger::log("Invalid PUF Data Block pointers", Fatal);
                status = false;
            }
        }
    }

    if (status)
    {
        Logger::log("QSPI read from Pointer 0", Debug);
        status = Qspi::qspiReadMultiple(pufDataBlock0Pointer, pufDataBlockSizeInWords, pufData);
        if (status)
        {
            Logger::log("Updating PUF Data section", Debug);
            updatePufDataSection(pufData, std::move(newSection), sectionOffset);
        }
    }

    if (status)
    {
        Logger::log("QSPI erase on Pointer 0", Debug);
        status = Qspi::qspiErase(pufDataBlock0Pointer, pufDataBlockSizeInWords);
    }

    if (status)
    {
        Logger::log("QSPI write updated PUF Data to pointer 0", Debug);
        status = Qspi::qspiWriteMultiple(pufDataBlock0Pointer, pufDataBlockSizeInWords, pufData);
    }

    if (status)
    {
        Logger::log("QSPI erase on Pointer 1", Debug);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        status = Qspi::qspiErase(pufDataBlock1Pointer, pufDataBlockSizeInWords);
    }

    if (status)
    {
        Logger::log("QSPI write updated PUF Data to pointer 1", Debug);
        status = Qspi::qspiWriteMultiple(pufDataBlock1Pointer, pufDataBlockSizeInWords, pufData);
        Logger::log("Finished updating PUF Data", Debug);
    }

    return status;
}

bool PufHandler::writeWkeyToFlash(std::vector<BYTE> wkeyData, PufType_t pufType)
{
    bool status = Qspi::qspiOpen();
    if (status)
    {
        try
        {
            status = Qspi::qspiSetCs(0x0, false, false);
            if (status)
            {
                switch (pufType)
                {
                    case USER_IID:
                        status = updatePufData(std::move(wkeyData), PufDataBlockSectionOffset::UserIidPufWrappedAesKey);
                        break;
                    case UDS_IID:
                        status = updatePufData(std::move(wkeyData), PufDataBlockSectionOffset::UdsIidPufWrappedAesKey);
                        break;
                    default:
                        Logger::log("writeWkeyToFlash: PUF type is invalid", Fatal);
                        status = false;
                        break;
                }
            }
            Qspi::qspiClose();
        }
        catch(const std::exception &ex)
        {
            Qspi::qspiClose();
            status = false;
        }
    }

    return status;
}

bool PufHandler::writePufHelpDataToFlash(std::vector<BYTE> pufHelpData, PufType_t pufType)
{
    bool status = Qspi::qspiOpen();
    if (status)
    {
        try
        {
            status = Qspi::qspiSetCs(0x0, false, false);
            if (status)
            {
                switch (pufType)
                {
                    case UDS_IID:
                        status = updatePufData(std::move(pufHelpData), PufDataBlockSectionOffset::UdsIidPufHelpData);
                        break;
                    case UDS_INTEL:
                        status = PufHandlerSdm1_5::updateBosPartition(std::move(pufHelpData), PufHandlerSdm1_5::BOS_PARTITION_DATA_BLOCK_SECTIONS::UDS_INTEL_PUF);
                        break;
                    default:
                        Logger::log("writePufHelpDataToFlash: PUF type is invalid", Fatal);
                        status = false;
                        break;
                }
            }
            Qspi::qspiClose();
        }
        catch(const std::exception &ex)
        {
            Qspi::qspiClose();
            status = false;
        }
    }

    return status;
}
