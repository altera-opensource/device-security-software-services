# BKP Programmer
This is a shared library intended to be part of a microservice architecture.

This library will expose an interface to enable communication between Quartus Programmer and BKP Service.

## Build in docker

To build BKPProgramer on docker, from docker folder run:
    ```bash
    ./build-ext.sh OPENSSL_AARCH64_VERSION="<openssl_version>" BOOST_AARCH64_VERSION="<boost_version>" LIBCURL_AARCH64_VERSION="<libcurl_version>"
    ```

## Development

To use shared library, compile your program and link with the library as in the example below:
1. Sample main.cpp:
    ```c++
    #include <iostream>
    #include "pgm_plugin_bkp_export.h"
    int main()
    {
        std::cout << get_version() << std::endl;
        return 0;
    }
    ```
1. Change name of the bkp_programmer library to libbkpprog.so and place it in your libraries folder:
1. Compile:
    ```bash
    g++ -Wall -I /path/to/header/files -L /path/to/libraries/folder main.cpp -lbkpprog -o program
    ```
1. If needed, add path to your libraries folder:
    ```bash
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/libraries/folder
    ```
1. Run:
    ```bash
    ./program
    ```
