#ifndef PBRICK_MOTOR_H
#define PBRICK_MOTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pbrick_board.h"
#include "app_pwm.h"

struct s_pbrick_motor_state
{
    uint8_t id;
    uint8_t direction;
    uint8_t pwm;
    uint8_t pins[2];
    uint8_t channel;
    const app_pwm_t   *driver;
};

typedef struct s_pbrick_motors
{
    uint8_t elements;
    uint8_t size;
    struct s_pbrick_motor_state motors[2];
} pbrick_motors;

/**@brief Initializes PBRICK motor
 *          - GPIO enables the OE pin
 *          - Runs pbrick_motor_enable()
 *          - Sets up reference motor states
 * s
 * @return ret_code_t
 */
ret_code_t pbrick_motor_init();

/**@brief Enables the motor driver
 *          - Enables the OE pin LOW
 *          - Sets the LEDOUT0 and LEDOUT1 to 0x02 (10)
 *
 * @return ret_code_t
 */
ret_code_t pbrick_motor_enable();

/**@brief Disables the motor driver
 *          - Sets the OE pin HGIH
 *          - Sets LEDOUT0 and LEDOUT1 to 0x00 (00)
 *          - Clears PWM on all pins
 *
 * @return ret_code_t
 */
ret_code_t pbrick_motor_disable();

/**@brief Sets the motor state from ble_pbrick
 * @param[in] uint8_t[] data    3 Byte [motor][direction][pwm]
 *      Motor 0x00 = Motor 0
 *      Motor 0x01 = Motor 1
 *      Motor 0x02 = Motor 0 + Motor 1 in same direction
 *      Motor 0x03 = Motor 0 + Motor 1 in opposing directions
 *
 *      Direction 0x00 = Normal
 *      Direction 0x01 = Counter
 *
 *      PWM 0x00 => 0xFF (0-256)
 *
 * @return ret_code_t
 */
ret_code_t pbrick_motor_set(const uint8_t data[]);

/**@brief Internal motor set
 * @param[in] uint8_t   motor       The motor to set
 * @param[in] uint8_t   pwm         The motor PWM setting
 * @param[in] uint8_t   direction   The motor direction
 * @return ret_code_t
 */
void pbrick_motor_set_internal(uint8_t motor, uint8_t pwm, uint8_t direction);

void pbrick_motor_stop(uint8_t motor);

void pbrick_motor_stop_all();

#ifdef __cplusplus
}
#endif

#endif // PBRICK_MOTOR_H