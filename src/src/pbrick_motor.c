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
    UNUSED_VARIABLE(ret);
    motors.size = 0;

    uint8_t motor_pins[2] = { PBRICK_PWM0_P1, PBRICK_PWM0_P2 };
    pbrick_motor_setup(motor_pins, PBRICK_PWM0_PWM);

    uint8_t motor_pins1[2] = { PBRICK_PWM1_P1, PBRICK_PWM1_P2 };
    pbrick_motor_setup(motor_pins1, PBRICK_PWM1_PWM);

    return NRF_SUCCESS;
}

ret_code_t pbrick_motor_enable()
{
    return NRF_SUCCESS;
}

ret_code_t pbrick_motor_disable()
{
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