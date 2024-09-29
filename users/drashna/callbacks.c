// Copyright 2021 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "drashna.h"
#include "user_config.h"
#include "sendchar.h"
#include "print.h"

#ifdef LAYER_MAP_ENABLE
#    include "layer_map.h"
#endif // LAYER_MAP_ENABLE
#ifdef DISPLAY_DRIVER_ENABLE
#    include "display/display.h"
#endif // DISPLAY_DRIVER_ENABLE
#ifdef QUANTUM_PAINTER_ENABLE
#    include "display/painter/painter.h"
#endif // QUANTUM_PAINTER_ENABLE
#if defined(OLED_ENABLE) && defined(CUSTOM_OLED_DRIVER)
#    include "display/oled/oled_stuff.h"
#endif
#ifdef CUSTOM_DYNAMIC_MACROS_ENABLE
#    include "keyrecords/custom_dynamic_macros.h"
#endif // CUSTOM_DYNAMIC_MACROS_ENABLE
#if defined(I2C_DRIVER_REQUIRED)
void housekeeping_task_i2c_scanner(void);
void keyboard_post_init_i2c(void);
#endif // I2C_DRIVER_REQUIRED
#ifdef RTC_ENABLE
#    include "rtc.h"
#endif // RTC_ENABLE
#ifndef RTC_TIMEZONE
#    define RTC_TIMEZONE -8
#endif // RTC_TIMEZONE
#ifdef CUSTOM_UNICODE_ENABLE
void keyboard_post_init_unicode(void);
#endif // CUSTOM_UNICODE_ENABLE
#ifdef WATCHDOG_ENABLE
#    include "watchdog.h"
#endif // WATCHDOG_ENABLE
#if defined(LAYER_LOCK_ENABLE) && defined(LAYER_LOCK_IDLE_TIMEOUT)
#    include "layer_lock.h"
#endif // LAYER_LOCK_ENABLE && LAYER_LOCK_IDLE_TIMEOUT
#ifdef SPLIT_KEYBOARD
#    include "split_util.h"
#    include "split/transport_sync.h"
#endif // SPLIT_KEYBOARD
#include "pointing/pointing.h"
#if defined(CUSTOM_RGBLIGHT)
#    include "rgb/rgb_stuff.h"
#endif // CUSTOM_RGBLIGHT
#if defined(CUSTOM_RGB_MATRIX)
#    include "rgb/rgb_matrix_stuff.h"
#endif // CUSTOM_RGB_MATRIX

user_runtime_config_t user_runtime_state;

/**
 * @brief Keyboard Pre-Initialization
 *
 */
__attribute__((weak)) void keyboard_pre_init_keymap(void) {}
void                       keyboard_pre_init_user(void) {
    print_set_sendchar(drashna_sendchar);
    eeconfig_read_user_datablock(&userspace_config);
    if (!eeconfig_is_user_datablock_valid() || !userspace_config.check) {
        eeconfig_init_user();
    }

    keyboard_pre_init_keymap();
}
// Add reconfigurable functions here, for keymap customization
// This allows for a global, userspace functions, and continued
// customization of the keymap.  Use _keymap instead of _user
// functions in the keymaps
// Call user matrix init, set default RGB colors and then
// call the keymap's init function

__attribute__((weak)) void keyboard_post_init_keymap(void) {}
void                       keyboard_post_init_user(void) {
#ifdef DISPLAY_DRIVER_ENABLE
    keyboard_post_init_display_driver();
#endif // DISPLAY_DRIVER_ENABLE
#if defined(CUSTOM_RGBLIGHT)
    keyboard_post_init_rgb_light();
#endif // CUSTOM_RGBLIGHT
#if defined(CUSTOM_RGB_MATRIX)
    keyboard_post_init_rgb_matrix();
#endif // CUSTOM_RGB_MATRIX
#if defined(SPLIT_KEYBOARD) && defined(SPLIT_TRANSACTION_IDS_USER)
    keyboard_post_init_transport_sync();
#endif // SPLIT_KEYBOARD && SPLIT_TRANSACTION_IDS_USER
#ifdef CUSTOM_QUANTUM_PAINTER_ENABLE
    void keyboard_post_init_qp(void);
    keyboard_post_init_quantum_painter();
#endif // CUSTOM_QUANTUM_PAINTER_ENABLE
#if defined(I2C_DRIVER_REQUIRED)
    keyboard_post_init_i2c();
#endif // I2C_DRIVER_REQUIRED
#ifdef CUSTOM_UNICODE_ENABLE
    keyboard_post_init_unicode();
#endif // CUSTOM_UNICODE_ENABLE

#ifdef DEBUG_MATRIX_SCAN_RATE_ENABLE
    userspace_config.debug.matrix_scan_print = true;
#endif // DEBUG_MATRIX_SCAN_RATE_ENABLE

#if defined(BOOTLOADER_CATERINA) && defined(__AVR__) && defined(__AVR_ATmega32U4__)
    DDRD &= ~(1 << 5);
    PORTD &= ~(1 << 5);

    DDRB &= ~(1 << 0);
    PORTB &= ~(1 << 0);
#endif // BOOTLOADER_CATERINA && __AVR__ && __AVR_ATmega32U4__
#ifdef CUSTOM_DYNAMIC_MACROS_ENABLE
    dynamic_macro_init();
#endif // CUSTOM_DYNAMIC_MACROS_ENABLE
#ifdef RTC_ENABLE
    rtc_init();
#endif // RTC_ENABLE
#ifdef WATCHDOG_ENABLE
    watchdog_init();
#endif // WATCHDOG_ENABLE
    keyboard_post_init_keymap();
}

/**
 * @brief Callback for software shutdown
 *
 */
__attribute__((weak)) bool shutdown_keymap(bool jump_to_bootloader) {
    return true;
}

bool shutdown_user(bool jump_to_bootloader) {
#ifdef WATCHDOG_ENABLE
    watchdog_shutdown();
#endif // WATCHDOG_ENABLE
    if (!shutdown_keymap(jump_to_bootloader)) {
        return false;
    }
#ifdef RGBLIGHT_ENABLE
    rgblight_shutdown(jump_to_bootloader);
#endif // RGBLIGHT_ENABLE
#ifdef RGB_MATRIX_ENABLE
    rgb_matrix_shutdown(jump_to_bootloader);
#endif // RGB_MATRIX_ENABLE
#if defined(OLED_ENABLE) && defined(CUSTOM_OLED_DRIVER)
    oled_shutdown(jump_to_bootloader);
#endif // OLED_ENABLE && CUSTOM_OLED_DRIVER
#ifdef CUSTOM_QUANTUM_PAINTER_ENABLE
    shutdown_quantum_painter(jump_to_bootloader);
#endif // CUSTOM_QUANTUM_PAINTER_ENABLE
    return true;
}

/**
 * @brief Suspend power down callback (constantly called when suspended)
 *
 */
__attribute__((weak)) void suspend_power_down_keymap(void) {}

void suspend_power_down_user(void) {
    set_is_device_suspended(true);
#ifdef WATCHDOG_ENABLE
    suspend_power_down_watchdog();
#endif // WATCHDOG_ENABLE
    if (layer_state_is(_GAMEPAD)) {
        layer_off(_GAMEPAD);
    }
    if (layer_state_is(_DIABLO)) {
        layer_off(_DIABLO);
    }
    if (layer_state_is(_DIABLOII)) {
        layer_off(_DIABLOII);
    }
#ifdef OLED_ENABLE
    oled_off();
#endif // OLED_ENABLE

#ifdef CUSTOM_QUANTUM_PAINTER_ENABLE
    suspend_power_down_quantum_painter();
#endif // CUSTOM_QUANTUM_PAINTER_ENABLE

    suspend_power_down_keymap();
}

/**
 * @brief Suspend wake-up callback (only called when actually waking up)
 *
 */
__attribute__((weak)) void suspend_wakeup_init_keymap(void) {}
void                       suspend_wakeup_init_user(void) {
#ifdef WATCHDOG_ENABLE
    suspend_wakeup_init_watchdog();
#endif // WATCHDOG_ENABLE
    // hack for re-enabling oleds/lights/etc when woken from usb
    void last_matrix_activity_trigger(void);
    last_matrix_activity_trigger();

    set_is_device_suspended(false);
#ifdef CUSTOM_QUANTUM_PAINTER_ENABLE
    suspend_wakeup_init_quantum_painter();
#endif // CUSTOM_QUANTUM_PAINTER_ENABLE
    suspend_wakeup_init_keymap();
}

// on layer change, no matter where the change was initiated
// Then runs keymap's layer change check
__attribute__((weak)) layer_state_t layer_state_set_keymap(layer_state_t state) {
    return state;
}
layer_state_t layer_state_set_user(layer_state_t state) {
    state = update_tri_layer_state(state, _RAISE, _LOWER, _ADJUST);
#if defined(CUSTOM_POINTING_DEVICE)
    state = layer_state_set_pointing(state);
#endif // CUSTOM_POINTING_DEVICE
#if defined(CUSTOM_RGBLIGHT)
    state = layer_state_set_rgb_light(state);
#endif // CUSTOM_RGBLIGHT
#if defined(AUDIO_ENABLE)
    set_doom_song(state);
#endif // AUDIO_ENABLE
    state = layer_state_set_keymap(state);

#ifndef NO_PRINT
    char layer_buffer[16 + 5];
    format_layer_bitmap_string(layer_buffer, state, default_layer_state);
    dprintf("layer state: %s\n", layer_buffer);
#endif // NO_PRINT

#ifdef LAYER_MAP_ENABLE
    set_layer_map();
#endif // LAYER_MAP_ENABLE

#ifdef SWAP_HANDS_ENABLE
    if (is_gaming_layer_active(state) && is_swap_hands_on()) {
        swap_hands_off();
    }
#endif // SWAP_HANDS_ENABLE
    return state;
}

// Runs state check and changes underglow color and animation
__attribute__((weak)) layer_state_t default_layer_state_set_keymap(layer_state_t state) {
    return state;
}

#if defined(AUDIO_ENABLE) && defined(DEFAULT_LAYER_SONGS)
static float default_layer_songs[][16][2] = DEFAULT_LAYER_SONGS;
#endif // AUDIO_ENABLE && DEFAULT_LAYER_SONGS

layer_state_t default_layer_state_set_user(layer_state_t state) {
    if (!is_keyboard_master()) {
        return state;
    }

    state = default_layer_state_set_keymap(state);
#if defined(CUSTOM_RGBLIGHT)
    state = default_layer_state_set_rgb_light(state);
#endif // CUSTOM_RGBLIGHT

    static bool has_init_been_ran = false;
    // We don't want to run this the first time it's called, since it's read from eeeprom and called
    // as part of the startup process. But after that, it's okay.
    if (has_init_been_ran) {
#if defined(AUDIO_ENABLE) && defined(DEFAULT_LAYER_SONGS)
        if (get_highest_layer(state) < MAX_LAYER) {
            PLAY_SONG(default_layer_songs[get_highest_layer(state)]);
        }
#endif // AUDIO_ENABLE && DEFAULT_LAYER_SONGS
        eeconfig_update_default_layer(state);
    } else {
        has_init_been_ran = true;
    }

#ifdef LAYER_MAP_ENABLE
    set_layer_map();
#endif // LAYER_MAP_ENABLE
    return state;
}

/**
 * @brief USB Host Lock LED callback
 *
 */
__attribute__((weak)) void led_set_keymap(uint8_t usb_led) {}
void                       led_set_user(uint8_t usb_led) {
    led_set_keymap(usb_led);
}

/**
 * @brief EEPROM comfiguration initialization
 *
 */
__attribute__((weak)) void eeconfig_init_keymap(void) {}
void                       eeconfig_init_user(void) {
    memset(&userspace_config, 0, sizeof(userspace_config_t));
    userspace_config.check            = true;
    userspace_config.rgb.layer_change = true;
#if defined(OLED_ENABLE)
    userspace_config.oled.brightness = OLED_BRIGHTNESS;
#else  // OLED_ENABLE
    userspace_config.oled.brightness = 255;
#endif // OLED_ENABLE
    userspace_config.painter.hsv.primary = (HSV){
        .h = 128,
        .s = 255,
        .v = 255,
    };
    userspace_config.painter.hsv.secondary = (HSV){
        .h = 48,
        .s = 255,
        .v = 255,
    };

    userspace_config.pointing.growth_rate = POINTING_DEVICE_ACCEL_GROWTH_RATE;
    userspace_config.pointing.offset      = POINTING_DEVICE_ACCEL_OFFSET;
    userspace_config.pointing.limit       = POINTING_DEVICE_ACCEL_LIMIT;
    userspace_config.pointing.takeoff     = POINTING_DEVICE_ACCEL_TAKEOFF;

    userspace_config.rtc.timezone = RTC_TIMEZONE;
    // ensure that nkro is enabled
    keymap_config.raw  = eeconfig_read_keymap();
    keymap_config.nkro = true;
    eeconfig_update_keymap(keymap_config.raw);

    eeconfig_update_user_datablock(&userspace_config);
    eeconfig_init_keymap();
}

/**
 * @brief Matrix scan callback ... only use for matrix scan rate task
 *
 */
void matrix_scan_user(void) {
    matrix_scan_rate_task();
}

/**
 * @brief Handle slave side scanning of keyboard
 *
 */
#ifdef SPLIT_KEYBOARD
__attribute__((weak)) void matrix_slave_scan_keymap(void) {}
void                       matrix_slave_scan_user(void) {
    matrix_scan_rate_task();
#    if defined(AUDIO_ENABLE) && defined(AUDIO_INIT_DELAY)
#        if defined(SPLIT_WATCHDOG_ENABLE) && !defined(SPLIT_WATCHDOG_TIMEOUT)
#            if defined(SPLIT_USB_TIMEOUT)
#                define SPLIT_WATCHDOG_TIMEOUT (SPLIT_USB_TIMEOUT + 100)
#            else
#                define SPLIT_WATCHDOG_TIMEOUT 3000
#            endif
#        endif
    if (!is_keyboard_master()) {
        static bool     delayed_tasks_run  = false;
        static uint16_t delayed_task_timer = 0;
        if (!delayed_tasks_run) {
            if (!delayed_task_timer) {
                delayed_task_timer = timer_read();
            } else if (timer_elapsed(delayed_task_timer) > (SPLIT_WATCHDOG_TIMEOUT + 100) && is_transport_connected()) {
                audio_startup();
                delayed_tasks_run = true;
            }
        }
    }
#    endif // AUDIO_ENABLE && AUDIO_INIT_DELAY
    matrix_slave_scan_keymap();
}
#endif // SPLIT_KEYBOARD

/**
 * @brief Housekeyping task
 *
 * sets user_runtime_state config to be synced later or just used. Also runs other "every tick" tasks
 */
__attribute__((weak)) void housekeeping_task_keymap(void) {}
void                       housekeeping_task_user(void) {
    if (is_keyboard_master()) {
        // we check if audio is enabled as it's only ran on master
#ifdef AUDIO_ENABLE
        user_runtime_state.audio.enable        = is_audio_on();
        user_runtime_state.audio.clicky_enable = is_clicky_on();
#endif // AUDIO_ENABLE
    }

#ifdef WATCHDOG_ENABLE
    watchdog_task();
#endif // WATCHDOG_ENABLE
#if defined(LAYER_LOCK_ENABLE) && defined(LAYER_LOCK_IDLE_TIMEOUT)
    layer_lock_task();
#endif                               // LAYER_LOCK_ENABLE && LAYER_LOCK_IDLE_TIMEOUT
#if defined(CUSTOM_TAP_DANCE_ENABLE) // Run Diablo 3 macro checking code.
    run_diablo_macro_check();
#endif // CUSTOM_TAP_DANCE_ENABLE
#if defined(CUSTOM_RGB_MATRIX)
    housekeeping_task_rgb_matrix();
#endif // CUSTOM_RGB_MATRIX
#if defined(CUSTOM_RGBLIGHT)
    housekeeping_task_rgb_light();
#endif // CUSTOM_RGBLIGHT
#if defined(I2C_DRIVER_REQUIRED)
    housekeeping_task_i2c_scanner();
#endif // I2C_DRIVER_REQUIRED
#ifdef CUSTOM_OLED_DRIVER
    housekeeping_task_oled();
#endif // CUSTOM_OLED_DRIVER
#if defined(SPLIT_KEYBOARD) && defined(SPLIT_TRANSACTION_IDS_USER)
    housekeeping_task_transport_sync();
#endif // SPLIT_KEYBOARD && SPLIT_TRANSACTION_IDS_USER
#ifdef CUSTOM_QUANTUM_PAINTER_ENABLE
    housekeeping_task_quantum_painter();
#endif // CUSTOM_QUANTUM_PAINTER_ENABLE
#ifdef RTC_ENABLE
    rtc_task();
#endif // RTC_ENABLE
#ifdef SENTENCE_CASE_ENABLE
    sentence_case_task();
#endif // SENTENCE_CASE_ENABLE
#ifdef SELECT_WORD_ENABLE
    select_word_task();
#endif // SELECT_WORD_ENABLE
#ifdef ACHORDION_ENABLE
    achordion_task();
#endif // ACHORDION_ENABLE
#ifdef ORBITAL_MOUSE_ENABLE
    orbital_mouse_task();
#endif // ORBITAL_MOUSE_ENABLE
#ifdef LAYER_MAP_ENABLE
    housekeeping_task_layer_map();
#endif // LAYER_MAP_ENABLE
    housekeeping_task_keymap();
}
