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

package com.intel.bkp.bkps.attestation;

import com.intel.bkp.bkps.attestation.mapping.CacheObjectMapper;
import com.intel.bkp.bkps.connector.DpConnector;
import com.intel.bkp.bkps.rest.prefetching.service.IPrefetchRepositoryService;
import lombok.extern.slf4j.Slf4j;

import java.util.Optional;

@Slf4j
public abstract class CacheObjectFetcherBase<T> {

    private final IPrefetchRepositoryService<T> repositoryService;
    private final CacheObjectMapper<T> mapper;
    private final DpConnector connector;

    CacheObjectFetcherBase(IPrefetchRepositoryService<T> repositoryService, DpConnector connector) {
        this.repositoryService = repositoryService;
        this.mapper = repositoryService.getMapper();
        this.connector = connector;
    }

    abstract boolean isValid(T obj);

    public Optional<T> fetch(String url) {
        return findValidInCache(url)
            .or(() -> downloadAndSaveInCache(url));
    }

    public Optional<T> fetchSkipCache(String url) {
        findValidInCache(url).ifPresent(data ->
            log.debug("Found valid data in cache, but fresh content shall be retrieved from url: {}", url));
        return downloadAndSaveInCache(url);
    }

    private Optional<T> findValidInCache(String url) {
        return repositoryService.find(url)
            .filter(this::isValid);
    }

    private Optional<T> downloadAndSaveInCache(String url) {
        return download(url).map(obj -> saveInCache(url, obj));
    }

    private Optional<T> download(String url) {
        log.debug("Downloading from url: {}", url);
        return connector.tryGetBytes(url).flatMap(mapper::parse);
    }

    private T saveInCache(String url, T obj) {
        repositoryService.save(url, obj);
        return obj;
    }
}
