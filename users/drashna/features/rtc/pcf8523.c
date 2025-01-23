// Copyright 2022 Jose Pablo Ramirez <jp.ramangulo@gmail.com>
// Copyright 2023 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "pcf8523.h"
#include "i2c_master.h"
#include "debug.h"
#include <math.h>
#include "print.h"
#include "timer.h"

#include <stdio.h>

static bool pcf8523_initialized = false;

/**
 * @brief Sets the time on the RTC
 *
 * @param t Time structure to set
 * @return i2c_status_t
 */
i2c_status_t pcf8523_set_time(rtc_time_t t) {
    // clang-format off
    uint8_t data[7] = {
        rtc_bin2bcd(t.second),
        rtc_bin2bcd(t.minute),
        rtc_bin2bcd(t.hour),
        rtc_bin2bcd(t.date),
        rtc_bin2bcd(week_to_int(t.day_of_the_week)),
        rtc_bin2bcd(t.month),
        rtc_bin2bcd(t.year - 2000U)
    };

    if (i2c_write_register(PCF8523_I2C_ADDRESS << 1, PCF8523_TIME_REG, data, ARRAY_SIZE(data), PCF8523_I2C_TIMEOUT) != I2C_STATUS_SUCCESS) {
        uprintf("Error while sending time to RTC!\n");
        return I2C_STATUS_ERROR;
    }

    uint8_t status[1] = {0}; // set to battery switchover mode
    return i2c_write_register(PCF8523_I2C_ADDRESS << 1, PCF8523_CONTROL_3_REG, status, ARRAY_SIZE(status), PCF8523_I2C_TIMEOUT);
}

/**
 * @brief Get the time object from the RTC
 *
 * @param time Time struct to fill
 * @return i2c_status_t
 */
i2c_status_t pcf8523_get_time(rtc_time_t *time) {
    uint8_t data[7] = {0, 0, 0, 0, 0, 0, 0};

    i2c_status_t status = i2c_read_register(PCF8523_I2C_ADDRESS << 1, PCF8523_TIME_REG, data, ARRAY_SIZE(data), PCF8523_I2C_TIMEOUT);
    if (status != I2C_STATUS_SUCCESS) {
        return status;
    }

    time->year            = rtc_bcd2bin(data[6]) + 2000;   /* get year */
    time->month           = rtc_bcd2bin(data[5]);                                 /* get month */
    time->day_of_the_week = rtc_bcd2bin(data[4]);                                        /* get week */
    time->date            = rtc_bcd2bin(data[3]);                                        /* get date */
    time->am_pm           = (rtc_time_am_pm_t)((data[2] >> 5) & 0x01);                   /* get am pm */
    time->format          = (rtc_time_format_t)((data[2] >> 6) & 0x01);                  /* get format */
    time->minute          = rtc_bcd2bin(data[1]);                                        /* get minute */
    time->second          = rtc_bcd2bin(data[0] & 0x7F);                                        /* get second */

    if (time->format == RTC_FORMAT_12H) {        /* if 12H */
        time->hour = rtc_bcd2bin(data[2] & 0x1F); /* get hour */
    } else {
        time->hour = rtc_bcd2bin(data[2] & 0x3F); /* get hour */
    }

    time->unixtime = convert_to_unixtime(*time);

    return I2C_STATUS_SUCCESS;
}

/**
 * @brief Check the OSF bit to see if the RTC has lost power or crystal has lost sync
 *
 * @return true Has lost power and needs to be reset
 * @return false Is primed and ready to go
 */
bool pcf8523_has_lost_power(void) {
    uint8_t status[1] = {0};
    i2c_read_register(PCF8523_I2C_ADDRESS << 1, PCF8523_STATUS_REG, status, ARRAY_SIZE(status), PCF8523_I2C_TIMEOUT);
    return status[0] >> 7;
}


/**
 * @brief Initialize the RTC and performs startup checks.
 *
 * @return true
 * @return false
 */
bool pcf8523_init(rtc_time_t *time) {
    if (pcf8523_initialized) {
        return true;
    }
    i2c_init();

    uint8_t data[1] = {0};
    i2c_status_t status = i2c_read_register(PCF8523_I2C_ADDRESS << 1, PCF8523_CONTROL_3_REG, data, ARRAY_SIZE(data), PCF8523_I2C_TIMEOUT);
    pcf8523_initialized = (status == I2C_STATUS_SUCCESS) && ((data[0] & 0xE0) != 0xE0) ;
    if (pcf8523_initialized) {
        pcf8523_initialized = true;
        dprintf("PCF8523: Initialized and initial read performed\n");
    } else {
        dprintf("PCF8523: Failed to initialize\n");
    }

#ifdef RTC_FORCE_INIT
    if (true)
#else
    if (pcf8523_has_lost_power() || !pcf8523_initialized)
#endif  // RTC_FORCE_INIT
    {
        // If there is an issue with the RTC config, then manually set the RTC time to the compile time
        // It's not exact, but it's better than nothing. Can be adjusted manually, later.
        dprintf("PCF8523: Date/time not set. Setting to compiled date/time as fallback\n");
        status = pcf8523_set_time(convert_date_time(__DATE__, __TIME__));
        pcf8523_initialized = (status == I2C_STATUS_SUCCESS);
    }

    // check clock stop bit
    i2c_read_register(PCF8523_I2C_ADDRESS << 1, PCF8523_CONTROL_1_REG, data, ARRAY_SIZE(data), PCF8523_I2C_TIMEOUT);
    // if clock is stopped, start it
    if (data[0] & (1<<5)) {
        data[0] &= ~(1<<5);
        i2c_write_register(PCF8523_I2C_ADDRESS << 1, PCF8523_CONTROL_1_REG, data, ARRAY_SIZE(data), PCF8523_I2C_TIMEOUT);
    };
    return pcf8523_initialized;
}

/**
 * @brief Calibrates the RTC by setting the aging offset
 *
 * @param mode hour vs minute mode
 * @param offset the offset, 64 to -64
 * @return i2c_status_t
 */
i2c_status_t pcf8523_calibate(pcf8523_offset_mode_t mode, int8_t offset) {
    uint8_t data[1] = { ((uint8_t)offset & 0x7F) | mode };
    return i2c_write_register(PCF8523_I2C_ADDRESS << 1, PCF8523_OFFSET_REG, data, ARRAY_SIZE(data), PCF8523_I2C_TIMEOUT);
}

/**
 * @brief Runs the RTC task. Reads from the RTC every PCF8523_READ_INTERVAL, and stores value locally
 *
 */
bool pcf8523_task(rtc_time_t *time) {
    if (!pcf8523_initialized) {
        return false;
    }
    static bool connected = false;

    i2c_status_t status = pcf8523_get_time(time);
    switch (status) {
        case I2C_STATUS_SUCCESS:
            connected    = true;
            break;
        case I2C_STATUS_ERROR:
            pcf8523_initialized = connected = false;
            dprintf("PCF8523: I2C error when reading from RTC!\n");
            break;
        case I2C_STATUS_TIMEOUT:
            connected = false;
            dprintf("PCF8523: Timeout error while reading time from RTC!\n");
            break;
    }
    return connected;
}
