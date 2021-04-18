#include "PCA9685.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include "sdk_common.h"
#include "pbrick_twi.h"
#include "nrf_log.h"
#include "nrf_delay.h"

#ifndef MIN
#define MIN(X,Y) ((X) < (Y) ? : (X) : (Y))
#endif

uint32_t _oscillatorFrequency;

ret_code_t PCA9685_init(uint8_t deviceAddress)
{
    uint8_t data[3] = {0};
    ret_code_t ret = twi_rx(deviceAddress, PCA9685_DEVICE_ID, &data[0], 3);

    if (data[0] == 0) {
        NRF_LOG_WARNING("Unable to connect to PCA9685 at address: %x", deviceAddress);
        return NRF_ERROR_INTERNAL;
    }

#ifdef PCA9685_DEBUG
    NRF_LOG_DEBUG("PCA9685 at address %x initialized with data %x", deviceAddress, data);
#endif

    return ret;
}

ret_code_t PCA9685_getMode1(uint8_t deviceAddress, PCA9685_MODE1_typedef *mode)
{
    uint8_t data = 0;
    ret_code_t ret = twi_rx(deviceAddress, PCA9685_MODE1, &data, 1);
    VERIFY_SUCCESS(ret);
    mode->d8 = data;

    return NRF_SUCCESS;
}

ret_code_t PCA9685_setMode1(uint8_t deviceAddress, PCA9685_MODE1_typedef *mode)
{
    uint8_t data = mode->d8;
    ret_code_t ret = twi_tx(deviceAddress, PCA9685_MODE1, &data, 1);
    VERIFY_SUCCESS(ret);

#ifdef PCA9685_DEBUG
    NRF_LOG_DEBUG("PCA9685 MODE 1 set to %x", mode->d8);
#endif
    return NRF_SUCCESS;
}

ret_code_t PCA9685_getMode2(uint8_t deviceAddress, PCA9685_MODE2_typedef *mode)
{
    uint8_t data = 0;
    ret_code_t ret = twi_rx(deviceAddress, PCA9685_MODE2, &data, 1);
    VERIFY_SUCCESS(ret);
    mode->d8 = data;

    return NRF_SUCCESS;
}

ret_code_t PCA9685_setMode2(uint8_t deviceAddress, PCA9685_MODE2_typedef *mode)
{
    uint8_t data = mode->d8;
    ret_code_t ret = twi_tx(deviceAddress, PCA9685_MODE2, &data, 1);
    VERIFY_SUCCESS(ret);

#ifdef PCA9685_DEBUG
    NRF_LOG_DEBUG("PCA9685 MODE 2 set to %x", mode->d8);
#endif
    return NRF_SUCCESS;
}

ret_code_t PCA9685_restart(uint8_t deviceAddress)
{
    ret_code_t ret = PCA9685_wakeup(deviceAddress);
    VERIFY_SUCCESS(ret);
    
    uint8_t data = PCA9685_MODE1_RESTART;
    ret = twi_tx(deviceAddress, PCA9685_MODE1, &data, 1);
    VERIFY_SUCCESS(ret);

#ifdef PCA9685_DEBUG
    NRF_LOG_DEBUG("PCA9685 restarted.");
#endif

    return NRF_SUCCESS;
}

ret_code_t PCA9685_sleep(uint8_t deviceAddress)
{
    PCA9685_MODE1_typedef mode1;
    ret_code_t ret = PCA9685_getMode1(deviceAddress, &mode1);
    VERIFY_SUCCESS(ret);

    mode1.d8 |=~PCA9685_MODE1_SLEEP;

    ret = PCA9685_setMode1(deviceAddress, &mode1);
    VERIFY_SUCCESS(ret);

    nrf_delay_ms(1);

#ifdef PCA9685_DEBUG
    NRF_LOG_DEBUG("PCA9685 put to sleep");
#endif
    return NRF_SUCCESS;
}

ret_code_t PCA9685_wakeup(uint8_t deviceAddress)
{
    PCA9685_MODE1_typedef mode1;
    ret_code_t ret = PCA9685_getMode1(deviceAddress, &mode1);
    VERIFY_SUCCESS(ret);

    mode1.d8 = mode1.d8 & ~PCA9685_MODE1_SLEEP;

    ret = PCA9685_setMode1(deviceAddress, &mode1);
    VERIFY_SUCCESS(ret);

    nrf_delay_ms(1);

#ifdef PCA9685_DEBUG
    NRF_LOG_DEBUG("PCA9685 woke up");
#endif
    return NRF_SUCCESS;
}

ret_code_t PCA9685_setPWMFrequency(uint8_t deviceAddress, float frequency)
{
    if (frequency < 1) {
        frequency = 1;
    }

    if (frequency > 3500) {
        frequency = 3500;
    }

    float prescaleeval = ((_oscillatorFrequency / (frequency * 4096.0)) + 0.5) - 1;
    if (prescaleeval < PCA9685_PRESCALE_MIN) {
        prescaleeval = PCA9685_PRESCALE_MIN;
    }

    if (prescaleeval > PCA9685_PRESCALE_MAX) {
        prescaleeval = PCA9685_PRESCALE_MAX;
    }

    uint8_t prescale = (uint8_t)prescaleeval;

    PCA9685_MODE1_typedef mode1;
    ret_code_t ret = PCA9685_getMode1(deviceAddress, &mode1);

    uint8_t oldMode = mode1.d8;
    uint8_t newMode = (oldMode & ~PCA9685_MODE1_RESTART) | PCA9685_MODE1_SLEEP;
    
    mode1.d8 = newMode;
    ret = PCA9685_setMode1(deviceAddress, &mode1);
    VERIFY_SUCCESS(ret);

    ret = twi_tx(deviceAddress, PCA9685_PRESCALE, &prescale, 1);
    VERIFY_SUCCESS(ret);

    mode1.d8 = oldMode;
    ret = PCA9685_setMode1(deviceAddress, &mode1);
    VERIFY_SUCCESS(ret);

#ifdef PCA9685_DEBUG
    NRF_LOG_DEBUG("PCA9685 Prescale set to %x", prescale);
#endif
    return NRF_SUCCESS;
}

ret_code_t PCA9685_setOutputMode(uint8_t deviceAddress, bool totempole)
{
    PCA9685_MODE2_typedef mode2;
    ret_code_t ret = PCA9685_getMode2(deviceAddress, &mode2);
    VERIFY_SUCCESS(ret);

    uint8_t oldMode = mode2.d8;
    uint8_t newMode;

    if (totempole) {
        newMode = oldMode | PCA9685_MODE2_OUTDRV;
    } else {
        newMode = oldMode & ~PCA9685_MODE2_OUTDRV;
    }

    mode2.d8 = newMode;
    ret = PCA9685_setMode2(deviceAddress, &mode2);
    VERIFY_SUCCESS(true);

#ifdef PCA9685_DEBUG
    NRF_LOG_DEBUG("Setting PCA9685 output mode to: %s", (totempole ? "totempole" : "open drain"));
#endif
    return NRF_SUCCESS;
}

ret_code_t PCA9685_getPWM(uint8_t deviceAddress, uint8_t num, uint8_t *pwm)
{
    ret_code_t ret = twi_rx(deviceAddress, (PCA9685_LED0_ON_L + (4 * num)), pwm, 1);
    VERIFY_SUCCESS(ret);

#ifdef PCA9685_DEBUG
    NRF_LOG_DEBUG("PWM %d at %x", num, pwm);
#endif
    return NRF_SUCCESS;
}

ret_code_t PCA9685_setPWM(uint8_t deviceAddress, uint8_t num, uint16_t on, uint16_t off)
{
    uint8_t data[4] = {0};
    data[0] = on;
    data[1] = on >> 8;
    data[2] = off;
    data[3] = off >> 8;

    ret_code_t ret = twi_tx(deviceAddress, (PCA9685_LED0_ON_L + 4 * num), &data[0], 4);
    VERIFY_SUCCESS(ret);

#ifdef PCA9685_DEBUG
    uint8_t pwm = 0;
    ret = PCA9685_getPWM(deviceAddress, num, &pwm);
    VERIFY_SUCCESS(true);
#endif
    return NRF_SUCCESS;
}

ret_code_t PCA9685_setPin(uint8_t deviceAddress, uint8_t num, uint16_t val, bool invert)
{
    ret_code_t ret;
    val = MIN(val, (uint16_t)4095);
    if (invert) {
        if (val == 0) {
            // Special value for signal fully on.
            ret = PCA9685_setPWM(deviceAddress, num, 4096, 0);
            VERIFY_SUCCESS(ret);
        } else if (val == 4095) {
            // Special value for signal fully off.
            ret = PCA9685_setPWM(deviceAddress, num, 0, 4096);
            VERIFY_SUCCESS(ret);
        } else {
            ret = PCA9685_setPWM(deviceAddress, num, 0, 4095 - val);
            VERIFY_SUCCESS(ret);
        }
    } else {
        if (val == 4095) {
            // Special value for signal fully on.
            ret = PCA9685_setPWM(deviceAddress, num, 4096, 0);
            VERIFY_SUCCESS(ret);
        } else if (val == 0) {
            // Special value for signal fully off.
            ret = PCA9685_setPWM(deviceAddress, num, 0, 4096);
            VERIFY_SUCCESS(ret);
        } else {
            ret = PCA9685_setPWM(deviceAddress, num, 0, val);
            VERIFY_SUCCESS(ret);
        }
    }

  return NRF_SUCCESS;
}

ret_code_t PCA9685_writeMicroseconds(uint8_t deviceAddress, uint8_t num, uint16_t microseconds)
{
    double pulse = microseconds;
    double pulselength = 1000000;

    uint8_t prescale = 0;
    ret_code_t ret = twi_rx(deviceAddress, PCA9685_PRESCALE, &prescale, 1);
    VERIFY_SUCCESS(ret);

#ifdef PCA9685_DEBUG
    NRF_LOG_DEBUG("PCA9685: Prescale: %x", prescale);
#endif

    prescale += 1;
    pulselength *= prescale;
    pulselength /= PCA9685_getOscillatorFrequency();

    pulse /= pulselength;
    ret = PCA9685_setPWM(deviceAddress, num, 0, pulse);
    VERIFY_SUCCESS(ret);

#ifdef PCA9685_DEBUG
    NRF_LOG_DEBUG("PCA9685: Setting PWM via microseconds on output %d : %d", num, microseconds);
#endif
    return NRF_SUCCESS;
}

uint32_t PCA9685_getOscillatorFrequency()
{
    return _oscillatorFrequency;
}

void PCA9685_setOscillatorFrequency(uint32_t frequency)
{
     _oscillatorFrequency = frequency;
}