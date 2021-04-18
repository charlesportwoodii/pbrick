#include "PCA9685.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include "sdk_common.h"
#include "pbrick_twi.h"
#include "nrf_log.h"

ret_code_t PCA9685_init(uint8_t deviceAddress)
{
    uint8_t data[3] = {0};
    ret_code_t ret = twi_rx(deviceAddress, PCA9685_DEVICE_ID, &data[0], 3);

    if (data[0] == 0) {
        NRF_LOG_WARNING("Unable to connect to PCA9685 at address: %x", deviceAddress);
        return NRF_ERROR_INTERNAL;
    }
    
    return ret;
}