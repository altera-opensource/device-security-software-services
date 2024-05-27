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

package com.intel.bkp.bkps.rest.configuration.service;

import com.intel.bkp.bkps.crypto.aesgcm.AesGcmSealingKeyProviderImpl;
import com.intel.bkp.bkps.crypto.sealingkey.SealingKeyManager;
import com.intel.bkp.bkps.domain.AesKey;
import com.intel.bkp.bkps.domain.ConfidentialData;
import com.intel.bkp.bkps.domain.enumeration.ImportMode;
import com.intel.bkp.bkps.exception.ProvisioningGenericException;
import com.intel.bkp.bkps.exception.ServiceConfigurationNotFound;
import com.intel.bkp.bkps.repository.ServiceConfigurationRepository;
import com.intel.bkp.bkps.rest.configuration.model.dto.ServiceConfigurationDTO;
import com.intel.bkp.bkps.rest.configuration.model.dto.ServiceConfigurationDetailsDTO;
import com.intel.bkp.bkps.rest.configuration.model.dto.ServiceConfigurationResponseDTO;
import com.intel.bkp.bkps.rest.configuration.model.mapper.ServiceConfigurationDetailsMapper;
import com.intel.bkp.bkps.rest.configuration.model.mapper.ServiceConfigurationMapper;
import com.intel.bkp.bkps.rest.errors.enums.ErrorCodeMap;
import com.intel.bkp.core.endianness.EndiannessActor;
import com.intel.bkp.core.endianness.StructureBuilder;
import com.intel.bkp.core.exceptions.BKPBadRequestException;
import com.intel.bkp.core.exceptions.BKPInternalServerException;
import com.intel.bkp.core.exceptions.ParseStructureException;
import com.intel.bkp.core.interfaces.IStructure;
import com.intel.bkp.core.psgcertificate.IPsgAesKeyBuilder;
import com.intel.bkp.core.psgcertificate.PsgAesKeyBuilderFactory;
import com.intel.bkp.core.psgcertificate.enumerations.StorageType;
import com.intel.bkp.core.psgcertificate.model.PsgAesKeyType;
import com.intel.bkp.crypto.exceptions.EncryptionProviderException;
import lombok.AccessLevel;
import lombok.RequiredArgsConstructor;
import lombok.Setter;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Isolation;
import org.springframework.transaction.annotation.Transactional;

import java.util.LinkedList;
import java.util.List;
import java.util.Optional;
import java.util.stream.Collectors;

import static com.intel.bkp.utils.HexConverter.fromHex;
import static com.intel.bkp.utils.HexConverter.toHex;
import static lombok.AccessLevel.PACKAGE;

/**
 * Service Implementation for managing ServiceConfiguration.
 */
@Service
@Transactional(isolation = Isolation.SERIALIZABLE)
@RequiredArgsConstructor(access = PACKAGE)
@Slf4j
public class ServiceConfigurationService {

    private final ServiceConfigurationRepository serviceConfigurationRepository;
    private final ServiceConfigurationDetailsMapper serviceConfigurationDetailsMapper;
    private final ServiceConfigurationMapper serviceConfigurationMapper;
    private final AesGcmSealingKeyProviderImpl aesGcmSealingKeyProvider;
    private final SealingKeyManager sealingKeyManager;
    private final ServiceConfigurationImportManager serviceConfigurationImportManager;

    @Setter(AccessLevel.PACKAGE)
    private PsgAesKeyBuilderFactory psgAesKeyBuilderFactory = new PsgAesKeyBuilderFactory();

    /**
     * Save a serviceConfiguration.
     *
     * @param serviceConfigurationDTO the entity to save
     *
     * @return the persisted entity
     */
    public ServiceConfigurationDTO save(ServiceConfigurationDTO serviceConfigurationDTO) {
        throwIfSealingKeyRotationPending();
        throwIfNoActiveSealingKey();

        final var config = serviceConfigurationMapper.toEntity(serviceConfigurationDTO);

        if (ImportMode.ENCRYPTED.equals(config.getConfidentialData().getImportMode())) {
            serviceConfigurationImportManager.decrypt(config.getConfidentialData());
        }

        final AesKey aesKey = config.getConfidentialData().getAesKey();
        setAesKeyDetails(aesKey);

        encryptConfidentialData(config.getConfidentialData());

        final var savedConfig = serviceConfigurationRepository.save(config);
        log.info("Configuration {} saved.", savedConfig.getId());
        return serviceConfigurationMapper.toDto(savedConfig);
    }

    private void setAesKeyDetails(AesKey aesKey) {
        try {
            final var aesKeyBytes = fromHex(aesKey.getValue());
            final var builder = getPsgAesKeySDMType(aesKeyBytes);

            parseKey(aesKeyBytes, builder);
            aesKey.setStorage(builder.getStorageType());
            aesKey.setKeyWrappingType(builder.getKeyWrappingType());
            verifyEfusesStorageTypeRequiredField(aesKey, builder.getAesKeyType());
            handleTestFlag(aesKey, builder.getAesKeyType());
        } catch (ParseStructureException e) {
            throw new BKPBadRequestException(ErrorCodeMap.CORRUPTED_AES_KEY, e);
        }
    }

    private void handleTestFlag(AesKey aesKeyFromDTO, PsgAesKeyType type) {
        if (aesKeyFromDTO.getTestProgram() != null) {
            if (!type.getTestprogramSupported()) {
                if (aesKeyFromDTO.getTestProgram()) {
                    log.warn("Compact certificate version (%d) does not support testProgram flag. Hence, resetting it to false.".formatted(type.getVersion()));
                    aesKeyFromDTO.setTestProgram(false);
                }
            }
        } else {
            aesKeyFromDTO.setTestProgram(false);
        }
    }

    private void verifyEfusesStorageTypeRequiredField(AesKey aesKey, PsgAesKeyType type) {
        if (PsgAesKeyType.SDM_1_2.equals(type)
            && StorageType.EFUSES == aesKey.getStorage()
            && aesKey.getTestProgram() == null) {
            throw new BKPBadRequestException(ErrorCodeMap.MISSING_FLAG_TEST_PROGRAM);
        }
    }

    private IPsgAesKeyBuilder<? extends StructureBuilder<?, ? extends IStructure>> getPsgAesKeySDMType(
        byte[] decryptedAesKey) {
        try {
            return psgAesKeyBuilderFactory
                .withActor(EndiannessActor.FIRMWARE)
                .getPsgAesKeyBuilder(decryptedAesKey);
        } catch (ParseStructureException e) {
            throw new ProvisioningGenericException("Failed to get AES Key SDM version.", e);
        }
    }

    private void parseKey(byte[] decryptedAesKey,
                          IPsgAesKeyBuilder<? extends StructureBuilder<?, ? extends IStructure>> builder) {
        try {
            builder.withActor(EndiannessActor.FIRMWARE).parse(decryptedAesKey);
        } catch (ParseStructureException e) {
            throw new BKPBadRequestException(ErrorCodeMap.CORRUPTED_AES_KEY, e);
        }
    }

    /**
     * Get all the serviceConfigurations.
     *
     * @return the list of entities
     */
    @Transactional(readOnly = true)
    public List<ServiceConfigurationResponseDTO> findAllForResponse() {
        log.debug("Request to get all ServiceConfigurations");
        return serviceConfigurationRepository.findAll().stream()
            .map(serviceConfigurationMapper::toResultDto)
            .collect(Collectors.toCollection(LinkedList::new));
    }

    /**
     * Get one serviceConfiguration by id.
     *
     * @param id the id of the entity
     *
     * @return the entity
     */
    @Transactional(readOnly = true)
    public Optional<ServiceConfigurationDetailsDTO> findOneForDetails(Long id) {
        log.debug("Request to get ServiceConfiguration : {}", id);
        return serviceConfigurationRepository.findById(id)
            .map(serviceConfigurationDetailsMapper::toDto);
    }

    /**
     * Delete the serviceConfiguration by id.
     *
     * @param id the id of the entity
     */
    public void delete(Long id) {
        log.debug("Request to delete ServiceConfiguration : {}", id);

        if (!exists(id)) {
            throw new ServiceConfigurationNotFound();
        }

        serviceConfigurationRepository.deleteById(id);
    }

    public boolean exists(Long id) {
        return serviceConfigurationRepository.existsById(id);
    }

    private void encryptConfidentialData(ConfidentialData confidentialData) {
        aesGcmSealingKeyProvider.initialize(sealingKeyManager.getActiveKey());

        byte[] customerAesKey = fromHex(confidentialData.getAesKey().getValue());

        confidentialData.getAesKey().setValue(encryptInternal(customerAesKey));
    }

    private void throwIfNoActiveSealingKey() {
        if (!sealingKeyManager.isActiveSealingKey()) {
            throw new BKPBadRequestException(ErrorCodeMap.ACTIVE_SEALING_KEY_DOES_NOT_EXIST);
        }
    }

    private void throwIfSealingKeyRotationPending() {
        if (sealingKeyManager.isPendingSealingKey()) {
            throw new BKPBadRequestException(ErrorCodeMap.SEALING_KEY_ROTATION_PENDING);
        }
    }

    private String encryptInternal(byte[] data) {
        try {
            return toHex(aesGcmSealingKeyProvider.encrypt(data));
        } catch (EncryptionProviderException e) {
            throw new BKPInternalServerException(ErrorCodeMap.FAILED_TO_ENCRYPT_SENSITIVE_DATA_WITH_SEALING_KEY, e);
        }
    }
}
