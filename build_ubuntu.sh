#!/bin/bash

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

OPENSSL_VERSION="3.1.4"
BOOST_VERSION="1.84.0"
LIBCURL_VERSION="8.5.0"
GTEST_VERSION="1.14.0"
LIBSPDM_VERSION="3.2.0"
MINIMUM_JAVA_VERSION="17.0.0"

FCSSERVER_IMAGE_NAME="fcsserver-builder"
FCS_DOCKER_BUILD_DIR="/fcs-server"
BKPPROG_IMAGE_NAME="bkpprogrammer-builder"
BKPPROG_DOCKER_BUILD_DIR="/bkpprogrammer"
CONTAINER_ID=

# path from bkps/src/integrationTest/resources/config/application.yml ssl.bundle.jks.web-server.truststore.location
TRUSTSTORE_PATH="/tmp/bkps-nonprod.p12"

REQUIRED_PACKAGE_NOT_INSTALLED="\n\n++++++++++ Package %s not installed. Try to install... +++++++++++++++\n\n"
REQUIRED_PACKAGE_INSTALLED="\n\n++++++++++++++++ Package %s already installed ++++++++++++++++++++++++\n\n"
BUILD_WRAPPER="\n\n+++++++++++++++++++++ BUILD SPDM WRAPPER +++++++++++++++++++++++++++++\n\n"
BUILD_BKPPROGRAMMER="\n\n+++++++++++++++++++++ BUILD BKPPROGRAMMER ++++++++++++++++++++++++++++\n\n"
BUILD_FCSSERVER="\n\n+++++++++++++++++++++ BUILD FCS SERVER +++++++++++++++++++++++++++++++\n\n"
LOG_PRIVILEGED_REQUIRED="\n\n++++++++++++++++++++++ Cannot connect to the Docker daemon ++++++++++++++++++++++
+++++++ If running docker from docker, make sure to give extended privileges ++++
++++++++ to host container (docker run --privileged) ++++++++++++++++++++++++++++\n\n"

LOG_SUCCESS="\n\n++++++++++++++++++++++++++++++++ SUCCESS ++++++++++++++++++++++++++++++++++++++++\n\n"
LOG_FAILURE="\n\n++++++++++++++++++++++++++++++++ FAILURE ++++++++++++++++++++++++++++++++++++++++\n\n"
LOG_OUTPUT="\n\n+++++++++++++++++++++++++++++ Output folder: %s ++++++++++++++++++++++++++++++++++++\n"

CURRENT_SCRIPT_PATH=$(dirname "$0")
cd "$CURRENT_SCRIPT_PATH"
CURRENT_SCRIPT_PATH=$(pwd)
printf "\n\n+++++++++++++++++++++++++++++++++ Current script path: %s ++++++++++++++++++++++++++++\n\n" ${CURRENT_SCRIPT_PATH}

OUT_PATH=${CURRENT_SCRIPT_PATH}/out
SPDM_WRAPPER_PATH=${CURRENT_SCRIPT_PATH}/spdm_wrapper
BKPPROGRAMMER_PATH=${CURRENT_SCRIPT_PATH}/bkpprogrammer
FCS_PATH=${CURRENT_SCRIPT_PATH}/FCS
openssl_root_dir=${CURRENT_SCRIPT_PATH}/dependencies/openssl
libspdm_root_dir=${CURRENT_SCRIPT_PATH}/dependencies/libspdm
boost_root_dir=${CURRENT_SCRIPT_PATH}/dependencies/boost
gtest_root_dir=${CURRENT_SCRIPT_PATH}/dependencies/gtest
libcurl_root_dir=${CURRENT_SCRIPT_PATH}/dependencies/libcurl

function build_spdm_wrapper() {
    printf "${BUILD_WRAPPER}"
    cd "${SPDM_WRAPPER_PATH}"
    local cmake_build_type=Release
    local sources_dir=$(pwd)
    local build_dir="build/${cmake_build_type}"
    mkdir -p "${build_dir}" && cd "${build_dir}"

    CMAKE_OPTS="-DCMAKE_BUILD_TYPE=${cmake_build_type}"
    echo "cmake ${CMAKE_OPTS} ${sources_dir}"
    cmake ${CMAKE_OPTS} "${sources_dir}"

    echo "cmake --build . -- -j $(nproc)"
    cmake --build . -- -j $(nproc)

    cd "${CURRENT_SCRIPT_PATH}"
    mkdir -p ${OUT_PATH}/spdm_wrapper/
    cp -r ${SPDM_WRAPPER_PATH}/build/Release/* ${OUT_PATH}/spdm_wrapper/
}

function build_bkpprogrammer() {
    check_if_docker_daemon_can_run
    if [[ $? -eq 0 ]]; then
        printf "${BUILD_BKPPROGRAMMER}"
        build_bkpprogrammer_internal
        if [[ $? -eq 1 ]]; then
            printf "${LOG_FAILURE}"
            cd "${CURRENT_SCRIPT_PATH}"
            return
        fi
        if echo result | grep -q 'Done'; then
            printf "${LOG_SUCCESS}"
        fi
    fi

    mkdir -p ${OUT_PATH}/bkpprogrammer/

    cp ${BKPPROGRAMMER_PATH}/docker/out_docker/libbkpprog.so ${OUT_PATH}/bkpprogrammer/
    cp ${BKPPROGRAMMER_PATH}/docker/out_docker/MinSizeRel/src/bkp_app/bkp_app ${OUT_PATH}/bkpprogrammer/
    cp ${BKPPROGRAMMER_PATH}/docker/out_docker/libgcc_* ${OUT_PATH}/bkpprogrammer/
    cp ${BKPPROGRAMMER_PATH}/docker/out_docker/libstdc++.so.6.0.25 ${OUT_PATH}/bkpprogrammer/
    cd "${CURRENT_SCRIPT_PATH}"
}

function build_fcsserver() {
    check_if_docker_daemon_can_run
    if [[ $? -eq 0 ]]; then
        printf "${BUILD_FCSSERVER}"
        build_fcs_server_internal
        if [[ $? -eq 1 ]]; then
            printf "${LOG_FAILURE}"
            cd "${CURRENT_SCRIPT_PATH}"
            return
        fi
        if echo result | grep -q 'Done'; then
            printf "${LOG_SUCCESS}"
        fi
    fi
    cd "${CURRENT_SCRIPT_PATH}"
    mkdir -p ${OUT_PATH}/FCS/
    cp -r ${FCS_PATH}/docker/out_docker/* ${OUT_PATH}/FCS/
}

function install_package_if_does_not_exist() {
    REQUIRED_PKG=$1
    check_if_package_exist $REQUIRED_PKG
    if [[ $? -eq 1 ]]; then
        install_package $REQUIRED_PKG
    fi
}

function check_if_package_exist() {
    REQUIRED_PKG=$1
    PKG_OK=$(dpkg-query -W --showformat='${Status}\n' $REQUIRED_PKG | grep "install ok installed")
    echo Checking for $REQUIRED_PKG: $REQUIRED_PKG
    if [ "install ok installed" = "$PKG_OK" ]; then
        printf "${REQUIRED_PACKAGE_INSTALLED}" "$REQUIRED_PKG"
        return 0
    else
        printf "${REQUIRED_PACKAGE_NOT_INSTALLED}" "$REQUIRED_PKG"
        return 1
    fi
}

function install_package() {
    REQUIRED_PKG=$1
    echo "No $REQUIRED_PKG. Setting up $REQUIRED_PKG."
    sudo apt update
    sudo apt --yes install $REQUIRED_PKG
}

function check_java() {
    if type -p java; then
        echo "Found java executable in PATH"
        _java=java
    elif [[ -n "$JAVA_HOME" ]] && [[ -x "$JAVA_HOME/bin/java" ]]; then
        echo "Found java executable in JAVA_HOME"
        _java="$JAVA_HOME/bin/java"
    else
        echo "No Java found. Install OPENJDK"
        install_package_if_does_not_exist openjdk-17-jdk
    fi

    if [[ "$_java" ]]; then
        version=$("$_java" -version 2>&1 | awk -F '"' '/version/ {print $2}')
        echo "Java version: $version"
        ver_comp "$version" $MINIMUM_JAVA_VERSION
        comp=$?
        if [[ $comp -lt 2 ]]; then
            echo "Your Java version is sufficient"
        else
            echo "Please update Java to version >=$MINIMUM_JAVA_VERSION"
            return 1
        fi
    fi
}

function ver_comp() {
    # shellcheck disable=SC2053
    if [[ $1 == $2 ]]; then
        return 0
    fi
    local IFS=.
    # shellcheck disable=SC2206
    local i ver1=($1) ver2=($2)
    # fill empty fields in ver1 with zeros
    for ((i = ${#ver1[@]}; i < ${#ver2[@]}; i++)); do
        ver1[i]=0
    done
    for ((i = 0; i < ${#ver1[@]}; i++)); do
        if [[ -z ${ver2[i]} ]]; then
            # fill empty fields in ver2 with zeros
            ver2[i]=0
        fi
        if ((10#${ver1[i]} > 10#${ver2[i]})); then
            return 1
        fi
        if ((10#${ver1[i]} < 10#${ver2[i]})); then
            return 2 # normally would be -1
        fi
    done
    return 0
}

function check_if_require_privileged() {
    if { docker ps 2>&1 >&3 3>&- | grep '^' >&2; } 3>&1; then
        case $(docker ps 2>&1) in
        *"Cannot connect to the Docker daemon at unix:///var/run/docker.sock."*)
            printf "${LOG_PRIVILEGED_REQUIRED}"
            return 0
            ;;
        *) return 1 ;;
        esac
    fi
    return 1
}

function check_if_docker_daemon_can_run() {
    check_if_require_privileged
    if [[ $? -eq 0 ]]; then
        return 1
    else
        docker ps
        if [ $? -eq 0 ]; then
            echo "Command succeeded"
            return 0
        else
            install_docker
            check_if_require_privileged
            if [[ $? -eq 0 ]]; then
                return 1
            fi
            return 0
        fi
    fi
}

function install_docker() {
    printf "${REQUIRED_PACKAGE_NOT_INSTALLED}" "docker"
    curl -fsSL https://get.docker.com -o install-docker.sh
    chmod +x install-docker.sh
    sudo sh install-docker.sh
}

function copy_dependencies() {
    local destination_folder=$1
    local dependency_folder=$2

    DEPENDENCIES_FOLDER=${destination_folder}/dependencies/
    mkdir -p ${DEPENDENCIES_FOLDER}
    cp -r ${dependency_folder} ${DEPENDENCIES_FOLDER}
}

function create_dummy_key_if_does_not_exist() {
    if [ -f "${TRUSTSTORE_PATH}" ]; then
        if keytool -list -keystore ${TRUSTSTORE_PATH} -storepass donotchange -alias dummy; then
            return
        fi
    fi
    keytool -genkey -keyalg RSA -keystore ${TRUSTSTORE_PATH} -keysize 2048 -keypass donotchange -storepass donotchange -dname "CN=Developer, OU=Department, O=Company, L=City, ST=State, C=CA" -alias dummy
}

function build_fcs_server_internal() {
    cd ${FCS_PATH}/docker/
    build_docker ${FCSSERVER_IMAGE_NAME}
    if [[ $? -eq 2 ]]; then
        printf "${LOG_PRIVILEGED_REQUIRED}"
        cd "${CURRENT_SCRIPT_PATH}"
        return 1
    fi
    run_container ${FCSSERVER_IMAGE_NAME}

    copy_files_to_docker_fcs
    run_build_fcs
    copy_files_from_docker_fcs

    clean_container
    echo "Done"
}

function build_bkpprogrammer_internal() {
    cd ${BKPPROGRAMMER_PATH}/docker/
    build_docker ${BKPPROG_IMAGE_NAME}
    if [[ $? -eq 2 ]]; then
        printf "${LOG_PRIVILEGED_REQUIRED}"
        cd "${CURRENT_SCRIPT_PATH}"
        return 1
    fi
    run_container ${BKPPROG_IMAGE_NAME}

    copy_files_to_docker_bkpprogrammer
    run_build_bkpprogrammer
    copy_files_from_docker_bkpprogrammer

    clean_container
    echo "Done"
}

function build_docker() { #FCSSERVER_IMAGE_NAME ${FCS_PATH}/docker/
    local image_name=$1
    if [[ -z "$(docker images ${image_name} | grep ${image_name})" ]]; then
        echo "--- Build docker image ${image_name} ---"
        case $(docker build -t ${image_name} -f Dockerfile . 2>&1) in
        *"failed to solve: failed to read dockerfile: failed to mount"*)
            return 2
            ;;
        *)
            if [[ $? != 0 ]]; then
                echo "Building docker failed!"
                return 1
            fi
            ;;
        esac
    fi
}

function run_container() {
    local image_name=$1
    echo "--- Run docker container ---"
    CONTAINER_ID=$(docker run --interactive --detach ${image_name}) || return 1
}

function clean_container() {
    echo "--- Clean docker container ---"
    docker stop ${CONTAINER_ID}
    docker rm ${CONTAINER_ID} || return
}

function copy_files_to_docker_fcs() {
    echo "--- Copy files to docker ---"
    docker exec --interactive ${CONTAINER_ID} mkdir -p FCSFilter
    docker exec --interactive ${CONTAINER_ID} mkdir -p FCSServer
    docker cp ${FCS_PATH}/FCSFilter/include/. ${CONTAINER_ID}:${FCS_DOCKER_BUILD_DIR}/FCSFilter/include
    docker cp ${FCS_PATH}/FCSFilter/src/. ${CONTAINER_ID}:${FCS_DOCKER_BUILD_DIR}/FCSFilter/src
    docker cp ${FCS_PATH}/FCSServer/src/. ${CONTAINER_ID}:${FCS_DOCKER_BUILD_DIR}/FCSServer/src
    docker cp ${FCS_PATH}/FCSFilter/test/. ${CONTAINER_ID}:${FCS_DOCKER_BUILD_DIR}/FCSFilter/test
    docker cp ${FCS_PATH}/Makefile ${CONTAINER_ID}:${FCS_DOCKER_BUILD_DIR}
    docker cp ${FCS_PATH}/build_gtest.sh ${CONTAINER_ID}:${FCS_DOCKER_BUILD_DIR}
    docker cp ${FCS_PATH}/FCSServer/install.sh ${CONTAINER_ID}:${FCS_DOCKER_BUILD_DIR}/FCSServer
    docker cp ${FCS_PATH}/FCSServer/fcsServer.service ${CONTAINER_ID}:${FCS_DOCKER_BUILD_DIR}/FCSServer
}

function copy_files_to_docker_bkpprogrammer() {
    echo "--- Copy files to docker ---"
    docker cp ${BKPPROGRAMMER_PATH}/src/. ${CONTAINER_ID}:${BKPPROG_DOCKER_BUILD_DIR}/src
    docker cp ${BKPPROGRAMMER_PATH}/CMakeLists.txt ${CONTAINER_ID}:${BKPPROG_DOCKER_BUILD_DIR}
    docker cp ${CURRENT_SCRIPT_PATH}/build-dependencies.sh ${CONTAINER_ID}:/
    docker cp ${FCS_PATH}/. ${CONTAINER_ID}:/FCS
}

function run_build_fcs() {
    echo "--- Make FCSServer ---"
    docker exec --interactive ${CONTAINER_ID} make build || return 1
    docker exec --interactive ${CONTAINER_ID} make test || return 1

}

function run_build_bkpprogrammer() {
    docker exec --interactive ${CONTAINER_ID} ../build-dependencies.sh OPENSSL_AARCH64_VERSION=${OPENSSL_VERSION} BOOST_AARCH64_VERSION=${BOOST_VERSION} LIBCURL_AARCH64_VERSION=${LIBCURL_VERSION} || return 1
    docker exec --interactive ${CONTAINER_ID} bash -c "mkdir -p build/MinSizeRel && cd build/MinSizeRel && cmake -DHPS_BUILD:BOOL=ON ../.. && cmake --build ." || return 1
}

function copy_files_from_docker_fcs() {
    echo "--- Copy FCSServer artifacts from docker ---"
    rm -rf ./out_docker
    mkdir ./out_docker
    docker cp ${CONTAINER_ID}:${FCS_DOCKER_BUILD_DIR}/out/. ./out_docker/
}

function copy_files_from_docker_bkpprogrammer() {
    echo "--- Copy BKPProgrammer artifacts from docker ---"
    rm -rf ./out_docker
    mkdir ./out_docker
    docker cp ${CONTAINER_ID}:${BKPPROG_DOCKER_BUILD_DIR}/build/. ./out_docker/
    docker cp ${CONTAINER_ID}:/usr/aarch64-linux-gnu/lib/libstdc++.so.6.0.25 ./out_docker/
    docker cp ${CONTAINER_ID}:/usr/aarch64-linux-gnu/lib/libgcc_s.so.1 ./out_docker/
}

main() {
    mkdir -p ${OUT_PATH}

    install_package_if_does_not_exist tar
    install_package_if_does_not_exist wget
    install_package_if_does_not_exist cmake
    install_package_if_does_not_exist make
    install_package_if_does_not_exist build-essential
    install_package_if_does_not_exist curl

    ./build-dependencies.sh OPENSSL_VERSION=${OPENSSL_VERSION} LIBSPDM_VERSION=${LIBSPDM_VERSION} \
        BOOST_VERSION=${BOOST_VERSION} LIBCURL_VERSION=${LIBCURL_VERSION} \
        GTEST_VERSION=${GTEST_VERSION}

    copy_dependencies ${SPDM_WRAPPER_PATH} ${openssl_root_dir}
    copy_dependencies ${SPDM_WRAPPER_PATH} ${libspdm_root_dir}
    copy_dependencies ${BKPPROGRAMMER_PATH} ${libcurl_root_dir}
    copy_dependencies ${BKPPROGRAMMER_PATH} ${gtest_root_dir}
    copy_dependencies ${BKPPROGRAMMER_PATH} ${boost_root_dir}
    copy_dependencies ${BKPPROGRAMMER_PATH} ${openssl_root_dir}

    build_spdm_wrapper
    build_fcsserver
    build_bkpprogrammer

    check_java
    create_dummy_key_if_does_not_exist
    KEYSTORE_DUMMY_ALIAS=dummy ./gradlew clean build deploy
    cp ./workload/build/libs/* ${OUT_PATH}/
    cp ./bkps/build/libs/* ${OUT_PATH}/
    cp ./Verifier/build/libs/* ${OUT_PATH}/
    cp ./Verifier/src/main/resources/config.properties ${OUT_PATH}/
    printf "${LOG_OUTPUT}" "${OUT_PATH}"
    echo $(ls ${OUT_PATH})
}

main "$@"
