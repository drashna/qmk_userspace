// Copyright 2022 Jose Pablo Ramirez <jp.ramangulo@gmail.com>
// Copyright 2023 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ds3231.h"
#include "i2c_master.h"
#include "debug.h"
#include <math.h>
#include "print.h"
#include "timer.h"

#include <stdio.h>

static float ds3231_temp        = 0.0f;
static bool  ds3231_initialized = false;

/**
 * @brief Sets the time on the RTC
 *
 * @param t Time structure to set
 * @return i2c_status_t
 */
i2c_status_t ds3231_set_time(rtc_time_t t) {
    // clang-format off
    uint8_t data[7] = {
        rtc_bin2bcd(t.second),
        rtc_bin2bcd(t.minute),
        rtc_bin2bcd(t.hour),
        rtc_bin2bcd(week_to_int(t.day_of_the_week)),
        rtc_bin2bcd(t.date),
        rtc_bin2bcd(t.month),
        rtc_bin2bcd(t.year - 2000U)
    };
    // clang-format on

    if (i2c_write_register(DS3231_I2C_ADDRESS << 1, DS3231_TIME_REG, data, ARRAY_SIZE(data), DS3231_I2C_TIMEOUT) !=
        I2C_STATUS_SUCCESS) {
        dprintf("Error while sending time to RTC!\n");
        return I2C_STATUS_ERROR;
    }

    uint8_t status[1] = {0};

    if (i2c_read_register(DS3231_I2C_ADDRESS << 1, DS3231_STATUS_REG, status, ARRAY_SIZE(status), DS3231_I2C_TIMEOUT) !=
        I2C_STATUS_SUCCESS) {
        dprintf("Error while reading status!\n");
        return I2C_STATUS_ERROR;
    }

    status[0] &= ~0x80; // flip OSF bit

    return i2c_write_register(DS3231_I2C_ADDRESS << 1, DS3231_STATUS_REG, status, ARRAY_SIZE(status),
                              DS3231_I2C_TIMEOUT);
}

/**
 * @brief Get the time object from the RTC
 *
 * @param time Time struct to fill
 * @return i2c_status_t
 */
i2c_status_t ds3231_get_time(rtc_time_t *time) {
    uint8_t data[7] = {0, 0, 0, 0, 0, 0, 0};

    i2c_status_t status =
        i2c_read_register(DS3231_I2C_ADDRESS << 1, DS3231_TIME_REG, data, ARRAY_SIZE(data), DS3231_I2C_TIMEOUT);
    if (status != I2C_STATUS_SUCCESS) {
        return status;
    }

    time->year            = rtc_bcd2bin(data[6]) + 2000 + ((data[5] >> 7) & 0x01) * 100; /* get year */
    time->month           = rtc_bcd2bin(data[5] & 0x1F);                                 /* get month */
    time->day_of_the_week = rtc_bcd2bin(data[3]);                                        /* get week */
    time->date            = rtc_bcd2bin(data[4]);                                        /* get date */
    time->am_pm           = (rtc_time_am_pm_t)((data[2] >> 5) & 0x01);                   /* get am pm */
    time->format          = (rtc_time_format_t)((data[2] >> 6) & 0x01);                  /* get format */
    time->minute          = rtc_bcd2bin(data[1]);                                        /* get minute */
    time->second          = rtc_bcd2bin(data[0]);                                        /* get second */

    if (time->format == RTC_FORMAT_12H) {         /* if 12H */
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
bool ds3231_has_lost_power(void) {
    uint8_t status[1] = {0};
    i2c_read_register(DS3231_I2C_ADDRESS << 1, DS3231_STATUS_REG, status, ARRAY_SIZE(status), DS3231_I2C_TIMEOUT);
    return status[0] >> 7;
}

/**
 * @brief Get the current value for the aging offset
 *
 * @return int8_t Aging offset
 */
int8_t ds3231_get_aging_offset(void) {
    uint8_t offset[1] = {0};
    i2c_read_register(DS3231_I2C_ADDRESS << 1, DS3231_AGING_OFFSET_REG, offset, ARRAY_SIZE(offset), DS3231_I2C_TIMEOUT);
    return (int8_t)offset[0];
}

/**
 * @brief Sets a new value for the aging offset
 *
 * @param offset
 */
void ds3231_set_aging_offset(int8_t offset) {
    uint8_t data[1] = {(uint8_t)offset};
    i2c_write_register(DS3231_I2C_ADDRESS << 1, DS3231_AGING_OFFSET_REG, data, ARRAY_SIZE(data), DS3231_I2C_TIMEOUT);
}

/**
 * @brief Get the temperature from the RTC
 *
 * @return float
 */
float ds3231_get_temperature(void) {
    uint8_t temp[2] = {0};

    i2c_read_register(DS3231_I2C_ADDRESS << 1, DS3231_TEMPERATURE_REG, temp, ARRAY_SIZE(temp), DS3231_I2C_TIMEOUT);
    return (float)temp[0] + (float)(temp[1] >> 6) * 0.25;
}

/**
 * @brief Initialize the RTC and performs startup checks.
 *
 * @return true
 * @return false
 */
bool ds3231_init(rtc_time_t *time) {
    if (ds3231_initialized) {
        return true;
    }
    i2c_init();

    i2c_status_t status = ds3231_get_time(time);
    ds3231_initialized  = (status == I2C_STATUS_SUCCESS);
    if (ds3231_initialized) {
        ds3231_initialized = true;
        dprintf("DS3231: Initialized and initial read performed\n");
    } else {
        dprintf("DS3231: Failed to initialize\n");
    }

#ifdef RTC_FORCE_INIT
    if (true)
#else
    if (ds3231_has_lost_power())
#endif // RTC_FORCE_INIT
    {
        // If there is an issue with the RTC config, then manually set the RTC time to the compile time
        // It's not exact, but it's better than nothing. Can be adjusted manually, later.
        dprintf("DS3231: Date/time not set. Setting to compiled date/time as fallback\n");
        ds3231_set_time(convert_date_time(__DATE__, __TIME__));
    }
    return ds3231_initialized;
}

/**
 * @brief Runs the RTC task. Reads from the RTC every DS3231_READ_INTERVAL, and stores value locally
 *
 */
bool ds3231_task(rtc_time_t *time) {
    if (!ds3231_initialized) {
        return false;
    }
    static bool connected = false;

    i2c_status_t status = ds3231_get_time(time);
    switch (status) {
        case I2C_STATUS_SUCCESS:
            ds3231_temp = ds3231_get_temperature();
            connected   = true;
            break;
        case I2C_STATUS_ERROR:
            ds3231_initialized = connected = false;
            dprintf("DS3231: I2C error when reading from RTC!\n");
            break;
        case I2C_STATUS_TIMEOUT:
            connected = false;
            dprintf("DS3231: Timeout error while reading time from RTC!\n");
            break;
    }
    return connected;
}

char *ds3231_read_temp_str(void) {
    static char temp_str[8] = {0};
#if PRINTF_SUPPORT_DECIMAL_SPECIFIERS
    snprintf(temp_str, sizeof(temp_str), "%2.2f", ds3231_temp);
#else // PRINTF_SUPPORT_DECIMAL_SPECIFIERS
#    pragma message "PRINTF_SUPPORT_DECIMAL_SPECIFIERS not enabled, RTC temperature will be rounded to nearest integer"
    uint16_t temp = (uint16_t)(ds3231_temp * 100);
    snprintf(temp_str, sizeof(temp_str), "%2d.%02d", temp / 100, temp % 100);
#endif // PRINTF_SUPPORT_DECIMAL_SPECIFIERS
    return temp_str;
}

char *ds3231_read_temp_imperial_str(void) {
    static char temp_str[8] = {0};
    float       temp_f      = (ds3231_temp * 9.0f / 5.0f) + 32.0f;
#if PRINTF_SUPPORT_DECIMAL_SPECIFIERS
    snprintf(temp_str, sizeof(temp_str), "%2.2f", temp_f);
#else // PRINTF_SUPPORT_DECIMAL_SPECIFIERS
#    pragma message "PRINTF_SUPPORT_DECIMAL_SPECIFIERS not enabled, RTC temperature will be rounded to nearest integer"
    uint16_t temp = (uint16_t)(temp_f * 100);
    snprintf(temp_str, sizeof(temp_str), "%2d.%02d", temp / 100, temp % 100);
#endif // PRINTF_SUPPORT_DECIMAL_SPECIFIERS
    return temp_str;
}

float ds3231_read_temp(void) {
    return ds3231_temp;
}
