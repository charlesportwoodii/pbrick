#include "PCA9624.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include "sdk_common.h"
#include "pbrick_twi.h"
#include "nrf_log.h"
#include "nrf_delay.h"

ret_code_t PCA9642_init(uint8_t deviceAddress)
{
    // Wake device from sleep mode
    ret_code_t ret = PCA9624_wakeup(deviceAddress);

    // Verify the MODE0[0:4] is 0
    uint8_t data[1] = {0};
    ret = twi_rx(deviceAddress, PCA9624_MODE0, &data[0], 1);
    VERIFY_SUCCESS(ret);

    if (data[0] != 0x01) {
        NRF_LOG_WARNING("Failed to wake device from sleep mode.");
    }

    // Have the device change on ACK instead of STOP
    data[0] = 0x13;
    ret = twi_tx(deviceAddress, PCA9624_MODE1, &data[0], 1);
    VERIFY_SUCCESS(ret);

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

    uint8_t data[1] = {0};
    data[0] = pwm;

    ret_code_t ret = twi_tx(deviceAddress, address, &data[0], 1);
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

ret_code_t PCA9624_sleep(uint8_t deviceAddress)
{
    uint8_t data[1] = {0};
    data[0] = 0x16;
    ret_code_t ret = twi_tx(deviceAddress, PCA9624_MODE0, &data[0], 1);
    VERIFY_SUCCESS(ret);
    return ret;
}

ret_code_t PCA9624_wakeup(uint8_t deviceAddress)
{
    uint8_t data[1] = {0};
    data[0] = 0x00;
    ret_code_t ret = twi_tx(deviceAddress, PCA9624_MODE0, &data[0], 1);
    VERIFY_SUCCESS(ret);

    // The oscillator needs 500 uS to wake up - wait 1 ms. 
    nrf_delay_ms(1);

    return ret;
}
