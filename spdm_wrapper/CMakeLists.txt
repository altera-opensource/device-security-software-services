cmake_minimum_required(VERSION 3.18)
project(libspdm_wrapper)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0 -fPIC -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0 -fPIC -Wall")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -g -O0 -fPIC")
set(ARCH "x64")
set(BASE_PATH ${CMAKE_SOURCE_DIR})

if(NOT DEFINED DEPENDENCIES_PATH)
    set(DEPENDENCIES_PATH ${BASE_PATH}/dependencies)
endif()

if(NOT DEFINED LIBSPDM_LIB_DIR)
    set(LIBSPDM_LIB_DIR ${DEPENDENCIES_PATH}/libspdm/lib)
endif()

if(NOT DEFINED OPENSSL_LIB_DIR)
    set(OPENSSL_LIB_DIR ${DEPENDENCIES_PATH}/openssl/lib)
endif()

if(NOT DEFINED LIBSPDM_INCLUDE_DIR)
    set(LIBSPDM_INCLUDE_DIR ${DEPENDENCIES_PATH}/libspdm/include)
endif()


include_directories(${LIBSPDM_INCLUDE_DIR})

add_subdirectory(wrapper)
