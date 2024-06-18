# How to build

User can run build on ubuntu machine using command:

`./build_ubuntu.sh`

Prerequisites: user in a 'sudo' group.
To update version of project dependencies, change version numbers at the top of build_ubuntu.sh
Otherwise, default versions will be used.
Produced binaries can be found in ./out folder.

## Manual build

To build each component manually, refer to README files:
- [BKPS](./bkps/README.md)
- [SPDM Wrapper](./spdm_wrapper/README.md)
- [Verifier](./Verifier/README.md)
- [FCS Server](./FCS/README.md)
- [BKPProgrammer](./bkpprogrammer/README.md)

## Release notes

| Version           | Date       | Release note                                                                                                                                                                                                                                                                             |
|:------------------|:-----------|:-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| 24.1              |            | BKPS + bkpprogrammer opensourced <br/> force_enrollment enabled in BKPProgrammer and BKP App                                                                                                                                                                                             |
| 23.4              | 01/19/2024 | SPDM based BKP enabled for Sundance Mesa <br/> New supported mailbox commands in FCS Filter (GET_DEVICE_IDENTITY, GET_IDCODE, QSPI_WRITE)                                                                                                                                                |
| 23.3              | 10/05/2023 | Verifier - Support for CoRIM and Design CoRIM <br/> BKPS - Device Onboarding support enabled (including Set Authority)                                                                                                                                                                   |
| previous versions |            | Both Sigma-based and SPDM-based device attestation. <br/>Communication with device through both Hard Processor System and System Console. Comparison between measurements from FPGA and reference integrity manifest (RIM). RIM certificate chain validation and signature verification. |


