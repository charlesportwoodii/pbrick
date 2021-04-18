#ifndef PCA9685_H__
#define PCA9685_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sdk_common.h"
#include "pbrick_twi.h"


#ifndef PCA9685_DEBUG
#define PCA9685_DEBUG 0
#endif

#define PCA9685_DEVICE_ID 0xF8 /** 3 Byte Device ID **/
#define PCA9685_MODE1 0x00      /**< Mode Register 1 */
#define PCA9685_MODE2 0x01      /**< Mode Register 2 */
#define PCA9685_SUBADR1 0x02    /**< I2C-bus subaddress 1 */
#define PCA9685_SUBADR2 0x03    /**< I2C-bus subaddress 2 */
#define PCA9685_SUBADR3 0x04    /**< I2C-bus subaddress 3 */
#define PCA9685_ALLCALLADR 0x05 /**< LED All Call I2C-bus address */
#define PCA9685_LED0_ON_L 0x06  /**< LED0 on tick, low byte*/
#define PCA9685_LED0_ON_H 0x07  /**< LED0 on tick, high byte*/
#define PCA9685_LED0_OFF_L 0x08 /**< LED0 off tick, low byte */
#define PCA9685_LED0_OFF_H 0x09 /**< LED0 off tick, high byte */

// etc all 16:  LED15_OFF_H 0x45
#define PCA9685_ALLLED_ON_L 0xFA  /**< load all the LEDn_ON registers, low */
#define PCA9685_ALLLED_ON_H 0xFB  /**< load all the LEDn_ON registers, high */
#define PCA9685_ALLLED_OFF_L 0xFC /**< load all the LEDn_OFF registers, low */
#define PCA9685_ALLLED_OFF_H 0xFD /**< load all the LEDn_OFF registers,high */
#define PCA9685_PRESCALE 0xFE     /**< Prescaler for PWM output frequency */
#define PCA9685_TESTMODE 0xFF     /**< defines the test mode to be entered */

typedef union {
    uint8_t d8;
    struct {
        uint8_t allcall : 1;
        uint8_t sub3 : 1;
        uint8_t sub2 : 1;
        uint8_t sub1 : 1;
        uint8_t sleep : 1;
        uint8_t ai : 1;
        uint8_t extclk : 1;
        uint8_t restart : 1;
    } data;
} PCA9685_MODE1_typedef;

typedef union {
    uint8_t d8;
    struct {
        uint8_t outne : 1;
        uint8_t outdrv : 1;
        uint8_t och : 1;
        uint8_t invrt : 1;
        uint8_t reserved3 : 1;
        uint8_t reserved2 : 1;
        uint8_t reserved1  : 1;
    } data;
} PCA9685_MODE2_typedef;

typedef union {
    uint8_t d8;
    struct {
        uint8_t reserved: 3;
        uint8_t LED : 1;
        uint8_t LED_H: 4;
    } data;
} PCA9685_LED_typedef;

// MODE1 bits
#define PCA9685_MODE1_ALLCAL 0x01  /**< respond to LED All Call I2C-bus address */
#define PCA9685_MODE1_SUB3 0x02    /**< respond to I2C-bus subaddress 3 */
#define PCA9685_MODE1_SUB2 0x04    /**< respond to I2C-bus subaddress 2 */
#define PCA9685_MODE1_SUB1 0x08    /**< respond to I2C-bus subaddress 1 */
#define PCA9685_MODE1_SLEEP 0x10   /**< Low power mode. Oscillator off */
#define PCA9685_MODE1_AI 0x20      /**< Auto-Increment enabled */
#define PCA9685_MODE1_EXTCLK 0x40  /**< Use EXTCLK pin clock */
#define PCA9685_MODE1_RESTART 0x80 /**< Restart enabled */

// MODE2 bits
#define PCA9685_MODE2_OUTNE_0 0x01 /**< Active LOW output enable input */
#define PCA9685_MODE2_OUTNE_1  0x02 /**< Active LOW output enable input - high impedience */
#define PCA9685_MODE2_OUTDRV 0x04 /**< totem pole structure vs open-drain */
#define PCA9685_MODE2_OCH 0x08    /**< Outputs change on ACK vs STOP */
#define PCA9685_MODE2_INVRT 0x10  /**< Output logic state inverted */

#define PCA9685_FREQUENCY_OSCILLATOR 25000000 /**< Int. osc. frequency in datasheet */

#define PCA9685_PRESCALE_MIN 3   /**< minimum prescale value */
#define PCA9685_PRESCALE_MAX 255 /**< maximum prescale value */

ret_code_t PCA9685_init(uint8_t deviceAddress);
ret_code_t PCA9685_getMode1(uint8_t deviceAddress, PCA9685_MODE1_typedef *mode);
ret_code_t PCA9685_setMode1(uint8_t deviceAddress, PCA9685_MODE1_typedef *mode);
ret_code_t PCA9685_getMode2(uint8_t deviceAddress, PCA9685_MODE2_typedef *mode);
ret_code_t PCA9685_setMode2(uint8_t deviceAddress, PCA9685_MODE2_typedef *mode);
ret_code_t PCA9685_restart(uint8_t deviceAddress);
ret_code_t PCA9685_sleep(uint8_t deviceAddress);
ret_code_t PCA9685_wakeup(uint8_t deviceAddress);
ret_code_t PCA9685_setPWMFrequency(uint8_t deviceAddress, float frequency);
ret_code_t PCA9685_setOutputMode(uint8_t deviceAddress, bool totempole);
ret_code_t PCA9685_getPWM(uint8_t deviceAddress, uint8_t num, uint8_t *pwm);
ret_code_t PCA9685_setPWM(uint8_t deviceAddress, uint8_t num, uint16_t on, uint16_t off);
ret_code_t PCA9685_setPin(uint8_t deviceAddress, uint8_t num, uint16_t val, bool invert);
ret_code_t PCA9685_writeMicroseconds(uint8_t deviceAddress, uint8_t num, uint16_t microseconds);

uint32_t PCA9685_getOscillatorFrequency();
void PCA9685_setOscillatorFrequency(uint32_t frequency);

#ifdef __cplusplus
}
#endif

#endif // PCA9685_H__

/** @} */