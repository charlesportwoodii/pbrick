#ifndef PBRICK_TWI_H__
#define PBRICK_TWI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_drv_twi.h"
#include "sdk_common.h"

#define TWI_MASTER_INSTANCE     0

typedef struct pbrick_twi {
    nrf_drv_twi_t  *master;
    uint8_t instance;
} pbrick_twi;

void twi_handler(nrf_drv_twi_evt_t const *p_event, void *p_context);

/**@brief Creates a dedicated TWI/I2C instance for PBrick to use
 * 
 * @param[in] pbrick_twi*       twi     Global TWI instance
 * @return ret_code_t
 */
ret_code_t pbrick_twi_init(pbrick_twi *twi);

/**@brief TWI/I2C write
 *
 * @param[in] pbrick_twi*   instance        The PBrick TWI instance
 * @param[in] uint8_t       deviceAddress   The TWI device address
 * @param[in] uint8_t       registerAddress The Register address to write to
 * @param[in] uint8_t*      data            Data to write to the TWI Slave
 * @param[in] uint8_t       length          Data length to write
 *
 * @return ret_code_t
 */
ret_code_t twi_tx(pbrick_twi *instance, uint8_t deviceAddress, uint8_t registerAddress, uint8_t *data, uint16_t length);

/**@brief TWI/I2C read
 *
 * @param[in]  pbrick_twi*   instance        The PBrick TWI instance
 * @param[in]  uint8_t       deviceAddress   The TWI device address
 * @param[in]  uint8_t       registerAddress The Register address to write to
 * @param[out] uint8_t*      data           Data buffer to be written to
 * @param[in]  uint8_t       length          Data length to read
 *
 * @return ret_code_t
 */
ret_code_t twi_rx(pbrick_twi *instance, uint8_t deviceAddress, uint8_t registerAddress, uint8_t *data, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif // PBRICK_TWI_H__

/** @} */