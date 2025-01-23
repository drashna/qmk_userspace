// Copyright 2023 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ds1307.h"
#include "i2c_master.h"
#include "debug.h"
#include <math.h>
#include "print.h"
#include "timer.h"
#include "util.h"

#include <stdio.h>

static bool ds1307_initialized = false;

/**
 * @brief Sets the time on the RTC
 *
 * @param t Time structure to set
 * @return i2c_status_t
 */
i2c_status_t ds1307_set_time(rtc_time_t t) {
    // clang-format off
    uint8_t data[7] = {
        rtc_bin2bcd(t.second) & 0x7F,
        rtc_bin2bcd(t.minute),
        rtc_bin2bcd(t.hour),
        rtc_bin2bcd(week_to_int(t.day_of_the_week)),
        rtc_bin2bcd(t.date),
        rtc_bin2bcd(t.month),
        rtc_bin2bcd(t.year - 2000U)
    };
    // clang-format on

    if (i2c_write_register(DS1307_I2C_ADDRESS << 1, DS1307_TIME_REG, data, ARRAY_SIZE(data), DS1307_I2C_TIMEOUT) !=
        I2C_STATUS_SUCCESS) {
        uprintf("Error while sending time to RTC!\n");
        return I2C_STATUS_ERROR;
    }

    return I2C_STATUS_SUCCESS;
}

/**
 * @brief Get the time object from the RTC
 *
 * @param t Time struct to fill
 * @return i2c_status_tn
 */
i2c_status_t ds1307_get_time(rtc_time_t *time) {
    uint8_t data[7] = {0, 0, 0, 0, 0, 0, 0};

    i2c_status_t status =
        i2c_read_register(DS1307_I2C_ADDRESS << 1, DS1307_TIME_REG, data, ARRAY_SIZE(data), DS1307_I2C_TIMEOUT);
    if (status != I2C_STATUS_SUCCESS) {
        return status;
    }

    time->year            = rtc_bcd2bin(data[6]) + 2000;                /* get year */
    time->month           = rtc_bcd2bin(data[5]);                       /* get month */
    time->day_of_the_week = rtc_bcd2bin(data[3]);                       /* get week */
    time->date            = rtc_bcd2bin(data[4]);                       /* get date */
    time->am_pm           = (rtc_time_am_pm_t)((data[2] >> 5) & 0x01);  /* get am pm */
    time->format          = (rtc_time_format_t)((data[2] >> 6) & 0x01); /* get format */
    time->minute          = rtc_bcd2bin(data[1]);                       /* get minute */
    time->second          = rtc_bcd2bin(data[0] & 0x7F);                /* get second */

    if (time->format == RTC_FORMAT_12H) {         /* if 12H */
        time->hour = rtc_bcd2bin(data[2] & 0x1F); /* get hour */
    } else {
        time->hour = rtc_bcd2bin(data[2] & 0x3F); /* get hour */
    }

    time->unixtime = convert_to_unixtime(*time);

    return I2C_STATUS_SUCCESS;
}

/**
 * @brief Check the Clock Halt bit (bit 7 of the seconds register) to see if clock is running or not
 *
 * @return true  Has lost power and needs to be reset
 * @return false Is primed and ready to go
 */
bool ds1307_has_lost_power(void) {
    uint8_t status[1] = {0};
    i2c_read_register(DS1307_I2C_ADDRESS << 1, DS1307_TIME_REG, status, ARRAY_SIZE(status), DS1307_I2C_TIMEOUT);
    return status[0] >> 7;
}

/**
 * @brief Initialize the RTC and performs startup checks.
 *
 * @return true
 * @return false
 */
bool ds1307_init(rtc_time_t *time) {
    if (ds1307_initialized) {
        return true;
    }
    i2c_init();

    i2c_status_t status = ds1307_get_time(time);
    ds1307_initialized  = (status == I2C_STATUS_SUCCESS);
    if (ds1307_initialized) {
        ds1307_initialized = true;
        dprintf("DS1307: Initialized and initial read performed\n");
    } else {
        dprintf("DS1307: Failed to initialize\n");
    }

#ifdef RTC_FORCE_INIT
    if (true)
#else
    if (ds1307_has_lost_power())
#endif // RTC_FORCE_INIT
    {
        // If there is an issue with the RTC config, then manually set the RTC time to the compile time
        // It's not exact, but it's better than nothing. Can be adjusted manually, later.
        xprintf("DS1307: Clock not running. Setting to compiled date/time as fallback\n");
        ds1307_set_time(convert_date_time(__DATE__, __TIME__));
    }
    return ds1307_initialized;
}

/**
 * @brief Runs the RTC task. Reads from the RTC every DS1307_READ_INTERVAL, and stores value locally
 *
 */
bool ds1307_task(rtc_time_t *time) {
    if (!ds1307_initialized) {
        return false;
    }
    static bool connected = false;

    i2c_status_t status = ds1307_get_time(time);
    switch (status) {
        case I2C_STATUS_SUCCESS:
            connected = true;
            break;
        case I2C_STATUS_ERROR:
            ds1307_initialized = connected = false;
            dprintf("DS1307: I2C error when reading from RTC!\n");
            break;
        case I2C_STATUS_TIMEOUT:
            connected = false;
            dprintf("DS1307: Timeout error while reading time from RTC!\n");
            break;
    }
    return connected;
}

/**
 * @brief Reads from the onboard NVRAM on the ds1307 RTC chip. Registers 0x08 - 0x3F are available for use.
 *        The first 8 bytes are reserved for the RTC registers.
 *
 * @param address Read from address starting at offset 0x00 for ease.
 * @param data Read data from RTC into this buffer
 * @param len Length of array to read
 * @return i2c_status_t
 */
i2c_status_t ds1307_read_nvram(uint8_t address, uint8_t *data, uint16_t len) {
    address += DS1307_NVRAM_REG;
    if (address > 0x3F) {
        return I2C_STATUS_ERROR;
    } else if (address + len > 0x3F) {
        len = 0x3F - address;
    }
    return i2c_read_register(DS1307_I2C_ADDRESS << 1, address, data, len, DS1307_I2C_TIMEOUT);
}

/**
 * @brief Writes to the onboard NVRAM on the ds1307 RTC chip. Registers 0x08 - 0x3F are available for use.
 *        The first 8 bytes are reserved for the RTC registers, and writing sequentially past 0x3F will overwrite
 *        the RTC registers.
 * @param address   Write to address starting at offset 0x00 for ease.
 * @param data Write data to RTC from this buffer
 * @param len Length of array to write
 * @return i2c_status_t
 */
i2c_status_t ds1307_write_nvram(uint8_t address, uint8_t *data, uint16_t len) {
    address += DS1307_NVRAM_REG;
    if (address > 0x3F) {
        return I2C_STATUS_ERROR;
    } else if (address + len > 0x3F) {
        len = 0x3F - address;
    }
    return i2c_write_register(DS1307_I2C_ADDRESS << 1, address, data, len, DS1307_I2C_TIMEOUT);
}
