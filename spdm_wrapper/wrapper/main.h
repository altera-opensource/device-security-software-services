/*
 * This project is licensed as below.
 *
 * **************************************************************************
 *
 * Copyright 2020-2024 Intel Corporation. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * **************************************************************************
 *
 */

#ifndef SPDM_WRAPPER_MAIN_H
#define SPDM_WRAPPER_MAIN_H

#include <cstdlib>
#include <cstdio>
#include <cstring>

extern "C" {
#include "library/spdm_common_lib.h"
#include "library/spdm_requester_lib.h"
#include "library/spdm_transport_mctp_lib.h"
#include "hal/library/memlib.h"
};

const uint8_t MCTP_ALIGNMENT_LEN = 3;
const uint8_t MCTP_RESERVED = 0x0;
const uint8_t MCTP_INITIATOR_ID = 0x0;
const uint8_t MCTP_MSG_TAG = 0x0;
const uint8_t MCTP_TO = 0x1;
const uint8_t MCTP_RSVD = 0x1;
const uint8_t MCTP_ENCAPSULATION_TYPE_0 = 0x0;
const uint8_t MCTP_ENCAPSULATION_TYPE_1 = 0x1;
const uint8_t WORD_SIZE = 4;
const uint8_t SECURE_MCTP_MESSAGE_TYPE = 0x06;

typedef void (*print_callback)
        (const char *message);

typedef libspdm_return_t (*mctp_encode_callback)
        (void *spdm_context, const uint32_t *session_id, bool is_app_message,
         bool is_requester, size_t message_size, void *message,
         size_t *transport_message_size, void **transport_message);

typedef libspdm_return_t (*mctp_decode_callback)
        (void *spdm_context, uint32_t **session_id,
         bool *is_app_message, bool is_requester,
         size_t transport_message_size, void *transport_message,
         size_t *message_size, void **message);

typedef libspdm_return_t (*spdm_device_send_message_callback)
        (void *spdm_context, size_t request_size, const void *request, uint64_t timeout);

typedef libspdm_return_t (*spdm_device_receive_message_callback)
        (void *spdm_context, size_t *response_size, void **response, uint64_t timeout);

typedef libspdm_return_t (*spdm_device_acquire_sender_buffer_callback)
        (void *context, void **msg_buf_ptr);

typedef void (*spdm_device_release_sender_buffer_callback)
        (void *context, const void *msg_buf_ptr);

typedef libspdm_return_t (*spdm_device_acquire_receiver_buffer_callback)
        (void *context, void **msg_buf_ptr);

typedef void (*spdm_device_release_receiver_buffer_callback)
        (void *context, const void *msg_buf_ptr);

typedef bool (*spdm_requester_data_sign_callback)
        (spdm_version_number_t spdm_version,
         uint8_t op_code,
         uint16_t req_base_asym_alg,
         uint32_t base_hash_algo, bool is_data_hash,
         const uint8_t *message, size_t message_size,
         uint8_t *signature, size_t *sig_size);

typedef struct {
    print_callback printCallback;
    mctp_encode_callback mctpEncodeCallback;
    mctp_decode_callback mctpDecodeCallback;
    spdm_device_send_message_callback spdmDeviceSendMessageCallback;
    spdm_device_receive_message_callback spdmDeviceReceiveMessageCallback;
    spdm_device_acquire_sender_buffer_callback spdmDeviceAcquireSenderBufferCallback;
    spdm_device_release_sender_buffer_callback spdmDeviceReleaseSenderBufferCallback;
    spdm_device_acquire_receiver_buffer_callback spdmDeviceAcquireReceiverBufferCallback;
    spdm_device_release_receiver_buffer_callback spdmDeviceReleaseReceiverBufferCallback;
    spdm_requester_data_sign_callback spdmRequesterDataSignCallback;

} session_callbacks_t;

libspdm_return_t libspdm_transport_mctp_encode_message_w(
        void *spdm_context, const uint32_t *session_id, bool is_app_message,
        bool is_request_message, size_t message_size, void *message,
        size_t *transport_message_size, void **transport_message);

libspdm_return_t libspdm_transport_mctp_decode_message_w(
        void *spdm_context, uint32_t **session_id,
        bool *is_app_message, bool is_request_message,
        size_t transport_message_size, void *transport_message,
        size_t *message_size, void **message);

extern "C" {

bool libspdm_psk_handshake_secret_hkdf_expand(
        spdm_version_number_t spdm_version,
        uint32_t base_hash_algo,
        const uint8_t *psk_hint,
        size_t psk_hint_size,
        const uint8_t *info,
        size_t info_size,
        uint8_t *out, size_t out_size)
{
    printf("[ERROR] Called libspdm_psk_handshake_secret_hkdf_expand but it is not implemented.");
    return false;
}

bool libspdm_responder_data_sign(
        spdm_version_number_t spdm_version, uint8_t op_code,
        uint32_t base_asym_algo,
        uint32_t base_hash_algo, bool is_data_hash,
        const uint8_t *message, size_t message_size,
        uint8_t *signature, size_t *sig_size)
{
    printf("[ERROR] Called libspdm_responder_data_sign but it is not implemented.");
    return false;
}

bool libspdm_psk_master_secret_hkdf_expand(
        spdm_version_number_t spdm_version,
        uint32_t base_hash_algo,
        const uint8_t *psk_hint,
        size_t psk_hint_size,
        const uint8_t *info,
        size_t info_size, uint8_t *out,
        size_t out_size)
{
    printf("[ERROR] Called libspdm_psk_master_secret_hkdf_expand but it is not implemented.");
    return false;
}

bool libspdm_encap_challenge_opaque_data(
        spdm_version_number_t spdm_version,
        uint8_t slot_id,
        uint8_t *measurement_summary_hash,
        size_t measurement_summary_hash_size,
        void *opaque_data,
        size_t *opaque_data_size)
{
    printf("[ERROR] Called libspdm_encap_challenge_opaque_data but it is not implemented.");
    return false;
}

bool libspdm_requester_data_sign(
        spdm_version_number_t spdm_version, uint8_t op_code,
        uint16_t req_base_asym_alg,
        uint32_t base_hash_algo, bool is_data_hash,
        const uint8_t *message, size_t message_size,
        uint8_t *signature, size_t *sig_size);

void set_callbacks(session_callbacks_t *callbacks);

void libspdm_get_version_w(void *spdm_context_p,
                           uint8_t *version_p);

libspdm_return_t libspdm_set_data_w(void *spdm_context,
                                    libspdm_data_type_t data_type,
                                    const libspdm_data_parameter_t *parameter,
                                    void *data,
                                    size_t data_size);

size_t libspdm_get_context_size_w();

void libspdm_deinit_context_w(void *spdm_context);

bool libspdm_is_capabilities_flag_supported_by_responder(void *spdm_context,
                                                         uint32_t responder_capabilities_flag);

libspdm_return_t libspdm_prepare_context_w(void *spdm_context,
                                           uint32_t bufferSize);

size_t libspdm_get_sizeof_required_scratch_buffer_w(void *spdm_context);

void libspdm_set_scratch_buffer_w(void *spdm_context,
                                  void *scratch_buffer,
                                  size_t scratch_buffer_size);

libspdm_return_t libspdm_init_connection_w(void *spdm_context,
                                           bool get_version_only);

libspdm_return_t libspdm_get_digest_w(void *spdm_context,
                                      const uint32_t *session_id,
                                      uint8_t *slot_mask,
                                      void *total_digest_buffer);

libspdm_return_t libspdm_get_certificate_w(void *spdm_context,
                                           const uint32_t *session_id,
                                           uint8_t slot_id,
                                           size_t *cert_chain_size,
                                           void *cert_chain);

libspdm_return_t libspdm_get_measurement_w(void *spdm_context,
                                           const uint32_t *session_id,
                                           uint8_t request_attribute,
                                           uint8_t measurement_operation,
                                           uint8_t slot_id,
                                           uint8_t *content_changed,
                                           uint8_t *number_of_blocks,
                                           uint32_t *measurement_record_length,
                                           void *measurement_record);

libspdm_return_t libspdm_set_certificate_w(void *spdm_context,
                                           const uint32_t *session_id, uint8_t slot_id,
                                           void *cert_chain, size_t cert_chain_size);

libspdm_return_t libspdm_start_session_w(void *spdm_context,
                                         bool use_psk,
                                         const void *psk_hint,
                                         uint16_t psk_hint_size,
                                         uint8_t measurement_hash_type,
                                         uint8_t slot_id,
                                         uint8_t session_policy,
                                         uint32_t *session_id,
                                         uint8_t *heartbeat_period,
                                         void *measurement_hash);

libspdm_return_t libspdm_stop_session_w(void *spdm_context,
                                        uint32_t session_id,
                                        uint8_t end_session_attributes);

libspdm_return_t libspdm_send_receive_data_w(void *spdm_context,
                                             const uint32_t *session_id,
                                             bool is_app_message,
                                             const void *request,
                                             size_t request_size,
                                             void *response,
                                             size_t *response_size);
}

#endif //SPDM_WRAPPER_MAIN_H
