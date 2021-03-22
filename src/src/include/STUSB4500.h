#ifndef STUSB4500_H__
#define STUSB4500_H__

/** @usage
 * 
 * ret_code_t ret;
 * 
 * ret = STUSB4500_init(STUSB4500_DEVICE_ADDRESS); // Iniitialize the STUSB4500 instance on a given address
 * APP_ERROR_CHECK(ret);
 * 
 * ret  = STUSB4500_check_cable_attached(STUSB4500_DEVICE_ADDRESS); // Optional: Ensure the USB cable is attached
 * APP_ERROR_CHECK(ret);
 * 
 * ret = STUSB4500_update_pdo(STUSB4500_DEVICE_ADDRESS, 2, 9000, 1500); // Set PDO1 to 9V1.5A
 * APP_ERROR_CHECK(ret);
 * 
 * ret = STUSB4500_update_pdo(STUSB4500_DEVICE_ADDRESS, 3, 12000, 1000); // Set PDO2 = 12V1A
 * APP_ERROR_CHECK(ret);
 * 
 * ret = STUSB4500_update_pdo_number(STUSB4500_DEVICE_ADDRESS, 3); // Enable PDO1, PDO2, and PDO3
 * APP_ERROR_CHECK(ret);
 * 
 * ret = STUSB4500_soft_reset(STUSB4500_DEVICE_ADDRESS); // Perform a soft reset to change the voltage without VBUS interruption
 * APP_ERROR_CHECK(ret);
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "sdk_common.h"
#include "pbrick_twi.h"

#ifndef STUSB4500_DEVICE_ADDRESS
#define STUSB4500_DEVICE_ADDRESS        0x28
#endif

#define STUSB4500_REG_DEVICE_ID         0x2F
#define STUSB4500_INTERRUPT             0x0B
#define STUSB4500_ALERT_STATUS_MASK     0x0C
#define STUSB4500_PORT_STATUS           0x0E
#define STUSB4500_TX_HEADER_LOW         0x51
#define STUSB4500_TX_HEADER_HIGH        0x52
#define STUSB4500_PD_COMMAND_CTRL       0x1A
#define STUSB4500_DPM_SNK_PDO1          0x85
#define STUSB4500_DPM_PDO_NUMB 0x70
#define STUSB4500_PD_HEADER_SOFTRESET 0x000D
#define STUSB4500_STUSBMASK_ATTACHED_STATUS 0x01
#define STUSB4500_CC_STATUS 0x11
#define STUSB4500_MASK_REVERSE 0x80 //0: CC1 is attached 1: CC2 is Attach.
#define STUSB4500_TYPE_C_STATUS 0x15

#define STUSB4500_CC_CC1 0x01
#define STUSB4500_CC_CC2 0x02

#define STUSB4500_NVM_DEFAULTS  { 0x00, 0x00, 0xB0, 0xAA, 0x00, 0x45, 0x00, 0x00, 0x10, 0x40, 0x9C, 0x1C, 0xFF, 0x01, 0x3C, 0xDF, 0x02, 0x40, 0x0F, 0x00, 0x32, 0x00, 0xFC, 0xF1, 0x00, 0x19, 0x56, 0xAF, 0xF5, 0x35, 0x5F, 0x00, 0x00, 0x4B, 0x90, 0x21, 0x43, 0x00, 0x40, 0xFB }

#define STUSB4500_FTP_CUST_PASSWORD_REG 0x95
#define STUSB4500_FTP_CUST_PASSWORD  0x47

#define STUSB4500_FTP_CTRL_0  0x96
#define STUSB4500_FTP_CUST_PWR    0x80
#define STUSB4500_FTP_CUST_RST_N  0x40
#define STUSB4500_FTP_CUST_REQ    0x10
#define STUSB4500_FTP_CUST_SECT   0x07
#define STUSB4500_FTP_CTRL_1          0x97

#define STUSB4500_FTP_CUST_SER_MASK     0xF8
#define STUSB4500_FTP_CUST_OPCODE_MASK  0x07

#define STUSB4500_RW_BUFFER 0x53

#define STUSB4500_READ            0x00
#define STUSB4500_WRITE_PL        0x01
#define STUSB4500_WRITE_SER       0x02
#define STUSB4500_READ_PL         0x03
#define STUSB4500_READ_SER        0x04
#define STUSB4500_ERASE_SECTOR    0x05
#define STUSB4500_PROG_SECTOR     0x06
#define STUSB4500_SOFT_PROG_SECTOR 0x07

#define STUSB4500_SECTOR_0 0x01
#define STUSB4500_SECTOR_1 0x02
#define STUSB4500_SECTOR_2 0x04
#define STUSB4500_SECTOR_3 0x08
#define STUSB4500_SECTOR_4 0x10

typedef union
{
    uint32_t d32;
    struct {
        uint32_t OPERATIONAL_CURRENT :10;
        uint32_t VOLTAGE :10;
        uint8_t RESERVED_22_20  :3;
        uint8_t FAST_ROLE_REQ_CUR : 2;  /* must be set to 0 in 2.0*/
        uint8_t DUAL_ROLE_DATA    :1;
        uint8_t USB_COMMUNICATIONS_CAPABLE :1;
        uint8_t UNCONSTRAINED_POWER :1;
        uint8_t HIGHER_CAPABILITY :1;
        uint8_t DUAL_ROLE_POWER :1;
        uint8_t FIXED_SUPPLY :2;
    } fixed;
    struct {
        uint32_t OPERATING_CURRENT :10;
        uint32_t MIN_VOLTAGE:10;
        uint32_t MAX_VOLTAGE:10;
        uint8_t VARIABLE_SUPPLY:2; 
      } variable;
      struct {
        uint32_t OPERATING_POWER :10;
        uint32_t MIN_VOLTAGE:10;
        uint32_t MAX_VOLTAGE:10;
        uint8_t BATTERY:2; 
      } battery;   
} STUSB4500_pdo_sink_typedef;

typedef union
{       
        uint8_t d8;
        struct
        {
            uint8_t ccAttachState			:1;
            uint8_t ccVConnSupplyState 		:1;    
            uint8_t ccDataRole 			: 1;   
            uint8_t ccPowerRole 			:1; 
            uint8_t startupPowerMode 		: 1;    
            uint8_t ccAttachMode: 3;
        } b;
} STUSB4500_cc_detection_typedef;

/**@brief Initializes a STUSB4500 instance for TWI/I2C on-demand reconfiguration
 *
 * @param[in] uint8_t           deviceAddress   Device Address of STUSB4500 (0x28 default)
 *
 * @return ret_code_t
 */
ret_code_t STUSB4500_init(uint8_t deviceAddress);

/**@brief Sends a reset to STUSB4500 to force a new PDO_SINK configuration with VBUS_SOURCE
 *
 * @param[in] uint8_t           deviceAddress   Device Address of STUSB4500 (0x28 default)
 *
 * @return ret_code_t
 */
ret_code_t STUSB4500_soft_reset(uint8_t deviceAddress);

/**@brief Updates a given PDO instance with a specific voltage and current
 *
 * @param[in] uint8_t          deviceAddress   Device Address of STUSB4500 (0x28 default)
 * @oaram[in] uint8_t          pdo         The PDO instance to update (1-3)
 * @param[in] int                   voltageMv   Voltage to set in 50 milli-volt incriments
 * @param[in] int                   currentMa   Current to set in 50 milli-amp incriments
 * 
 * @return ret_code_t
 */
ret_code_t STUSB4500_update_pdo(uint8_t deviceAddress, uint8_t pdo, int voltageMv, int currentMa);

/**@brief Updates the number of active PDO instances
 * 
 * @param[in] uint8_t          deviceAddress   Device Address of STUSB4500 (0x28 default)
 * @param[in] uint8_t          number                The number of active PDO instances. Must be minimum of 1 (5V)
 *
* @return ret_code_t
 */
ret_code_t STUSB4500_update_pdo_number(uint8_t deviceAddress, uint8_t number);

/**@brief Checks if the USB Type-C cable is attached
 * 
 * @param[in] uint8_t          deviceAddress   Device Address of STUSB4500 (0x28 default)
 * 
 *  @return ret_code_t
 */
ret_code_t STUSB4500_check_cable_attached(uint8_t deviceAddress);

/**@brief Loads active PDO sink data into storage buffer
 * 
 * @param[in] uint8_t          deviceAddress   Device Address of STUSB4500 (0x28 default)
 * @param[in] STUSB4500_pdo_sink_typedef pdoSink Object container for PDO Sink Data
 * 
 *  @return ret_code_t
 */
ret_code_t STUSB4500_read_sink_pdo(uint8_t deviceAddress, STUSB4500_pdo_sink_typedef *pdoSink);

/**@brief Outputs active PDO sink data to RTT
 * 
 * @param[in] uint8_t          deviceAddress   Device Address of STUSB4500 (0x28 default)
 * 
 *  @return ret_code_t
 */
ret_code_t STUSB4500_print_sink_pdo(uint8_t deviceAddress);

/**@brief Gets the current PDO sink number
 * 
 * @param[in] uint8_t          deviceAddress   Device Address of STUSB4500 (0x28 default)
 * @param[out] uint8_t*     pdo                        The PDO number that's currently active
 * 
 *  @return ret_code_t
 */
ret_code_t STUSB4500_get_current_sink_pdo(uint8_t deviceAddress, uint8_t *pdo);

#ifdef __cplusplus
}
#endif

#endif // STUSB4500_H__

/** @} */