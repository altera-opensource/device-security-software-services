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

set(TARGET_NAME core_test)

include_directories(headers)
include_directories(../core/headers)
include_directories("${PROJECT_BINARY_DIR}")

file(GLOB_RECURSE SOURCE_FILES cpp/*.cpp)
add_executable(core_test ${SOURCE_FILES})


if(NOT DEFINED DEPENDENCIES_PATH)
    set(DEPENDENCIES_PATH ${CMAKE_SOURCE_DIR}/dependencies)
endif()
set(OPENSSL_LIB_DIR ${DEPENDENCIES_PATH}/openssl/lib)
set(OPENSSL_INCLUDE_DIR ${DEPENDENCIES_PATH}/openssl/include)
set(LIBCURL_LIB_DIR ${DEPENDENCIES_PATH}/libcurl/lib)
set(LIBCURL_INCLUDE_DIR ${DEPENDENCIES_PATH}/libcurl/include)
set(BOOST_LIB_DIR ${DEPENDENCIES_PATH}/boost/lib)
set(BOOST_INCLUDE_DIR ${DEPENDENCIES_PATH}/boost/include)
set(GTEST_LIB_DIR ${DEPENDENCIES_PATH}/gtest/lib)
set(GTEST_INCLUDE_DIR ${DEPENDENCIES_PATH}/gtest/include)

target_include_directories(${TARGET_NAME} SYSTEM PRIVATE ${OPENSSL_INCLUDE_DIR})
target_include_directories(${TARGET_NAME} SYSTEM PRIVATE ${LIBCURL_INCLUDE_DIR})
target_include_directories(${TARGET_NAME} SYSTEM PRIVATE ${BOOST_INCLUDE_DIR})
target_include_directories(${TARGET_NAME} SYSTEM PRIVATE ${GTEST_INCLUDE_DIR})
if (NOT WIN32)
    target_link_libraries(${TARGET_NAME} PRIVATE
        ${LIBCURL_LIB_DIR}/libcurl.so
        ${OPENSSL_LIB_DIR}/libcrypto.so
        ${OPENSSL_LIB_DIR}/libssl.so
        ${GTEST_LIB_DIR}/libgtest.a
        ${GTEST_LIB_DIR}/libgtest_main.a
        ${GTEST_LIB_DIR}/libgmock.a
        ${GTEST_LIB_DIR}/libgmock_main.a
    )
    target_link_libraries(${TARGET_NAME} PRIVATE dl)
else()
    file(GLOB libboost_container_file "${BOOST_LIB_DIR}/libboost_container*.lib")
    file(GLOB libboost_json_file "${BOOST_LIB_DIR}/libboost_json*.lib")
    target_link_libraries(${TARGET} PRIVATE ${LIBS})
    target_link_libraries(${TARGET_NAME} PRIVATE
        ${LIBCURL_LIB_DIR}/libcurl.lib
        ${OPENSSL_LIB_DIR}/libcrypto.lib
        ${OPENSSL_LIB_DIR}/libssl.lib
        ${libboost_container_file}
        ${libboost_json_file})
endif ()

add_test(NAME core_test_suite COMMAND core_test)
set_tests_properties(core_test_suite PROPERTIES ENVIRONMENT LD_LIBRARY_PATH=$ENV{LD_LIBRARY_PATH_TMP})

if (NOT WIN32)
    target_link_libraries(core_test PRIVATE dl)
endif ()

target_link_libraries(core_test PRIVATE pthread bkpprog)

