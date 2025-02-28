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

package com.intel.bkp.verifier.database;

import com.intel.bkp.verifier.database.repository.CacheEntityServiceBase;
import com.intel.bkp.verifier.database.repository.S10CacheEntityService;
import com.intel.bkp.verifier.model.DatabaseConfiguration;
import lombok.extern.slf4j.Slf4j;
import org.apache.commons.dbutils.QueryRunner;
import org.apache.commons.dbutils.handlers.ScalarHandler;

import java.sql.Connection;
import java.sql.SQLException;
import java.util.Map;
import java.util.stream.Stream;

import static com.intel.bkp.verifier.database.CacheEntityType.S10;
import static java.util.stream.Collectors.toUnmodifiableMap;

@Slf4j
public class SQLiteHelper implements AutoCloseable {

    /**
     * Updating database schema requires updating CURRENT_SCHEMA_VERSION.
     */
    private static final int CURRENT_SCHEMA_VERSION = 1;


    private static final String SQL_SCHEMA_VERSION = "PRAGMA user_version";

    private final DatabaseManager databaseManager;
    private final Connection connection;

    private Map<CacheEntityType, CacheEntityServiceBase> entityServices;

    private final QueryRunner runner = new QueryRunner();

    public SQLiteHelper(DatabaseConfiguration dbConfig) {
        this.databaseManager = DatabaseManager.instance(dbConfig);
        this.connection = databaseManager.getConnection();

        entityServices = Stream.of(
            new S10CacheEntityService(connection)
        ).collect(toUnmodifiableMap(CacheEntityServiceBase::getCacheEntityType, s -> s));

        final int oldVersion = getDatabaseVersion();
        log.debug("SQLite database version: {}", oldVersion);
        log.debug("SQLite database current supported version: {}", CURRENT_SCHEMA_VERSION);
        entityServices.forEach((flowType, migratable) -> migratable.migrate(oldVersion, CURRENT_SCHEMA_VERSION));
        setDatabaseVersion();
    }

    @Override
    public void close() {
        databaseManager.closeDatabase();
        entityServices = null;
    }

    public S10CacheEntityService getS10CacheEntityService() {
        return (S10CacheEntityService)entityServices.get(S10);
    }

    private int getDatabaseVersion() {
        try {
            return runner.query(connection, SQL_SCHEMA_VERSION, new ScalarHandler<>());
        } catch (SQLException e) {
            log.error("Database error: {}", e.getMessage());
            log.debug("Stacktrace: ", e);
            return 0;
        }
    }

    private void setDatabaseVersion() {
        try {
            runner.update(connection, String.format("%s = %d", SQL_SCHEMA_VERSION, CURRENT_SCHEMA_VERSION));
        } catch (SQLException e) {
            log.error("Database error: {}", e.getMessage());
            log.debug("Stacktrace: ", e);
        }
    }
}
