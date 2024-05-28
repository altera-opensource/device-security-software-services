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
| 24.1              | 5/28/2024  | BKPS + bkpprogrammer opensourced <br/> force_enrollment enabled in BKPProgrammer and BKP App                                                                                                                                                                                             


