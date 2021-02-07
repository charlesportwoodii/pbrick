#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include "sdk_common.h"
#include "pbrick_twi.h"
#include "pbrick_board.h"
#include "nrf_log.h"
#include "nrf_error.h"

nrf_drv_twi_t m_twi_master = NRF_DRV_TWI_INSTANCE(TWI_MASTER_INSTANCE);

ret_code_t pbrick_twi_init()
{
    ret_code_t err_code;
    const nrf_drv_twi_config_t config =
    {
        .scl = PBRICK_SCL,
        .sda = PBRICK_SLA,
        .frequency  = NRF_DRV_TWI_FREQ_400K,
        .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
        .clear_bus_init  = false
    };
    
    err_code = nrf_drv_twi_init(&m_twi_master, &config, NULL, NULL);

    if (NRF_SUCCESS == err_code) {
        nrf_drv_twi_enable(&m_twi_master);
        return NRF_SUCCESS;
    }

    return NRF_ERROR_NOT_SUPPORTED;
}

ret_code_t twi_tx(uint8_t deviceAddress, uint8_t registerAddress, uint8_t *data, uint16_t length)
{
    ret_code_t ret;
    uint8_t rwData[255] = {0};
    rwData[0] = registerAddress;
    memcpy(&rwData[1], data, length);

    ret = nrf_drv_twi_tx(&m_twi_master, deviceAddress, rwData, sizeof(length), false);

    return ret;
}

ret_code_t twi_rx(uint8_t deviceAddress, uint8_t registerAddress, uint8_t *data, uint16_t length)
{
    ret_code_t ret;
    ret = nrf_drv_twi_tx(&m_twi_master, deviceAddress, &registerAddress, sizeof(registerAddress), true);
    if (NRF_SUCCESS == ret) {
        ret = nrf_drv_twi_rx(&m_twi_master, deviceAddress, data, length);
    }

    return ret;
}