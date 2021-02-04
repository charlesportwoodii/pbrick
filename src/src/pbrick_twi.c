#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include "sdk_common.h"
#include "pbrick_twi.h"
#include "pbrick_board.h"
#include "nrf_log.h"
#include "nrf_error.h"

static volatile bool m_xfer_done = false;

void twi_handler(nrf_drv_twi_evt_t const *p_event, void *p_context)
{
    NRF_LOG_INFO("EVENT DONE %X", p_event->xfer_desc.type);
    switch (p_event->type) {
        case NRF_DRV_TWI_EVT_DONE:

            m_xfer_done = true;
            break;
        default:
            break;
    }
}

ret_code_t pbrick_twi_init(pbrick_twi *twi)
{
    ret_code_t err_code;
    nrf_drv_twi_t m_twi_master = NRF_DRV_TWI_INSTANCE(TWI_MASTER_INSTANCE);

    const nrf_drv_twi_config_t config =
    {
        .scl = PBRICK_SCL,
        .sda = PBRICK_SLA,
        .frequency  = NRF_DRV_TWI_FREQ_100K,
        .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
        .clear_bus_init  = false
    };
    
    err_code = nrf_drv_twi_init(&m_twi_master, &config, twi_handler, NULL);

    if (NRF_SUCCESS == err_code) {
            nrf_drv_twi_enable(&m_twi_master);

            pbrick_twi twi_instance = {
                .master = &m_twi_master,
                .instance = TWI_MASTER_INSTANCE
            };

           twi = &twi_instance;

            return NRF_SUCCESS;
    }

    return NRF_ERROR_NOT_SUPPORTED;
}

ret_code_t twi_tx(pbrick_twi *instance, uint8_t deviceAddress, uint8_t registerAddress, uint8_t *data, uint16_t length)
{
    ret_code_t ret;
    uint8_t rwData[255] = {0};
    rwData[0] = registerAddress;
    memcpy(&rwData[1], data, length);

    m_xfer_done = false;
    ret = nrf_drv_twi_tx(instance->master, deviceAddress, rwData, sizeof(length), false);
    APP_ERROR_CHECK(ret);
    while (m_xfer_done == false);

    return ret;
}

ret_code_t twi_rx(pbrick_twi *instance, uint8_t deviceAddress, uint8_t registerAddress, uint8_t *data, uint16_t length)
{
    ret_code_t ret;
    m_xfer_done = false;
    
    ret = nrf_drv_twi_tx(instance->master, deviceAddress, &registerAddress, sizeof(registerAddress), true);
    APP_ERROR_CHECK(ret);
    while(m_xfer_done == false);

    m_xfer_done = false;
    ret = nrf_drv_twi_rx(instance->master, deviceAddress, data, length);
    APP_ERROR_CHECK(ret);
    while (m_xfer_done == false);

    return ret;
}