#
# This project is licensed as below.
#
# ***************************************************************************
#
# Copyright 2020-2025 Altera Corporation. All Rights Reserved.
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

cmake_minimum_required(VERSION 3.10)

project(Programmer)

option(DISABLE_TESTS "Disable all tests" OFF)
option(DISABLE_PROGRAMMER "Do not build BKP Programmer" OFF)
option(DEBUG_SESSION "Disable all security compiler flags, enable debug symbols and run address sanitizer" OFF)
option(PRODUCTION_MODE "Export symbols only for provisioning" OFF)
option(VALGRIND_SESSION "Disable all security compiler flags and enable debug symbols for Valgrind" OFF)

enable_testing()
set(CMAKE_CXX_STANDARD 17)
set(BASE_PATH ${CMAKE_SOURCE_DIR})
set(ARCH "x64")

if(DEFINED HPS_BUILD)
    set(DISABLE_TESTS 1)
    set(CMAKE_SYSTEM_PROCESSOR aarch64)
    set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
    set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)
    set(ARCH aarch64)
endif()

if (WIN32)
    message("Running RELEASE build.")
    message("Running PRODUCTION build.")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPORT=WINDOWS /guard:cf /W3")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /guard:cf")

    link_libraries(winhttp.lib crypt32.lib)

else ()
    if(ARCH STREQUAL "x64")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include glibc_redirection.h")
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=1")

    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "5.0")
        set(STACK_PROTECTOR_FLAG "-fstack-protector")
    else()
        set(STACK_PROTECTOR_FLAG "-fstack-protector-strong")
    endif()

    if (DEBUG_SESSION)
        message("Running DEBUG_SESSION")
        set(CMAKE_CXX_FLAGS "-g -O0 -fPIC -fsanitize=address -fno-omit-frame-pointer")
        set(CMAKE_SHARED_LINKER_FLAGS "-g -O0 -fPIC -fsanitize=address -fno-omit-frame-pointer")
    elseif (VALGRIND_SESSION)
        message("Running VALGRIND_SESSION")
        set(CMAKE_CXX_FLAGS "-g -O0 -fPIC")
        set(CMAKE_SHARED_LINKER_FLAGS "-g -O0 -fPIC")
    else ()
        message("Running RELEASE build.")

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-overflow -fno-delete-null-pointer-checks -fwrapv -pthread -Wall -Wextra -Werror -O3 -D_FORTIFY_SOURCE=2 ${STACK_PROTECTOR_FLAG} -fPIC -Wformat -Wformat-security -rdynamic")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wall -Wextra -Werror -O3 -fPIC -Wl,-z,noexecstack,-z,relro,-z,now -pthread")

        if (PRODUCTION_MODE)
            message("Running PRODUCTION build.")
            set(CMAKE_SKIP_INSTALL_RPATH ON)
            set(CMAKE_SKIP_RPATH ON)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
        else()
            message("Running DEVELOPMENT build.")
        endif()

    endif ()

    # just != WINDOWS; unix has to be lowercase, because UNIX is already defined
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPORT=unix")

endif (WIN32)


add_subdirectory(src)
