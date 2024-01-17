#
# This project is licensed as below.
#
# ***************************************************************************
#
# Copyright 2020-2024 Intel Corporation. All Rights Reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
# PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
# OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# ***************************************************************************
#

if( (NOT DEFINED DEPENDENCIES_ARTIFACTORY_FOLDER) OR (DEPENDENCIES_ARTIFACTORY_FOLDER STREQUAL "#AUTOMATIC#") )
    resolve_automatic_artifactory_folder(result_artifactory_folder)
    set(DEPENDENCIES_ARTIFACTORY_FOLDER "${result_artifactory_folder}")
    message("Setting automatic parameter --> DEPENDENCIES_ARTIFACTORY_FOLDER: ${DEPENDENCIES_ARTIFACTORY_FOLDER}")
endif()

if(NOT DEFINED OPENSSL_VERSION)
    set(OPENSSL_VERSION "3.1.1")
endif()

if(NOT DEFINED BOOST_VERSION)
    set(BOOST_VERSION "1.82.0")
endif()

if(NOT DEFINED LIBCURL_VERSION)
    set(LIBCURL_VERSION "8.2.0")
endif()

if(NOT DEFINED GTEST_VERSION)
    set(GTEST_VERSION "1.12.1")
endif()

set(DEPENDENCY_FILENAME_CORE "${BASE_PATH}/src/core/dependencies.txt")
set(DEPENDENCY_FILENAME_BKP_APP "${BASE_PATH}/src/bkp_app/dependencies.txt")
set(DEPENDENCY_FILENAME_TEST "${BASE_PATH}/src/core_test/dependencies.txt")
set(DEPENDENCY_FILENAME_BKP_APP_TEST "${BASE_PATH}/src/bkp_app_test/dependencies.txt")
configure_file(${DEPENDENCY_FILENAME_BKP_APP}.in ${DEPENDENCY_FILENAME_BKP_APP})
configure_file(${DEPENDENCY_FILENAME_CORE}.in ${DEPENDENCY_FILENAME_CORE})
configure_file(${DEPENDENCY_FILENAME_TEST}.in ${DEPENDENCY_FILENAME_TEST})
configure_file(${DEPENDENCY_FILENAME_BKP_APP_TEST}.in ${DEPENDENCY_FILENAME_BKP_APP_TEST})

if (NOT DISABLE_PROGRAMMER)
    add_subdirectory(core)
endif ()

if (NOT DISABLE_TESTS)
    add_subdirectory(core_test)
    if (NOT DISABLE_BKP_APP)
        add_subdirectory(bkp_app_test)
    endif ()
endif ()

if (NOT DISABLE_BKP_APP)
    add_subdirectory(bkp_app)
endif ()
