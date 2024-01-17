#!/bin/bash

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

GETOPT_RESULT=$(getopt -o h,b,c,a:,d,su,v: --long help,build,clean,debug,skip-unit-tests,version:,windows,bullseye,valgrind,production,hps,simulator,config:,build-dependencies,rebuild-dependencies,arti-pass:,skip-programmer -- "$@")

if [[ $? != 0 ]]; then
    echo "Ups! getopt failed!"
    exit 1
fi

readonly SCRIPT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd)
readonly BUILD_DIR_BASE="$SCRIPT_DIR/build"
readonly BUILD_DIR_RELEASE="$BUILD_DIR_BASE/Release"
readonly BUILD_DIR_DEBUG="$BUILD_DIR_BASE/Debug"
readonly BUILD_DIR_DEBUG_SIM="$BUILD_DIR_BASE/DebugSpdmSim"
BUILD_DIR="$BUILD_DIR_RELEASE"

BUILD=false
CLEAN=false
RUN_UNIT_TEST=true
BULLSEYE=false
WINDOWS_BUILD=false
HPS_BUILD=false
SPDM_SIM="1.5"
SPDM_SIM_BUILD=false
CMAKE_OPTS='-DCMAKE_RULE_MESSAGES:BOOL=OFF -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON'
BUILD_TYPE=Release
VALGRIND=false
REBUILD_DEPENDENCIES=false
BUILD_DEPENDENCIES=false
ARTIFACTORY_PASS=

CONFIG_FILENAME="config.txt"
DEPENDENCIES_ARTIFACTORY_FOLDER=""
OPENSSL_VERSION=""
BOOST_VERSION=""
LIBCURL_VERSION=""
GTEST_VERSION=""

function usage() {
    echo "Usage: $0 [OPTIONS]"
    echo "Options:"
    echo "no_option                    - run clean, build, run unit tests"
    echo "-h | --help                  - show this message"
    echo "-b | --build                 - build sources and run unit tests"
    echo "-c | --clean                 - only clean output files"
    echo "-d | --debug                 - build debug version"
    echo "-su | --skip-unit-tests      - skip unit tests after build"
    echo "--skip-programmer            - skip building BKP Programmer library"
    echo "-v | --version <version>     - pass build version to build script"
    echo "--production                 - build for production"
    echo "--bullseye                   - run with Bullseye Coverage tools"
    echo "--windows                    - run programmer build on Windows with default options"
    echo "--hps                        - run BKP app and programmer build for aarch64 architecture"
    echo "--simulator <version>        - run BKP app and programmer build with SPDM simulator supporting SDM version 1.2 or 1.5"
    echo "--valgrind                   - add special cmake opts for Valgrind debug"
    echo "--config <path>              - path to dependencies configuration file : default (config.txt)"
    echo "--build-dependencies         - build required dependencies that are not yet present in artifactory and upload them"
    echo "--rebuild-dependencies       - rebuilds all Programmer dependencies and pastes it into folder to be used by programmer build. It does not upload them to artifactory"
    echo "--arti-pass <password>       - password for Artifactory account"
    exit 1
}

function parse_args() {
    if [[ "$1" == "" ]]; then
        CLEAN=true
        BUILD=true
        RUN_UNIT_TEST=true
    else
        while true; do
            case "$1" in
            -c | --clean)
                CLEAN=true
                ;;
            -b | --build)
                BUILD=true
                ;;
            -h | --help)
                usage
                ;;
            -su | --skip-unit-tests)
                RUN_UNIT_TEST=false
                set_cmake_param_bool "DISABLE_TESTS" "ON"
                ;;
            --skip-programmer)
                set_cmake_param_bool "DISABLE_PROGRAMMER" "ON"
                ;;
            -d | --debug)
                set_cmake_param_bool "DEBUG_SESSION" "ON"
                BUILD_TYPE=Debug
                BUILD_DIR="$BUILD_DIR_DEBUG"
                ;;
            --production)
                set_cmake_param_bool "PRODUCTION_MODE" "ON"
                ;;
            -v | --version)
                shift
                set_cmake_param "BUILD_VERSION" $1
                ;;
            --windows)
                WINDOWS_BUILD=true
                BUILD_TYPE=RelWithDebInfo
                BUILD_DIR="$BUILD_DIR_BASE"
                set_cmake_param "CMAKE_SYSTEM_VERSION" "10.0.14393"
                set_cmake_param_bool "DISABLE_TESTS" "ON"
                set_cmake_param_bool "DISABLE_BKP_APP" "ON"
                ;;
            --simulator)
                shift
                BUILD_TYPE=Debug
                SPDM_SIM_BUILD=true
                BUILD_DIR="$BUILD_DIR_DEBUG_SIM"
                if [ $1 != "1.2" ] && [ $1 != "1.5" ]; then
                    echo "SPDM simulator does not support version $1"
                    exit 1
                else
                    SPDM_SIM=$1
                    set_cmake_param "SPDM_SIM" "$SPDM_SIM"
                fi
                ;;
            --hps)
                HPS_BUILD=true
                BUILD_TYPE=MinSizeRel
                BUILD_DIR="$BUILD_DIR_BASE"
                set_cmake_param_bool "HPS_BUILD" "ON"
                ;;
            --bullseye)
                BULLSEYE=true
                ;;
            --valgrind)
                set_cmake_param_bool "VALGRIND_SESSION" "ON"
                BUILD_TYPE=Debug
                VALGRIND=true
                ;;
            --config)
                shift
                CONFIG_FILENAME=$1
                ;;
            --arti-pass)
                shift
                ARTIFACTORY_PASS="$1"
                ;;
            --build-dependencies)
                BUILD_DEPENDENCIES=true
                ;;
            --rebuild-dependencies)
                REBUILD_DEPENDENCIES=true
                ;;
            --)
                shift
                break
                ;;
            *)
                break
                ;;
            esac
            shift
        done
    fi

  set_cmake_param "CMAKE_BUILD_TYPE" "${BUILD_TYPE}"
}

function set_cmake_param() {
    local parameter=$1
    local value=$2

    if [[ "$value" != "" ]]; then
        CMAKE_OPTS="${CMAKE_OPTS} -D $parameter:STRING=$value"
        echo "$parameter set to $value"
    fi
}

function set_cmake_param_bool() {
    local parameter=$1
    local value=$2

    if [[ "$value" != "" ]]; then
        CMAKE_OPTS="${CMAKE_OPTS} -D $parameter:BOOL=$value"
        echo "$parameter set to $value"
    fi
}

function build() {
    echo "Building sources for architecture 'x64' in configuration '${BUILD_TYPE}' ..."
    if [[ ! -d ${BUILD_DIR} ]]; then
        mkdir -p "$BUILD_DIR"
    fi

    if [[ "$REBUILD_DEPENDENCIES" == true ]]; then
        rebuild_dependencies
        check_error_code
    fi
    if [[ "$BUILD_DEPENDENCIES" == true ]]; then
        build_missing_dependencies
        check_error_code
    fi

    cd ${BUILD_DIR}
    echo cmake ${CMAKE_OPTS} ${SCRIPT_DIR}
    cmake ${CMAKE_OPTS} ${SCRIPT_DIR}
    echo cmake --build . -- -j $(nproc)
    cmake --build . -- -j $(nproc)

    check_error_code

    cd ${SCRIPT_DIR}
    echo "Build passed."
}

function build_windows() {
    echo "Building sources for architecture 'x64' in configuration '${BUILD_TYPE}' ..."
    if [[ ! -d ${BUILD_DIR} ]]; then
        mkdir -p "$BUILD_DIR"
    fi

    cd ${BUILD_DIR}

    echo cmake ${CMAKE_OPTS} -G "Visual Studio 15 2017 Win64" -T "v141" ${SCRIPT_DIR}
    cmake ${CMAKE_OPTS} -G "Visual Studio 15 2017 Win64" -T "v141" ${SCRIPT_DIR}

    check_error_code
    echo cmake --build . --config ${BUILD_TYPE}
    cmake --build . --config ${BUILD_TYPE}

    check_error_code

    cd ${SCRIPT_DIR}
    echo "Build passed."
}

function check_error_code() {
    ERROR_CODE="$?"

    if [[ "$ERROR_CODE" != "0" ]]; then
        echo "Build failed with error code $ERROR_CODE! Aborting!"
        disableBullseye
        exit 1
    fi
}

function rebuild_dependencies() {
    echo "Removing dependencies folder..."
    rm -rf dependencies/
    echo "Building dependencies..."
    ./../tools/build-dependencies.sh --clone --config ${CONFIG_FILENAME} --output dependencies --openssl --boost --libcurl --gtest --arti-pass ${ARTIFACTORY_PASS}
}

function build_missing_dependencies() {
    echo "Building missing dependencies..."
    ./../tools/build-dependencies.sh --clone --config ${CONFIG_FILENAME} --output dependencies --arti-pass ${ARTIFACTORY_PASS} --push --build-only-missing-bkpprog
}

function run_unit_tests() {
    if [[ "$VALGRIND" == true ]]; then
        echo "Valgrind session - unit tests skipped!"
    elif [[ "$RUN_UNIT_TEST" == false ]]; then
        echo "Unit tests skipped!"
    else
        echo "Running unit tests..."

        cd ${BUILD_DIR}
        ctest --verbose

        ERROR_CODE="$?"
        if [[ "$ERROR_CODE" != "0" ]]; then
            echo "Unit tests failed with error code $ERROR_CODE! Aborting!"
            exit 1
        fi

        cd ${SCRIPT_DIR}
        echo "Unit tests passed."
    fi
}

function clean() {
    if [[ "$CLEAN" == true ]]; then
        echo "Cleaning sources..."
        if [[ -d ${BUILD_DIR_BASE} ]]; then
            clean_cmake_directory "${BUILD_DIR_RELEASE}"
            clean_cmake_directory "${BUILD_DIR_DEBUG}"
            clean_cmake_directory "${BUILD_DIR_DEBUG_SIM}"
            rm -r "${BUILD_DIR_BASE}"
        fi
        mkdir -pv "${BUILD_DIR_RELEASE}"
        mkdir -pv "${BUILD_DIR_DEBUG}"
        mkdir -pv "${BUILD_DIR_DEBUG_SIM}"
    fi
}

function clean_cmake_directory() {
    local directory=$1

    if [[ -d ${directory} ]]; then
        cd "${directory}"
        echo cmake --build . --target clean >/dev/null 2>&1
        cmake --build . --target clean >/dev/null 2>&1
        cd -
    fi
}

function disableBullseye() {
    if [[ -x "$(command -v cov01)" ]]; then
        cov01 --off --quiet
        covselect --deleteAll --quiet
    fi
}

function enableBullseye() {
    if [[ "$BULLSEYE" == true ]]; then
        path="$(echo $PATH)"
        if [[ ${path} != *"BullseyeCoverage"* ]]; then
            echo "BullseyeCoverage tool path is not set! Aborting!"
            exit 1
        fi

        echo "Enabled Bullseye coverage report"
        COVFILE=$(pwd)/bkpprogrammer.cov
        rm $(pwd)/bullseye/ -rf
        export COVFILE
        covselect --import BullseyeCoverageExclusions -q
        cov01 -1
    fi
}

function publishBullseyeReport() {
    if [[ "$BULLSEYE" == true ]]; then
        echo "CURRENT PATH: $(pwd)"
        covxml --file ${COVFILE} --output BKP_Programmer_CoverageBullseye.xml -q
        bullshtml -f ${COVFILE} coverage_report
        rm bullseye -rf
        mkdir bullseye -p
        mv *.cov bullseye/
        mv coverage_report/ bullseye/
        mv BKP_Programmer_CoverageBullseye.xml bullseye/
    fi
}

function read_config() {
    local dependencies_artifactory_folder=""
    local openssl_version=""
    local boost_version=""
    local libcurl_version=""
    local gtest_version=""

    if [[ -f "$CONFIG_FILENAME" ]]; then
        echo "Reading configuration file: $CONFIG_FILENAME"
        while IFS='=' read -r key value; do
            key=$(echo $key | tr '.' '_')
            eval ${key}=${value}
        done <"$CONFIG_FILENAME"
    else
        echo "Configuration file ( ${CONFIG_FILENAME} ) not found."
        exit 1
    fi

    set_cmake_param "DEPENDENCIES_ARTIFACTORY_FOLDER" ${dependencies_artifactory_folder}
    set_cmake_param "OPENSSL_VERSION" ${openssl_version}
    set_cmake_param "BOOST_VERSION" ${boost_version}
    set_cmake_param "LIBCURL_VERSION" ${libcurl_version}
    set_cmake_param "GTEST_VERSION" ${gtest_version}
}

function prepare_building_environment() {
    source ~/.bashrc
}

function print_done() {
    local gcc_version=$(gcc --version | grep gcc)
    echo "Done for GCC version: ${gcc_version}"
}

function downloadSdpmSim() {
    local sdm_version="$1"
    local spdmSimFile="spdmSim$sdm_version"
    mkdir -p dependencies/
    ../tools/artifactory_manager.sh --src cpp/spdmSim/$spdmSimFile.tar.gz --dest ./dependencies/$spdmSimFile.tar.gz --download --arti-pass ${ARTIFACTORY_PASS} || exit
    tar -xvzf ./dependencies/$spdmSimFile.tar.gz --directory ./dependencies/
    rm -f ./dependencies/$spdmSimFile.tar.gz
}

main() {
    parse_args "$@"

    read_config

    if [[ "$WINDOWS_BUILD" == true ]]; then
        build_windows
    else
        prepare_building_environment
        disableBullseye
        clean
        if [[ "$BUILD" == true ]]; then
            enableBullseye
            if [[ "$SPDM_SIM_BUILD" == true ]]; then
                downloadSdpmSim $SPDM_SIM
            fi
            build
            run_unit_tests
            disableBullseye
        fi
        publishBullseyeReport
        print_done
    fi
}

main "$@"
