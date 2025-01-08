/*
 * This project is licensed as below.
 *
 * **************************************************************************
 *
 * Copyright 2020-2025 Altera Corporation. All Rights Reserved.
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

#include "main.h"

session_callbacks_t *cb;

bool m_send_receive_buffer_acquired = false;
uint8_t m_send_receive_buffer[20064];
size_t m_send_receive_buffer_size = 20064;

libspdm_return_t spdm_device_acquire_sender_buffer(
        void *context, void **msg_buf_ptr) {
    if (m_send_receive_buffer_acquired) {
        return LIBSPDM_STATUS_ACQUIRE_FAIL;
    }
    *msg_buf_ptr = m_send_receive_buffer;
    libspdm_zero_mem(m_send_receive_buffer, m_send_receive_buffer_size);
    m_send_receive_buffer_acquired = true;
    return LIBSPDM_STATUS_SUCCESS;
}

void spdm_device_release_sender_buffer(
        void *context, const void *msg_buf_ptr) {
    m_send_receive_buffer_acquired = false;
}

libspdm_return_t spdm_device_acquire_receiver_buffer(
        void *context, void **msg_buf_ptr) {
    if (m_send_receive_buffer_acquired) {
        return LIBSPDM_STATUS_ACQUIRE_FAIL;
    }
    *msg_buf_ptr = m_send_receive_buffer;
    libspdm_zero_mem(m_send_receive_buffer, m_send_receive_buffer_size);
    m_send_receive_buffer_acquired = true;
    return LIBSPDM_STATUS_SUCCESS;
}

void spdm_device_release_receiver_buffer(
        void *context, const void *msg_buf_ptr) {
    m_send_receive_buffer_acquired = false;
}

bool libspdm_requester_data_sign(
        spdm_version_number_t spdm_version, uint8_t op_code,
        uint16_t req_base_asym_alg,
        uint32_t base_hash_algo, bool is_data_hash,
        const uint8_t *message, size_t message_size,
        uint8_t *signature, size_t *sig_size) {
    cb->printCallback("Called libspdm_requester_data_sign.");

    return cb->spdmRequesterDataSignCallback != nullptr &&
           cb->spdmRequesterDataSignCallback(spdm_version, op_code, req_base_asym_alg, base_hash_algo, is_data_hash,
                                             message, message_size, signature, sig_size);
}

libspdm_return_t libspdm_transport_mctp_encode_message_w(
        void *spdm_context, const uint32_t *session_id, bool is_app_message,
        bool is_request_message, size_t message_size, void *message,
        size_t *transport_message_size, void **transport_message) {
    cb->printCallback("Called libspdm_transport_mctp_encode_message_w.");

    libspdm_return_t encode_result = libspdm_transport_mctp_encode_message(spdm_context, session_id, is_app_message,
                                                                           is_request_message, message_size,
                                                                           message, transport_message_size,
                                                                           transport_message);

    if (encode_result != LIBSPDM_STATUS_SUCCESS || *transport_message_size == 0) {
        cb->printCallback("MCTP encode failed.");
        return encode_result;
    }

    uint8_t **tmp_buffer = ((uint8_t **) transport_message);
    uint8_t *tmp_buffer_with_mctp_header = *tmp_buffer - MCTP_ALIGNMENT_LEN;

    tmp_buffer_with_mctp_header[0] = MCTP_RESERVED;
    tmp_buffer_with_mctp_header[1] = MCTP_INITIATOR_ID;

    uint8_t mctp_encapsulation_type = tmp_buffer_with_mctp_header[3] == SECURE_MCTP_MESSAGE_TYPE
                                      ? MCTP_ENCAPSULATION_TYPE_0
                                      : (cb->mctpEncapsulationTypeCallback != nullptr)
                                        ? cb->mctpEncapsulationTypeCallback()
                                        : MCTP_ENCAPSULATION_TYPE_1;
    tmp_buffer_with_mctp_header[2] = MCTP_MSG_TAG << 5 | MCTP_TO << 4 | MCTP_RSVD << 1 | mctp_encapsulation_type;

    size_t new_size = *transport_message_size + MCTP_ALIGNMENT_LEN;
    size_t padding_len = new_size % WORD_SIZE == 0 ? 0 : WORD_SIZE - (new_size % WORD_SIZE);
    libspdm_zero_mem(tmp_buffer_with_mctp_header + new_size, padding_len);

    *transport_message = tmp_buffer_with_mctp_header;
    *transport_message_size = new_size + padding_len;

    return encode_result;
}

libspdm_return_t libspdm_transport_mctp_decode_message_w(
        void *spdm_context, uint32_t **session_id,
        bool *is_app_message, bool is_request_message,
        size_t transport_message_size, void *transport_message,
        size_t *message_size, void **message) {
    cb->printCallback("Called libspdm_transport_mctp_decode_message_w.");

    if (transport_message_size < MCTP_ALIGNMENT_LEN) {
        cb->printCallback("MCTP decode failed.");
        return LIBSPDM_STATUS_INVALID_PARAMETER;
    }

    transport_message_size -= MCTP_ALIGNMENT_LEN;
    transport_message = ((uint8_t *) transport_message) + MCTP_ALIGNMENT_LEN;
    return libspdm_transport_mctp_decode_message(spdm_context, session_id, is_app_message,
                                                 is_request_message, transport_message_size,
                                                 transport_message, message_size,
                                                 message);
}

void set_callbacks(session_callbacks_t *callbacks) {
    cb = callbacks;
}

bool verify_spdm_cert_chain_func(void *spdm_context,
                                 uint8_t slot_id,
                                 size_t cert_chain_size,
                                 const void *cert_chain,
                                 const void **trust_anchor,
                                 size_t *trust_anchor_size) {
    // default verification fails on parsing DICE structures; the certificate chain is later fully verified in Java
    return true;
}

void libspdm_get_version_w(void *spdm_context, uint8_t *version_p) {
    cb->printCallback("Called libspdm_get_version_w.");

    spdm_version_number_t spdm_version_number_entry;
    libspdm_data_parameter_t parameter;
    parameter.location = LIBSPDM_DATA_LOCATION_CONNECTION;
    size_t data_size = sizeof(spdm_version_number_entry);
    libspdm_get_data(spdm_context, LIBSPDM_DATA_SPDM_VERSION, &parameter,
                     &spdm_version_number_entry, &data_size);

    // We are only interested in [15:12] MajorVersion [11:8] MinorVersion part of VersionNumberEntry
    *version_p = spdm_version_number_entry >> SPDM_VERSION_NUMBER_SHIFT_BIT;
}

libspdm_return_t libspdm_set_data_w(void *spdm_context,
                                    libspdm_data_type_t data_type,
                                    const libspdm_data_parameter_t *parameter,
                                    void *data,
                                    size_t data_size) {
    return libspdm_set_data(spdm_context, data_type, parameter, data, data_size);
}

size_t libspdm_get_context_size_w() {
    return libspdm_get_context_size();
}

void libspdm_deinit_context_w(void *spdm_context) {
    cb->printCallback("Called libspdm_deinit_context_w.");
    libspdm_deinit_context(spdm_context);
}

bool libspdm_is_capabilities_flag_supported_by_responder(void *spdm_context,
                                                         uint32_t responder_capabilities_flag) {
    cb->printCallback("Called libspdm_is_capabilities_flag_supported_by_responder.");

    libspdm_data_parameter_t parameter;
    parameter.location = LIBSPDM_DATA_LOCATION_CONNECTION;

    size_t data_size = sizeof(uint32_t);
    uint32_t negotiated_responder_capabilities_flag;

    libspdm_return_t status = libspdm_get_data(spdm_context, LIBSPDM_DATA_CAPABILITY_FLAGS, &parameter,
                                               &negotiated_responder_capabilities_flag, &data_size);

    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        return false;
    }

    if (data_size != sizeof(uint32_t)) {
        cb->printCallback("Returned data_size is invalid.");
        return false;
    }

    return (responder_capabilities_flag == 0)
           || ((negotiated_responder_capabilities_flag & responder_capabilities_flag) != 0);
}

libspdm_return_t libspdm_prepare_context_w(void *spdm_context,
                                           uint32_t bufferSize) {
    uint32_t senderBufferSize = bufferSize;
    uint32_t receiverBufferSize = bufferSize;
    uint32_t maxSpdmMessageSize = bufferSize
                                  - LIBSPDM_MCTP_TRANSPORT_HEADER_SIZE - MCTP_ALIGNMENT_LEN -
                                  LIBSPDM_MCTP_TRANSPORT_TAIL_SIZE;

    libspdm_return_t status = libspdm_init_context(spdm_context);

    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        return status;
    }

    libspdm_register_verify_spdm_cert_chain_func(spdm_context, verify_spdm_cert_chain_func);

    libspdm_register_device_buffer_func(spdm_context,
                                        senderBufferSize,
                                        receiverBufferSize,
                                        cb->spdmDeviceAcquireSenderBufferCallback != nullptr
                                        ? cb->spdmDeviceAcquireSenderBufferCallback
                                        : spdm_device_acquire_sender_buffer,
                                        cb->spdmDeviceReleaseSenderBufferCallback != nullptr
                                        ? cb->spdmDeviceReleaseSenderBufferCallback
                                        : spdm_device_release_sender_buffer,
                                        cb->spdmDeviceAcquireReceiverBufferCallback != nullptr
                                        ? cb->spdmDeviceAcquireReceiverBufferCallback
                                        : spdm_device_acquire_receiver_buffer,
                                        cb->spdmDeviceReleaseReceiverBufferCallback != nullptr
                                        ? cb->spdmDeviceReleaseReceiverBufferCallback
                                        : spdm_device_release_receiver_buffer);

    libspdm_register_device_io_func(spdm_context, cb->spdmDeviceSendMessageCallback,
                                    cb->spdmDeviceReceiveMessageCallback);

    libspdm_register_transport_layer_func(spdm_context,
                                          maxSpdmMessageSize,
                                          LIBSPDM_MCTP_TRANSPORT_HEADER_SIZE + MCTP_ALIGNMENT_LEN,
                                          LIBSPDM_MCTP_TRANSPORT_TAIL_SIZE,
                                          cb->mctpEncodeCallback != nullptr
                                          ? cb->mctpEncodeCallback
                                          : libspdm_transport_mctp_encode_message_w,
                                          cb->mctpDecodeCallback != nullptr
                                          ? cb->mctpDecodeCallback
                                          : libspdm_transport_mctp_decode_message_w);
    return LIBSPDM_STATUS_SUCCESS;
}

size_t libspdm_get_sizeof_required_scratch_buffer_w(void *spdm_context) {
    return libspdm_get_sizeof_required_scratch_buffer(spdm_context);
}

void libspdm_set_scratch_buffer_w(void *spdm_context,
                                  void *scratch_buffer,
                                  size_t scratch_buffer_size) {
    libspdm_set_scratch_buffer(spdm_context, scratch_buffer, scratch_buffer_size);
}

libspdm_return_t libspdm_init_connection_w(void *spdm_context,
                                           bool get_version_only) {
    cb->printCallback("Called libspdm_init_connection_w.");
    return libspdm_init_connection(spdm_context, get_version_only);
}

libspdm_return_t libspdm_get_digest_w(void *spdm_context,
                                      const uint32_t *session_id,
                                      uint8_t *slot_mask,
                                      void *total_digest_buffer) {
    cb->printCallback("Called libspdm_get_digest_w.");
    return libspdm_get_digest(spdm_context, session_id, slot_mask, total_digest_buffer);
}

libspdm_return_t libspdm_get_certificate_w(void *spdm_context,
                                           const uint32_t *session_id,
                                           uint8_t slot_id,
                                           size_t *cert_chain_size,
                                           void *cert_chain) {
    cb->printCallback("Called libspdm_get_certificate_w.");
    return libspdm_get_certificate(spdm_context, session_id, slot_id, cert_chain_size, cert_chain);
}

libspdm_return_t libspdm_get_measurement_w(void *spdm_context,
                                           const uint32_t *session_id,
                                           uint8_t request_attribute,
                                           uint8_t measurement_operation,
                                           uint8_t slot_id,
                                           uint8_t *content_changed,
                                           uint8_t *number_of_blocks,
                                           uint32_t *measurement_record_length,
                                           void *measurement_record) {
    cb->printCallback("Called libspdm_get_measurement_w.");
    return libspdm_get_measurement(spdm_context, session_id, request_attribute, measurement_operation,
                                   slot_id, content_changed, number_of_blocks, measurement_record_length,
                                   measurement_record);
}

libspdm_return_t libspdm_set_certificate_w(void *spdm_context,
                                           const uint32_t *session_id,
                                           uint8_t slot_id,
                                           void *cert_chain,
                                           size_t cert_chain_size) {
    cb->printCallback("Called libspdm_set_certificate_w.");
    return libspdm_set_certificate(spdm_context, session_id, slot_id, cert_chain, cert_chain_size);
}

libspdm_return_t libspdm_start_session_w(void *spdm_context,
                                         bool use_psk,
                                         const void *psk_hint,
                                         uint16_t psk_hint_size,
                                         uint8_t measurement_hash_type,
                                         uint8_t slot_id,
                                         uint8_t session_policy,
                                         uint32_t *session_id,
                                         uint8_t *heartbeat_period,
                                         void *measurement_hash) {
    cb->printCallback("Called libspdm_start_session_w.");
    return libspdm_start_session(spdm_context, use_psk, psk_hint, psk_hint_size, measurement_hash_type,
                                 slot_id, session_policy, session_id, heartbeat_period, measurement_hash);
}

libspdm_return_t libspdm_stop_session_w(void *spdm_context,
                                        uint32_t session_id,
                                        uint8_t end_session_attributes) {
    cb->printCallback("Called libspdm_stop_session_w.");
    return libspdm_stop_session(spdm_context, session_id, end_session_attributes);
}

libspdm_return_t libspdm_send_receive_data_w(void *spdm_context,
                                             const uint32_t *session_id,
                                             bool is_app_message,
                                             const void *request,
                                             size_t request_size,
                                             void *response,
                                             size_t *response_size) {
    cb->printCallback("Called libspdm_send_receive_data_w.");
    return libspdm_send_receive_data(spdm_context, session_id, is_app_message, request,
                                     request_size, response, response_size);
}
