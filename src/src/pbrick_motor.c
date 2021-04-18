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

    ret = pbrick_motor_enable();
     if (ret != NRF_SUCCESS) {
         NRF_LOG_ERROR("Failed to enable PCA9624 device drivers");
        return NRF_ERROR_INTERNAL;
    }

    nrf_gpio_cfg_output(PBRICK_MOTOR_ENABLE);
    motors.size = 0;

    uint8_t motor_pins[2] = { PBRICK_PWM0_P1, PBRICK_PWM0_P2 };
    pbrick_motor_setup(motor_pins, PBRICK_PWM0_PWM);

    uint8_t motor_pins1[2] = { PBRICK_PWM1_P1, PBRICK_PWM1_P2 };
    pbrick_motor_setup(motor_pins1, PBRICK_PWM1_PWM);

    return ret;
}

ret_code_t pbrick_motor_enable()
{
    ret_code_t ret;
    UNUSED_PARAMETER(ret);

    // Set the OE pin ACTIVE LOW
    nrf_gpio_pin_clear(PBRICK_MOTOR_ENABLE);
    PCA9624_LEDOUT_typedef ledout;

    ledout.ledout.ldr0 = 0x02;
    ledout.ledout.ldr1 = 0x02;
    ledout.ledout.ldr2 = 0x02;
    ledout.ledout.ldr3 = 0x02;

    ret = PCA9624_set_ledout(PCA9624_DEVICE_ADDRESS, PCA9624_LEDOUT0, ledout);
    if (ret != NRF_SUCCESS) {
        NRF_LOG_ERROR("Failed to set PCA9624 LEDOUT0");
        return NRF_ERROR_INTERNAL;
    }

    ret = PCA9624_set_ledout(PCA9624_DEVICE_ADDRESS, PCA9624_LEDOUT1, ledout);
    if (ret != NRF_SUCCESS) {
         NRF_LOG_ERROR("Failed to set PCA9624 LEDOUT1");
        return NRF_ERROR_INTERNAL;
    }

    ret = PCA9624_wakeup(PCA9624_DEVICE_ADDRESS);
    NRF_LOG_DEBUG("Motor enabled");
    return NRF_SUCCESS;
}

ret_code_t pbrick_motor_disable()
{
    ret_code_t ret;
    UNUSED_PARAMETER(ret);

    // Clear the OE pin ACTIVE LOW
    nrf_gpio_pin_set(PBRICK_MOTOR_ENABLE);
    PCA9624_LEDOUT_typedef ledout;

    ledout.ledout.ldr0 = 0x00;
    ledout.ledout.ldr1 = 0x00;
    ledout.ledout.ldr2 = 0x00;
    ledout.ledout.ldr3 = 0x00;

    ret = PCA9624_set_ledout(PCA9624_DEVICE_ADDRESS, PCA9624_LEDOUT0, ledout);
    if (ret != NRF_SUCCESS) {
         NRF_LOG_ERROR("Failed to set PCA9624 LEDOUT0");
        return NRF_ERROR_INTERNAL;
    }

    ret = PCA9624_set_ledout(PCA9624_DEVICE_ADDRESS, PCA9624_LEDOUT1, ledout);
    if (ret != NRF_SUCCESS) {
         NRF_LOG_ERROR("Failed to set PCA9624 LEDOUT1");
        return NRF_ERROR_INTERNAL;
    }

    for (int i = 0; i < motors.size; i++) {
        // Clear the PWM driver pin
        ret = PCA9624_set(PCA9624_DEVICE_ADDRESS, motors.motors[i].driverPin, 0x00);
        for (int j = 0; j < 2; j++) {
            // Clear the PWM direction pin
            ret = PCA9624_set(PCA9624_DEVICE_ADDRESS, motors.motors[i].pins[j], 0x00);
        }

        ret = PCA9624_sleep(PCA9624_DEVICE_ADDRESS);
        return NRF_SUCCESS;
    }

    NRF_LOG_DEBUG("Motor Disabled");
    return NRF_SUCCESS;
}

ret_code_t pbrick_motor_set(const uint8_t data[])
{
    ret_code_t ret;

    uint8_t motor = data[0];
    uint8_t direction = data[1];
    uint8_t pwm = data[2];

    // 0x00 is a special stop code
    if (motor == 0x00 && direction == 0x00 && pwm == 0x00) {
        NRF_LOG_DEBUG("Shutting down motor");
        ret =  pbrick_motor_disable();
        return ret;
    }

    // Enable the motor as a failsafe
    ret = pbrick_motor_enable();
    if (ret != NRF_SUCCESS) {
        NRF_LOG_ERROR("Failed to re-enable PCA9624.");
        return NRF_ERROR_INTERNAL;
    }

    ret = pbrick_motor_set_internal(motor, pwm, direction);
    return ret;
}

ret_code_t pbrick_motor_set_internal(uint8_t motor, uint8_t pwm, uint8_t direction)
{
    ret_code_t ret;
    UNUSED_PARAMETER(ret);
    
    NRF_LOG_DEBUG("Motor: %x | PWM: %x | Direction: %x", motor, pwm, direction);

    nrf_gpio_pin_clear(PBRICK_MOTOR_ENABLE);
    
    ret = PCA9624_set(PCA9624_DEVICE_ADDRESS, PCA9624_PWM6, 0xFF);
    NRF_LOG_DEBUG("%x", ret);

    ret = PCA9624_set(PCA9624_DEVICE_ADDRESS, PCA9624_PWM4, 0xFF);
    NRF_LOG_DEBUG("%x", ret);

    ret = PCA9624_set(PCA9624_DEVICE_ADDRESS, PCA9624_PWM3, 0x00);
    NRF_LOG_DEBUG("%x", ret);

    ret = PCA9624_set(PCA9624_DEVICE_ADDRESS, PCA9624_PWM5, 0xFF);
    NRF_LOG_DEBUG("%x", ret);

    ret = PCA9624_set(PCA9624_DEVICE_ADDRESS, PCA9624_PWM2, 0xFF);
    NRF_LOG_DEBUG("%x", ret);

    ret = PCA9624_set(PCA9624_DEVICE_ADDRESS, PCA9624_PWM1, 0x00);
    NRF_LOG_DEBUG("%x", ret);
    return NRF_SUCCESS;

    // Clear the PWM flag to avoid motor burnout from fast switching/stall conditions
    if (motor == 0x01 || motor == 0x02) {
        NRF_LOG_DEBUG("Clearing Driver Pin");
        ret = PCA9624_set(PCA9624_DEVICE_ADDRESS, motors.motors[motor].driverPin, 0x00);
    } else {
        NRF_LOG_DEBUG("Clearing All Driver Pin");
        ret = PCA9624_set(PCA9624_DEVICE_ADDRESS, motors.motors[0].driverPin, 0x00);
        ret = PCA9624_set(PCA9624_DEVICE_ADDRESS, motors.motors[1].driverPin, 0x00);
    }

    // Set Direction
    if (motor == 0x01 || motor == 0x02) {
        if (direction == 0x00 || direction == 0x01) {
            uint8_t directionPwmFlag;

            if (direction == 0x00) {
                directionPwmFlag = 0;
            } else if (direction == 0x01) {
                directionPwmFlag = 0xFF;
            }

            NRF_LOG_DEBUG("Setting Direction Flag");
            ret = PCA9624_set(PCA9624_DEVICE_ADDRESS, motors.motors[motor].pins[0], directionPwmFlag);
            ret = PCA9624_set(PCA9624_DEVICE_ADDRESS, motors.motors[motor].pins[0], (directionPwmFlag ^ 0xFF));
            motors.motors[motor].direction = direction;
        } else {
            NRF_LOG_WARNING("Invalid motor direction.");
            return NRF_SUCCESS;
        }
    } else if (motor == 0x03) {
        NRF_LOG_DEBUG("Setting both motors to same direction");
        ret = pbrick_motor_set_internal(0x01, pwm, direction);
        ret = pbrick_motor_set_internal(0x02, pwm, direction);
        return NRF_SUCCESS;
    } else if (motor == 0x04) {
        NRF_LOG_DEBUG("Setting both motors to inverse direction");
        ret = pbrick_motor_set_internal(0x01, pwm, direction);
        ret = pbrick_motor_set_internal(0x02, pwm, (direction ^ 0x01));
        return NRF_SUCCESS;
    }

    // Set the driver pin PWM
    NRF_LOG_DEBUG("Setting Driver Pin");
    ret = PCA9624_set(PCA9624_DEVICE_ADDRESS, motors.motors[motor].driverPin, pwm);

    motors.motors[motor].pwm = pwm;

    return NRF_SUCCESS;
}