#if __GNUC__ >= 4
    #define FCS_DLLEXPORT __attribute__ ((visibility ("default")))
#else
    #define FCS_DLLEXPORT
#endif

#include <cstring>
#include <vector>
#include <cstdint>
#include <stdint.h>
#include "CommandHeader.h"
#include "FcsSimulator.h"
#include "altera_fcs_structs.h"
#include "utils.h"

extern "C" {
    FCS_DLLEXPORT int fcs_get_chip_id(uint32_t* chip_id_lo, uint32_t* chip_id_hi)
    {
        uint8_t* chipIdBuffer = new uint8_t[2*WORD_SIZE];
        int error = FcsSimulator::fcs_get_response("GET_CHIPID", chipIdBuffer, new uint32_t);
        memcpy(chip_id_lo, chipIdBuffer, WORD_SIZE);
        memcpy(chip_id_hi, chipIdBuffer + WORD_SIZE, WORD_SIZE);
        return error;
    }

    FCS_DLLEXPORT int fcs_get_jtag_idcode(uint32_t* idcode)
    {
        uint8_t* idCodeBuffer = new uint8_t[WORD_SIZE];
        int error = FcsSimulator::fcs_get_response("GET_JTAG_IDCODE", idCodeBuffer, new uint32_t);
        memcpy(idcode, idCodeBuffer, WORD_SIZE);
        return error;
    }

    FCS_DLLEXPORT int fcs_attestation_get_certificate(int cert_request, char* cert, uint32_t* cert_size)
    {
        return (cert_request != 0x4) || FcsSimulator::fcs_get_response("GET_ATTESTATION_CERTIFICATE", reinterpret_cast<uint8_t *>(cert), cert_size);
    }

    FCS_DLLEXPORT int fcs_mctp_cmd_send(char* mctp_cmd, int cmd_len, char* mctp_resp, int* resp_len)
    {
        std::vector<uint32_t> expected_mctp_cmd = {0x05130000, 0x00008410};
        std::vector<uint8_t> expected_mctp_cmd_bytes = Utils::byteBufferFromWordPointer(expected_mctp_cmd.data(), expected_mctp_cmd.size());
        return (std::equal(expected_mctp_cmd_bytes.begin(), expected_mctp_cmd_bytes.end(), reinterpret_cast<uint8_t *>(mctp_cmd)) ? 0 : 1) ||
               ((size_t)cmd_len != expected_mctp_cmd_bytes.size()) ||
               FcsSimulator::fcs_get_response("MCTP", reinterpret_cast<uint8_t *>(mctp_resp), reinterpret_cast<uint32_t*>(resp_len));
    }

    FCS_DLLEXPORT int fcs_get_device_identity(char* dev_identity, int* dev_identity_length)
    {
        return FcsSimulator::fcs_get_response("GET_DEVICE_IDENTITY", reinterpret_cast<uint8_t *>(dev_identity), reinterpret_cast<uint32_t*>(dev_identity_length));
    }

    FCS_DLLEXPORT int fcs_qspi_open()
    {
        return 0;
    }

    FCS_DLLEXPORT int fcs_qspi_close()
    {
        return 0;
    }

    FCS_DLLEXPORT int fcs_qspi_set_cs(uint32_t chipSel)
    {
        return (chipSel != 0);
    }

    FCS_DLLEXPORT int fcs_qspi_erase(uint32_t qspiAddr, uint32_t numWords)
    {
        return (qspiAddr != 0) ||
               (numWords != 0x200);
    }

    FCS_DLLEXPORT int fcs_qspi_read(uint32_t qspiAddr, char* buffer, uint32_t numWords)
    {
        return (qspiAddr != 0) ||
               (numWords != 0x200) ||
               FcsSimulator::fcs_get_response("QSPI_READ", reinterpret_cast<uint8_t *>(buffer), new uint32_t);
    }

    FCS_DLLEXPORT int fcs_qspi_write(uint32_t qspiAddr, char* buffer, uint32_t numWords)
    {
        uint8_t* readBuffer = new uint8_t[numWords * WORD_SIZE];
        FcsSimulator::fcs_get_response("QSPI_READ", readBuffer, new uint32_t);
        return (qspiAddr != 0) ||
               (numWords != 0x200) ||
               std::equal(readBuffer, readBuffer + numWords, reinterpret_cast<uint8_t *>(buffer)) ? 0 : 1;
    }

    FCS_DLLEXPORT int libfcs_init(char* log_level)
    {
        (void) log_level;
        return 0;
    }
}
