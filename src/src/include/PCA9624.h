#ifndef PCA9642_H__
#define PCA9642_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sdk_common.h"
#include "pbrick_twi.h"
#include "pbrick_board.h"

#define PCA9624_DEVICE_ID   0xF8
#define PCA9624_LEDOUT0     0x0C
#define PCA9624_LEDOUT1     0x0D

#define PCA9624_MODE1          0x00
#define PCA9624_MODE2          0x01

#define PCA9624_PWM0            0x02
#define PCA9624_PWM1            0x03
#define PCA9624_PWM2            0x04
#define PCA9624_PWM3            0x05
#define PCA9624_PWM4            0x06
#define PCA9624_PWM5            0x07
#define PCA9624_PWM6            0x08
#define PCA9624_PWM7            0x09

#define PCA9624_MODE1_SLEEP 0x10
typedef union {
    uint8_t d8;
    struct {
        uint8_t ALLCALL :1;
        uint8_t SUB3 :1;
        uint8_t SUB2 :1;
        uint8_t SUB1 :1;
        uint8_t SLEEP :1;
        uint8_t AI0 :1;
        uint8_t AI1 :1;
        uint8_t AI2 :1;
    } mode1;
} PCA9624_MODE1_typedef;

typedef union  {
	uint8_t d8;
	struct
     {
		uint8_t ldr0 :2;
        uint8_t ldr1 :2;
		uint8_t ldr2 :2;
        uint8_t ldr3 :2;
	} ledout;
} PCA9624_LEDOUT_typedef;


/**@brief Initializes a PCA9624 instance for TWI/I2C on-demand reconfiguration
 *
 * @param[in] uint8_t           deviceAddress   Device Address of PCA9624
 *
 * @return ret_code_t
 */
ret_code_t PCA9642_init(uint8_t deviceAddress);

/**@brief Gets MODE1
 *
 * @param[in] uint8_t                                                deviceAddress           Device Address of PCA9624
 * @param[in] PCA9624_MODE01_typedef       mode                       MODE1 information
 *
 * @return ret_code_t
 */
ret_code_t PCA9624_getMode1(uint8_t deviceAddress, PCA9624_MODE1_typedef *mode);

/**@brief Sets Mode1
 *
 * @param[in] uint8_t                                               deviceAddress           Device Address of PCA9624
 * @param[in] PCA9624_MODE01_typedef       mode                       MODE1 information
 *
 * @return ret_code_t
 */
ret_code_t PCA9624_setMode1(uint8_t deviceAddress, PCA9624_MODE1_typedef *mode);

/**@brief Sets LED[0-7] PWM frequency
 *
 * @param[in] uint8_t       deviceAddress           Device Address of PCA9624
 * @param[in] uint8_t       address                         PWM[0-7] register address
 * @param[in] uint8_t       pwm                              pwm value 0x00 - 0xFF (0-255)
 *
 * @return ret_code_t
 */
ret_code_t PCA9624_setPWM(uint8_t deviceAddress, uint8_t address, uint8_t pwm);

/**@brief Get LED[0-7] PWM frequency
 *
 * @param[in] uint8_t       deviceAddress           Device Address of PCA9624
 * @param[in] uint8_t       address                         PWM[0-7] register address
 * @param[in] uint8_t       pwm                              pwm value 0x00 - 0xFF (0-255)
 *
 * @return ret_code_t
 */
ret_code_t PCA9624_getPWM(uint8_t deviceAddress, uint8_t address, uint8_t *pwm);

/**@brief Sets LEDOUT
 *
 * @param[in] uint8_t                                               deviceAddress           Device Address of PCA9624
 * @param[in] uint8_t                                               address                         LEDOUT address
 * @param[in] PCA9624_LEDOUT_typedef      ledout                            LEDOUT data
 *
 * @return ret_code_t
 */
ret_code_t PCA9624_setLedout(uint8_t deviceAddress,  uint8_t address, PCA9624_LEDOUT_typedef ledout);

/**@brief Get LEDOUT
 *
 * @param[in] uint8_t                                               deviceAddress           Device Address of PCA9624
 * @param[in] uint8_t                                               address                         LEDOUT address
 * @param[in] PCA9624_LEDOUT_typedef      ledout                            LEDOUT data
 *
 * @return ret_code_t
 */
ret_code_t PCA9624_getLedout(uint8_t deviceAddress,  uint8_t address, PCA9624_LEDOUT_typedef ledout);

/**@brief Software reset
 *
 * @param[in] uint8_t                                               deviceAddress           Device Address of PCA9624
 * @return ret_code_t
 */
ret_code_t PCA9624_reset(uint8_t deviceAddress);

/**@brief Puts the device in low-power mode, oscillator off
 *
 * @param[in] uint8_t                                               deviceAddress           Device Address of PCA9624
 * @return ret_code_t
 */
ret_code_t PCA9624_sleep(uint8_t deviceAddress);

/**@brief Wakes the device up
 *
 * @param[in] uint8_t                                               deviceAddress           Device Address of PCA9624
 * @return ret_code_t
 */
ret_code_t PCA9624_wakeup(uint8_t deviceAddress);

#ifdef __cplusplus
}
#endif

#endif // PCA9642_H__