#ifndef PBRICK_TWI_H__
#define PBRICK_TWI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_drv_twi.h"
#include "sdk_common.h"

#define TWI_MASTER_INSTANCE     0

extern nrf_drv_twi_t m_twi_master;

extern void twi_handler(nrf_drv_twi_evt_t const *p_event, void *p_context);

/**@brief Creates a dedicated TWI/I2C instance for PBrick to use
 * 
 * @return ret_code_t
 */
ret_code_t pbrick_twi_init();

/**@brief TWI/I2C write
 *
 * @param[in] uint8_t       deviceAddress   The TWI device address
 * @param[in] uint8_t       registerAddress The Register address to write to
 * @param[in] uint8_t*      data            Data to write to the TWI Slave
 * @param[in] uint8_t       length          Data length to write
 *
 * @return ret_code_t
 */
ret_code_t twi_tx(uint8_t deviceAddress, uint8_t registerAddress, uint8_t *data, uint16_t length);

/**@brief TWI/I2C read
 *
 * @param[in]  uint8_t       deviceAddress   The TWI device address
 * @param[in]  uint8_t       registerAddress The Register address to write to
 * @param[out] uint8_t*      data           Data buffer to be written to
 * @param[in]  uint8_t       length          Data length to read
 *
 * @return ret_code_t
 */
ret_code_t twi_rx(uint8_t deviceAddress, uint8_t registerAddress, uint8_t *data, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif // PBRICK_TWI_H__

/** @} */