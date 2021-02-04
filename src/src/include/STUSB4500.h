#ifndef STUSB4500_H__
#define STUSB4500_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sdk_common.h"
#include "pbrick_twi.h"

#define STUSB4500_DEVICE_ADDRESS            0x28
#define STUSB4500_REG_DEVICE_ID                 0x2F
#define STUSB4500_INTERRUPT                         0x0B
#define STUSB4500_ALERT_STATUS_MASK    0x0c
#define STUSB4500_PORT_STATUS                   0x0E
#define STUSB4500_TX_HEADER_LOW            0x51
#define STUSB4500_PD_COMMAND_CTRL    0x1A

/**@brief Initializes a STUSB4500 instance for TWI/I2C on-demand reconfiguration
 *
 * @param[in] uint8_t           deviceAddress   Device Address of STUSB4500 (0x28 default)
 *
 * @return ret_code_t
 */
ret_code_t STUSB4500_init(uint8_t deviceAddress);

/**@brief Sends a reset to STUSB4500 to force a new PDO_SINK configuration with VBUS_SOURCE
 *
 * @param[in] uint8_t           deviceAddress   Device Address of STUSB4500 (0x28 default)
 *
 * @return ret_code_t
 */
ret_code_t STUSB4500_soft_reset(uint8_t deviceAddress);

#ifdef __cplusplus
}
#endif

#endif // STUSB4500_H__

/** @} */