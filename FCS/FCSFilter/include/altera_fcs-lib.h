/*
This project, FPGA Crypto Service Server, is licensed as below

***************************************************************************

Copyright 2020-2025 Altera Corporation. All Rights Reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

***************************************************************************
*/
struct altera_fcs_dev {

    struct fcs_qspi_read {
        char* buffer;
    };

    struct fcs_chipid {
        uint32_t chip_id_lo;
        uint32_t chip_id_hi;
    };

    struct fcs_idcode {
        uint32_t idcode;
    };

    struct fcs_attestation_certificate {
        char*    cert;
        uint32_t cert_size;
    };

    struct fcs_mctp {
        char* mctp_resp;
        int   resp_len;
    };

    struct fcs_device_identity {
        char* dev_identity;
        int   dev_identity_length;
    };

    /* command parameters */
    union {
        struct fcs_qspi_read                c_qspi_read;
        struct fcs_chipid	                c_chipid;
        struct fcs_idcode	                c_idcode;
        struct fcs_attestation_certificate	c_attestation_certificate;
        struct fcs_mctp                     c_mctp;
        struct fcs_device_identity          c_device_identity;
    } com_paras;
};

