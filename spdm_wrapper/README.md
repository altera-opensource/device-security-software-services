## SPDM Wrapper

Wrapper library built with DMTF/libspdm that enables SPDM protocol.

### Building SPDM wrapper and its dependencies
#### Linux build
1. Download and build latest version of [OpenSSL](https://www.openssl.org/) as described in OpenSSL docs, e.g.:
    - Download and unpack OpenSSL 3.1.1
    - Run OpenSSL build
    ```shell script
    ./config shared -L-fPIC -L-g -L-O0 -fPIC -g -O0
    make -j $(nproc) --silent
    ```
2. Download and build [libspdm](https://github.com/DMTF/libspdm)
    - Clone the sources (version 3.1.0 should be used)
    - Update cmocka submodule
    - Create build dir and enter it
    - Run cmake. openssl_root_dir variable should be a path to openssl dir from previous step. e.g. /home/user/build/openssl
    - Run make. The build will fail on unit tests. This is expected with custom OpenSSL. The libraries will be built and should be present in libspdm/build/lib
    ```shell script
    git clone https://github.com/DMTF/libspdm.git
    cd libspdm/
    git fetch --all --tags
    git checkout tags/3.1.0 -b 3.1.0
    git submodule update --init -- unit_test/cmockalib/cmocka
    mkdir build
    cd build
    cmake -DARCH=x64 -DTOOLCHAIN=GCC -DTARGET=Release -DCRYPTO=openssl -DENABLE_BINARY_BUILD=1 -DCMAKE_C_FLAGS="-I${openssl_root_dir}/include -DLIBSPDM_MAX_CERT_CHAIN_BLOCK_LEN=15000 -DLIBSPDM_MAX_CERT_CHAIN_SIZE=18000 -DLIBSPDM_MAX_MEASUREMENT_RECORD_SIZE=15000" -DCOMPILED_LIBCRYPTO_PATH=${openssl_root_dir}/libcrypto.a -DCOMPILED_LIBSSL_PATH=${openssl_root_dir}/libssl.a ..
    make copy_sample_key
    make
    ```
3. Build SPDM Wrapper
    - Copy openssl and libspdm directories to "dependencies" dir inside spdm_wrapper dir. You can provide custom dependencies directory by calling Cmake in the next step with

   `-DDEPENDENCIES_PATH=/absolute/path/to/dependencies`

    ```
    spdm_wrapper/
    ├─ dependencies/
    │  ├─ libspdm/
    │  │  ├─ include/
    │  │  ├─ build/
    │  │  │  ├─ lib/
    │  │  │  │  ├─ lib*.a
    │  ├─ openssl/
    │  │  ├─ include/
    │  │  ├─ libcrypto.a
    │  │  ├─ libssl.a
    ```
   You can also use a different folder structure with custom paths to `lib/` and `include/` directories for libspdm and openssl. In such case use cmake variables:
    ```
   -DLIBSPDM_LIB_DIR=<path_to_libspdm_lib> -DOPENSSL_LIB_DIR=<path_to_openssl_lib> -DLIBSPDM_INCLUDE_DIR=<path_to_libspdm_include> -DOPENSSL_INCLUDE_DIR=<path_to_openssl_include>
   ```
    - Run build
    ```shell script
    mkdir -p build/Release
    cd build/Release
    cmake -DCMAKE_BUILD_TYPE=Release ../..
    make
    ```
#### Windows build
1. Download and build latest version of [OpenSSL](https://www.openssl.org/) as described in openssl/NOTES.WIN, e.g.:
    - Download and unpack OpenSSL 1.1.1t
    - Run OpenSSL build
    ```shell script
    perl.exe configure VC-WIN64A no-asm
    nmake
    ```
2. Download and build [libspdm] (https://github.com/DMTF/libspdm)
    - Build process is similar as in Linux. Only the cmake command is different:
    ```shell script
    cmake -DARCH=x64 -DTOOLCHAIN=VS2019 -DTARGET=Release -DCRYPTO=openssl -DENABLE_BINARY_BUILD=1 -DCMAKE_C_FLAGS="-I${openssl_root_dir}/include" -DCOMPILED_LIBCRYPTO_PATH=${openssl_root_dir}/libcrypto_static.lib -DCOMPILED_LIBSSL_PATH=${openssl_root_dir}/libssl_static.lib ..
    ```
3. Build SPDM Wrapper
    - Copy openssl and libspdm directories to "dependencies" dir inside spdm_wrapper dir
    ```
    spdm_wrapper/
    ├─ dependencies/
    │  ├─ libspdm/
    │  │  ├─ include/
    │  │  ├─ build/
    │  │  │  ├─ lib/
    │  │  │  │  ├─ *.lib
    │  ├─ openssl/
    │  │  ├─ include/
    │  │  ├─ libcrypto_static.lib
    │  │  ├─ libssl_static.lib
    ```
   You can also use a different folder structure with custom paths to `lib/` and `include/` directories for libspdm and openssl. In such case use cmake variables:
    ```
   -DLIBSPDM_LIB_DIR=<path_to_libspdm_lib> -DOPENSSL_LIB_DIR=<path_to_openssl_lib> -DLIBSPDM_INCLUDE_DIR=<path_to_libspdm_include> -DOPENSSL_INCLUDE_DIR=<path_to_openssl_include>
   ```
    - Run build
    ```shell script
    mkdir build
    cd build
    cmake -G "Visual Studio 15 2017 Win64" -T "v141" ..
    cmake --build . --config Release
    ```
