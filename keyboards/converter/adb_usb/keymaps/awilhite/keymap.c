// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include QMK_KEYBOARD_H

enum user_keycodes {
    QP_CYCLE_MENU_BLOCK = QK_USER,
};

#ifdef QUANTUM_PAINTER_ENABLE
#    include "qp_surface.h"
#    include "qp.h"
#    include "qp_comms.h"
#    include "qp_st77xx_opcodes.h"
#    include "painter/fonts/font_oled.qff.h"
#    include "painter/graphics.qgf.h"
#    include "painter/keylogger.h"
#endif
#ifdef COMMUNITY_MODULE_POINTING_DEVICE_ACCEL_ENABLE
#    include "pointing_device_accel.h"
#endif // COMMUNITY_MODULE_POINTING_DEVICE_ACCEL_ENABLE
#ifdef COMMUNITY_MODULE_QP_HELPERS_ENABLE
#    include "qp_helpers.h"
#endif
#ifdef COMMUNITY_MODULE_DRAG_SCROLL_ENABLE
#    include "drag_scroll.h"
#endif
#ifdef COMMUNITY_MODULE_RTC_ENABLE
#    include "rtc.h"
#endif // COMMUNITY_MODULE_RTC_ENABLE
#ifdef COMMUNITY_MODULE_LAYER_MAP_ENABLE
#    include "layer_map.h"
#endif // COMMUNITY_MODULE_LAYER_MAP_ENABLE
#include "display_menu.h"
#include "qp_render_menu.h"
#include "version.h"
#include "hardware_id.h"

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_ext_ansi(
    KC_ESC,  KC_F1,   KC_F2,  KC_F3,  KC_F4,  KC_F5,  KC_F6,  KC_F7,  KC_F8,   KC_F9,  KC_F10,  KC_F11,  KC_F12,                 KC_PSCR,KC_SCRL,KC_PAUS,                                QP_CYCLE_MENU_BLOCK,
    KC_GRV,  KC_1,    KC_2,   KC_3,   KC_4,   KC_5,   KC_6,   KC_7,   KC_8,    KC_9,   KC_0,    KC_MINS, KC_EQL,  KC_BSPC,       KC_INS, KC_HOME, KC_PGUP,     KC_NUM,  KC_EQL, KC_PSLS, KC_PAST,
    KC_TAB,  KC_Q,    KC_W,   KC_E,   KC_R,   KC_T,   KC_Y,   KC_U,   KC_I,    KC_O,   KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,       KC_DEL, KC_END,  KC_PGDN,     KC_P7,   KC_P8,  KC_P9,   KC_PMNS,
    KC_LCAP, KC_A,    KC_S,   KC_D,   KC_F,   KC_G,   KC_H,   KC_J,   KC_K,    KC_L,   KC_SCLN, KC_QUOT,          KC_ENT,                                      KC_P4,   KC_P5,  KC_P6,   KC_PPLS,
    KC_LSFT, KC_Z,    KC_X,   KC_C,   KC_V,   KC_B,   KC_N,   KC_M,   KC_COMM, KC_DOT, KC_SLSH,                   KC_RSFT,                KC_UP,               KC_P1,   KC_P2,  KC_P3,
    KC_LCTL, KC_LALT, KC_LGUI,                KC_SPC,                                  KC_RALT, KC_RCTL,                         KC_LEFT, KC_DOWN, KC_RGHT,    KC_P0,           KC_PDOT, KC_PENT
    ),
};

#ifdef COMMUNITY_MODULE_LAYER_MAP_ENABLE
keypos_t layer_remap[LAYER_MAP_ROWS][LAYER_MAP_COLS] = {
    { {   5,   6 }, { 255, 255 }, {   2,  15 }, {   0,  15 }, {   3,  12 }, {   6,  14 }, {   0,  12 }, {   1,  12 }, {   2,  12 }, {   4,  12 }, {   5,  12 }, {   5,  13 }, {   7,  12 }, {   7,  13 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, {   1,  13 }, {   3,  13 }, {   1,  14 }, { 255, 255 }, {   7,  15 }, {   7,  15 } },
    { { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 } },
    { {   2,   6 }, {   2,   2 }, {   3,   2 }, {   4,   2 }, {   5,   2 }, {   7,   2 }, {   6,   2 }, {   2,   3 }, {   4,   3 }, {   1,   3 }, {   5,   3 }, {   3,   3 }, {   0,   3 }, {   3,   6 }, { 255, 255 }, {   2,  14 }, {   3,  14 }, {   4,  14 }, { 255, 255 }, {   8,   7 }, {   1,  10 }, {   3,   9 }, {   3,   8 } },
    { {   0,   6 }, {   4,   1 }, {   5,   1 }, {   6,   1 }, {   7,   1 }, {   1,   2 }, {   0,   2 }, {   0,   4 }, {   2,   4 }, {   7,   3 }, {   3,   4 }, {   1,   4 }, {   6,   3 }, {   2,   5 }, { 255, 255 }, {   5,  14 }, {   7,  14 }, {   1,  15 }, { 255, 255 }, {   1,  11 }, {   3,  11 }, {   4,  11 }, {   5,   8 } },
    { {   1,   7 }, {   0,   0 }, {   1,   0 }, {   2,   0 }, {   3,   0 }, {   5,   0 }, {   4,   0 }, {   6,   4 }, {   0,   5 }, {   5,   4 }, {   1,   5 }, {   7,   4 }, {   4,   4 }, {   4,   4 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, {   6,  10 }, {   7,  10 }, {   0,  11 }, {   6,   9 } },
    { {   0,   7 }, {   0,   7 }, {   6,   0 }, {   7,   0 }, {   0,   1 }, {   1,   1 }, {   3,   1 }, {   5,   5 }, {   6,   5 }, {   3,   5 }, {   7,   5 }, {   4,   5 }, {   3,  15 }, {   3,  15 }, { 255, 255 }, { 255, 255 }, {   6,   7 }, { 255, 255 }, { 255, 255 }, {   3,  10 }, {   4,  10 }, {   5,  10 }, {   4,   9 } },
    { {   6,   6 }, { 255, 255 }, {   2,   7 }, { 255, 255 }, {   7,   6 }, {   1,   6 }, {   1,   6 }, {   1,   6 }, {   1,   6 }, {   1,   6 }, {   1,   6 }, {   4,  15 }, { 255, 255 }, {   5,  15 }, { 255, 255 }, {   3,   7 }, {   5,   7 }, {   4,   7 }, { 255, 255 }, {   2,  10 }, {   2,  10 }, {   1,   8 }, {   4,   9 } },
};
#endif

KEYCODE_STRING_NAMES_USER(
    {KC_LCAP, "KC_LCAP"},
    {KC_NUM, "KC_NUM"},
    {KC_SCRL, "KC_SCRL"},
    {KC_PDOT, "KC_PDOT"},
    {KC_PEQL, "KC_PEQL"},
    {KC_PSLS, "KC_PSLS"},
    {KC_PAST, "KC_PAST"},
    {KC_PENT, "KC_PENT"},
    {KC_PMNS, "KC_PMNS"},
    {KC_PPLS, "KC_PPLS"},
    #ifdef COMMUNITY_MODULE_DISPLAY_MENU_ENABLE
    {DISPLAY_MENU, "DS_MENU"},
    #endif // COMMUNITY_MODULE_DISPLAY_MENU_ENABLE
    #ifdef COMMUNITY_MODULE_DRAG_SCROLL_ENABLE
    {DRAG_SCROLL_TOGGLE, "DRG_TOG"},
    {DRAG_SCROLL_MOMENTARY, "DRG_SCL"},
    #endif // COMMUNITY_MODULE_DRAG_SCROLL_ENABLE
    #ifdef COMMUNITY_MODULE_KEYBOARD_LOCK_ENABLE
    {KEYLOCK, "KEYLOCK"},
    #endif // COMMUNITY_MODULE_KEYBOARD_LOCK_ENABLE
    #ifdef COMMUNITY_MODULE_POINTING_DEVICE_ACCEL_ENABLE
    {CM_MOUSE_ACCEL_TOGGLE, "MA_TOG"},
    {CM_MOUSE_ACCEL_TAKEOFF, "MA_TKOF"},
    {CM_MOUSE_ACCEL_GROWTH_RATE, "MA_GROW"},
    {CM_MOUSE_ACCEL_OFFSET, "MA_OFST"},
    {CM_MOUSE_ACCEL_LIMIT, "MA_LMT"},
    #endif // COMMUNITY_MODULE_POINTING_DEVICE_ACCEL_ENABLE
    #ifdef COMMUNITY_MODULE_POINTING_DEVICE_GESTURE_ENABLE
    {CM_MOUSE_GESTURE_TAP, "GST_TAP"},
    {CM_MOUSE_GESTURE_HOLD, "GST_HLD"},
    #endif // COMMUNITY_MODULE_POINTING_DEVICE_GESTURE_ENABLE
    #ifdef COMMUNITY_MODULE_UNICODE_TYPING_ENABLE
    {KC_NOMODE, "KC_NORM"},
    {KC_WIDE, "KC_WIDE"},
    {KC_SCRIPT, "KC_SCPT"},
    {KC_BLOCKS, "KC_BLCK"},
    {KC_REGIONAL, "KC_REG"},
    {KC_AUSSIE, "KC_AUSS"},
    {KC_ZALGO, "KC_ZALG"},
    {KC_SUPER, "KC_SUPR"},
    {KC_COMIC, "KC_COMC"},
    {KC_FRAKTUR, "KC_FRAK"},
    {KC_DOUBLE_STRUCK, "KC_DBSK"},
    {KC_SCREAM_CYPHER, "KC_SCRM"},
#endif
);
// clang-format on

#ifdef QUANTUM_PAINTER_ENABLE
static painter_device_t st7789_170x320_display;
static painter_device_t st7789_170x320_surface_display;
static uint8_t          display_buffer[SURFACE_REQUIRED_BUFFER_BYTE_SIZE(320, 170, 16)];
painter_font_handle_t   font_oled;
void                    painter_render_user(void);
static bool             force_redraw = true;

painter_image_handle_t qmk_banner;
painter_image_handle_t windows_logo, apple_logo, linux_logo;
painter_image_handle_t shift_icon, control_icon, alt_icon, command_icon, windows_icon;

uint16_t   last_keycode   = KC_NO;
keyevent_t last_key_event = {};

struct {
    hsv_t primary;
    hsv_t secondary;
} typedef dual_hsv_t;

#    ifndef DISPLAY_CONSOLE_LOG_LINE_NUM
#        define DISPLAY_CONSOLE_LOG_LINE_NUM 24
#    endif
#    if !defined(DISPLAY_CONSOLE_LOG_LINE_START)
#        if DISPLAY_CONSOLE_LOG_LINE_NUM > 10
#            define DISPLAY_CONSOLE_LOG_LINE_START (DISPLAY_CONSOLE_LOG_LINE_NUM - 11)
#        else // DISPLAY_CONSOLE_LOG_LINE_NUM
#            define DISPLAY_CONSOLE_LOG_LINE_START 0
#        endif // DISPLAY_CONSOLE_LOG_LINE_NUM
#    endif     // DISPLAY_CONSOLE_LOG_LINE_START
#    ifndef DISPLAY_CONSOLE_LOG_LINE_LENGTH
#        define DISPLAY_CONSOLE_LOG_LINE_LENGTH 52
#    endif // DISPLAY_CONSOLE_LOG_LINE_LENGTH

uint32_t       sendchar_timer      = 0;
bool           console_has_redrawn = false;
static uint8_t log_write_idx       = 0;
static char    loglines[DISPLAY_CONSOLE_LOG_LINE_NUM + 1][DISPLAY_CONSOLE_LOG_LINE_LENGTH + 2];
char          *logline_ptrs[DISPLAY_CONSOLE_LOG_LINE_NUM + 1];

int8_t drashna_sendchar(uint8_t c) {
    uint8_t ret    = 0;
    sendchar_timer = timer_read32();

    extern int8_t sendchar(uint8_t c);
    ret                     = sendchar(c);
    static bool first_setup = false;
    if (!first_setup) {
        memset(loglines, 0, sizeof(loglines));
        for (int i = 0; i < (DISPLAY_CONSOLE_LOG_LINE_NUM + 1); ++i) {
            logline_ptrs[i] = loglines[i];
        }
        first_setup = true;
    }

    if (c == '\n') {
        logline_ptrs[DISPLAY_CONSOLE_LOG_LINE_NUM][log_write_idx] = 0;
        char *tmp                                                 = logline_ptrs[0];
        for (int i = 0; i < DISPLAY_CONSOLE_LOG_LINE_NUM; ++i) {
            logline_ptrs[i] = logline_ptrs[i + 1];
        }
        logline_ptrs[DISPLAY_CONSOLE_LOG_LINE_NUM]    = tmp;
        log_write_idx                                 = 0;
        logline_ptrs[DISPLAY_CONSOLE_LOG_LINE_NUM][0] = 0;
        console_has_redrawn                           = false;
    } else if (log_write_idx >= (DISPLAY_CONSOLE_LOG_LINE_LENGTH)) {
        // Ignore.
    } else {
        logline_ptrs[DISPLAY_CONSOLE_LOG_LINE_NUM][log_write_idx++] = c;
        logline_ptrs[DISPLAY_CONSOLE_LOG_LINE_NUM][log_write_idx]   = 0;
        console_has_redrawn                                         = false;
    }
    return ret;
}

#    if defined(AUTOCORRECT_ENABLE) || defined(COMMUNITY_MODULE_AUTOCORRECT_ENABLE)
// 2 strings, 2q chars each + null terminator. max autocorrect length is 19 chars but 128px/6 supports 21 chars
char autocorrected_str[2][21]     = {"    automatically\0", "      corrected\0"};
char autocorrected_str_raw[2][21] = {"automatically\0", "corrected\0"};
bool autocorrect_str_has_changed  = false;

#        include <send_string.h>
#        include <ctype.h>

#        define PGM_LOADBIT(mem, pos) ((pgm_read_byte(&((mem)[(pos) / 8])) >> ((pos) % 8)) & 0x01)
char send_string_get_next_ram(void *arg);

typedef struct send_string_memory_state_t {
    const char *string;
} send_string_memory_state_t;

void add_autocorrect_char_to_keylogger_str(char ascii_code) {
    if (ascii_code == '\a') { // BEL
        return;
    }

    uint8_t keycode    = pgm_read_byte(&ascii_to_keycode_lut[(uint8_t)ascii_code]);
    bool    is_shifted = PGM_LOADBIT(ascii_to_shift_lut, (uint8_t)ascii_code);
    bool    is_altgred = PGM_LOADBIT(ascii_to_altgr_lut, (uint8_t)ascii_code);

    uint8_t mods = 0;

    if (is_shifted) {
        mods |= MOD_BIT(KC_LEFT_SHIFT);
    }

    if (is_altgred) {
        mods |= MOD_BIT(KC_RIGHT_ALT);
    }

    add_keycode_to_keylogger_str(keycode, mods);
}

static void update_keylogger_string(char (*getter)(void *), void *arg) {
    while (1) {
        char ascii_code = getter(arg);
        if (!ascii_code) break;
        if (ascii_code == SS_QMK_PREFIX) {
            ascii_code = getter(arg);

            if (ascii_code == SS_TAP_CODE) {
                // tap
                uint8_t keycode = getter(arg);
                add_autocorrect_char_to_keylogger_str(keycode);
            } else if (ascii_code == SS_DOWN_CODE) {
                // down
                uint8_t keycode = getter(arg);
                add_autocorrect_char_to_keylogger_str(keycode);
            } else if (ascii_code == SS_UP_CODE) {
                // up
                getter(arg);
            } else if (ascii_code == SS_DELAY_CODE) {
                // delay
                int ms     = 0;
                ascii_code = getter(arg);

                while (isdigit(ascii_code)) {
                    ms *= 10;
                    ms += ascii_code - '0';
                    ascii_code = getter(arg);
                }
            }

            // if we had a delay that terminated with a null, we're done
            if (ascii_code == 0) break;
        } else {
            add_autocorrect_char_to_keylogger_str(ascii_code);
        }
    }
}
/**
 * @brief hack for snprintf warning
 *
 */
#        define snprintf_nowarn(...) (snprintf(__VA_ARGS__) < 0 ? abort() : (void)0)

/**
 * @brief Center text in a string. Useful for monospaced font rendering such as oled display feature.
 *
 * @param text pointer to input string to center
 * @param output pointer to output string to write to
 * @param width total width of the output string
 */
void center_text(const char *text, char *output, uint8_t width) {
    /* If string is bigger than the available width, trim it */
    if (strlen(text) > width) {
        memcpy(output, text, width);
        output[width] = '\0';
        return;
    }

    /* Try to center the TEXT, TODO: Handle Even lengths*/
    uint8_t padlen_l = (width - strlen(text)) / 2;
    uint8_t padlen_r = (padlen_l * 2) + strlen(text) == width ? padlen_l : padlen_l + 1;
    snprintf_nowarn(output, width + 1, "%*s%s%*s", padlen_l, " ", text, padlen_r, " ");
    output[width] = '\0';
}

bool apply_autocorrect(uint8_t backspaces, const char *str, char *typo, char *correct) {
    strncpy(autocorrected_str_raw[0], typo, sizeof(autocorrected_str_raw[0]) - 1);
    strncpy(autocorrected_str_raw[1], correct, sizeof(autocorrected_str_raw[1]) - 1);

    center_text(typo, autocorrected_str[0], sizeof(autocorrected_str[0]) - 1);
    center_text(correct, autocorrected_str[1], sizeof(autocorrected_str[1]) - 1);
    // printf("Autocorrected %s to %s (original: %s)\n", typo, correct, str);
    autocorrect_str_has_changed = true;
    for (uint8_t i = 0; i <= backspaces; ++i) {
#        if defined(WPM_ENABLE) && defined(WPM_ALLOW_COUNT_REGRESSION)
        update_wpm(KC_BSPC);
#        endif // WPM_ENABLE

        keylog_shift_right();
    }

    send_string_memory_state_t state = {str};

    if (strncmp("ushould", typo, strlen(typo)) == 0) {
        // If we're correcting "ushould" to "you should", we want to add an extra space to the keylogger string after
        // the correction so that it doesn't look like "youshould" in the keylogger.
        add_autocorrect_char_to_keylogger_str('u');
        add_autocorrect_char_to_keylogger_str(' ');
    }

    update_keylogger_string(send_string_get_next_ram, &state);

#        if 0
    if (last_keycode == KC_SPC) {
        // If the last keycode was space, we need to add a space to the keylogger string
        // so that it doesn't look like the autocorrected word is the first word in the sentence.
        add_autocorrect_char_to_keylogger_str(' ');
    }
#        endif

    // add extra handling for "yo ushould" -> you should
    if (strncmp("ushould", typo, strlen(typo)) == 0) {
        for (uint8_t i = 0; i < (backspaces + 1); ++i) {
            tap_code(KC_BSPC);
        }
        tap_code(KC_U);
        tap_code(KC_SPC);
        send_string_P(str);

        return false;
    }

    return true;
}
#    endif // AUTOCORRECT_ENABLE || COMMUNITY_MODULE_AUTOCORRECT_ENABLE

#    ifdef WPM_ENABLE
#        define WPM_GRAPH_SAMPLES        20
#        define WPM_PAINTER_GRAPH_HEIGHT 53
#        define WPM_PAINTER_GRAPH_WIDTH  109

uint8_t wpm_graph_samples[WPM_GRAPH_SAMPLES] = {0};
uint8_t wpm_avg = 0, wpm_peak = 0;

void update_wpm_avg(void) {
    static uint8_t wpm_samples[10] = {0};
    static uint8_t index           = 0;
    uint16_t       sum             = 0;

    wpm_samples[index] = get_current_wpm();
    index              = (index + 1) % 10;

    for (uint8_t i = 0; i < 10; i++) {
        sum += wpm_samples[i];
    }

    wpm_avg = sum / 10;
    if (index == 9) {
        // Shift the wpm_graph_samples array to the right
        for (uint8_t i = WPM_GRAPH_SAMPLES - 1; i > 0; i--) {
            wpm_graph_samples[i] = wpm_graph_samples[i - 1];
        }

        // Update the first element of the array with the new average WPM
        wpm_graph_samples[0] = wpm_avg;
    }
}

void housekeeping_task_wpm(void) {
    if (is_keyboard_master()) {
        static uint32_t timer    = 0;
        static uint16_t interval = 0;

        if (wpm_peak < get_current_wpm()) {
            wpm_peak = get_current_wpm();
            timer    = timer_read32();
        } else if (timer_elapsed32(timer) > 10000) {
            wpm_peak = get_current_wpm();
        }
        if (timer_elapsed(interval) >= 100) {
            update_wpm_avg();
            interval = timer_read();
        }
    }
}
#    endif

void keyboard_pre_init_user(void) {
    print_set_sendchar(drashna_sendchar);
#    if (EECONFIG_USER_DATA_SIZE) > 0
    eeconfig_read_user_datablock(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
#    endif
    void populate_layer_map(void);
    populate_layer_map();
}

void keyboard_post_init_user(void) {
    uint16_t width;
    uint16_t height;

    st7789_170x320_display = qp_st7789_make_spi_device(170, 320, DISPLAY_CS_PIN, DISPLAY_DC_PIN, DISPLAY_RST_PIN, 1, 3);
    st7789_170x320_surface_display = qp_make_rgb565_surface(320, 170, display_buffer);

    qp_init(st7789_170x320_display, QP_ROTATION_270);
    qp_init(st7789_170x320_surface_display, QP_ROTATION_0);
    qp_set_viewport_offsets(st7789_170x320_display, 0, 35);
    qp_get_geometry(st7789_170x320_display, &width, &height, NULL, NULL, NULL);
    qp_clear(st7789_170x320_display);
    qp_rect(st7789_170x320_display, 0, 0, width - 1, height - 1, 0, 0, 0, true);

    qp_power(st7789_170x320_display, true);
    qp_flush(st7789_170x320_display);

    font_oled = qp_load_font_mem(font_oled_font);

    windows_logo = qp_load_image_mem(gfx_windows_logo);
    apple_logo   = qp_load_image_mem(gfx_apple_logo);
    linux_logo   = qp_load_image_mem(gfx_linux_logo);

    shift_icon   = qp_load_image_mem(gfx_shift_icon);
    control_icon = qp_load_image_mem(gfx_control_icon);
    alt_icon     = qp_load_image_mem(gfx_alt_icon);
    command_icon = qp_load_image_mem(gfx_command_icon);
    windows_icon = qp_load_image_mem(gfx_windows_icon);

    qmk_banner = qp_load_image_mem(gfx_qmk_powered_by);

    if (!is_backlight_enabled()) {
        backlight_enable();
        backlight_level_noeeprom(get_backlight_level());
    }
    if (get_backlight_level() == 0) {
        backlight_level(BACKLIGHT_LEVELS);
    }
    painter_render_user();
}

#    ifdef BACKLIGHT_ENABLE
static uint8_t last_backlight = 255;
#    endif

void qp_backlight_enable(void) {
    if (last_backlight != 255) {
        backlight_level_noeeprom(last_backlight);
        last_backlight = 255;
    }
}

void qp_backlight_disable(void) {
    if (last_backlight == 255) {
        last_backlight = get_backlight_level();
        backlight_level_noeeprom(0);
    }
}

void suspend_power_down_user(void) {
    qp_backlight_disable();
    qp_power(st7789_170x320_display, false);
}

void suspend_wakeup_init_user(void) {
    qp_power(st7789_170x320_display, true);
    qp_backlight_enable();
    // force_redraw = true;
}

void housekeeping_task_user(void) {
    static uint32_t last_tick = 0;
    uint32_t        now       = timer_read32();

#    ifdef WPM_ENABLE
    housekeeping_task_wpm();
#    endif

    if (TIMER_DIFF_32(now, last_tick) >= (QUANTUM_PAINTER_TASK_THROTTLE)) {
        painter_render_user();
        last_tick = now;
    }
#    if (QUANTUM_PAINTER_DISPLAY_TIMEOUT) > 0
    if (is_keyboard_master() && (last_input_activity_elapsed() > QUANTUM_PAINTER_DISPLAY_TIMEOUT)) {
        qp_backlight_disable();
    } else {
        qp_backlight_enable();
    }
#    endif
}

const char *os_variant_to_string(os_variant_t os_detected) {
    switch (os_detected) {
        case OS_WINDOWS:
            return "Windows";
        case OS_WINDOWS_UNSURE:
            return "Windows?";
        case OS_MACOS:
            return "MacOS";
        case OS_IOS:
            return "iOS";
        case OS_PS5:
            return "PS5";
        case OS_HANDHELD:
            return "Handheld";
        case OS_LINUX:
            return "Linux";
        default:
            return "Unknown";
    }
}

/**
 * @brief Truncates text to fit within a certain width
 *
 * @param text original text
 * @param max_width max width in pixels
 * @param font font being used
 * @param from_start truncate from start or end
 * @param add_ellipses add ellipses to truncated text
 * @return char* truncated text
 */
char *truncate_text(const char *text, uint16_t max_width, painter_font_handle_t font, bool from_start,
                    bool add_ellipses) {
    static char truncated_text[50];
    strncpy(truncated_text, text, sizeof(truncated_text) - 1);
    truncated_text[sizeof(truncated_text) - 1] = '\0';

    uint16_t text_width = qp_textwidth(font, truncated_text);
    if (text_width <= max_width) {
        return truncated_text;
    }

    size_t      len            = strlen(truncated_text);
    const char *ellipses       = "...";
    uint16_t    ellipses_width = add_ellipses ? qp_textwidth(font, ellipses) : 0;

    if (from_start) {
        size_t start_index = 0;
        while (start_index < len && text_width > max_width - ellipses_width) {
            start_index++;
            text_width = qp_textwidth(font, truncated_text + start_index);
        }

        if (add_ellipses) {
            char temp[75];
            snprintf(temp, sizeof(temp), "%s%s", ellipses, truncated_text + start_index);
            strncpy(truncated_text, temp, sizeof(truncated_text) - 1);
            truncated_text[sizeof(truncated_text) - 1] = '\0';
        } else {
            memmove(truncated_text, truncated_text + start_index, len - start_index + 1);
        }
    } else {
        while (len > 0 && text_width > max_width - ellipses_width) {
            len--;
            truncated_text[len] = '\0';
            text_width          = qp_textwidth(font, truncated_text);
        }

        if (add_ellipses) {
            snprintf(truncated_text + len, sizeof(truncated_text) - len, "%s", ellipses);
        }
    }

    return truncated_text;
}

void painter_render_frame_box(painter_device_t device, hsv_t hsv, uint16_t x_buffer, uint16_t y_buffer,
                              int16_t x_offset, int16_t y_offset, bool top_indents, bool side_indents) {
    uint16_t width = 0, height = 0;
    qp_get_geometry(device, &width, &height, NULL, NULL, NULL);

    width -= (x_buffer + x_offset);
    height -= (y_buffer + y_offset);

    // draw top of frame
    qp_line(device, x_buffer + 7 + x_offset, y_buffer + y_offset, width - 7 + x_offset, y_buffer + y_offset, hsv.h,
            hsv.s, hsv.v);
    // draw angled bits
    qp_line(device, x_buffer, y_buffer + 6 + y_offset, x_buffer + 6 + x_offset, y_buffer + y_offset, hsv.h, hsv.s,
            hsv.v);
    qp_line(device, width - 7 + x_offset, y_buffer + y_offset, width - 1 + x_offset, 6 + y_buffer + y_offset, hsv.h,
            hsv.s, hsv.v);

    if (top_indents) {
        for (uint8_t line = 0; line < 13; line++) {
            qp_line(device, x_buffer + 14 + line + x_offset, y_buffer + line + y_offset, width - 14 - line + x_offset,
                    y_buffer + line + y_offset, hsv.h, hsv.s, hsv.v);
        }
    }

    // // lines for frame sides
    qp_line(device, x_buffer + x_offset, y_buffer + 7 + y_offset, x_buffer + x_offset, height - (7 + 2) + y_offset,
            hsv.h, hsv.s, hsv.v);
    qp_line(device, width - 1 + x_offset, y_buffer + 7 + y_offset, width - 1 + x_offset, height - (7 + 2) + y_offset,
            hsv.h, hsv.s, hsv.v);

    if (side_indents) {
        for (uint8_t line = 0; line < 8; line++) {
            qp_line(device, x_buffer + line + x_offset, y_buffer + 14 + line + y_offset, x_buffer + line + x_offset,
                    height - 14 - line + y_offset, hsv.h, hsv.s, hsv.v);
            qp_line(device, width - 1 - line + x_offset, y_buffer + 14 + line + y_offset, width - 1 - line + x_offset,
                    height - 14 - line + y_offset, hsv.h, hsv.s, hsv.v);
        }
    }

    // draw angled bits
    qp_line(device, x_buffer + 1 + x_offset, height - 6 - 2 + y_offset, x_buffer + 6 + 1 + x_offset,
            height - 2 + y_offset, hsv.h, hsv.s, hsv.v);
    qp_line(device, width - 7 + x_offset, height - 2 + y_offset, width - 1 + x_offset, height - 6 - 2 + y_offset, hsv.h,
            hsv.s, hsv.v);

    if (top_indents) {
        for (uint8_t line = 0; line < 11; line++) {
            qp_line(device, x_buffer + 14 + line + x_offset, height - 3 - line + y_offset,
                    width - 2 - 14 - line + x_offset, height - 3 - line + y_offset, hsv.h, hsv.s, hsv.v);
        }
    }

    // frame bottom
    qp_line(device, x_buffer + 8 + x_offset, height - 2 + y_offset, width - 8 + x_offset, height - 2 + y_offset, hsv.h,
            hsv.s, hsv.v);
}

#    ifndef ACCEL_GRAPH_SAMPLES
#        define ACCEL_GRAPH_SAMPLES 40
#    endif

void painter_render_pd_accel_graph(painter_device_t device, uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                                   bool force_redraw, dual_hsv_t *curr_hsv) {
#    if defined(COMMUNITY_MODULE_POINTING_DEVICE_ACCEL_ENABLE) && defined(COMMUNITY_MODULE_QP_HELPERS_ENABLE) && \
        defined(POINTING_DEVICE_ENABLE)
    static pointing_device_accel_config_t local        = {0};
    bool                                  needs_redraw = false;
    static uint8_t                        graph_samples[ACCEL_GRAPH_SAMPLES];

    if (memcmp(&local, &g_pointing_device_accel_config, sizeof(pointing_device_accel_config_t)) != 0) {
        local        = g_pointing_device_accel_config;
        needs_redraw = true;
        pointing_device_accel_plot_curve(graph_samples, ACCEL_GRAPH_SAMPLES);
    }

    if (force_redraw || needs_redraw) {
        const graph_line_t lines[] = {
            {
                .data      = graph_samples,
                .color     = curr_hsv->secondary,
                .mode      = LINE,
                .max_value = 127,
            },
            GRAPHS_END,
        };

        const graph_config_t config = {
            .device      = device,
            .start       = {.x = x, .y = y},
            .size        = {.x = width, .y = height},
            .axis        = curr_hsv->primary,
            .background  = {.h = 0, .s = 0, .v = 0},
            .data_points = ACCEL_GRAPH_SAMPLES,
        };

        qp_draw_graph(&config, lines);
    }
#    endif
}

/**
 * @brief Render the keylogger to the display
 *
 * @param device device to render to
 * @param font font to render with
 * @param x x position to start rendering
 * @param y y position to start rendering
 * @param force_redraw do we forcibly redraw the keylogger
 * @param curr_hsv painter colors
 */
void painter_render_wpm(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y, bool force_redraw,
                        dual_hsv_t *curr_hsv) {
#    ifdef WPM_ENABLE
    static uint8_t last_wpm_update = 0;
    static char    buf[4]          = {0};
    uint16_t       temp_x = x + 4, temp_y = y + 4;
    if (force_redraw || last_wpm_update != get_current_wpm()) {
        last_wpm_update = get_current_wpm();
        temp_x += qp_drawtext_recolor(device, temp_x, temp_y, font, "WPM: ", curr_hsv->primary.h, curr_hsv->primary.s,
                                      curr_hsv->primary.v, 0, 0, 0) +
                  26;
        snprintf(buf, sizeof(buf), "%3u", get_current_wpm());
        qp_drawtext_recolor(device, temp_x, temp_y, font, buf, curr_hsv->secondary.h, curr_hsv->secondary.s,
                            curr_hsv->secondary.v, 0, 0, 0);
    }
#    endif // WPM_ENABLE
}

/**
 * @brief Render wpm graph to the display
 *
 * @param device device to render to
 * @param font font to render with
 * @param x x position to start rendering
 * @param y y position to start rendering
 * @param force_redraw do we forcibly redraw the keylogger
 * @param curr_hsv painter colors
 */
void painter_render_wpm_graph(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                              uint16_t width, uint16_t height, bool force_redraw, dual_hsv_t *curr_hsv) {
#    if defined(WPM_ENABLE) && defined(COMMUNITY_MODULE_QP_HELPERS_ENABLE)
    static uint16_t wpm_timer = 0;

    if (force_redraw || timer_elapsed(wpm_timer) > 1000) {
        wpm_timer = timer_read();
        extern uint8_t     wpm_graph_samples[WPM_GRAPH_SAMPLES];
        const graph_line_t lines[] = {
            {
                .data      = wpm_graph_samples,
                .color     = curr_hsv->secondary,
                .mode      = LINE,
                .max_value = 120,
            },
            GRAPHS_END,
        };

        const graph_config_t config = {
            .device      = device,
            .start       = {.x = x, .y = y},
            .size        = {.x = width, .y = height - 1},
            .axis        = curr_hsv->primary,
            .background  = {.h = 0, .s = 0, .v = 0},
            .data_points = WPM_GRAPH_SAMPLES,
        };

        qp_draw_graph(&config, lines);
    }
#    endif // WPM_ENABLE
}

/**
 * @brief Renders the modifier icons on the display.
 *
 * This function draws the current state of the modifier keys (Shift, Control, Alt, Command, Windows) on the specified
 * device using the provided font and coordinates. It recolors the icons based on whether the modifiers are active or
 * not.
 *
 * @param device The painter device to render on.
 * @param font The font handle to use for rendering text.
 * @param x The x-coordinate to start rendering.
 * @param y The y-coordinate to start rendering.
 * @param width The width of the area to render.
 * @param force_redraw If true, forces the redraw of the modifiers regardless of state change.
 * @param curr_hsv The current HSV color values to use for active and inactive states.
 * @param disabled_val The value to use for the disabled state of the modifiers.
 */

void painter_render_modifiers(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                              uint16_t width, bool force_redraw, dual_hsv_t *curr_hsv, uint8_t disabled_val) {
    extern painter_image_handle_t shift_icon, control_icon, alt_icon, command_icon, windows_icon;
    static uint8_t                last_mods    = 0;
    uint8_t                       current_mods = get_mods() | get_weak_mods() | get_oneshot_mods();
    if (force_redraw || last_mods != current_mods) {
        last_mods = current_mods;
        x += qp_drawtext_recolor(device, x, y + 1, font, "Modifiers:", curr_hsv->primary.h, curr_hsv->primary.s,
                                 curr_hsv->primary.v, 0, 0, 0) +
             2;

        if (qp_drawimage_recolor(
                device, x, y, shift_icon, last_mods & MOD_BIT_LSHIFT ? curr_hsv->secondary.h : curr_hsv->primary.h,
                curr_hsv->primary.s, last_mods & MOD_BIT_LSHIFT ? curr_hsv->primary.v : disabled_val, 0, 0, 0)) {
            x += shift_icon->width + 2;
        }
        if (qp_drawimage_recolor(device, x, y, keymap_config.swap_lctl_lgui ? command_icon : windows_icon,
                                 last_mods & MOD_BIT_LGUI ? curr_hsv->secondary.h : curr_hsv->primary.h,
                                 last_mods & MOD_BIT_LGUI ? curr_hsv->secondary.s : curr_hsv->primary.s,
                                 last_mods & MOD_BIT_LGUI ? curr_hsv->primary.v : disabled_val, 0, 0, 0)) {
            x += windows_icon->width + 2;
        }
        if (qp_drawimage_recolor(device, x, y, alt_icon,
                                 last_mods & MOD_BIT_LALT ? curr_hsv->secondary.h : curr_hsv->primary.h,
                                 last_mods & MOD_BIT_LALT ? curr_hsv->secondary.s : curr_hsv->primary.s,
                                 last_mods & MOD_BIT_LALT ? curr_hsv->primary.v : disabled_val, 0, 0, 0)) {
            x += alt_icon->width + 2;
        }
        if (qp_drawimage_recolor(device, x, y, control_icon,
                                 last_mods & MOD_BIT_LCTRL ? curr_hsv->secondary.h : curr_hsv->primary.h,
                                 last_mods & MOD_BIT_LCTRL ? curr_hsv->secondary.s : curr_hsv->primary.s,
                                 last_mods & MOD_BIT_LCTRL ? curr_hsv->primary.v : disabled_val, 0, 0, 0)) {
            x += control_icon->width + 2;
        }
        if (qp_drawimage_recolor(device, x, y, control_icon,
                                 last_mods & MOD_BIT_RCTRL ? curr_hsv->secondary.h : curr_hsv->primary.h,
                                 last_mods & MOD_BIT_RCTRL ? curr_hsv->secondary.s : curr_hsv->primary.s,
                                 last_mods & MOD_BIT_RCTRL ? curr_hsv->primary.v : disabled_val, 0, 0, 0)) {
            x += control_icon->width + 2;
        }
        if (qp_drawimage_recolor(device, x, y, alt_icon,
                                 last_mods & MOD_BIT_RALT ? curr_hsv->secondary.h : curr_hsv->primary.h,
                                 last_mods & MOD_BIT_RALT ? curr_hsv->secondary.s : curr_hsv->primary.s,
                                 last_mods & MOD_BIT_RALT ? curr_hsv->primary.v : disabled_val, 0, 0, 0)) {
            x += alt_icon->width + 2;
        }
        if (qp_drawimage_recolor(device, x, y, keymap_config.swap_rctl_rgui ? command_icon : windows_icon,
                                 last_mods & MOD_BIT_RGUI ? curr_hsv->secondary.h : curr_hsv->primary.h,
                                 last_mods & MOD_BIT_RGUI ? curr_hsv->secondary.s : curr_hsv->primary.s,
                                 last_mods & MOD_BIT_RGUI ? curr_hsv->primary.v : disabled_val, 0, 0, 0)) {
            x += windows_icon->width + 2;
        }
        if (qp_drawimage_recolor(device, x, y, shift_icon,
                                 last_mods & MOD_BIT_RSHIFT ? curr_hsv->secondary.h : curr_hsv->primary.h,
                                 last_mods & MOD_BIT_RSHIFT ? curr_hsv->secondary.s : curr_hsv->primary.s,
                                 last_mods & MOD_BIT_RSHIFT ? curr_hsv->primary.v : disabled_val, 0, 0, 0)) {
            x += shift_icon->width + 2;
        }
    }
}

/**
 * @brief Renders RTC Time to display
 *
 * @param device screen to render to
 * @param font font to use
 * @param x x position to start rendering
 * @param y y position to start rendering
 * @param display_width maximum width for rendering
 * @param force_redraw do we need to redraw regardless of time
 * @param rtc_timer timer to check for redraw
 * @param hsv hsv value to render with
 */
void painter_render_rtc_time(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                             uint16_t display_width, bool force_redraw, hsv_t *hsv) {
#    ifdef COMMUNITY_MODULE_RTC_ENABLE
    static uint8_t last_second = 0xFF;
    bool           rtc_redraw  = false;

    if (rtc_is_connected()) {
        rtc_time_t time = rtc_read_time_struct();
        if (time.second != last_second) {
            last_second = time.second;
            rtc_redraw  = true;
        }
    }
    if (force_redraw || rtc_redraw) {
        char buf[40] = {0};
        if (rtc_is_connected()) {
            snprintf(buf, sizeof(buf), "RTC Date/Time: %s", rtc_read_date_time_str());
        } else {
            snprintf(buf, sizeof(buf), "RTC Device Not Connected");
        }

        uint16_t total_width = display_width - 6 - x;
        uint16_t title_width = qp_textwidth(font, buf);
        if (title_width > (total_width)) {
            title_width = total_width;
        }
        uint16_t title_xpos = (total_width - title_width) / 2 + x;

        qp_drawtext_recolor(device, title_xpos, y, font, truncate_text(buf, total_width, font, false, false), 0, 0, 0,
                            hsv->h, hsv->s, hsv->v);
    }
#    endif // COMMUNITY_MODULE_RTC_ENABLE
}

/**
 * @brief Renders the detected operating system on the display.
 *
 * This function checks the currently detected operating system and displays it on the specified device.
 *
 * @param device The painter device to render on.
 * @param font The font handle to use for rendering text.
 * @param x The x-coordinate to start rendering.
 * @param y The y-coordinate to start rendering.
 * @param width The width of the area to render.
 * @param force_redraw A boolean flag to force redraw even if the OS has not changed.
 * @param curr_hsv A pointer to a dual_hsv_t structure containing the current HSV color values for primary and secondary
 * colors.
 */
void painter_render_os_detection(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                                 uint16_t width, bool force_redraw, dual_hsv_t *curr_hsv) {
#    ifdef OS_DETECTION_ENABLE
    static os_variant_t last_detected_os    = {0};
    char                buf[50]             = {0};
    os_variant_t        current_detected_os = detected_host_os();
    if (force_redraw || last_detected_os != current_detected_os) {
        last_detected_os = current_detected_os;

        qp_drawtext_recolor(device, x + 2, y, font, "Detected OS:", curr_hsv->primary.h, curr_hsv->primary.s,
                            curr_hsv->primary.v, 0, 0, 0);

        y += font->line_height + 4;
        snprintf(buf, sizeof(buf), "%12s", os_variant_to_string(current_detected_os));
        qp_drawtext_recolor(device, x + 4, y, font, buf, curr_hsv->secondary.h, curr_hsv->secondary.s,
                            curr_hsv->secondary.v, 0, 0, 0);
    }
#    endif
}

/**
 * @brief Renders the keylogger information on the display.
 *
 * This function renders the keylogger information on the specified device using the given font and coordinates.
 * It checks if the keylogger state has changed or if a forced redraw is requested, and updates the display accordingly.
 *
 * @param device The painter device to render on.
 * @param font The font handle to use for rendering text.
 * @param x The x-coordinate to start rendering.
 * @param y The y-coordinate to start rendering.
 * @param width The width of the area to render within.
 * @param force_redraw A boolean indicating whether to force a redraw regardless of keylogger state change.
 * @param curr_hsv A pointer to the current HSV color values to use for rendering.
 */
void painter_render_keylogger(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                              uint16_t width, bool force_redraw, dual_hsv_t *curr_hsv) {
    if (is_keylogger_dirty() || force_redraw) {
        qp_rect(device, x, y, x + width - 1, y + font->line_height + 2, 0, 0, 0, true);
        qp_drawtext_recolor(device, x, y, font, truncate_text(get_keylogger_str(), width, font, true, false),
                            curr_hsv->primary.h, curr_hsv->primary.s, curr_hsv->primary.v, 0, 255, 0);
        keylogger_set_dirty(false);
    }
}

/**
 * @brief Render the Caps Lock, Scroll Lock and Num Lock states to the display
 *
 * @param device device to render to
 * @param font font to draw with
 * @param x x position to start rendering
 * @param y y position to start rendering
 * @param force_redraw do we forcible redraw the lock states
 * @param curr_hsv painter hsv values
 * @param disabled_val disabled entry render value
 */
void painter_render_lock_state(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                               bool force_redraw, dual_hsv_t *curr_hsv, uint8_t disabled_val) {
    static led_t last_led_state = {0};
    if (force_redraw || last_led_state.raw != host_keyboard_led_state().raw) {
        last_led_state.raw = host_keyboard_led_state().raw;
        qp_drawtext_recolor(device, x, y, font, "CAPS",
                            last_led_state.caps_lock ? curr_hsv->secondary.h : curr_hsv->primary.h,
                            last_led_state.caps_lock ? curr_hsv->secondary.s : curr_hsv->primary.s,
                            last_led_state.caps_lock ? curr_hsv->primary.v : disabled_val, 0, 0, 0);
        y += font->line_height + 4;
        qp_drawtext_recolor(device, x, y, font, "SCRL",
                            last_led_state.scroll_lock ? curr_hsv->secondary.h : curr_hsv->primary.h,
                            last_led_state.scroll_lock ? curr_hsv->secondary.s : curr_hsv->primary.s,
                            last_led_state.scroll_lock ? curr_hsv->primary.v : disabled_val, 0, 0, 0);
        y += font->line_height + 4;
        qp_drawtext_recolor(device, x, y, font, " NUM",
                            last_led_state.num_lock ? curr_hsv->secondary.h : curr_hsv->primary.h,
                            last_led_state.num_lock ? curr_hsv->secondary.s : curr_hsv->primary.s,
                            last_led_state.num_lock ? curr_hsv->primary.v : disabled_val, 0, 0, 0);
    }
}

/**
 * @brief Renders the autocorrected and original text on the display.
 *
 * This function displays the autocorrected text and the original text on the specified device
 * using the provided font. It updates the display if the text has changed or if a forced redraw
 * is requested.
 *
 * @param device The painter device to render on.
 * @param font The font handle to use for rendering text.
 * @param x The x-coordinate where the text rendering starts.
 * @param y The y-coordinate where the text rendering starts.
 * @param width The width of the area to render the text.
 * @param force_redraw A boolean flag indicating whether to force a redraw of the text.
 * @param curr_hsv A pointer to a dual_hsv_t structure containing the primary and secondary HSV colors for text
 * rendering.
 */
void painter_render_autocorrect(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                                uint16_t width, bool force_redraw, dual_hsv_t *curr_hsv) {
#    if defined(AUTOCORRECT_ENABLE) || defined(COMMUNITY_MODULE_AUTOCORRECT_ENABLE)
    extern bool autocorrect_str_has_changed;
    extern char autocorrected_str_raw[2][21];
    char        buf[50] = {0};

    if (force_redraw || autocorrect_str_has_changed) {
        qp_drawtext_recolor(device, x, y, font, "Autocorrected: ", curr_hsv->primary.h, curr_hsv->primary.s,
                            curr_hsv->primary.v, 0, 0, 0);
        y += font->line_height + 4;
        snprintf(buf, sizeof(buf), "%20s", autocorrected_str_raw[0]);
        qp_drawtext_recolor(device, x, y, font, buf, curr_hsv->secondary.h, curr_hsv->secondary.s,
                            curr_hsv->secondary.v, 0, 0, 0);

        y += font->line_height + 4;
        qp_drawtext_recolor(device, x, y, font, "Original Text: ", curr_hsv->primary.h, curr_hsv->primary.s,
                            curr_hsv->primary.v, 0, 0, 0);
        y += font->line_height + 4;
        snprintf(buf, sizeof(buf), "%20s", autocorrected_str_raw[1]);

        qp_drawtext_recolor(device, x, y, font, buf, curr_hsv->secondary.h, curr_hsv->secondary.s,
                            curr_hsv->secondary.v, 0, 0, 0);
        autocorrect_str_has_changed = false;
    }
#    endif // AUTOCORRECT_ENABLE
}
void painter_render_main_block(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                               uint16_t width, uint16_t height, bool forced_redraw, dual_hsv_t *curr_hsv) {
    uint16_t      xpos = x, ypos = y;
    const uint8_t disabled_val = curr_hsv->primary.v / 2;

    if (forced_redraw) {
        // horizontal line below scan rate
        qp_line(device, xpos, ypos + 14, xpos + 78, ypos + 14, curr_hsv->primary.h, curr_hsv->primary.s,
                curr_hsv->primary.v);

        // vertical lines next to scan rate + wpm + pointing
        qp_line(device, xpos + 78, ypos, xpos + 78, ypos + 123, curr_hsv->primary.h, curr_hsv->primary.s,
                curr_hsv->primary.v);

        // horizontal line below pointing devices
        qp_line(device, xpos, ypos + 68, xpos + 78, ypos + 68, curr_hsv->primary.h, curr_hsv->primary.s,
                curr_hsv->primary.v);

        // horizontal line below pd accel graph + above modifiers
        qp_line(device, xpos + 78, ypos + 123, xpos + 316, ypos + 123, curr_hsv->primary.h, curr_hsv->primary.s,
                curr_hsv->primary.v);

        // vertical line right of modifiers
        qp_line(device, xpos + 154, ypos + 124, xpos + 154, ypos + 140, curr_hsv->primary.h, curr_hsv->primary.s,
                curr_hsv->primary.v);

        // horizontal line below keymap config
        qp_line(device, xpos + 78, ypos + 28, xpos + 206, ypos + 28, curr_hsv->primary.h, curr_hsv->primary.s,
                curr_hsv->primary.v);

        // horizontal line below autocorrect output
        qp_line(device, xpos + 78, ypos + 80, xpos + 206, ypos + 80, curr_hsv->primary.h, curr_hsv->primary.s,
                curr_hsv->primary.v);

        // vertical line right of rgb block
        qp_line(device, xpos + 206, ypos, xpos + 206, ypos + 123, curr_hsv->primary.h, curr_hsv->primary.s,
                curr_hsv->primary.v);

        // caps lock vertical line
        qp_line(device, xpos + 286, ypos, xpos + 286, ypos + 42, curr_hsv->primary.h, curr_hsv->primary.s,
                curr_hsv->primary.v);

        // led lock horizontal line
        qp_line(device, xpos + 286, ypos + 42, xpos + 316, ypos + 42, curr_hsv->primary.h, curr_hsv->primary.s,
                curr_hsv->primary.v);

        // line above wpm value
        qp_line(device, xpos + 206, ypos + 28, xpos + 286, ypos + 28, curr_hsv->primary.h, curr_hsv->primary.s,
                curr_hsv->primary.v);

        // line above wpm value
        qp_line(device, xpos + 206, ypos + 42, xpos + 286, ypos + 42, curr_hsv->primary.h, curr_hsv->primary.s,
                curr_hsv->primary.v);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Matrix scan rate
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    static uint32_t last_scan_rate   = 0;
    static uint16_t scan_rate_timer  = 0;
    bool            scan_rate_redraw = false;
    if (timer_elapsed(scan_rate_timer) > 250) {
        scan_rate_timer = timer_read();
        if (last_scan_rate != get_matrix_scan_rate()) {
            last_scan_rate   = get_matrix_scan_rate();
            scan_rate_redraw = true;
        }
    }
    if (forced_redraw || scan_rate_redraw) {
        char buf[6] = {0};
        xpos        = x + 3;
        ypos        = y + 4;
        xpos += qp_drawtext_recolor(device, xpos, ypos, font_oled, "SCANS: ", curr_hsv->primary.h, curr_hsv->primary.s,
                                    curr_hsv->primary.v, 0, 0, 0);
        snprintf(buf, sizeof(buf), "%5lu", last_scan_rate);
        qp_drawtext_recolor(device, xpos, ypos, font_oled, buf, curr_hsv->primary.h, curr_hsv->primary.s,
                            curr_hsv->primary.v, 0, 0, 0);
    }

#    if defined(POINTING_DEVICE_ENABLE)
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Pointing Device Auto Mouse Layer
    ypos = y + 20;

#        ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
    static uint8_t last_am_state     = 0xFF;
    bool           auto_mouse_redraw = false;
    if (last_am_state != get_auto_mouse_enable()) {
        last_am_state     = get_auto_mouse_enable();
        auto_mouse_redraw = true;
    }
    if (forced_redraw || auto_mouse_redraw) {
        xpos = x + 3;
        qp_drawtext_recolor(device, xpos, ypos, font_oled, "Auto Layer:", curr_hsv->primary.h, curr_hsv->primary.s,
                            curr_hsv->primary.v, 0, 0, 0);
    }
    ypos += font_oled->line_height + 4;

    static uint8_t last_am_layer = 0xFF;
    if (forced_redraw || last_am_layer != get_auto_mouse_layer() || auto_mouse_redraw) {
        last_am_layer = get_auto_mouse_layer();
        char buf[10]  = {0};
        xpos          = x + 3;
        snprintf(buf, sizeof(buf), "%u", get_auto_mouse_layer());
        qp_drawtext_recolor(device, xpos, ypos, font_oled, buf,
                            get_auto_mouse_enable() ? curr_hsv->secondary.h : curr_hsv->primary.h,
                            get_auto_mouse_enable() ? curr_hsv->secondary.s : curr_hsv->primary.s,
                            get_auto_mouse_enable() ? curr_hsv->secondary.v : disabled_val, 0, 0, 0);
    }
    ypos += font_oled->line_height + 4;
#        endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Pointing Device Drag Scroll

#        if defined(COMMUNITY_MODULE_DRAG_SCROLL_ENABLE)
    static uint32_t last_ds_state = 0xFFFFFFFF;
    if (forced_redraw || last_ds_state != get_drag_scroll_scrolling()) {
        last_ds_state = get_drag_scroll_scrolling();
        xpos          = x + 3;
        xpos += qp_drawtext_recolor(device, xpos, ypos, font_oled, "Drag-Scroll",
                                    last_ds_state ? curr_hsv->secondary.h : curr_hsv->primary.h,
                                    last_ds_state ? curr_hsv->secondary.s : curr_hsv->primary.s,
                                    last_ds_state ? curr_hsv->secondary.v : disabled_val, 0, 0, 0);
    }
    ypos += font_oled->line_height + 4;
#        endif // (defined(KEYBOARD_bastardkb_charybdis) || defined(KEYBOARD_handwired_tractyl_manuform))

#        ifdef COMMUNITY_MODULE_POINTING_DEVICE_ACCEL_ENABLE
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Pointing Device Sniping mode
    bool pointing_device_accel_get_enabled(void);

    static bool last_accel_state = false;

    if (forced_redraw || last_accel_state != pointing_device_accel_get_enabled()) {
        last_accel_state = pointing_device_accel_get_enabled();
        xpos             = x + 3;
        xpos += qp_drawtext_recolor(device, xpos, ypos, font_oled, "Acceleration",
                                    last_accel_state ? curr_hsv->secondary.h : curr_hsv->primary.h,
                                    last_accel_state ? curr_hsv->secondary.s : curr_hsv->primary.s,
                                    last_accel_state ? curr_hsv->secondary.v : disabled_val, 0, 0, 0);
    }
    ypos += font_oled->line_height + 4;

    xpos                          = x;
    ypos                          = y + 70;
    static bool last_accel_state2 = false;
    if (forced_redraw || last_accel_state2 != pointing_device_accel_get_enabled()) {
        last_accel_state2 = pointing_device_accel_get_enabled();
        if (last_accel_state2) {
            painter_render_pd_accel_graph(device, xpos - 1, ypos, 78, 54, forced_redraw, curr_hsv);
        } else {
            qp_rect(device, xpos + 1, ypos, xpos + 78, ypos + 54 - 1, 0, 0, 0, true);
            qp_line(device, xpos + 1, ypos + 10, xpos + 78, ypos + 10, curr_hsv->secondary.h, curr_hsv->secondary.s,
                    curr_hsv->secondary.v);
            qp_line(device, xpos + 1, ypos + 54 - 1, xpos + 78, ypos + 54 - 1, curr_hsv->primary.h, curr_hsv->primary.s,
                    curr_hsv->primary.v);
        }
    }
#        endif // COMMUNITY_MODULE_POINTING_DEVICE_ACCEL_ENABLE
#    endif     // POINTING_DEVICE_ENABLE

    painter_render_modifiers(device, font_oled, x + 3, y + 128, 150, forced_redraw, curr_hsv, disabled_val);

    ypos                                        = y + 4;
    static keymap_config_t last_keymap_config   = {0};
    bool                   keymap_config_redraw = false;
    if (last_keymap_config.raw != keymap_config.raw) {
        last_keymap_config.raw = keymap_config.raw;
        keymap_config_redraw   = true;
    }
#    ifdef CAPS_WORD_ENABLE
    static bool caps_words = false;
    if (caps_words != is_caps_word_on()) {
        caps_words           = is_caps_word_on();
        keymap_config_redraw = true;
    }
#    endif // CAPS_WORD_ENABLE
    if (forced_redraw || keymap_config_redraw) {
        xpos = x + 82;
        xpos += qp_drawtext_recolor(device, xpos, ypos, font_oled, "NKRO",
                                    last_keymap_config.nkro ? curr_hsv->secondary.h : curr_hsv->primary.h,
                                    last_keymap_config.nkro ? curr_hsv->secondary.s : curr_hsv->primary.s,
                                    last_keymap_config.nkro ? curr_hsv->secondary.v : disabled_val, 0, 0, 0) +
                5;
        xpos +=
            qp_drawtext_recolor(device, xpos, ypos, font_oled, "CRCT",
                                last_keymap_config.autocorrect_enable ? curr_hsv->secondary.h : curr_hsv->primary.h,
                                last_keymap_config.autocorrect_enable ? curr_hsv->secondary.s : curr_hsv->primary.s,
                                last_keymap_config.autocorrect_enable ? curr_hsv->secondary.v : disabled_val, 0, 0, 0) +
            5;
        xpos += qp_drawtext_recolor(device, xpos, ypos, font_oled, "1SHT",
                                    last_keymap_config.oneshot_enable ? curr_hsv->secondary.h : curr_hsv->primary.h,
                                    last_keymap_config.oneshot_enable ? curr_hsv->secondary.s : curr_hsv->primary.s,
                                    last_keymap_config.oneshot_enable ? curr_hsv->secondary.v : disabled_val, 0, 0, 0) +
                5;
#    ifdef CAPS_WORD_ENABLE
        xpos += qp_drawtext_recolor(device, xpos, ypos, font_oled, "CAPS",
                                    is_caps_word_on() ? curr_hsv->secondary.h : curr_hsv->primary.h,
                                    is_caps_word_on() ? curr_hsv->secondary.s : curr_hsv->primary.s,
                                    is_caps_word_on() ? curr_hsv->secondary.v : disabled_val, 0, 0, 0);
#    endif
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Device Config (Audio, Audio Clicky, Host Driver lock, Swap Hands)

    ypos += font_oled->line_height + 4;
#    ifdef AUDIO_ENABLE
    extern audio_config_t audio_config;
    static audio_config_t last_audio_config = {0};
#    endif // AUDIO_ENABLE
    static bool last_keyboard_lock = false;
#    ifdef SWAP_HANDS_ENABLE
    static bool last_swap_hands = false;
#    endif
    bool get_keyboard_lock(void);

    if (forced_redraw
#    ifdef AUDIO_ENABLE
        || memcmp(&audio_config, &last_audio_config, sizeof(audio_config_t))
#    endif
        || last_keyboard_lock != get_keyboard_lock()
#    ifdef SWAP_HANDS_ENABLE
        || last_swap_hands != swap_hands
#    endif
    ) {

#    ifdef AUDIO_ENABLE
        memcpy(&last_audio_config, &audio_config, sizeof(audio_config_t));
#    endif // AUDIO_ENABLE
        last_keyboard_lock = get_keyboard_lock();
#    ifdef SWAP_HANDS_ENABLE
        last_swap_hands = swap_hands;
#    endif
        xpos = x + 82;
#    ifdef AUDIO_ENABLE
        xpos += qp_drawtext_recolor(device, xpos, ypos, font_oled, "AUDIO",
                                    last_audio_config.enable ? curr_hsv->secondary.h : curr_hsv->primary.h,
                                    last_audio_config.enable ? curr_hsv->secondary.s : curr_hsv->primary.s,
                                    last_audio_config.enable ? curr_hsv->secondary.v : disabled_val, 0, 0, 0) +
                5;
        xpos += qp_drawtext_recolor(device, xpos, ypos, font_oled, "CLCK",
                                    last_audio_config.clicky_enable ? curr_hsv->secondary.h : curr_hsv->primary.h,
                                    last_audio_config.clicky_enable ? curr_hsv->secondary.s : curr_hsv->primary.s,
                                    last_audio_config.clicky_enable ? curr_hsv->secondary.v : disabled_val, 0, 0, 0) +
                5;
#    endif
        xpos += qp_drawtext_recolor(device, xpos, ypos, font_oled, "HOST",
                                    !last_keyboard_lock ? curr_hsv->secondary.h : curr_hsv->primary.h,
                                    !last_keyboard_lock ? curr_hsv->secondary.s : curr_hsv->primary.s,
                                    !last_keyboard_lock ? curr_hsv->secondary.v : disabled_val, 0, 0, 0) +
                5;
#    ifdef SWAP_HANDS_ENABLE
        xpos += qp_drawtext_recolor(device, xpos, ypos, font_oled, "SWAP",
                                    last_swap_hands ? curr_hsv->secondary.h : curr_hsv->primary.h,
                                    last_swap_hands ? curr_hsv->secondary.s : curr_hsv->primary.s,
                                    last_swap_hands ? curr_hsv->secondary.v : disabled_val, 0, 0, 0);
#    endif
    }

    ypos += font_oled->line_height + 8;
    xpos = x + 82;
    painter_render_autocorrect(device, font_oled, xpos, ypos, 128, forced_redraw, curr_hsv);

    ypos                                 = y + 84;
    xpos                                 = x + 82;
    static keyevent_t last_current_event = {0};
    if (forced_redraw || memcmp(&last_current_event, &last_key_event, sizeof(keyevent_t))) {
        char buf[21] = {0};
        memcpy(&last_current_event, &last_key_event, sizeof(keyevent_t));
        uint16_t temp_x = xpos;
        qp_drawtext_recolor(device, xpos, ypos, font_oled, "Last keyevent:", curr_hsv->primary.h, curr_hsv->primary.s,
                            curr_hsv->primary.v, 0, 0, 0);
        ypos += font_oled->line_height + 5;
        temp_x += qp_drawtext_recolor(device, xpos, ypos, font_oled, "Row:", curr_hsv->primary.h, curr_hsv->primary.s,
                                      curr_hsv->primary.v, 0, 0, 0);

        snprintf(buf, 17, "%16s", get_u8_str(last_current_event.key.row, ' '));
        qp_drawtext_recolor(device, temp_x, ypos, font_oled, buf, curr_hsv->secondary.h, curr_hsv->secondary.s,
                            curr_hsv->secondary.v, 0, 0, 0);
        ypos += font_oled->line_height + 5;
        temp_x = xpos + qp_drawtext_recolor(device, xpos, ypos, font_oled, "Column:", curr_hsv->primary.h,
                                            curr_hsv->primary.s, curr_hsv->primary.v, 0, 0, 0);

        snprintf(buf, 14, "%13s", get_u8_str(last_current_event.key.col, ' '));
        qp_drawtext_recolor(device, temp_x, ypos, font_oled, buf, curr_hsv->secondary.h, curr_hsv->secondary.s,
                            curr_hsv->secondary.v, 0, 0, 0);
    }

#    ifdef OS_DETECTION_ENABLE
    ypos = y + 4;
    xpos = x + 208;
    painter_render_os_detection(device, font_oled, xpos, ypos, 80, forced_redraw, curr_hsv);
#    endif // OS_DETECTION_ENABLE

#    ifdef WPM_ENABLE
    xpos = x + 206;
    ypos = y + 28;
    painter_render_wpm(device, font_oled, xpos, ypos, forced_redraw, curr_hsv);
    ypos += font_oled->line_height + 2 * 4;

    xpos = x + 206;
    ypos = y + 43;
    painter_render_wpm_graph(device, font_oled, xpos, ypos, WPM_PAINTER_GRAPH_WIDTH, WPM_PAINTER_GRAPH_HEIGHT,
                             forced_redraw, curr_hsv);
#    endif

    ypos                                 = y + 100;
    xpos                                 = x + 210;
    static uint16_t last_checked_keycode = 0xFFFF;
    if (forced_redraw || last_keycode != last_checked_keycode) {
        char buf[18]         = {0};
        last_checked_keycode = last_keycode;
        qp_drawtext_recolor(device, xpos, ypos, font_oled, "Last keycode:", curr_hsv->primary.h, curr_hsv->primary.s,
                            curr_hsv->primary.v, 0, 0, 0);
        ypos += font_oled->line_height + 4;
        snprintf(buf, 18, "%17s", get_keycode_string(last_keycode));
        qp_drawtext_recolor(device, xpos, ypos, font_oled, buf, curr_hsv->secondary.h, curr_hsv->secondary.s,
                            curr_hsv->secondary.v, 0, 0, 0);
    }

    ypos = y + 4;
    xpos = x + 290;
    painter_render_lock_state(device, font_oled, xpos, ypos, forced_redraw, curr_hsv, disabled_val);

    xpos = x + 160;
    ypos = y + 128;
    painter_render_keylogger(device, font_oled, xpos, ypos, 150, forced_redraw, curr_hsv);
}

/**
 * @brief Render the console log to the display
 *
 * @param device device to render ton
 * @param font font to render with
 * @param x x position to start rendering
 * @param y y position to start rendering
 * @param display_width maximum width for rendering
 * @param force_redraw do we forcibly redraw the console log
 * @param hsv colors to render with
 * @param start start line to render
 * @param end last line to render
 */
void painter_render_console(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                            uint16_t display_width, bool force_redraw, hsv_t *hsv, uint8_t start, uint8_t end) {
    if (!console_has_redrawn || force_redraw) {
        for (uint8_t i = start; i < end; i++) {
            uint16_t xpos =
                x + qp_drawtext_recolor(device, x, y, font, logline_ptrs[i], hsv->h, hsv->s, hsv->v, 0, 0, 0);
            qp_rect(device, xpos, y, display_width, y + font->line_height, 0, 0, 0, true);
            y += font->line_height + 4;
        }
        console_has_redrawn = true;
    }
}

void painter_render_menu_block_console(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                                       uint16_t width, uint16_t height, bool forced_redraw, dual_hsv_t *curr_hsv) {
    painter_render_console(device, font, x + 2, y + 4, width, forced_redraw, &curr_hsv->primary,
                           DISPLAY_CONSOLE_LOG_LINE_START, DISPLAY_CONSOLE_LOG_LINE_NUM);
}

/**
 * @brief Renders QMK information on the display.
 *
 * This function displays the build date, QMK version, and compiler version on the specified device.
 * It also draws a QMK banner image.
 *
 * @param device        The painter device to render on.
 * @param font          The font handle to use for text rendering.
 * @param x             The x-coordinate to start rendering.
 * @param y             The y-coordinate to start rendering.
 * @param width         The width of the rendering area.
 * @param force_redraw  A flag to force redraw.
 * @param curr_hsv      Pointer to a dual_hsv_t structure containing the current HSV color values.
 */
void painter_render_qmk_info(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                             uint16_t width, bool force_redraw, dual_hsv_t *curr_hsv) {
    char     buf[50] = {0};
    uint16_t xpos    = x + 5;

    if (force_redraw) {
        snprintf(buf, sizeof(buf), "%s", QMK_BUILDDATE);
        xpos += qp_drawtext_recolor(device, xpos, y, font, "Built on: ", curr_hsv->primary.h, curr_hsv->primary.s,
                                    curr_hsv->primary.v, 0, 0, 0);
        qp_drawtext_recolor(device, xpos, y, font, buf, curr_hsv->secondary.h, curr_hsv->secondary.s,
                            curr_hsv->secondary.v, 0, 0, 0);
        xpos = x + 5;
        y += font->line_height + 4;
        snprintf(buf, sizeof(buf), "%s", QMK_VERSION);
        xpos += qp_drawtext_recolor(device, xpos, y, font, "Built from: ", curr_hsv->primary.h, curr_hsv->primary.s,
                                    curr_hsv->primary.v, 0, 0, 0);
        qp_drawtext_recolor(device, xpos, y, font, buf, curr_hsv->secondary.h, curr_hsv->secondary.s,
                            curr_hsv->secondary.v, 0, 0, 0);
        xpos = x + 5;
        y += font->line_height + 4;
        xpos += qp_drawtext_recolor(device, xpos, y, font, "Built with: ", curr_hsv->primary.h, curr_hsv->primary.s,
                                    curr_hsv->primary.v, 0, 0, 0);
        xpos += qp_drawtext_recolor(device, xpos, y, font, __VERSION__, curr_hsv->secondary.h, curr_hsv->secondary.s,
                                    curr_hsv->secondary.v, 0, 0, 0) +
                10;
        xpos += qp_drawtext_recolor(device, xpos, y, font, "Newlib: ", curr_hsv->primary.h, curr_hsv->primary.s,
                                    curr_hsv->primary.v, 0, 0, 0);
        qp_drawtext_recolor(device, xpos, y, font, _NEWLIB_VERSION, curr_hsv->secondary.h, curr_hsv->secondary.s,
                            curr_hsv->secondary.v, 0, 0, 0);
        xpos = x + 5;
        y += font->line_height + 4;

        qp_drawimage(device, x, y, qmk_banner);
    }
}

void painter_render_menu_block_qmk_info(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                                        uint16_t width, uint16_t height, bool force_redraw, dual_hsv_t *curr_hsv) {
    painter_render_qmk_info(device, font, x, y + 5, width, force_redraw, curr_hsv);
}

/**
 * @brief Renders the layer map on the display.
 *
 * This function renders the current keymap based on the active layers on the specified painter device.
 *
 * @param device The display device to render on.
 * @param font The font handle to use for rendering text.
 * @param x The x-coordinate to start rendering from.
 * @param y The y-coordinate to start rendering from.
 * @param width The width of the area to render within.
 * @param force_redraw A boolean indicating whether to force a redraw of the layer map.
 * @param curr_hsv A pointer to a dual_hsv_t structure containing the primary and secondary HSV color values to use for
 * rendering.
 */
void painter_render_layer_map(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                              uint16_t width, bool force_redraw, dual_hsv_t *curr_hsv) {
#    ifdef COMMUNITY_MODULE_LAYER_MAP_ENABLE
    if (force_redraw || get_layer_map_has_updated()) {
        y += font->line_height + 14;
        uint16_t xpos = x, ypos = y;
        for (uint8_t lm_y = 0; lm_y < LAYER_MAP_ROWS; lm_y++) {
            xpos = x + 20;
            for (uint8_t lm_x = 0; lm_x < LAYER_MAP_COLS; lm_x++) {
                uint16_t keycode = extract_non_basic_keycode(layer_map[lm_y][lm_x], NULL, false);
#        ifdef LAYER_MAP_REMAPPING
                keypos_t key = layer_remap[lm_y][lm_x];
#        else  // LAYER_MAP_REMAPPING
                keypos_t key = {.row = lm_y, .col = lm_x};
#        endif // LAYER_MAP_REMAPPING

                xpos += MAX(qp_drawtext_recolor(device, xpos, ypos, font_oled, get_keyode_character(keycode, &key),
                                                curr_hsv->primary.h, curr_hsv->primary.s,
                                                peek_matrix_layer_map(lm_y, lm_x) ? 0 : curr_hsv->primary.v,
                                                curr_hsv->secondary.h, curr_hsv->secondary.s,
                                                peek_matrix_layer_map(lm_y, lm_x) ? curr_hsv->secondary.v : 0),
                            6);
                xpos += qp_drawtext_recolor(device, xpos, ypos, font, " ", 0, 0, 0, 0, 0, 0);
            }
            ypos += font->line_height + 4;
        }
        set_layer_map_has_updated(false);
    }
#    endif // COMMUNITY_MODULE_LAYER_MAP_ENABLE
}

void painter_render_menu_block_layer_map(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                                         uint16_t width, uint16_t height, bool force_redraw, dual_hsv_t *curr_hsv) {
    painter_render_layer_map(device, font, x, y, width, force_redraw, curr_hsv);
}

typedef struct {
    void (*painter_function)(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                             uint16_t width, uint16_t height, bool force_redraw, dual_hsv_t *curr_hsv);
    const char *name;
} painter_display_menu_block_mode_t;

painter_display_menu_block_mode_t painter_display_menu_block_modes[] = {
    {painter_render_main_block, "Info"},
    {painter_render_menu_block_console, "Console"},
    {painter_render_menu_block_qmk_info, "QMK Info"},
#    ifdef COMMUNITY_MODULE_LAYER_MAP_ENABLE
    {painter_render_menu_block_layer_map, "Layer Map"},
#    endif // COMMUNITY_MODULE_LAYER_MAP_ENABLE
};

const uint8_t painter_display_menu_block_modes_count = ARRAY_SIZE(painter_display_menu_block_modes);
uint8_t       painter_display_menu_block_mode_index  = 0;

void painter_render_user(void) {
    bool          hue_redraw = force_redraw;
    uint16_t      width = 0, height = 0;
    dual_hsv_t    curr_hsv     = (dual_hsv_t){.primary   = (hsv_t){.h = 128, .s = 255, .v = 255},
                                              .secondary = (hsv_t){.h = 40, .s = 255, .v = 255}};
    const uint8_t disabled_val = curr_hsv.primary.v / 2;
    (void)disabled_val;

    qp_get_geometry(st7789_170x320_display, &width, &height, NULL, NULL, NULL);

    if (force_redraw) {
        qp_rect(st7789_170x320_surface_display, 0, 0, width, height, 0, 0, 0, true);
    }

    if (hue_redraw) {
        painter_render_frame_box(st7789_170x320_surface_display, curr_hsv.primary, 1, 0, 0, 3, true, false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Title text
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    if (hue_redraw) {
        char title[50] = {0};
        snprintf(title, sizeof(title), "%s", PRODUCT);
        uint16_t title_width = qp_textwidth(font_oled, title);
        if (title_width > (width - 55)) {
            title_width = width;
        }
        uint16_t title_xpos = (width - title_width) / 2;
        qp_drawtext_recolor(st7789_170x320_surface_display, title_xpos, 6, font_oled, title, 0, 0, 0,
                            curr_hsv.primary.h, curr_hsv.primary.s, curr_hsv.primary.v);
    }
    if (painter_render_menu(st7789_170x320_surface_display, font_oled, 2, 16, 316, 140, true, curr_hsv.primary,
                            curr_hsv.secondary)) {
        force_redraw = true;
    } else {
        painter_display_menu_block_modes[painter_display_menu_block_mode_index].painter_function(
            st7789_170x320_surface_display, font_oled, 2, 16, 316, 140, hue_redraw, &curr_hsv);
    }

#    ifdef COMMUNITY_MODULE_RTC_ENABLE
    painter_render_rtc_time(st7789_170x320_surface_display, font_oled, 0, height - (3 + font_oled->line_height), width,
                            hue_redraw, &curr_hsv.primary);
#    endif

    qp_surface_draw(st7789_170x320_surface_display, st7789_170x320_display, 0, 0, false);
    qp_flush(st7789_170x320_display);
    force_redraw = false;
}

#endif // QUANTUM_PAINTER_ENABLE

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        last_keycode   = keycode;
        last_key_event = record->event;

        switch (keycode) {
            case QP_CYCLE_MENU_BLOCK:
                painter_display_menu_block_mode_index =
                    (painter_display_menu_block_mode_index + 1) % painter_display_menu_block_modes_count;
                force_redraw = true;
                break;
            default:
                break;
        }
    }
    return true;
}
