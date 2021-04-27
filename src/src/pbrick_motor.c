#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include "pbrick_board.h"
#include "pbrick_motor.h"
#include "sdk_common.h"
#include "nrf_log.h"

#include "nrf_gpio.h"

pbrick_motors motors;

static void pbrick_motor_setup(uint8_t pins[2], uint8_t driverPin)
{
    struct s_pbrick_motor_state s;

    memcpy(s.pins, pins, 2);
    s.direction = 1;
    s.pwm = 0;
    s.driverPin = driverPin;

    motors.motors[motors.size] = s;
    motors.size += 1;
}

ret_code_t pbrick_motor_init()
{
    ret_code_t ret;
    ret = PCA9642_init(PCA9624_DEVICE_ADDRESS);
    if (ret != NRF_SUCCESS) {
        NRF_LOG_ERROR("Failed to initialize PCA9624 device");
        return NRF_ERROR_INTERNAL;
    }

    nrf_gpio_cfg_output(PBRICK_MOTOR_ENABLE);
    nrf_gpio_pin_clear(PBRICK_MOTOR_ENABLE);

    motors.size = 0;

    uint8_t motor_pins[2] = { PBRICK_PWM0_P1, PBRICK_PWM0_P2 };
    pbrick_motor_setup(motor_pins, PBRICK_PWM0_PWM);

    uint8_t motor_pins1[2] = { PBRICK_PWM1_P1, PBRICK_PWM1_P2 };
    pbrick_motor_setup(motor_pins1, PBRICK_PWM1_PWM);

    PCA9624_MODE1_typedef mode;
    ret = PCA9624_getMode1(PCA9624_DEVICE_ADDRESS, &mode);
    VERIFY_SUCCESS(ret);
    NRF_LOG_DEBUG("Mode1 Sleep: %x %x", mode.d8, mode.mode1.SLEEP);

    ret = pbrick_motor_enable();
     if (ret != NRF_SUCCESS) {
         NRF_LOG_ERROR("Failed to enable PCA9624 device drivers");
        return NRF_ERROR_INTERNAL;
    }

    ret = PCA9624_getMode1(PCA9624_DEVICE_ADDRESS, &mode);
    VERIFY_SUCCESS(ret);
    NRF_LOG_DEBUG("Mode1 Sleep: %x %x", mode.d8, mode.mode1.SLEEP);

    PCA9624_MODE1_typedef mode1;
    mode1.d8 = 0x00;
    ret = PCA9624_setMode1( PCA9624_DEVICE_ADDRESS,  &mode1);
    VERIFY_SUCCESS(ret);
    PCA9624_LEDOUT_typedef ledout0;
    PCA9624_LEDOUT_typedef ledout1;

    // Set LEDOUT
    ret = PCA9624_getLedout(PCA9624_DEVICE_ADDRESS, PCA9624_LEDOUT0, ledout0);
    ret = PCA9624_getLedout(PCA9624_DEVICE_ADDRESS, PCA9624_LEDOUT1, ledout1);
    NRF_LOG_DEBUG("LEDOUT0: %x", ledout0.d8);
    NRF_LOG_DEBUG("LEDOUT1: %x", ledout1.d8);

    ledout0.ledout.ldr0 = 3;
    ledout0.ledout.ldr1 = 3;
    ledout0.ledout.ldr2 = 3;
    ledout0.ledout.ldr3 = 3;

    ledout1.ledout.ldr0 = 3;
    ledout1.ledout.ldr1 = 3;
    ledout1.ledout.ldr2 = 3;
    ledout1.ledout.ldr3 = 3;

    ret = PCA9624_setLedout(PCA9624_DEVICE_ADDRESS, PCA9624_LEDOUT0, ledout0);
    ret = PCA9624_setLedout(PCA9624_DEVICE_ADDRESS, PCA9624_LEDOUT1, ledout1);

    ret = PCA9624_getLedout(PCA9624_DEVICE_ADDRESS, PCA9624_LEDOUT0, ledout0);
    ret = PCA9624_getLedout(PCA9624_DEVICE_ADDRESS, PCA9624_LEDOUT1, ledout1);
    NRF_LOG_DEBUG("LEDOUT0: %x", ledout0.d8);
    NRF_LOG_DEBUG("LEDOUT1: %x", ledout1.d8);

    for (int i = 0; i < 8; i++) {
        uint8_t pwm;
        ret = PCA9624_getPWM(PCA9624_DEVICE_ADDRESS, PCA9624_PWM0 + i, &pwm);
    }

    ret = PCA9624_setPWM(PCA9624_DEVICE_ADDRESS, PBRICK_PWM0_PWM, 0xFF);
    ret = PCA9624_setPWM(PCA9624_DEVICE_ADDRESS, PBRICK_PWM0_P1, 0xFF);
    ret = PCA9624_setPWM(PCA9624_DEVICE_ADDRESS, PBRICK_PWM0_P2, 0x00);

    ret = PCA9624_setPWM(PCA9624_DEVICE_ADDRESS, PBRICK_PWM1_PWM, 0xFF);
    ret = PCA9624_setPWM(PCA9624_DEVICE_ADDRESS, PBRICK_PWM1_P1, 0xFF);
    ret = PCA9624_setPWM(PCA9624_DEVICE_ADDRESS, PBRICK_PWM1_P2, 0x00);

    for (int i = 0; i < 8; i++) {
        uint8_t pwm;
        ret = PCA9624_getPWM(PCA9624_DEVICE_ADDRESS, PCA9624_PWM0 + i, &pwm);
    }
    return NRF_SUCCESS;
}

ret_code_t pbrick_motor_enable()
{
    ret_code_t ret = PCA9624_wakeup(PCA9624_DEVICE_ADDRESS);
    VERIFY_SUCCESS(ret);
    return NRF_SUCCESS;
}

ret_code_t pbrick_motor_disable()
{
    ret_code_t ret = PCA9624_sleep(PCA9624_DEVICE_ADDRESS);
    VERIFY_SUCCESS(ret);
    return NRF_SUCCESS;
}

ret_code_t pbrick_motor_set(const uint8_t data[])
{
    return NRF_SUCCESS;
}

ret_code_t pbrick_motor_set_internal(uint8_t motor, uint8_t pwm, uint8_t direction)
{
    return NRF_SUCCESS;
}