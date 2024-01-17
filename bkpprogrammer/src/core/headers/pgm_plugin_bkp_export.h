/*
 * This project is licensed as below.
 *
 * **************************************************************************
 *
 * Copyright 2020-2023 Intel Corporation. All Rights Reserved.
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
 */

#ifndef INC_PGM_PLUGIN_BKP_EXPORT_H
#define INC_PGM_PLUGIN_BKP_EXPORT_H

#include <string>
#include <iostream>
#include <stdint.h>

/**
 * Platform PGM_PLUGIN_DLLEXPORT
 */
#if PORT==WINDOWS
    #define PGM_PLUGIN_DLLEXPORT __declspec (dllexport)
#else
    #if __GNUC__ == 4
        #if __GNUC_MINOR__ >= 5 && __GNUC_MINOR__ <= 8
            #define PGM_PLUGIN_DLLEXPORT __attribute__((visibility("default")))
        #else
            #define PGM_PLUGIN_DLLEXPORT  __attribute__((visibility("protected")))
        #endif
    #elif __GNUC__ >= 5
        #define PGM_PLUGIN_DLLEXPORT __attribute__((visibility("default")))
    #else
        #define PGM_PLUGIN_DLLEXPORT UNKNOWN
    #endif
#endif

/**
* Status codes
*/
typedef enum
{
    ST_OK,
    ST_GENERIC_ERROR
} Status_t;

/**
* Levels for log messages
*/
typedef enum
{
    L_DEBUG,
    L_INFO,
    L_WARNING,
    L_ERROR
} LogLevel_t;

typedef enum
{
    SEND_PACKET = 0,
    PUSH_WRAPPED_KEY = 1,
    PUSH_WRAPPED_KEY_USER_IID = 2,
    PUSH_WRAPPED_KEY_UDS_IID = 3,
    PUSH_HELPER_DATA_UDS_IID = 4,
    PUSH_HELPER_DATA_UDS_INTEL = 5,
} Message_t;

typedef enum
{
    UDS_IID = 0,
    UDS_INTEL = 1,
    UDS_EFUSE = 2,
    USER_IID = 3,
} PufType_t;

/**
 * Configuration keys
 */
static const std::string CFG_ID = "bkp_cfg_id";
static const std::string CFG_BKPSVC_IP = "bkp_ip";
static const std::string CFG_BKPSVC_PORT = "bkp_port";
static const std::string CFG_PROXY_ADDRESS = "bkp_proxy_address";
static const std::string CFG_PROXY_USER = "bkp_proxy_user";
static const std::string CFG_PROXY_PASS = "bkp_proxy_password";
static const std::string CFG_TLS_CA_CERT = "bkp_tls_ca_cert";
static const std::string CFG_TLS_PROG_CERT = "bkp_tls_prog_cert";
static const std::string CFG_TLS_PROG_KEY = "bkp_tls_prog_key";
static const std::string CFG_TLS_PROG_KEY_PASS = "bkp_tls_prog_key_pass";

/**
* Interface of the object passed by Quartus Programmer to BKP Programmer library.
*/
class QuartusContext {
public:

    virtual ~QuartusContext(){};

    /**
    * Log debuging message
    *
    * @param IN level - log level
    * @param IN msg - message to be logged
    *
    * @return None
    */
    virtual void log(LogLevel_t level, const std::string& msg) = 0;

    /**
    * Send a request to SDM and receive a response.
    *
    * @param IN messageType - identifier for message type defining Quartus behavior for this particular command;
    *       list of supported commands shall be retrieved with get_supported_command()
    * @param IN inBuf - buffer for the outgoing message (max 4kB)
    * @param IN inBufSize - size of the buffer for the outgoing message in 4-byte words (max 4kB)
    * @param OUT outBuf - buffer for the incoming response
    * @param outBufSize IN/OUT, size of the buffer for the incoming response in 4-byte words. On input it contains the size of the buffer.
    *        After receiving the response it contains the actual size of the response.
    *
    * @return S_OK on success, S_GENERIC_ERROR on other error
    */
    virtual Status_t send_message(Message_t messageType, const uint32_t *inBuf, const size_t inBufSize,
                                  uint32_t **outBuf, size_t &outBufSize) = 0;

    /**
    * Get configuration value.
    *
    * @param IN key - name of the configuration value. See CFG_... configuration keys defined above.
    *   supported config keys:
    *      [required] "bkp_ip" - IP address or hostname of the BKP Service
    *           Example: bkp_ip = 192.167.1.1
    *           Example: bkp_ip = bkps.intel.com
    *      [required] "bkp_port" - Port to connect to BKP Service
    *           Example: bkp_port = 443
    *      [required] "bkp_cfg_id" - BKPS configuration ID used for provisioning. Configuration includes PUF type, key information, etc. In BKPS, there maybe multiple devices refer to same configuration flow.
    *           Example: bkp_cfg_id = 1
    *      [required] "bkp_tls_ca_cert" - Root certificate that issued the BKPS certificate
    *      [required] "bkp_tls_prog_cert" - Client certificate for BKP Programmer plugin
    *      [required] "bkp_tls_prog_key" - Private key corresponding to client certificate
    *      [required] "bkp_tls_prog_key_pass" - Password to the private key
    *      [optional] "bkp_proxy_address" - Proxy host and port
    *           Example: bkp_proxy_address = http://1.2.3.4:1234
    *      [optional] "bkp_proxy_user" - User name for proxy authentication
    *           Example: bkp_proxy_user = admin
    *      [optional] "bkp_proxy_password" - Password for proxy authentication
    *           Example: bkp_proxy_password = 1234
    *
    * @return string representing configuration value or empty string if the value does not exist.
    */
    virtual std::string get_config(const std::string& key) = 0;

    /**
     * Retrieve 4-byte integer mask (big-endian - most significant byte first) with message types (to be provided to send_message)
     * supported by Quartus. Each command is identified by the unique number starting from 0. In case of adding a new
     * command or removing a no longer supported command the original numbering should stay for backward compatibility.
     * The mask is created by setting the proper bit in result integer to 1 for the supported command. In this approach
     * there can be up to 32 supported commands.
     *
     * The examples below provide instruction on how the enumerable with message types should be maintained.
     *
     * Currently:
     * enum Message_t {
     *      SEND_PACKET = 0,
     *      PUSH_WRAPPED_KEY = 1,
     *      PUSH_WRAPPED_KEY_USER_IID = 2,
     *      PUSH_WRAPPED_KEY_UDS_IID = 3,
     *      PUSH_HELPER_DATA_UDS_IID = 4,
     *      PUSH_HELPER_DATA_UDS_INTEL = 5,
     * };
     *
     * Adding new command (NEW_COMMAND):
     * enum Message_t {
     *      SEND_PACKET = 0,
     *      PUSH_WRAPPED_KEY = 1,
     *      PUSH_WRAPPED_KEY_USER_IID = 2,
     *      PUSH_WRAPPED_KEY_UDS_IID = 3,
     *      PUSH_HELPER_DATA_UDS_IID = 4,
     *      PUSH_HELPER_DATA_UDS_INTEL = 5,
     *      NEW_COMMAND = 6
     * };
     *
     * Removing no longer supported commands:
     * enum Message_t {
     *      SEND_PACKET = 0,
     *      NEW_COMMAND = 6
     * };
     *
     * These examples show how the mask with message types is created.
     * -- Example 1 --
     * For commands: SEND_PACKET (= 0)
     * Returns: 1
     * Because: 00000000 00000000 00000000 00000001 = 1
     *
     * -- Example 2 --
     * For commands: SEND_PACKET (= 0), PUSH_WRAPPED_KEY (= 1)
     * Returns: 3
     * Because: 00000000 00000000 00000000 00000011 = 3
     *
     * -- Example 3 (in case some new command NEW_COMMAND is added) --
     * For commands: SEND_PACKET (= 0), PUSH_WRAPPED_KEY (= 1), NEW_COMMAND (= 5)
     * Returns: 35
     * Because: 00000000 00000000 00000000 00100011 = 35
     *
     * -- Example 4 (in case SEND_PACKET is no longer supported in some time in future) --
     * For commands: PUSH_WRAPPED_KEY (= 1), NEW_COMMAND (= 5)
     * Returns: 34
     * Because: 00000000 00000000 00000000 00100010 = 34
     *
     * -- Example 5 (in case SEND_PACKET and PUSH_WRAPPED_KEY are no longer supported in some time in future) --
     * For commands: NEW_COMMAND (= 5)
     * Returns: 32
     * Because: 00000000 00000000 00000000 00100000 = 32
     * @return mask with supported commands
     */
    virtual unsigned int get_supported_commands() = 0;

};

/**
 * Function returns current version of the BKP Programmer library.
 * @return version
 */
typedef const char *(*PGM_PLUGIN_BKP_GET_VERSION_FUNCTION)();
extern "C" PGM_PLUGIN_DLLEXPORT const char* get_version();


/**
* Function exposed by BKP Programmer library and called by Quartus Programmer.
* It will block the thread until the provisioning is completed or the error occurs.
* During bkp_provision() execution all callback object's methods may be called multiple times.
*
* @param IN qc - callback object passed by Quartus Programmer
*
* @return S_OK on success, S_GENERIC_ERROR on error
*/
typedef Status_t (*PGM_PLUGIN_BKP_PROVISION_FUNCTION)(QuartusContext &qc);
extern "C" PGM_PLUGIN_DLLEXPORT Status_t bkp_provision(QuartusContext &qc);

/**
* Function exposed by BKP Programmer library and called by Quartus Programmer.
* It will block the thread until the prefetching of the certificates
* used for provisioning is completed or the error occurs.
* During bkp_prefetch() execution all callback object's methods may be called multiple times.
*
* @param IN qc - callback object passed by Quartus Programmer
*
* @return S_OK on success, S_GENERIC_ERROR on error
*/
typedef Status_t (*PGM_PLUGIN_BKP_PREFETCH_FUNCTION)(QuartusContext &qc);
extern "C" PGM_PLUGIN_DLLEXPORT Status_t bkp_prefetch(QuartusContext &qc);

/**
* Function exposed by BKP Programmer library and called by Quartus Programmer.
* It calls BKPS API for PUF activation
* It will block the thread until PUF activation is completed or the error occurs.
* During bkp_puf_activate() execution all callback object's methods may be called multiple times.
*
* @param IN qc - callback object passed by Quartus Programmer
* @param IN puf_type - the type of PUF to activate
*
* @return S_OK on success, S_GENERIC_ERROR on error
*/
typedef Status_t (*PGM_PLUGIN_BKP_PUF_ACTIVATE_FUNCTION)(QuartusContext &qc, PufType_t puf_type);
extern "C" PGM_PLUGIN_DLLEXPORT Status_t bkp_puf_activate(QuartusContext &qc, PufType_t puf_type);

/**
* Function exposed by BKP Programmer library and called by Quartus Programmer.
* It sets Intel authority chain with suitable chain of certificates issued by Intel per requested UDS type.
* It will block the thread until the process is completed or the error occurs.
* During bkp_set_authority() execution all callback object's methods may be called multiple times.
*
* @param IN qc - callback object passed by Quartus Programmer
* @param IN puf_type - the type of PUF to activate
* @param IN slot_id - the slot ID to which the certificate chain needs to be written
* @param IN force_enrollment - force provisioning of chain with Enrollment#0 certificate instead of DeviceID certificate
*
* @return S_OK on success, S_GENERIC_ERROR on error
*/
typedef Status_t (*PGM_PLUGIN_BKP_SET_AUTHORITY_FUNCTION)(
        QuartusContext &qc, PufType_t puf_type, uint8_t slot_id, bool force_enrollment);
extern "C" PGM_PLUGIN_DLLEXPORT Status_t bkp_set_authority(
        QuartusContext &qc, PufType_t puf_type, uint8_t slot_id, bool force_enrollment);

#endif //INC_PGM_PLUGIN_BKP_EXPORT_H
