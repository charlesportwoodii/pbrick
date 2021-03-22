#include "PCA9624.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include "sdk_common.h"
#include "pbrick_twi.h"
#include "nrf_log.h"

ret_code_t PCA9642_init(uint8_t deviceAddress)
{
    uint8_t data[1] = {0};
    ret_code_t ret = twi_rx(deviceAddress, PCA9624_MODE0, &data[0], 1);
    return ret;
}

ret_code_t PCA9624_set(uint8_t deviceAddress, uint8_t address, uint8_t pwm)
{
    // Clamp PWM from 0x00 to 0xFF
    if (pwm < 0) {
        pwm = 0x00;
    }

    if (pwm > 0xFF) {
        pwm = 0xFF;
    }

    uint8_t data = pwm;

    ret_code_t ret = twi_tx(deviceAddress, address, &data, 1);
    return ret;
}

ret_code_t PCA9624_set_ledout(uint8_t deviceAddress,  uint8_t address, PCA9624_LEDOUT_typedef ledout)
{
    uint8_t data[1] = {0};
    memcpy(data, &ledout.d8, sizeof(ledout.d8));

    ret_code_t ret = twi_tx(deviceAddress, address, &data[0], 1);
    return ret;
}

ret_code_t PCA9624_reset(uint8_t deviceAddress)
{
    return NRF_SUCCESS;
}