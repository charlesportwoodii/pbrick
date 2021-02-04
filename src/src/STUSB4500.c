#include "STUSB4500.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include "sdk_common.h"
#include "pbrick_twi.h"
#include "nrf_log.h"

ret_code_t STUSB4500_init(pbrick_twi *instance, uint8_t deviceAddress)
{
    ret_code_t ret;
    uint8_t data[40] = {0};
    uint8_t registerDeviceId = 0;

    ret = twi_rx(instance, deviceAddress, STUSB4500_REG_DEVICE_ID, &registerDeviceId, 1);
    NRF_LOG_INFO("Get Device ID Result: %x", ret);
    if (NRF_SUCCESS != ret) {
        NRF_LOG_INFO("Failed to get device id");
        return ret;
    }

    NRF_LOG_INFO("Device Address %x; Register Address: %x", deviceAddress, registerDeviceId);

    return ret;

    // Clear interrupts 0x0D => 0x16
    for (int i = 0; i < 12; i++) {
        ret = twi_rx(instance, deviceAddress, (STUSB4500_INTERRUPT + i), &data[0], 1);
        if (NRF_SUCCESS != ret) {
            NRF_LOG_INFO("Failed to clear interrupt %x", (STUSB4500_INTERRUPT + i));
            return ret;
        }
        NRF_LOG_INFO("Alert ID %x : %x", (STUSB4500_INTERRUPT + i), ret);
    }

    // Set interrupt to umask (0xFF => ALERT_STATUS_MASK)
    data[0] = 0xFF;
    ret = twi_tx(instance, deviceAddress, STUSB4500_ALERT_STATUS_MASK, &data[0], 1);
    if (NRF_SUCCESS != ret) {
        NRF_LOG_INFO("Failed set interrupt mask");
        return ret;
    }

    // Pull the port status from the device
    ret = twi_rx(instance, deviceAddress, STUSB4500_PORT_STATUS, &data[0], 10);
    if (NRF_SUCCESS != ret) {
        NRF_LOG_INFO("Failed to read port status");
        return ret;
    }

    NRF_LOG_INFO("Port Status");
    for (int i = 0; i < 10; i++) {
        NRF_LOG_INFO("Port Status: %x %x", i, data[i]);
    }

    return NRF_SUCCESS;
}

ret_code_t STUSB4500_soft_reset(pbrick_twi *instance, uint8_t deviceAddress)
{
    ret_code_t ret;
    uint8_t data[40] = {0};

    // Write 0x0D (SOFT_RESET) => 0x51 (TX_HEADER_LOW);
    data[0] = 0x0D; // SOFT_RESET
    ret = twi_tx(instance, deviceAddress, STUSB4500_TX_HEADER_LOW, &data[0], 1);
    if (NRF_SUCCESS != ret) {
        return ret;
    }

    // Write 0x26 (SEND_COMMAND) => 0x1A (PD_COMMAND_CTRL)
    data[0] = 0x26; // SEND_COMMAND
    ret = twi_tx(instance, deviceAddress, STUSB4500_PD_COMMAND_CTRL, &data[0], 1);
    if (NRF_SUCCESS != ret) {
        return ret;
    }

    return NRF_SUCCESS;
}