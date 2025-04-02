// Copyright 2024 burkfers (@burkfers)
// Copyright 2024 Wimads (@wimads)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "action.h"
#include "report.h"

// lower/higher value = curve starts more smoothly/abruptly
#ifndef POINTING_DEVICE_ACCEL_TAKEOFF
#    define POINTING_DEVICE_ACCEL_TAKEOFF 2.0
#endif // POINTING_DEVICE_ACCEL_TAKEOFF
// lower/higher value = curve reaches its upper limit slower/faster
#ifndef POINTING_DEVICE_ACCEL_GROWTH_RATE
#    define POINTING_DEVICE_ACCEL_GROWTH_RATE 0.25
#endif // POINTING_DEVICE_ACCEL_GROWTH_RATE
// lower/higher value = acceleration kicks in earlier/later
#ifndef POINTING_DEVICE_ACCEL_OFFSET
#    define POINTING_DEVICE_ACCEL_OFFSET 2.2
#endif // POINTING_DEVICE_ACCEL_OFFSET
// lower limit of accel curve (minimum acceleration factor)
#ifndef POINTING_DEVICE_ACCEL_LIMIT
#    define POINTING_DEVICE_ACCEL_LIMIT 0.2
#endif // POINTING_DEVICE_ACCEL_LIMIT
// milliseconds to wait between requesting the device's current DPI
#ifndef POINTING_DEVICE_ACCEL_CPI_THROTTLE_MS
#    define POINTING_DEVICE_ACCEL_CPI_THROTTLE_MS 200
#endif // POINTING_DEVICE_ACCEL_CPI_THROTTLE_MS
// upper limit of accel curve, recommended to leave at 1; adjust DPI setting instead.
#ifndef POINTING_DEVICE_ACCEL_LIMIT_UPPER
#    define POINTING_DEVICE_ACCEL_LIMIT_UPPER 1
#endif // POINTING_DEVICE_ACCEL_LIMIT_UPPER
// milliseconds after which to reset quantization error correction (forget rounding remainder)
#ifndef POINTING_DEVICE_ACCEL_ROUNDING_CARRY_TIMEOUT_MS
#    define POINTING_DEVICE_ACCEL_ROUNDING_CARRY_TIMEOUT_MS 200
#endif // POINTING_DEVICE_ACCEL_ROUNDING_CARRY_TIMEOUT_MS

typedef struct PACKED pointing_device_accel_config_t {
    float growth_rate;
    float offset;
    float limit;
    float takeoff;
    bool  enabled;
} pointing_device_accel_config_t;

extern pointing_device_accel_config_t g_pointing_device_accel_config;

void pointing_device_accel_enabled(bool enable);
bool pointing_device_accel_get_enabled(void);
void pointing_device_accel_toggle_enabled(void);

float pointing_device_accel_get_takeoff(void);
void  pointing_device_accel_set_takeoff(float val);
void  pointing_device_accel_takeoff_increment(void);

float pointing_device_accel_get_growth_rate(void);
void  pointing_device_accel_set_growth_rate(float val);
void  pointing_device_accel_growth_rate_increment(void);

float pointing_device_accel_get_offset(void);
void  pointing_device_accel_set_offset(float val);
void  pointing_device_accel_offset_increment(void);

float pointing_device_accel_get_limit(void);
void  pointing_device_accel_set_limit(float val);
void  pointing_device_accel_set_limit_increment(void);

void pointing_device_config_update(pointing_device_accel_config_t *config);
void pointing_device_config_read(pointing_device_accel_config_t *config);
