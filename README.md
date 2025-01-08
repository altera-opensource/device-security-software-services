# How to build
## Linux Build
User can run build on ubuntu machine using command:

`./build_ubuntu.sh`

Prerequisites: user in a 'sudo' group.
To update version of project dependencies, change version numbers at the top of build_ubuntu.sh
Otherwise, default versions will be used.
Produced binaries can be found in ./out folder.

## Windows Build
### Prerequisites
1. Install Visual Studio 2017
2. Install Java OpenJDK 17
3. Ensure "JAVA_HOME" environment variable is set to <OpenJDK_installation_path>
4. Update Visual Studio installation path in build-dependencies.bat file

### 
User can run build on Windows machine using command:

`./build-dependencies.bat`

To update version of project dependencies, change version numbers in config.txt.
Otherwise, default versions will be used.
Refer to below table for more details on currently supported dependencies for build-dependencies.bat.

| Name           | Supported Value | Download Source |
|:------------------|:-----------|:------------------------------------------------------------------------------------------------------------------------------|
| always_build              | 1 |
| openssl.version              | 3.1.4 | https://github.com/openssl/openssl.git
| libspdm.version              | 3.2.0 | https://github.com/DMTF/libspdm.git
| libcurl.version              | 8.5.0 | https://curl.se
| boost.version              | 1.84.0 | https://github.com/boostorg/boost/releases
| gtest.version              | 1.14.0 | https://github.com/google/googletest.git

Produced binaries can be found in ./out_windows folder.

## Manual build

To build each component manually, refer to README files:
- [BKPS](./bkps/README.md)
- [SPDM Wrapper](./spdm_wrapper/README.md)
- [Verifier](./Verifier/README.md)
- [FCS Server](./FCS/README.md)
- [BKPProgrammer](./bkpprogrammer/README.md)

## Release notes

| Version | Date      | Release note                                                                                                                                                                                                                                                                             |
|:--------|:----------|:-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| 24.3.1  | 12/2/2024 | BKPS + bkpprogrammer + BKP App opensourced                                                                                                                                                                                             
