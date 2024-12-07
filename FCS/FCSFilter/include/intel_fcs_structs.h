/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2020, Intel Corporation
 */

#ifndef __INTEL_FCS_STRUCTS_H
#define __INTEL_FCS_STRUCTS_H

#include <stdint.h>

#define FCS_IMAGE_HPS_VAB	0
#define FCS_IMAGE_COUNTER_SET	1
#define FCS_IMAGE_KEY_CANCEL	2

#define SDM_CERT_MAGIC_NUM	0x25D04E7F

#define SHA256_SZ		32
#define SHA384_SZ		48
#define FCS_MAX_COUNTERS	5
#define VAB_DATA_SZ		64

#define FCS_USER_KEY		0
#define FCS_INTEL_KEY		1

#define FCS_INCREMENT_COUNTER	0
#define FCS_BASE_COUNTER	1

#define SDOS_PLAINDATA_MIN_SZ	32
#define SDOS_PLAINDATA_MAX_SZ	32672

#define SDOS_HEADER_SZ		40
#define SDOS_HMAC_SZ		48

#define SDOS_DECRYPTED_MIN_SZ	(SDOS_PLAINDATA_MIN_SZ + SDOS_HEADER_SZ)
#define SDOS_DECRYPTED_MAX_SZ	(SDOS_PLAINDATA_MAX_SZ + SDOS_HEADER_SZ)

#define SDOS_ENCRYPTED_MIN_SZ	(SDOS_PLAINDATA_MIN_SZ + SDOS_HEADER_SZ + SDOS_HMAC_SZ)
#define SDOS_ENCRYPTED_MAX_SZ	(SDOS_PLAINDATA_MAX_SZ + SDOS_HEADER_SZ + SDOS_HMAC_SZ)

#define ATTESTATION_SUBKEY_CMD_MAX_SZ		4092	/* 4K - 4 bytes */
#define ATTESTATION_SUBKEY_RSP_MAX_SZ		820	/* 205 words */
#define ATTESTATION_MEASUREMENT_CMD_MAX_SZ	4092	/* 4K - 4 bytes */
#define ATTESTATION_MEASUREMENT_RSP_MAX_SZ	4092	/* 4K - 4 bytes */
#define ATTESTATION_CERTIFICATE_RSP_MAX_SZ	4096	/* 1024 words */

#define CRYPTO_EXPORTED_KEY_OBJECT_MAX_SZ	364	/* 91 (88+3 header) words */
#define CRYPTO_GET_KEY_INFO_MAX_SZ		144	/* 36 words w/ header */

#define AES_CRYPT_CMD_MAX_SZ			4194304	/* set MAX size as 4 Mb for now */

#define RANDOM_NUMBER_EXT_MAX_SZ		4080

#define MBOX_SEND_CMD_MAX_SZ	4092	/* 4K - 4 bytes. 4 bytes reserved for header*/
#define MBOX_SEND_RSP_MAX_SZ	4092	/* 4K - 4 bytes. 4 bytes reserved for header */

/*
 * struct fcs_hps_generic_header
 * @cert_magic_num: Certificate Magic Word (0x25D04E7F)
 * @cert_data_sz: size of this certificate header (0x80)
 *	Includes magic number all the way to the certificate
 *      signing keychain (excludes cert. signing keychain)
 * @cert_ver: Certificate Version
 * @cert_type: Certificate Type
 * @data: 64 bytes of data.
 */
struct fcs_hps_generic_header {
	uint32_t cert_magic_num;		/* offset 0 */
	uint32_t cert_data_sz;
	uint32_t cert_ver;
	uint32_t cert_type;
	uint8_t  fcs_data[VAB_DATA_SZ];		/* offset 0x20 */
	/* keychain starts at offset 0x50 */
};

/*
 * struct fcs_hps_vab_certificate_data
 * @flags: Reserved for future use.
 * @fcs_data: Data words being certificate signed.
 * @cert_sign_keychain: Certificate Signing Keychain
 */
struct fcs_hps_vab_certificate_data {
	uint32_t rsvd0_0;			/* offset 0x10 */
	uint32_t flags;
	uint8_t  rsvd0_1[8];
	uint8_t  fcs_sha384[SHA384_SZ];		/* offset 0x20 */
};

/*
 * struct fcs_hps_vab_certificate_header
 * @cert_magic_num: Certificate Magic Word (0x25D04E7F)
 * @cert_data_sz: size of this certificate header (0x80)
 *	Includes magic number all the way to the certificate
 *      signing keychain (excludes cert. signing keychain)
 * @cert_ver: Certificate Version
 * @cert_type: Certificate Type
 * @data: VAB HPS Image Certificate data
 */
struct fcs_hps_vab_certificate_header {
	uint32_t cert_magic_num;		/* offset 0 */
	uint32_t cert_data_sz;
	uint32_t cert_ver;
	uint32_t cert_type;
	struct fcs_hps_vab_certificate_data d;	/* offset 0x20 */
	/* keychain starts at offset 0x50 */
};

/*
 * struct fcs_counter_set_flags - Flags for counter set.
 * @test: Flag to indicate don't write fuses, write to cache only
 * @rsvd: Reserved (write as 0)
 */
struct fcs_counter_set_flags {
	uint32_t  rsvd;
};

/*
 * struct fcs_counter_sel - structure of the counter_select
 * @rsvd: Reserved (write as 0)
 * @counter_type: Select the counter type
 *	0 = Explicit Key cancellation
 *	1-5 = Valid counter select are from 1 to 5
 *	255 = Cancel Owner Root Hash (255)
 * @subcounter_type: Select the subcounter type/Key Cancel
 *	When counter_type == 0:
 *		0 = User explicit key cancel (0)
 *		1 = Intel explicit key cancel (1)
 *	When counter_type == 1:
 *		0 = Select Incremental Portion of counter
 *		1 = Select the Base counter
 */
struct fcs_counter_sel {
#ifdef LITTLE_ENDIAN
	uint32_t  counter_type:8;
	uint32_t  subcounter_type:8;
	uint32_t  rsvd:16;
#else
	uint32_t  rsvd:16;
	uint32_t  subcounter_type:8;
	uint32_t  counter_type:8;
#endif
};

/*
 * struct fcs_counter_set_data
 * @flags:
 *	Bit 31 - Test Flag. When set, do not write fuses.
 *		 write to cache only.
 *	Bits 30:0 - Reserved (write as 0)
 * @select: See structure above.
 * @fcs_counter_value:
 *	If select.counter_type is 0, this is for explicit user cancellation.
 *	   The value range shall be from 0 to 31.
 *	If select.counter_type is 1 and select.sub_counter_type == 1, this
 *	   is the base counter. The value range is from 0 to 255.
 *	If select.counter_type is 1 and select.sub_counter_type == 0, this
 *	   is the incremental counter. The value range is from 0 to 494.
 *	If counter sel is 2 to 5, these are the small counters
 *	   The value range is from 0 to 63.
 * @user_root_hash: if select.counter_type is 0, this is the root hash.
 *		    otherwise, reserved (write as 0)
 * @cert_sign_keychain: Certificate Signing Keychain
 */
struct fcs_counter_set_data {
	uint32_t rsvd0_0;			/* offset 0x10 */
	uint32_t flags;
	struct fcs_counter_sel select;		/* offset 0x18 */
	uint32_t fcs_counter_value;		/* offset 0x1C */
	uint8_t  root_hash[SHA384_SZ];		/* offset 0x20 */
};

/*
 * struct fcs_counter_set_certificate_header
 * @cert_magic_num: Certificate Magic Word (0x25D04E7F)
 * @cert_data_sz: size of this certificate header (0x80)
 *	Includes magic number all the way to the certificate
 *      signing keychain (excludes cert. signing keychain)
 * @cert_ver: Certificate Version
 * @cert_type: Certificate Type
 * @data: VAB Counter Set Certificate data
 */
struct fcs_counter_set_certificate_header {
	uint32_t cert_magic_num;		/* offset 0 */
	uint32_t cert_data_sz;
	uint32_t cert_ver;
	uint32_t cert_type;
	struct fcs_counter_set_data data;	/* offset 0x20 */
	/* keychain starts at offset 0x50 */
};

/*
 * struct fcs_counter - structure of the counters
 * @counter_sel: Selects the counter to set (from 1 to 5)
 *             value = 1 selects the big counter
 *             value = 2 to 5 selects the SVN counters
 * @counter_val: Value to set the counter
 */
struct fcs_counter {
#ifdef LITTLE_ENDIAN
	uint32_t  counter_val:24;
	uint32_t  counter_sel:8;
#else
	uint32_t  counter_sel:8;
	uint32_t  counter_val:24;
#endif
};

/*
 * struct fcs_hash_256 - structure of vab hash256
 * @owner_root_hash: value of owner root hash
 * @cancel_status: cancellation status. (each bit for 1 key)
 */
struct fcs_hash_256 {
	uint8_t  owner_root_hash[SHA256_SZ];
	uint32_t  cancel_status;
};

/*
 * struct fcs_hash_384 - structure of vab hash384
 * @owner_root_hash: value of owner root hash
 * @cancel_status: cancellation status (each bit for 1 key)
 */
struct fcs_hash_384 {
	uint8_t  owner_root_hash[SHA384_SZ];
	uint32_t  cancel_status;
};

/*
 * enum fcs_hash_type - enumeration of hash types
 * @INTEL_FCS_HASH_SECP256: Hash type is SHA256
 * @INTEL_FCS_HASH_SECP384R1: Hash type is SHA384
 */
enum fcs_hash_type {
	INTEL_FCS_HASH_SECP256 = 1,
	INTEL_FCS_HASH_SECP384R1 = 2
};

/*
 * struct fcs_get_provision_header - Header of provision data
 * @provision_status: 0 = no provision done, 1 = sucessful provision
 *		      2 = provision error
 * @intel_key_status: 0 = No cancellation, 1 = cancelled
 * @test: Flag. when set don't write fuses, write to cache only
 * @co_sign_status: 0 = Not co-signed, 1 = co-signed
 * @root_hash_status: 0 = No cancellation, 1 = cancelled
 *	Bit 0 for root hash 0
 *	Bit 1 for root hash 1
 *	Bit 2 for root hash 2
 * @num_hashes: value from 1 to 3.
 * @type_hash: 1 = secp256; 2=secp384r1 (corresponds to command)
 */
struct fcs_get_provision_header {
#ifdef LITTLE_ENDIAN
	uint32_t  provision_status;
	uint32_t  intel_key_status;
	uint32_t  type_hash:8;
	uint32_t  num_hashes:8;
	uint32_t  root_hash_status:3;
	uint32_t  co_sign_status:1;
	uint32_t  rsvd:12;
#else
	uint32_t  provision_status;
	uint32_t  intel_key_status;
	uint32_t  rsvd:12;
	uint32_t  co_sign_status:1;
	uint32_t  root_hash_status:3;
	uint32_t  num_hashes:8;
	uint32_t  type_hash:8;
#endif
};

/*
 * struct fcs_get_counters_data - counter data
 * @big_cntr_base_value
 * @big_cntr_count_value
 * @svn_count_val3
 * @svn_count_val2
 * @svn_count_val1
 * @svn_count_val0
 */
struct fcs_get_counters_data {
#ifdef LITTLE_ENDIAN
	uint32_t  big_cntr_count_value:24;
	uint32_t  big_cntr_base_value:8;
	uint8_t   svn_count_val0;
	uint8_t   svn_count_val1;
	uint8_t   svn_count_val2;
	uint8_t   svn_count_val3;
#else
	uint32_t  big_cntr_base_value:8;
	uint32_t  big_cntr_count_value:24;
	uint8_t   svn_count_val3;
	uint8_t   svn_count_val2;
	uint8_t   svn_count_val1;
	uint8_t   svn_count_val0;
#endif
};

/*
 * struct fcs_get_provision_data - result of get_provision_data command
 * @header: header data.
 * the hash are different sizes and will depend on the header.type_hash value.
 * @hash_256: hash256 array (can be 1 to 3 elements)
 * @hash_384: hash384 array (can be 1 to 3 elements)
 * @counters: The data counters
 */
struct fcs_get_provision_data {
	struct fcs_get_provision_header header;
	/* Depends on whether type hash is 256 or 384 */
	union {
		struct fcs_hash_256 hash_256[3];	/* May be 1 to 3 */
		struct fcs_hash_384 hash_384[3];	/* May be 1 to 3 */
	};
	struct fcs_get_counters_data counters;
};

/*
 * struct fcs_aes_crypt_header
 * @magic_number: different for input or output buffer
 *	input = 0xACBDBDED
 *	output = 0x53424112
 * @data_len: length of the data to encrypt/decrypt
 * @pad: length of padding in bytes
 * @srk_indx: service root key index has the value 0
 * @app_spec_obj_info: Application Specific Object Info
 * @owner_id: Used for key derivation
 * @hdr_pad: Header Padding: 0x01020304
 * @iv_field: output data to store the generated IV
 */
struct fcs_aes_crypt_header {
	uint32_t  magic_number;
	uint32_t  data_len;
	uint8_t   pad;
	uint8_t   srk_indx;
	uint16_t  app_spec_obj_info;
	uint8_t   owner_id[8];
	uint32_t  hdr_pad;
	uint8_t  iv_field[16];
};

#endif

