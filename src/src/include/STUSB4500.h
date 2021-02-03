#ifndef STUSB4500_H__
#define STUSB4500_H__

#include "nrf_drv_twi.h"

#ifdef __cplusplus
extern "C" {
#endif

#define STUSB4500_DEFAULT_ADDRESS                        0x28
#define STUSB4500_DEFAULT                                            0xFF

#define STUSB4500_FTP_CUST_PASSWORD_REG       0x95
#define STUSB4500_FTP_CUST_PASSWORD                 0x47

#define STUSB4500_FTP_CTRL_0                                     0x96
#define STUSB4500_FTP_CUST_PWR                              0x80 
#define STUSB4500_FTP_CUST_RST_N                           0x40
#define STUSB4500_FTP_CUST_REQ                               0x10
#define STUSB4500_FTP_CUST_SECT                              0x07
#define STUSB4500_FTP_CTRL_1                                     0x97
#define STUSB4500_FTP_CUST_SER                                0xF8
#define STUSB4500_FTP_CUST_OPCODE                      0x07
#define STUSB4500_RW_BUFFER                                     0x53

#define STUSB4500_READ                                                  0x00
#define STUSB4500_WRITE_PL                                         0x01
#define STUSB4500_WRITE_SER                                      0x02
#define STUSB4500_ERASE_SECTOR                              0x05
#define STUSB4500_PROG_SECTOR                               0x06
#define STUSB4500_SOFT_PROG_SECTOR                  0x07

#define STUSB4500_SECTOR_0                                        0x01
#define STUSB4500_SECTOR_1                                        0x02
#define STUSB4500_SECTOR_2                                        0x04
#define STUSB4500_SECTOR_3                                        0x08
#define STUSB4500_SECTOR_4                                        0x10

ret_code_t STUSB4500_init(const nrf_drv_twi_t *m_twi_master, uint8_t deviceAddress);

#ifdef __cplusplus
}
#endif

#endif // STUSB4500_H__

/** @} */