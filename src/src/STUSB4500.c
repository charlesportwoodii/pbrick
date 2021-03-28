#include "STUSB4500.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include "sdk_common.h"
#include "pbrick_twi.h"
#include "nrf_log.h"

ret_code_t STUSB4500_init(uint8_t deviceAddress)
{
    ret_code_t ret;
    uint8_t data[40] = {0};
    uint8_t registerDeviceId = 0;

    ret = twi_rx(deviceAddress, STUSB4500_REG_DEVICE_ID, &registerDeviceId, 1);
    if (NRF_SUCCESS != ret) {
        NRF_LOG_WARNING("Failed to get device id");
        return ret;
    }

    if (registerDeviceId == 0x25) {
        NRF_LOG_ERROR("STUSB4500L is in use. Unable to configure.");
        return NRF_ERROR_NOT_SUPPORTED;
    }

    STUSB4500_pdo_sink_typedef pdoSink[3];
    ret = STUSB4500_read_sink_pdo(deviceAddress, &pdoSink[0]);

    // Clear interrupts 0x0D => 0x16
    for (int i = 0; i < 12; i++) {
        ret = twi_rx(deviceAddress, (STUSB4500_INTERRUPT + i), &data[0], 1);
        if (NRF_SUCCESS != ret) {
            NRF_LOG_WARNING("Failed to clear interrupt %x", (STUSB4500_INTERRUPT + i));
            return ret;
        }
    }

    // Set interrupt to umask (0xFF => ALERT_STATUS_MASK)
    data[0] = 0xFF;
    ret = twi_tx(deviceAddress, STUSB4500_ALERT_STATUS_MASK, &data[0], 1);
    if (NRF_SUCCESS != ret) {
        NRF_LOG_WARNING("Failed set interrupt mask");
        return ret;
    }
    return NRF_SUCCESS;
}

ret_code_t STUSB4500_soft_reset(uint8_t deviceAddress)
{
    ret_code_t ret;

    ret = STUSB4500_check_cable_attached(deviceAddress);
    if (NRF_SUCCESS == ret) {
        uint16_t data16 = STUSB4500_PD_HEADER_SOFTRESET;
        uint8_t data8[2] = {0};

        data8[0] = data16 & 0xFF;
        data8[1] = (data16 >> 8) & 0xFF;

        // Write soft reset to device
        ret = twi_tx(deviceAddress, STUSB4500_TX_HEADER_LOW, data8, 2);
        if (NRF_SUCCESS != ret) {
            NRF_LOG_WARNING("Unable to write soft-reset to device");
            return ret;
        }

        // Send reset command to device
        uint8_t newCommand = 0x26;
        ret = twi_tx(deviceAddress,  STUSB4500_PD_COMMAND_CTRL, &newCommand, 1);
        if (NRF_SUCCESS != ret) {
            NRF_LOG_WARNING("Unable to write command to device.");
            return ret;
        }

        return NRF_SUCCESS;
    }

    return NRF_ERROR_NOT_SUPPORTED;
}

ret_code_t STUSB4500_update_pdo(uint8_t deviceAddress, uint8_t pdo, int voltageMv, int currentMa)
{
    // Only allow PDO1, PDO2, and PDO3 to be set
    if (pdo <= 0 || pdo > 3) {
        return NRF_ERROR_NOT_SUPPORTED;
    }

    // Don't allow negative or 0 mv / ma configurations to prevent div/0
    if (voltageMv <= 50 || currentMa <= 10) {
        return NRF_ERROR_NOT_SUPPORTED;
    }

    uint32_t current = currentMa / 10;
    uint32_t voltage = voltageMv / 50;

    // Constrain voltage to a minimum of 5v
    if (voltageMv < 5000) {
        voltage = 100;
    }

    // Constrain voltage to a maximum of 20v
    if (voltageMv >= 20000) {
        voltage = 400;
    }

    //  Must have a minimum of .5A
    if (currentMa < 500) {
        current = 50;
    }

    // Max of 5A
    if (currentMa >= 5000) {
        current = 500;
    }

    // Force PDO1 to 5V (5000/50 = 100) per USB-PD specification
    if (pdo == 1) {
        voltage = 100;
    }

    ret_code_t ret;
    uint8_t address = STUSB4500_DPM_SNK_PDO1 + 4 * (pdo - 1);

    STUSB4500_pdo_sink_typedef pdoSink[3];
    ret = STUSB4500_read_sink_pdo(deviceAddress, &pdoSink[0]);

    uint32_t old = pdoSink[pdo - 1].d32;
    pdoSink[pdo - 1].fixed.OPERATIONAL_CURRENT = current;
    pdoSink[pdo - 1].fixed.VOLTAGE = voltage;

    uint8_t data[4] = {0};
    memcpy(data, &pdoSink[pdo - 1].d32, sizeof(pdoSink[pdo - 1].d32));
    NRF_LOG_DEBUG("Current: %X - Writing %X to PDO %x", old, data, pdo);

    // Transmit the changes to the device
    ret = twi_tx(deviceAddress, address, &data[0], 4);
    /*
    ret = twi_tx(deviceAddress, address, &data[0], 1);
    ret = twi_tx(deviceAddress, address + 1, &data[1], 1);
    ret = twi_tx(deviceAddress, address + 2, &data[2], 1);
    ret = twi_tx(deviceAddress, address + 3, &data[3], 1);
    */
    if (NRF_SUCCESS != ret) {
        NRF_LOG_DEBUG("Unable to update PDO data");
        return ret;
    }

    return ret;
}

ret_code_t STUSB4500_update_pdo_number(uint8_t deviceAddress, uint8_t number)
{
    ret_code_t ret;

    if (number >= 1 && number <= 3) {
        ret = twi_tx(deviceAddress, STUSB4500_DPM_PDO_NUMB, &number, 1);
        if (NRF_SUCCESS != ret) {
            NRF_LOG_DEBUG("Unable to update PDO");
            return ret;
        }

        NRF_LOG_DEBUG("Enabled %x PDO instances", number);
        return ret;
    }

    return NRF_ERROR_NOT_SUPPORTED;
}

ret_code_t STUSB4500_check_cable_attached(uint8_t deviceAddress)
{
    ret_code_t ret;
    uint8_t data;

    // Read CC attachment status
    ret = twi_rx(deviceAddress, STUSB4500_PORT_STATUS, &data, 1);
    if (NRF_SUCCESS != ret) {
        NRF_LOG_WARNING("Unable to read port status");
        return ret;
    }

    if ((data & STUSB4500_STUSBMASK_ATTACHED_STATUS) == 1) {
        ret = twi_rx(deviceAddress, STUSB4500_TYPE_C_STATUS, &data, 1);
        if (NRF_SUCCESS != ret) {
            NRF_LOG_WARNING("Unable to read Type-C status");
            return ret;
        }

        if ((data & STUSB4500_MASK_REVERSE) == 0) {
            NRF_LOG_INFO("CC1 orientation");
        } else {
            NRF_LOG_INFO("CC2 orientation");
        }

        return NRF_SUCCESS;
    }

    NRF_LOG_WARNING("USB Cable is not attached. Check connection and try again: %x", data);
    return NRF_ERROR_NOT_SUPPORTED;
}

ret_code_t STUSB4500_read_sink_pdo(uint8_t deviceAddress, STUSB4500_pdo_sink_typedef *pdoSink)
{
    ret_code_t ret;
    uint8_t data[12] = {0};

    ret = twi_rx(deviceAddress, STUSB4500_DPM_PDO_NUMB, &data[0], 1);

    if (NRF_SUCCESS != ret) {
        NRF_LOG_DEBUG("Unable to read PDO NUM %x %x", ret);
        return ret;
    }

    uint8_t pdoSinkNumber = (data[0] & 0x03);
    ret = twi_rx(deviceAddress, STUSB4500_DPM_SNK_PDO1, &data[0], (pdoSinkNumber * 4));
    if (NRF_SUCCESS != ret) {
        NRF_LOG_DEBUG("Unable to read PDO sink data");
        return ret;
    }

    int j = 0;
    for (int i = 0; i < 3; i++) {
        pdoSink[i].d32 = (uint32_t )( data[j] +(data[j+1] << 8) + (data[j+2] << 16) + (data[j+3] << 24));
        j+= 4;
    }

    return NRF_SUCCESS;
}

ret_code_t STUSB4500_print_sink_pdo(uint8_t deviceAddress)
{
    ret_code_t ret;
    STUSB4500_pdo_sink_typedef pdoSink[3];

    ret = STUSB4500_read_sink_pdo(deviceAddress, &pdoSink[0]);

    if (NRF_SUCCESS != ret) {
        NRF_LOG_DEBUG("Unable to read sink PDO status");
        return ret;
    }

    for (int i = 0; i < 3; i++) {
        float voltage =  pdoSink[i].fixed.VOLTAGE / 20;
        float current = pdoSink[i].fixed.OPERATIONAL_CURRENT / 100;
        float power = (voltage * current);

        NRF_LOG_INFO("PDO %x Status", i);
        NRF_LOG_INFO(" Voltage "NRF_LOG_FLOAT_MARKER, NRF_LOG_FLOAT(voltage));
        NRF_LOG_INFO(" Amperage: "NRF_LOG_FLOAT_MARKER, NRF_LOG_FLOAT(current));
        NRF_LOG_INFO(" Power: "NRF_LOG_FLOAT_MARKER, NRF_LOG_FLOAT(power));
    }

    return NRF_SUCCESS;
}

ret_code_t STUSB4500_get_current_sink_pdo(uint8_t deviceAddress, uint8_t *pdo)
{
    ret_code_t ret;
    uint8_t data;

    ret = twi_rx(deviceAddress, STUSB4500_DPM_PDO_NUMB, &data, 1);
    if (NRF_SUCCESS == ret) {
        *pdo = data;
    }

    return ret;
}