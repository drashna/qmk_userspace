// Copyright 2021 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// Copyright 2024 burkfers (@burkfers)
// Copyright 2024 Wimads (@wimads)
// SPDX-License-Identifier: GPL-3.0-or-later

#include "pointing_device_auto_mouse.h"
#include "keyrecords/process_records.h"
#include "action_util.h"
#include "drashna_layers.h"
#include "drashna_runtime.h"
#include "drashna_util.h"
#include "pointing.h"
#include "math.h"
#include <stdlib.h>
#ifdef AUDIO_ENABLE
#    include "audio.h"
#    ifdef AUDIO_CLICKY
#        include "process_clicky.h"
#    endif // AUDIO_CLICKY
#endif     // AUDIO_ENABLE

static uint16_t mouse_debounce_timer = 0;

#ifdef TAPPING_TERM_PER_KEY
#    define TAP_CHECK get_tapping_term(QK_MOUSE_BUTTON_1, NULL)
#else // TAPPING_TERM_PER_KEY
#    ifndef TAPPING_TERM
#        define TAPPING_TERM 200
#    endif
#    define TAP_CHECK TAPPING_TERM
#endif // TAPPING_TERM_PER_KEY
#ifdef AUDIO_ENABLE
#    ifndef POINTING_ACCEL_ON_SONG
#        define POINTING_ACCEL_ON_SONG SONG(ZSA_STARTUP)
#    endif // POINTING_ACCEL_ON_SONG
#    ifndef POINTING_ACCEL_OFF_SONG
#        define POINTING_ACCEL_OFF_SONG SONG(ZSA_GOODBYE)
#    endif // POINTING_ACCEL_OFF_SONG
static float accel_on_song[][2]  = POINTING_ACCEL_ON_SONG;
static float accel_off_song[][2] = POINTING_ACCEL_OFF_SONG;
#endif // AUDIO_ENABLE

#ifndef MOUSE_JIGGLER_THRESHOLD
#    define MOUSE_JIGGLER_THRESHOLD 20
#endif // MOUSE_JIGGLER_THRESHOLD
#ifndef MOUSE_JIGGLER_INTERVAL_MS
#    define MOUSE_JIGGLER_INTERVAL_MS 16
#endif // MOUSE_JIGGLER_INTERVAL_MS

#define _CONSTRAIN(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
#define CONSTRAIN_REPORT(val)      (mouse_xy_report_t) _CONSTRAIN(val, XY_REPORT_MIN, XY_REPORT_MAX)

static uint16_t     mouse_jiggler_timer          = 0;
static uint32_t     mouse_jiggler_debounce_timer = 0;
static const int8_t deltas[32]                   = {0, -1, -2, -2, -3, -3, -4, -4, -4, -4, -3, -3, -2, -2, -1, 0,
                                                    0, 1,  2,  2,  3,  3,  4,  4,  4,  4,  3,  3,  2,  2,  1,  0};
typedef struct {
    mouse_xy_report_t x;
    mouse_xy_report_t y;
    mouse_hv_report_t v;
    mouse_hv_report_t h;
} mouse_movement_t;
mouse_movement_t total_mouse_movement = {0, 0, 0, 0};

static uint32_t pointing_device_accel_timer = 0;

#ifdef AUDIO_ENABLE
// brackets: 1st is number of buttons, 2nd is number of notes, 3rd is number of octaves
// Increase the first 2,if you want more than 2 notes here
float fp_mouse_sounds[][2][2] = {
    SONG(FP_MOUSE_SOUND_1),
    SONG(FP_MOUSE_SOUND_2),
    SONG(FP_MOUSE_SOUND_3),
};

_Static_assert(ARRAY_SIZE(fp_mouse_sounds) <= 8, "Too many mouse sounds defined");

uint8_t pointing_device_handle_buttons(uint8_t buttons, bool pressed, pointing_device_buttons_t button) {
    uint8_t button_mask = 1 << button;

    if ((buttons & button_mask) != (pressed ? button_mask : 0) && pressed && button <= ARRAY_SIZE(fp_mouse_sounds) &&
        !is_clicky_on() && userspace_config.pointing.audio_mouse_clicky) {
        PLAY_SONG(fp_mouse_sounds[button]);
    }

    if (pressed) {
        buttons |= button_mask;
    } else {
        buttons &= ~button_mask;
    }

    return buttons;
}
#endif // AUDIO_ENABLE

__attribute__((weak)) void pointing_device_init_keymap(void) {}

void pointing_device_init_user(void) {
    set_auto_mouse_enable(userspace_config.pointing.auto_mouse_layer.enable);
    set_auto_mouse_layer(userspace_config.pointing.auto_mouse_layer.layer);
    set_auto_mouse_timeout(userspace_config.pointing.auto_mouse_layer.timeout);
    set_auto_mouse_debounce(userspace_config.pointing.auto_mouse_layer.debounce);

    pointing_device_init_keymap();
}

bool mouse_movement_threshold_check(report_mouse_t* mouse_report, mouse_movement_t* movement, uint16_t threshold) {
    movement->x += mouse_report->x;
    movement->y += mouse_report->y;
    movement->h += mouse_report->h;
    movement->v += mouse_report->v;
    return abs(movement->x) > threshold || abs(movement->y) > threshold || abs(movement->h) > threshold ||
           abs(movement->v) > threshold;
}

void mouse_jiggler_check(report_mouse_t* mouse_report) {
    static mouse_movement_t jiggler_threshold = {0, 0, 0, 0};
    if (mouse_movement_threshold_check(mouse_report, &jiggler_threshold, MOUSE_JIGGLER_THRESHOLD)) {
        userspace_runtime_state.pointing.mouse_jiggler.running = false;
        jiggler_threshold                                      = (mouse_movement_t){.x = 0, .y = 0, .h = 0, .v = 0};
    }
    if (userspace_config.pointing.mouse_jiggler.enable &&
        timer_elapsed(mouse_jiggler_timer) > MOUSE_JIGGLER_INTERVAL_MS) {
        if (userspace_runtime_state.pointing.mouse_jiggler.running) {
            static uint8_t phase = 0;
            mouse_report->x += deltas[phase];
            mouse_report->y += deltas[(phase + 8) & 31];
            phase = (phase + 1) & 31;
        } else if (timer_elapsed32(mouse_jiggler_debounce_timer) >
                       (userspace_config.pointing.mouse_jiggler.timeout * 1000) &&
                   !is_device_suspended()) {
            userspace_runtime_state.pointing.mouse_jiggler.running = true;
        }
        jiggler_threshold   = (mouse_movement_t){.x = 0, .y = 0, .h = 0, .v = 0};
        mouse_jiggler_timer = timer_read();
    }
}

__attribute__((weak)) report_mouse_t pointing_device_task_keymap(report_mouse_t mouse_report) {
    return mouse_report;
}

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    static report_mouse_t last_mouse_report = {0};
    mouse_jiggler_check(&mouse_report);

    if (memcmp(&mouse_report, &last_mouse_report, sizeof(report_mouse_t)) != 0) {
        last_mouse_report            = mouse_report;
        mouse_jiggler_debounce_timer = timer_read32();
    }

    if (timer_elapsed(mouse_debounce_timer) < TAP_CHECK) {
        mouse_report.x = 0;
        mouse_report.y = 0;
    }
    // rounding carry to recycle dropped floats from int mouse reports, to smoothen low speed movements (credit
    // @ankostis)
    static float rounding_carry_x = 0;
    static float rounding_carry_y = 0;
    // time since last mouse report:
    const uint16_t delta_time = timer_elapsed32(pointing_device_accel_timer);
    // skip maccel maths if report = 0, or if maccel not enabled.
    if ((mouse_report.x == 0 && mouse_report.y == 0) || !userspace_config.pointing.accel.enabled) {
        return pointing_device_task_keymap(mouse_report);
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
    const float k = userspace_config.pointing.accel.takeoff;
    const float g = userspace_config.pointing.accel.growth_rate;
    const float s = userspace_config.pointing.accel.offset;
    const float m = userspace_config.pointing.accel.limit;
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
#ifdef POINTING_DEVICE_ACCEL_DEBUG
    const float distance_out = sqrtf(x * x + y * y);
    const float velocity_out = velocity * pointing_device_accel_factor;
    printf("MACCEL: DPI:%4i Tko: %.3f Grw: %.3f Ofs: %.3f Lmt: %.3f | Fct: %.3f v.in: %.3f v.out: %.3f d.in: %3i "
           "d.out: %3i\n",
           device_cpi, userspace_config.pointing.takeoff, userspace_config.pointing.growth_rate,
           userspace_config.pointing.offset, userspace_config.pointing.limit, pointing_device_accel_factor, velocity,
           velocity_out, CONSTRAIN_REPORT(distance), CONSTRAIN_REPORT(distance_out));
#endif // POINTING_DEVICE_ACCEL_DEBUG

    // report back accelerated values
    mouse_report.x = x;
    mouse_report.y = y;

    return pointing_device_task_keymap(mouse_report);
}

bool process_record_pointing(uint16_t keycode, keyrecord_t* record) {
    switch (keycode) {
        case PD_JIGGLER:
            if (record->event.pressed) {
                pointing_device_mouse_jiggler_toggle();
            }
            break;
        case PD_ACCEL_TOGGLE:
            if (record->event.pressed) {
                pointing_device_accel_toggle_enabled();
            }
            break;
        case PD_ACCEL_TAKEOFF:
            if (record->event.pressed) {
                pointing_device_accel_set_takeoff(
                    pointing_device_accel_get_takeoff() +
                    pointing_device_accel_get_mod_step(POINTING_DEVICE_ACCEL_TAKEOFF_STEP));
#ifdef POINTING_DEVICE_ACCEL_DEBUG
                printf("MACCEL:keycode: TKO: %.3f gro: %.3f ofs: %.3f lmt: %.3f\n",
                       userspace_config.pointing.accel.takeoff, userspace_config.pointing.accel.growth_rate,
                       userspace_config.pointing.accel.offset, userspace_config.pointing.accel.limit);
#endif // POINTING_DEVICE_ACCEL_DEBUG
            }
            break;
        case PD_ACCEL_GROWTH_RATE:
            if (record->event.pressed) {
                pointing_device_accel_set_growth_rate(
                    pointing_device_accel_get_growth_rate() +
                    pointing_device_accel_get_mod_step(POINTING_DEVICE_ACCEL_GROWTH_RATE_STEP));
#ifdef POINTING_DEVICE_ACCEL_DEBUG
                printf("MACCEL:keycode: tko: %.3f GRO: %.3f ofs: %.3f lmt: %.3f\n",
                       userspace_config.pointing.accel.takeoff, userspace_config.pointing.accel.growth_rate,
                       userspace_config.pointing.accel.offset, userspace_config.pointing.accel.limit);
#endif // POINTING_DEVICE_ACCEL_DEBUG
            }
            break;
        case PD_ACCEL_OFFSET:
            if (record->event.pressed) {
                pointing_device_accel_set_offset(pointing_device_accel_get_offset() +
                                                 pointing_device_accel_get_mod_step(POINTING_DEVICE_ACCEL_OFFSET_STEP));
#ifdef POINTING_DEVICE_ACCEL_DEBUG
                printf("MACCEL:keycode: tko: %.3f gro: %.3f OFS: %.3f lmt: %.3f\n",
                       userspace_config.pointing.accel.takeoff, userspace_config.pointing.accel.growth_rate,
                       userspace_config.pointing.accel.offset, userspace_config.pointing.accel.limit);
#endif // POINTING_DEVICE_ACCEL_DEBUG
            }
            break;
        case PD_ACCEL_LIMIT:
            if (record->event.pressed) {
                pointing_device_accel_set_limit(pointing_device_accel_get_limit() +
                                                pointing_device_accel_get_mod_step(POINTING_DEVICE_ACCEL_LIMIT_STEP));
#ifdef POINTING_DEVICE_ACCEL_DEBUG
                printf("MACCEL:keycode: tko: %.3f gro: %.3f ofs: %.3f LMT: %.3f\n",
                       userspace_config.pointing.accel.takeoff, userspace_config.pointing.accel.growth_rate,
                       userspace_config.pointing.accel.offset, userspace_config.pointing.accel.limit);
#endif // POINTING_DEVICE_ACCEL_DEBUG
            }
            break;
        default:
            if (!IS_MOUSE_KEYCODE(keycode)) {
                mouse_debounce_timer         = timer_read();
                mouse_jiggler_debounce_timer = timer_read32();
                if (userspace_runtime_state.pointing.mouse_jiggler.running && record->event.pressed) {
                    userspace_runtime_state.pointing.mouse_jiggler.running = false;
                }
            }
            break;
    }
    return true;
}

layer_state_t layer_state_set_pointing(layer_state_t state) {
    if (layer_state_cmp(state, _GAMEPAD) || layer_state_cmp(state, _DIABLO) || layer_state_cmp(state, _DIABLOII)) {
        state |= ((layer_state_t)1 << _MOUSE);
        set_auto_mouse_enable(false); // auto mouse can be disabled any time during run time
    } else {
        set_auto_mouse_enable(userspace_config.pointing.auto_mouse_layer.enable);
    }
    return state;
}

bool has_mouse_report_changed(report_mouse_t* new_report, report_mouse_t* old_report) {
    return ((new_report->buttons != old_report->buttons) || (new_report->x != 0 && new_report->x != old_report->x) ||
            (new_report->y != 0 && new_report->y != old_report->y) ||
            (new_report->h != 0 && new_report->h != old_report->h) ||
            (new_report->v != 0 && new_report->v != old_report->v));
}

#if defined(POINTING_DEVICE_AUTO_MOUSE_ENABLE)
__attribute__((weak)) bool is_mouse_record_keymap(uint16_t keycode, keyrecord_t* record) {
    return false;
}

bool is_mouse_record_user(uint16_t keycode, keyrecord_t* record) {
    if (is_mouse_record_keymap(keycode, record)) {
        return true;
    }
    switch (keycode) {
#    if defined(KEYBOARD_ploopy)
        case DPI_CONFIG:
#    elif (defined(KEYBOARD_bastardkb_charybdis) || defined(KEYBOARD_handwired_tractyl_manuform)) && \
            !defined(NO_CHARYBDIS_KEYCODES) ||                                                       \
        (defined(KEYBOARD_bastardkb_dilemma) && !defined(NO_DILEMMA_KEYCODES))
        case QK_KB ... QK_KB_MAX:
#    endif
        case PD_JIGGLER ... PD_ACCEL_LIMIT:
            return true;
    }
    return false;
}
#endif

void pointing_device_mouse_jiggler_toggle(void) {
    mouse_jiggler_timer          = timer_read();
    mouse_jiggler_debounce_timer = timer_read32() + (userspace_config.pointing.mouse_jiggler.timeout - 5) * 1000;
    userspace_config.pointing.mouse_jiggler.enable = !userspace_config.pointing.mouse_jiggler.enable;
    eeconfig_update_user_datablock(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
}

/**
 * @brief Gets the takeoff value for the acceleration curve
 *
 * @return float
 */
float pointing_device_accel_get_takeoff(void) {
    return userspace_config.pointing.accel.takeoff;
}

/**
 * @brief Gets the growth rate for the acceleration curve
 *
 * @return float
 */
float pointing_device_accel_get_growth_rate(void) {
    return userspace_config.pointing.accel.growth_rate;
}

/**
 * @brief Gets the offset for the acceleration curve
 *
 * @return float
 */
float pointing_device_accel_get_offset(void) {
    return userspace_config.pointing.accel.offset;
}

/**
 * @brief Gets the limit for the acceleration curve
 *
 * @return float
 */
float pointing_device_accel_get_limit(void) {
    return userspace_config.pointing.accel.limit;
}

/**
 * @brief Sets the takeoff value for the acceleration curve
 *
 * @param val
 */
void pointing_device_accel_set_takeoff(float val) {
    if (val >= 0.5) { // value less than 0.5 leads to nonsensical results
        userspace_config.pointing.accel.takeoff = val;
    }
    eeconfig_update_user_datablock(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
}

/**
 * @brief Sets the growth rate for the acceleration curve
 *
 * @param val
 */
void pointing_device_accel_set_growth_rate(float val) {
    if (val >= 0) { // value less 0 leads to nonsensical results
        userspace_config.pointing.accel.growth_rate = val;
    }
    eeconfig_update_user_datablock(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
}

/**
 * @brief Sets the offset for the acceleration curve
 *
 * @param val
 */
void pointing_device_accel_set_offset(float val) {
    userspace_config.pointing.accel.offset = val;
    eeconfig_update_user_datablock(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
}

/**
 * @brief Sets the limit for the acceleration curve
 *
 * @param val
 */
void pointing_device_accel_set_limit(float val) {
    if (val >= 0) {
        userspace_config.pointing.accel.limit = val;
    }
    eeconfig_update_user_datablock(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
}

/**
 * @brief Enables or disables the acceleration curve
 *
 * @param enable
 */
void pointing_device_accel_enabled(bool enable) {
    userspace_config.pointing.accel.enabled = enable;
    eeconfig_update_user_datablock(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
#ifdef POINTING_DEVICE_ACCEL_DEBUG
    printf("maccel: enabled: %d\n", userspace_config.pointing.accel.enabled);
#endif
#ifdef AUDIO_ENABLE
    if (enable) {
        PLAY_SONG(accel_on_song);
    } else {
        PLAY_SONG(accel_off_song);
    }
#endif // AUDIO_ENABLE
}

/**
 * @brief Toggles the acceleration curve
 *
 */
bool pointing_device_accel_get_enabled(void) {
    return userspace_config.pointing.accel.enabled;
}

/**
 * @brief Toggles the acceleration curve
 *
 */
void pointing_device_accel_toggle_enabled(void) {
    pointing_device_accel_enabled(!pointing_device_accel_get_enabled());
}

/**
 * @brief Gets the step value for the acceleration curve
 *
 * @param step
 * @return float
 */
float pointing_device_accel_get_mod_step(float step) {
    const uint8_t mod_mask = get_mods() | get_oneshot_mods();
    if (mod_mask & MOD_MASK_CTRL) {
        step *= 10; // control increases by factor 10
    }
    if (mod_mask & MOD_MASK_SHIFT) {
        step *= -1; // shift inverts
    }
    return step;
}

#ifdef POINTING_MODE_MAP_ENABLE
enum keymap_pointing_mode_ids {
    PM_BROW = POINTING_MODE_MAP_START, // BROWSER TAB Manipulation                      [mode id  6]
    PM_APP,                            // Open App browsing                             [mode id  7]
    POSTMAP_PM_SAFE_RANGE              // To avoid overlap when adding additional modes [mode id  8]
};
// (optional) enum to make things easier to read (index numbers can be used directly)
// Must be in the same order as the above modes
enum keymap_pointing_mode_maps_index {
    _PM_BROW, // first mode map  [index  0]
    _PM_APP   // second mode map [index  1]
};

const uint16_t PROGMEM pointing_mode_maps[POINTING_MODE_MAP_COUNT][POINTING_NUM_DIRECTIONS] = {
    [_PM_BROW] = POINTING_MODE_LAYOUT(C(S(KC_PGUP)), C(S(KC_TAB)), C(KC_TAB), C(S(KC_PGDN))),
    [_PM_APP]  = POINTING_MODE_LAYOUT(KC_NO, A(S(KC_TAB)), A(KC_TAB), KC_NO)};
#endif // POINTING_MODE_MAP_ENABLE
