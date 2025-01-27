// Copyright 2021 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "drashna.h"
#include "drashna_runtime.h"
#include "sendchar.h"
#include "print.h"

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
#ifndef RTC_TIMEZONE
#    define RTC_TIMEZONE -8
#endif // RTC_TIMEZONE
#ifdef CUSTOM_UNICODE_ENABLE
void keyboard_post_init_unicode(void);
#endif // CUSTOM_UNICODE_ENABLE
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
#ifdef COMMUNITY_MODULE_RTC_ENABLE
#    include "rtc.h"
#endif // COMMUNITY_MODULE_RTC_ENABLE
#ifdef CUSTOM_TAP_DANCE_ENABLE
#    include "keyrecords/custom_tap_dance.h"
#endif // CUSTOM_TAP_DANCE_ENABLE
#ifdef COMMUNITY_MODULE_I2C_SCANNER_ENABLE
#    include "i2c_scanner.h"
#endif // COMMUNITY_MODULE_I2C_SCANNER_ENABLE
#ifdef COMMUNITY_MODULE_CONSOLE_KEYLOGGING_ENABLE
#    include "console_keylogging.h"
#endif // COMMUNITY_MODULE_CONSOLE_KEYLOGGING_ENABLE

userspace_runtime_state_t userspace_runtime_state;

/**
 * @brief Keyboard Pre-Initialization
 *
 */
__attribute__((weak)) void keyboard_pre_init_keymap(void) {}
void                       keyboard_pre_init_user(void) {
    print_set_sendchar(drashna_sendchar);
    eeconfig_read_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
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
#ifdef CUSTOM_UNICODE_ENABLE
    keyboard_post_init_unicode();
#endif // CUSTOM_UNICODE_ENABLE
#ifdef COMMUNITY_MODULE_I2C_SCANNER_ENABLE
    i2c_scanner_set_enabled(userspace_config.debug.i2c_scanner_enable);
#endif // COMMUNITY_MODULE_I2C_SCANNER_ENABLE
#ifdef COMMUNITY_MODULE_CONSOLE_KEYLOGGING_ENABLE
    console_keylogger_set_enabled(userspace_config.debug.console_keylogger);
#endif // COMMUNITY_MODULE_CONSOLE_KEYLOGGING_ENABLE
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
#ifdef WPM_ENABLE
    void keyboard_post_init_wpm(void);
    keyboard_post_init_wpm();
#endif // WPM_ENABLE
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
    // hack for re-enabling oleds/lights/etc when woken from usb
    void last_matrix_activity_trigger(void);
    last_matrix_activity_trigger();

    set_is_device_suspended(false);
#ifdef CUSTOM_QUANTUM_PAINTER_ENABLE
    suspend_wakeup_init_quantum_painter();
#endif // CUSTOM_QUANTUM_PAINTER_ENABLE
    suspend_wakeup_init_keymap();
}

void layer_state_set_gaming(layer_state_t state) {
    static bool l_is_gaming_layer_active = false, is_swap_active = false;
    if (l_is_gaming_layer_active != is_gaming_layer_active(state)) {
        l_is_gaming_layer_active = is_gaming_layer_active(state);
        if (l_is_gaming_layer_active) {
            is_swap_active               = keymap_config.swap_lctl_lgui;
            keymap_config.swap_lctl_lgui = false;
        } else {
            if (is_swap_active) {
                eeconfig_read_keymap(&keymap_config);
            }
        }
    }
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

#ifdef SWAP_HANDS_ENABLE
    if (is_gaming_layer_active(state) && is_swap_hands_on()) {
        swap_hands_off();
    }
#endif // SWAP_HANDS_ENABLE
    layer_state_set_gaming(state);
    return state;
}

// Runs state check and changes underglow color and animation
__attribute__((weak)) layer_state_t default_layer_state_set_keymap(layer_state_t state) {
    return state;
}

layer_state_t default_layer_state_set_user(layer_state_t state) {
    if (!is_keyboard_master()) {
        return state;
    }

    state = default_layer_state_set_keymap(state);
#if defined(CUSTOM_RGBLIGHT)
    state = default_layer_state_set_rgb_light(state);
#endif // CUSTOM_RGBLIGHT

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
 * @brief EEPROM configuration initialization
 *
 */
__attribute__((weak)) void eeconfig_init_keymap(void) {}
void                       eeconfig_init_user(void) {
    memset(&userspace_config, 0, sizeof(userspace_config_t));
    userspace_config.check            = true;
    userspace_config.rgb.layer_change = true;
#if defined(OLED_ENABLE)
    userspace_config.display.oled.brightness = OLED_BRIGHTNESS;
#else  // OLED_ENABLE
    userspace_config.display.oled.brightness = 255;
#endif // OLED_ENABLE
    userspace_config.display.oled.pet.kaki_speed  = 40;
    userspace_config.display.oled.pet.mati_speed  = 60;
    userspace_config.display.oled.pet.sleep_speed = 10;
    userspace_config.display.painter.hsv.primary  = (HSV){
         .h = 128,
         .s = 255,
         .v = 255,
    };
    userspace_config.display.painter.hsv.secondary = (HSV){
        .h = 48,
        .s = 255,
        .v = 255,
    };
    userspace_config.display.menu_render_side = 3;

    userspace_config.pointing.auto_mouse_layer.enable   = true;
    userspace_config.pointing.auto_mouse_layer.layer    = _MOUSE;
    userspace_config.pointing.auto_mouse_layer.timeout  = AUTO_MOUSE_TIME;
    userspace_config.pointing.auto_mouse_layer.debounce = AUTO_MOUSE_DEBOUNCE;
    userspace_config.pointing.mouse_jiggler.enable      = false;
    userspace_config.pointing.mouse_jiggler.timeout     = 30;

    userspace_config.rtc.timezone = RTC_TIMEZONE;
    // ensure that nkro is enabled
    eeconfig_read_keymap(&keymap_config);
    keymap_config.nkro = true;
    eeconfig_update_keymap(&keymap_config);

    eeconfig_init_keymap();
    eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
#ifdef COMMUNITY_MODULE_POINTING_DEVICE_ACCEL_ENABLE
    void eeconfig_init_pointing_device(void);
    eeconfig_init_pointing_device();
#endif // COMMUNITY_MODULE_POINTING_DEVICE_ACCEL_ENABLE
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
 * sets userspace_runtime_state config to be synced later or just used. Also runs other "every tick" tasks
 */
__attribute__((weak)) void housekeeping_task_keymap(void) {}
void                       housekeeping_task_user(void) {
    if (is_keyboard_master()) {
        // we check if audio is enabled as it's only ran on master
#ifdef AUDIO_ENABLE
        userspace_runtime_state.audio.enable = is_audio_on();
#    ifdef AUDIO_CLICKY
        userspace_runtime_state.audio.clicky_enable = is_clicky_on();
        extern float clicky_freq;
        extern float clicky_rand;
        userspace_runtime_state.audio.clicky_freq = clicky_freq;
        userspace_runtime_state.audio.clicky_rand = clicky_rand;
#    endif // AUDIO_CLICKY
#    ifdef MUSIC_ENABLE
        userspace_runtime_state.audio.music_enable = is_music_on();
#    endif
#endif // AUDIO_ENABLE
    }
#ifdef DISPLAY_DRIVER_ENABLE
    void housekeeping_task_display(void);
    housekeeping_task_display();
#endif
#if defined(CUSTOM_TAP_DANCE_ENABLE) // Run Diablo 3 macro checking code.
    run_diablo_macro_check();
#endif // CUSTOM_TAP_DANCE_ENABLE
#if defined(CUSTOM_RGB_MATRIX)
    housekeeping_task_rgb_matrix();
#endif // CUSTOM_RGB_MATRIX
#if defined(CUSTOM_RGBLIGHT)
    housekeeping_task_rgb_light();
#endif // CUSTOM_RGBLIGHT
#if defined(SPLIT_KEYBOARD) && defined(SPLIT_TRANSACTION_IDS_USER)
    housekeeping_task_transport_sync();
#endif // SPLIT_KEYBOARD && SPLIT_TRANSACTION_IDS_USER
#ifdef WPM_ENABLE
    void housekeeping_task_wpm(void);
    housekeeping_task_wpm();
#endif // WPM_ENABLE
    housekeeping_task_keymap();
}

#ifdef COMMUNITY_MODULE_RTC_ENABLE
bool rtc_needs_sync = false;

void rtc_check_dst_format(rtc_time_t *time) {
#    ifdef DS1307_RTC_DRIVER_ENABLE
    time->is_dst = userspace_config.rtc.is_dst;
#    endif // DS1307_RTC_DRIVER_ENABLE
#    ifdef DS3231_RTC_DRIVER_ENABLE
    time->is_dst = userspace_config.rtc.is_dst;
#    endif // DS3231_RTC_DRIVER_ENABLE
#    ifdef PCF8523_RTC_DRIVER_ENABLE
    time->is_dst = userspace_config.rtc.is_dst;
#    endif // PCF8523_RTC_DRIVER_ENABLE
#    ifdef VENDOR_RTC_DRIVER_ENABLE
    time->format = userspace_config.rtc.format_24h;
#    endif // VENDOR_RTC_DRIVER_ENABLE
    time->timezone = userspace_config.rtc.timezone;
}

bool rtc_set_time_user(rtc_time_t *time) {
#    ifdef COMMUNITY_MODULE_DISPLAY_MENU_ENABLE
    display_menu_set_dirty(true);
#    endif // COMMUNITY_MODULE_DISPLAY_MENU_ENABLE
    userspace_config.rtc.is_dst     = time->is_dst;
    userspace_config.rtc.timezone   = time->timezone;
    userspace_config.rtc.format_24h = time->format;
    eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
    if (is_keyboard_master()) {
        rtc_needs_sync = rtc_is_connected();
    }
    return true;
}
#endif
