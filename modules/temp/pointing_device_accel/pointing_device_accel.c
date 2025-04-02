// Copyright 2021 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// Copyright 2024 burkfers (@burkfers)
// Copyright 2024 Wimads (@wimads)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "pointing_device_internal.h"
#include "pointing_device_accel.h"
#include "math.h"

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 1, 0);

static uint32_t pointing_device_accel_timer;

static pointing_device_accel_config_t *pointing_device_accel_config = NULL;

void set_pointing_device_accel_config_pointer(pointing_device_accel_config_t *config) {
    *pointing_device_accel_config = *config;
}

pointing_device_accel_config_t *pointing_device_accel_config_get_pointer(void) {
    return pointing_device_accel_config;
}

#ifndef POINTING_DEVICE_ACCEL_TAKEOFF_STEP
#    define POINTING_DEVICE_ACCEL_TAKEOFF_STEP 0.01f
#endif
#ifndef POINTING_DEVICE_ACCEL_GROWTH_RATE_STEP
#    define POINTING_DEVICE_ACCEL_GROWTH_RATE_STEP 0.01f
#endif
#ifndef POINTING_DEVICE_ACCEL_OFFSET_STEP
#    define POINTING_DEVICE_ACCEL_OFFSET_STEP 0.1f
#endif
#ifndef POINTING_DEVICE_ACCEL_LIMIT_STEP
#    define POINTING_DEVICE_ACCEL_LIMIT_STEP 0.01f
#endif

float pointing_device_accel_get_takeoff(void) {
    return pointing_device_accel_config->takeoff;
}

void pointing_device_accel_set_takeoff(float val) {
    if (val >= 0.5) { // value less than 0.5 leads to nonsensical results
        pointing_device_accel_config->takeoff = val;
        pointing_device_accel_config_update(pointing_device_accel_config);
    }
}

float pointing_device_accel_get_growth_rate(void) {
    return pointing_device_accel_config->growth_rate;
}
void pointing_device_accel_set_growth_rate(float val) {
    if (val >= 0) { // value less 0 leads to nonsensical results
        pointing_device_accel_config->growth_rate = val;
        pointing_device_accel_config_update(pointing_device_accel_config);
    }
}

float pointing_device_accel_get_offset(void) {
    return pointing_device_accel_config->offset;
}

void pointing_device_accel_set_offset(float val) {
    pointing_device_accel_config->offset = val;
    pointing_device_accel_config_update(pointing_device_accel_config);
}

float pointing_device_accel_get_limit(void) {
    return pointing_device_accel_config->limit;
}

void pointing_device_accel_set_limit(float val) {
    if (val >= 0) {
        pointing_device_accel_config->limit = val;
        pointing_device_accel_config_update(pointing_device_accel_config);
    }
}

void pointing_device_accel_enabled(bool enable) {
    pointing_device_accel_config->enabled = enable;
    pointing_device_accel_config_update(pointing_device_accel_config);
    pd_dprintf("maccel: enabled: %d\n", pointing_device_accel_config->enabled);
}

bool pointing_device_accel_get_enabled(void) {
    return pointing_device_accel_config->enabled;
}

void pointing_device_accel_toggle_enabled(void) {
    pointing_device_accel_enabled(!pointing_device_accel_get_enabled());
}

#define _CONSTRAIN(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
#define CONSTRAIN_REPORT(val)      (mouse_xy_report_t) _CONSTRAIN(val, XY_REPORT_MIN, XY_REPORT_MAX)

report_mouse_t pointing_device_task_pointing_device_accel(report_mouse_t mouse_report) {
    if (pointing_device_accel_config == NULL) {
        return mouse_report;
    }
    
    // rounding carry to recycle dropped floats from int mouse reports, to smoothen low speed movements (credit
    // @ankostis)
    static float rounding_carry_x = 0;
    static float rounding_carry_y = 0;
    // time since last mouse report:
    const uint16_t delta_time = timer_elapsed32(pointing_device_accel_timer);
    // skip pointing_device_accel maths if report = 0, or if pointing_device_accel not enabled.
    if ((mouse_report.x == 0 && mouse_report.y == 0) || !pointing_device_accel_config->enabled) {
        return mouse_report;
    }
    // reset timer:
    pointing_device_accel_timer = timer_read32();
    // Reset carry if too much time passed
    if (delta_time > POINTING_DEVICE_ACCEL_ROUNDING_CARRY_TIMEOUT_MS) {
        rounding_carry_x = 0;
        rounding_carry_y = 0;
    }
    // Reset carry when pointer swaps direction, to follow user's hand.
    if (mouse_report.x * rounding_carry_x < 0) rounding_carry_x = 0;
    if (mouse_report.y * rounding_carry_y < 0) rounding_carry_y = 0;
    // Limit expensive calls to get device cpi settings only when mouse stationary for > 200ms.
    static uint16_t device_cpi = 300;
    if (delta_time > POINTING_DEVICE_ACCEL_CPI_THROTTLE_MS) {
        device_cpi = pointing_device_get_cpi();
    }
    // calculate dpi correction factor (for normalizing velocity range across different user dpi settings)
    const float dpi_correction = (float)1000.0f / device_cpi;
    // calculate euclidean distance moved (sqrt(x^2 + y^2))
    const float distance = sqrtf(mouse_report.x * mouse_report.x + mouse_report.y * mouse_report.y);
    // calculate delta velocity: dv = distance/dt
    const float velocity_raw = distance / delta_time;
    // correct raw velocity for dpi
    const float velocity = dpi_correction * velocity_raw;
    // letter variables for readability of maths:
    const float k = pointing_device_accel_config->takeoff;
    const float g = pointing_device_accel_config->growth_rate;
    const float s = pointing_device_accel_config->offset;
    const float m = pointing_device_accel_config->limit;
    // acceleration factor: f(v) = 1 - (1 - M) / {1 + e^[K(v - S)]}^(G/K):
    // Generalised Sigmoid Function, see https://www.desmos.com/calculator/k9vr0y2gev
    const float pointing_device_accel_factor =
        POINTING_DEVICE_ACCEL_LIMIT_UPPER -
        (POINTING_DEVICE_ACCEL_LIMIT_UPPER - m) / powf(1 + expf(k * (velocity - s)), g / k);
    // multiply mouse reports by acceleration factor, and account for previous quantization errors:
    const float new_x = rounding_carry_x + pointing_device_accel_factor * mouse_report.x;
    const float new_y = rounding_carry_y + pointing_device_accel_factor * mouse_report.y;
    // Accumulate any difference from next integer (quantization).
    rounding_carry_x = new_x - (int)new_x;
    rounding_carry_y = new_y - (int)new_y;
    // clamp values
    const mouse_xy_report_t x = CONSTRAIN_REPORT(new_x);
    const mouse_xy_report_t y = CONSTRAIN_REPORT(new_y);

// console output for debugging (enable/disable in config.h)
#ifdef POINTING_DEVICE_DEBUG
    const float distance_out = sqrtf(x * x + y * y);
    const float velocity_out = velocity * pointing_device_accel_factor;
    pd_dprintf("MACCEL: DPI:%4i Tko: %.3f Grw: %.3f Ofs: %.3f Lmt: %.3f | Fct: %.3f v.in: %.3f v.out: %.3f d.in: %3i "
               "d.out: %3i\n",
               device_cpi, pointing_device_accel_config->takeoff, pointing_device_accel_config->growth_rate,
               pointing_device_accel_config->offset, pointing_device_accel_config->limit, pointing_device_accel_factor,
               velocity, velocity_out, CONSTRAIN_REPORT(distance), CONSTRAIN_REPORT(distance_out));
#endif // POINTING_DEVICE_ACCEL_DEBUG

    // report back accelerated values
    mouse_report.x = x;
    mouse_report.y = y;

    return pointing_device_task_pointing_device_accel_kb(mouse_report);
}

float pointing_device_accel_get_mod_step(float step) {
    const uint8_t mod_mask = get_mods();
    if (mod_mask & MOD_MASK_CTRL) {
        step *= 10; // control increases by factor 10
    }
    if (mod_mask & MOD_MASK_SHIFT) {
        step *= -1; // shift inverts
    }
    return step;
}

void pointing_device_accel_takeoff_increment(void) {
    pointing_device_accel_set_takeoff(pointing_device_accel_get_takeoff() +
                                      pointing_device_accel_get_mod_step(POINTING_DEVICE_ACCEL_TAKEOFF_STEP));
    pd_dprintf("MACCEL:keycode: TKO: %.3f gro: %.3f ofs: %.3f lmt: %.3f\n", pointing_device_accel_config->takeoff,
               pointing_device_accel_config->growth_rate, pointing_device_accel_config->offset,
               pointing_device_accel_config->limit);
}

void pointing_device_accel_growth_rate_increment(void) {
    pointing_device_accel_set_growth_rate(pointing_device_accel_get_growth_rate() +
                                          pointing_device_accel_get_mod_step(POINTING_DEVICE_ACCEL_GROWTH_RATE_STEP));
    pd_dprintf("MACCEL:keycode: tko: %.3f GRO: %.3f ofs: %.3f lmt: %.3f\n", pointing_device_accel_config->takeoff,
               pointing_device_accel_config->growth_rate, pointing_device_accel_config->offset,
               pointing_device_accel_config->limit);
}

void pointing_device_accel_offset_increment(void) {
    pointing_device_accel_set_offset(pointing_device_accel_get_offset() +
                                     pointing_device_accel_get_mod_step(POINTING_DEVICE_ACCEL_OFFSET_STEP));
    pd_dprintf("MACCEL:keycode: tko: %.3f gro: %.3f OFS: %.3f lmt: %.3f\n", pointing_device_accel_config->takeoff,
               pointing_device_accel_config->growth_rate, pointing_device_accel_config->offset,
               pointing_device_accel_config->limit);
}

void pointing_device_accel_set_limit_increment(void) {
    pointing_device_accel_set_limit(pointing_device_accel_get_limit() +
                                    pointing_device_accel_get_mod_step(POINTING_DEVICE_ACCEL_LIMIT_STEP));
    pd_dprintf("MACCEL:keycode: tko: %.3f gro: %.3f ofs: %.3f LMT: %.3f\n", pointing_device_accel_config->takeoff,
               pointing_device_accel_config->growth_rate, pointing_device_accel_config->offset,
               pointing_device_accel_config->limit);
}

bool process_record_pointing_device_accel(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_pointing_device_accel_kb(keycode, record)) {
        return true;
    }

    if (record->event.pressed) {
        switch (keycode) {
            case CM_MOUSE_ACCEL_TOGGLE:
                pointing_device_accel_toggle_enabled();
                break;
            case CM_MOUSE_ACCEL_TAKEOFF:
                pointing_device_accel_takeoff_increment();
                break;
            case CM_MOUSE_ACCEL_GROWTH_RATE:
                pointing_device_accel_growth_rate_increment();
                break;
            case CM_MOUSE_ACCEL_OFFSET:
                pointing_device_accel_offset_increment();
                break;
            case CM_MOUSE_ACCEL_LIMIT:
                pointing_device_accel_set_limit_increment();
                break;
        }
    }
    return true;
}

__attribute__((weak)) void pointing_device_accel_config_update(pointing_device_accel_config_t *config) {
    // Co nothing since we're saving/storing in memory.
    // Can be overridden to implement NVM storage.  VIA module does this, in fact.
}

__attribute__((weak)) void pointing_device_accel_config_read(pointing_device_accel_config_t *config) {
    config->growth_rate = POINTING_DEVICE_ACCEL_GROWTH_RATE;
    config->offset      = POINTING_DEVICE_ACCEL_OFFSET;
    config->limit       = POINTING_DEVICE_ACCEL_LIMIT;
    config->takeoff     = POINTING_DEVICE_ACCEL_TAKEOFF;
    config->enabled     = true;
}

__attribute__((weak)) void keyboard_post_init_pointing_device_accel(void) {
    pointing_device_accel_config_t config;
    // Read initial config into memory.
    // Via module reads settings from NVM into memory.
    pointing_device_accel_config_read(&config);
    set_pointing_device_accel_config_pointer(&config);

    keyboard_post_init_pointing_device_accel_kb();
}

// Unfortunately, there is no way to automatically call this as of 1.0.0 or 1.1.0
// Compounding this is that NVM abstraction prevents us from saving to arbitrary locations, safely.
// We could still do so, but to future proof this .... for now, it needs to be called from either
// eeconfig_init_user(void) or eeconfig_init_kb(void).
void eeconfig_init_pointing_device(void) {
    pointing_device_accel_config_t config = (pointing_device_accel_config_t){
        .growth_rate = POINTING_DEVICE_ACCEL_GROWTH_RATE,
        .offset      = POINTING_DEVICE_ACCEL_OFFSET,
        .limit       = POINTING_DEVICE_ACCEL_LIMIT,
        .takeoff     = POINTING_DEVICE_ACCEL_TAKEOFF,
        .enabled     = true,
    };
    set_pointing_device_accel_config_pointer(&config);
    pointing_device_accel_config_update(&config);
}
