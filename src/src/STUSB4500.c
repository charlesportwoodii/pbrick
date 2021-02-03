#include "STUSB4500.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include "sdk_common.h"
#include "pbrick_board.h"
#include "nrf_log.h"
#include "nrf_drv_twi.h"

ret_code_t STUSB4500_init(const nrf_drv_twi_t *m_twi_master, uint8_t deviceAddress)
{
    ret_code_t ret;
    do {
        ret = nrf_drv_twi_rx(m_twi_master,  deviceAddress, 0, 0x00);
        if (NRF_SUCCESS != ret) {
            break;
        }
    } while(0);

    if (NRF_ERROR_DRV_TWI_ERR_ANACK == ret) {
        NRF_LOG_INFO("STUSB4500 is not connected via QWIIC.");
    }
    return ret;
}