// Copyright 2021 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// Copyright 2024 burkfers (@burkfers)
// Copyright 2024 Wimads (@wimads)
// SPDX-License-Identifier: GPL-3.0-or-later

#include QMK_KEYBOARD_H
#include "pointing_device_internal.h"
#include "pointing_device_accel.h"
#include "math.h"

static uint32_t pointing_device_accel_timer;

// lower/higher value = curve starts more smoothly/abruptly
#ifndef MACCEL_TAKEOFF
#    define MACCEL_TAKEOFF 2.0
#endif // MACCEL_TAKEOFF
// lower/higher value = curve reaches its upper limit slower/faster
#ifndef MACCEL_GROWTH_RATE
#    define MACCEL_GROWTH_RATE 0.25
#endif // MACCEL_GROWTH_RATE
// lower/higher value = acceleration kicks in earlier/later
#ifndef MACCEL_OFFSET
#    define MACCEL_OFFSET 2.2
#endif // MACCEL_OFFSET
// lower limit of accel curve (minimum acceleration factor)
#ifndef MACCEL_LIMIT
#    define MACCEL_LIMIT 0.2
#endif // MACCEL_LIMIT
// milliseconds to wait between requesting the device's current DPI
#ifndef MACCEL_CPI_THROTTLE_MS
#    define MACCEL_CPI_THROTTLE_MS 200
#endif // MACCEL_CPI_THROTTLE_MS
// upper limit of accel curve, recommended to leave at 1; adjust DPI setting instead.
#ifndef MACCEL_LIMIT_UPPER
#    define MACCEL_LIMIT_UPPER 1
#endif // MACCEL_LIMIT_UPPER
// milliseconds after which to reset quantization error correction (forget rounding remainder)
#ifndef MACCEL_ROUNDING_CARRY_TIMEOUT_MS
#    define MACCEL_ROUNDING_CARRY_TIMEOUT_MS 200
#endif // MACCEL_ROUNDING_CARRY_TIMEOUT_MS

pointing_device_accel_config_t g_pointing_device_accel_config = {
    // clang-format off
    .growth_rate =  MACCEL_GROWTH_RATE,
    .offset =       MACCEL_OFFSET,
    .limit =        MACCEL_LIMIT,
    .takeoff =      MACCEL_TAKEOFF,
    .enabled =      true
    // clang-format on
};

#ifndef MACCEL_TAKEOFF_STEP
#    define MACCEL_TAKEOFF_STEP 0.01f
#endif
#ifndef MACCEL_GROWTH_RATE_STEP
#    define MACCEL_GROWTH_RATE_STEP 0.01f
#endif
#ifndef MACCEL_OFFSET_STEP
#    define MACCEL_OFFSET_STEP 0.1f
#endif
#ifndef MACCEL_LIMIT_STEP
#    define MACCEL_LIMIT_STEP 0.01f
#endif
#

float pointing_device_accel_get_takeoff(void) {
    return g_pointing_device_accel_config.takeoff;
}
void pointing_device_accel_set_takeoff(float val) {
    if (val >= 0.5) { // value less than 0.5 leads to nonsensical results
        g_pointing_device_accel_config.takeoff = val;
    }
}
float pointing_device_accel_get_growth_rate(void) {
    return g_pointing_device_accel_config.growth_rate;
}
void pointing_device_accel_set_growth_rate(float val) {
    if (val >= 0) { // value less 0 leads to nonsensical results
        g_pointing_device_accel_config.growth_rate = val;
    }
}
float pointing_device_accel_get_offset(void) {
    return g_pointing_device_accel_config.offset;
}
void pointing_device_accel_set_offset(float val) {
    g_pointing_device_accel_config.offset = val;
}
float pointing_device_accel_get_limit(void) {
    return g_pointing_device_accel_config.limit;
}
void pointing_device_accel_set_limit(float val) {
    if (val >= 0) {
        g_pointing_device_accel_config.limit = val;
    }
}

void pointing_device_accel_enabled(bool enable) {
    g_pointing_device_accel_config.enabled = enable;
    pd_dprintf("maccel: enabled: %d\n", g_pointing_device_accel_config.enabled);
}
bool pointing_device_accel_get_enabled(void) {
    return g_pointing_device_accel_config.enabled;
}
void pointing_device_accel_toggle_enabled(void) {
    pointing_device_accel_enabled(!pointing_device_accel_get_enabled());
}

#define _CONSTRAIN(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
#define CONSTRAIN_REPORT(val)      (mouse_xy_report_t) _CONSTRAIN(val, XY_REPORT_MIN, XY_REPORT_MAX)

report_mouse_t pointing_device_task_pointing_device_accel(report_mouse_t mouse_report) {
    // rounding carry to recycle dropped floats from int mouse reports, to smoothen low speed movements (credit
    // @ankostis)
    static float rounding_carry_x = 0;
    static float rounding_carry_y = 0;
    // time since last mouse report:
    const uint16_t delta_time = timer_elapsed32(pointing_device_accel_timer);
    // skip pointing_device_accel maths if report = 0, or if pointing_device_accel not enabled.
    if ((mouse_report.x == 0 && mouse_report.y == 0) || !g_pointing_device_accel_config.enabled) {
        return mouse_report;
    }
    // reset timer:
    pointing_device_accel_timer = timer_read32();
    // Reset carry if too much time passed
    if (delta_time > MACCEL_ROUNDING_CARRY_TIMEOUT_MS) {
        rounding_carry_x = 0;
        rounding_carry_y = 0;
    }
    // Reset carry when pointer swaps direction, to follow user's hand.
    if (mouse_report.x * rounding_carry_x < 0) rounding_carry_x = 0;
    if (mouse_report.y * rounding_carry_y < 0) rounding_carry_y = 0;
    // Limit expensive calls to get device cpi settings only when mouse stationary for > 200ms.
    static uint16_t device_cpi = 300;
    if (delta_time > MACCEL_CPI_THROTTLE_MS) {
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
    const float k = g_pointing_device_accel_config.takeoff;
    const float g = g_pointing_device_accel_config.growth_rate;
    const float s = g_pointing_device_accel_config.offset;
    const float m = g_pointing_device_accel_config.limit;
    // acceleration factor: f(v) = 1 - (1 - M) / {1 + e^[K(v - S)]}^(G/K):
    // Generalised Sigmoid Function, see https://www.desmos.com/calculator/k9vr0y2gev
    const float pointing_device_accel_factor =
        MACCEL_LIMIT_UPPER - (MACCEL_LIMIT_UPPER - m) / powf(1 + expf(k * (velocity - s)), g / k);
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
               device_cpi, g_pointing_device_accel_config.takeoff, g_pointing_device_accel_config.growth_rate, g_pointing_device_accel_config.offset,
               g_pointing_device_accel_config.limit, pointing_device_accel_factor, velocity, velocity_out, CONSTRAIN_REPORT(distance),
               CONSTRAIN_REPORT(distance_out));
#endif // MACCEL_DEBUG

    // report back accelerated values
    mouse_report.x = x;
    mouse_report.y = y;

    return pointing_device_task_pointing_device_accel_kb(mouse_report);
}

static inline float get_mod_step(float step) {
    const uint8_t mod_mask = get_mods();
    if (mod_mask & MOD_MASK_CTRL) {
        step *= 10; // control increases by factor 10
    }
    if (mod_mask & MOD_MASK_SHIFT) {
        step *= -1; // shift inverts
    }
    return step;
}

bool process_record_pointing_device_accel(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_pointing_device_accel_kb(keycode, record)) {
        return true;
    }

    if (record->event.pressed) {
        switch (keycode) {
            case CM_MOUSE_ACCEL_TOGGLE:
                pointing_device_accel_toggle_enabled();
                return false;
            case CM_MOUSE_ACCEL_TAKEOFF:
                pointing_device_accel_set_takeoff(pointing_device_accel_get_takeoff() + get_mod_step(MACCEL_TAKEOFF_STEP));
                pd_dprintf("MACCEL:keycode: TKO: %.3f gro: %.3f ofs: %.3f lmt: %.3f\n", g_pointing_device_accel_config.takeoff,
                           g_pointing_device_accel_config.growth_rate, g_pointing_device_accel_config.offset, g_pointing_device_accel_config.limit);
                return false;
            case CM_MOUSE_ACCEL_GROWTH_RATE:
                pointing_device_accel_set_growth_rate(pointing_device_accel_get_growth_rate() + get_mod_step(MACCEL_GROWTH_RATE_STEP));
                pd_dprintf("MACCEL:keycode: tko: %.3f GRO: %.3f ofs: %.3f lmt: %.3f\n", g_pointing_device_accel_config.takeoff,
                           g_pointing_device_accel_config.growth_rate, g_pointing_device_accel_config.offset, g_pointing_device_accel_config.limit);
                return false;
            case CM_MOUSE_ACCEL_OFFSET:
                pointing_device_accel_set_offset(pointing_device_accel_get_offset() + get_mod_step(MACCEL_OFFSET_STEP));
                pd_dprintf("MACCEL:keycode: tko: %.3f gro: %.3f OFS: %.3f lmt: %.3f\n", g_pointing_device_accel_config.takeoff,
                           g_pointing_device_accel_config.growth_rate, g_pointing_device_accel_config.offset, g_pointing_device_accel_config.limit);
                return false;
            case CM_MOUSE_ACCEL_LIMIT:
                pointing_device_accel_set_limit(pointing_device_accel_get_limit() + get_mod_step(MACCEL_LIMIT_STEP));
                pd_dprintf("MACCEL:keycode: tko: %.3f gro: %.3f ofs: %.3f LMT: %.3f\n", g_pointing_device_accel_config.takeoff,
                           g_pointing_device_accel_config.growth_rate, g_pointing_device_accel_config.offset, g_pointing_device_accel_config.limit);
                return false;
        }
    }
    return true;
}
