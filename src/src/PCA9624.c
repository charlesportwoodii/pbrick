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
    uint8_t data[3] = {0};
    ret_code_t ret = twi_rx(deviceAddress, PCA9624_DEVICE_ID, &data[0], 3);

    NRF_LOG_DEBUG("Device ID %x %x %x", data[0], data[1], data[2]);

    data[0] =0x16;
    ret = twi_tx(deviceAddress, PCA9624_MODE1, &data[0], 1);
    return NRF_SUCCESS;
    return ret;
}

ret_code_t PCA9624_getMode1(uint8_t deviceAddress, PCA9624_MODE1_typedef *mode)
{
    uint8_t data = 0;
    ret_code_t ret = twi_rx(deviceAddress, PCA9624_MODE1, &data, 1);
    VERIFY_SUCCESS(ret);

    mode->d8 = data;

#if PCA9624_DEBUG
    NRF_LOG_DEBUG("PCA9624 MODE1 Data: %x", mode->d8);
#endif
    return NRF_SUCCESS;
}

ret_code_t PCA9624_setMode1(uint8_t deviceAddress, PCA9624_MODE1_typedef *mode)
{
    uint8_t data = mode->d8;
    ret_code_t ret = twi_tx(deviceAddress, PCA9624_MODE1, &data, 1);
    VERIFY_SUCCESS(ret);

#ifdef PCA9624_DEBUG
    NRF_LOG_DEBUG("PCA9624 MODE 1 set to %x", mode->d8);
#endif
    return NRF_SUCCESS;
}

ret_code_t PCA9624_setPWM(uint8_t deviceAddress, uint8_t address, uint8_t pwm)
{
    uint8_t data = pwm;
    ret_code_t ret = twi_tx(deviceAddress, address, &data, 1);
    VERIFY_SUCCESS(ret);

#if PCA9624_DEBUG
    NRF_LOG_DEBUG("PWM %x set to %x", address - 2, pwm);
#endif
    return NRF_SUCCESS;
}

ret_code_t PCA9624_getPWM(uint8_t deviceAddress, uint8_t address, uint8_t *pwm)
{
    uint8_t data = 0;
    ret_code_t ret = twi_rx(deviceAddress, address, &data, 1);
    VERIFY_SUCCESS(ret);
    memcpy(pwm, &data, 1);

#if PCA9624_DEBUG
    NRF_LOG_DEBUG("PWM %x: %x", address - 2, data);
#endif
    return NRF_SUCCESS;
}

ret_code_t PCA9624_setLedout(uint8_t deviceAddress,  uint8_t address, PCA9624_LEDOUT_typedef ledout)
{
    uint8_t data = ledout.d8;
    ret_code_t ret = twi_tx(deviceAddress, address, &data, 1);
    VERIFY_SUCCESS(ret);
    
    return NRF_SUCCESS;
}

ret_code_t PCA9624_getLedout(uint8_t deviceAddress,  uint8_t address, PCA9624_LEDOUT_typedef ledout)
{
    uint8_t data = 0;
    ret_code_t ret = twi_rx(deviceAddress, address, &data, 1);
    VERIFY_SUCCESS(ret);

#if PCA9624_DEBUG
    NRF_LOG_DEBUG("LEDOUT %x = %x", address, data);
#endif
    return NRF_SUCCESS;
}

ret_code_t PCA9624_reset(uint8_t deviceAddress)
{
    return NRF_SUCCESS;
}

ret_code_t PCA9624_sleep(uint8_t deviceAddress)
{
    PCA9624_MODE1_typedef mode;
    ret_code_t ret = PCA9624_getMode1(deviceAddress, &mode);
    VERIFY_SUCCESS(ret);

    mode.d8 |= PCA9624_MODE1_SLEEP;

    ret = PCA9624_setMode1(deviceAddress, &mode);
    VERIFY_SUCCESS(ret);

    nrf_delay_ms(1);

#ifdef PCA9624_DEBUG
    NRF_LOG_DEBUG("PCA9685 put to sleep");
#endif
    return NRF_SUCCESS;
}

ret_code_t PCA9624_wakeup(uint8_t deviceAddress)
{
    PCA9624_MODE1_typedef mode;
    ret_code_t ret = PCA9624_getMode1(deviceAddress, &mode);
    VERIFY_SUCCESS(ret);

    mode.d8 = mode.d8 & ~PCA9624_MODE1_SLEEP;

    ret = PCA9624_setMode1(deviceAddress, &mode);
    VERIFY_SUCCESS(ret);

    nrf_delay_ms(1);

#ifdef PCA9624_DEBUG
    NRF_LOG_DEBUG("PCA9685 woke up");
#endif
    return NRF_SUCCESS;
}
