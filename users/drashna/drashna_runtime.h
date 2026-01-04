// Copyright 2024 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <util.h>
#include <quantum/color.h>
#include <action_layer.h>
#include <keycode_config.h>
#include <led.h>
#include "debug.h"

typedef struct PACKED {
    hsv_t primary;
    hsv_t secondary;
} dual_hsv_t;

typedef struct PACKED {
    bool    inverted;
    uint8_t rotation;
    uint8_t display_mode;
    uint8_t display_logo;
    bool    display_logo_cycle;
} painter_options_t;

typedef union PACKED {
    uint8_t raw[EECONFIG_USER_DATA_SIZE];
    struct {
        struct {
            bool layer_change : 1;
            bool idle_anim    : 1;
        } rgb;
        struct {
            bool is_overwatch     : 1;
            bool swapped_numbers  : 1;
            bool clap_trap_enable : 1;
            bool song_enable      : 1;
        } gaming;
        struct {
            uint8_t menu_render_side : 2;
            struct {
                uint8_t brightness  : 8;
                bool    screen_lock : 1;
                uint8_t rotation    : 2;
                bool    inverted    : 1;
                struct {
                    uint8_t index       : 2;
                    uint8_t sleep_speed : 8;
                    uint8_t kaki_speed  : 8;
                    uint8_t mati_speed  : 8;
                } pet;
            } oled;
            struct {
                painter_options_t left;
                painter_options_t right;
                dual_hsv_t        hsv;
            } painter;
        } display;
        struct {
            bool i2c_scanner_enable : 1;
            bool matrix_scan_print  : 1;
            bool console_keylogger  : 1;
        } debug;
        struct {
            struct {
                bool  enabled : 1;
                float growth_rate;
                float offset;
                float limit;
                float takeoff;
            } accel;
            bool audio_mouse_clicky : 1;
            struct {
                bool     enable   : 1;
                uint8_t  layer    : 6;
                uint16_t timeout  : 16;
                uint8_t  debounce : 8;
            } auto_mouse_layer;
            struct {
                bool    enable  : 1;
                uint8_t timeout : 8;
            } mouse_jiggler;
        } pointing;
        struct {
            bool   format_24h : 1;
            bool   is_dst     : 1;
            int8_t timezone   : 6;
        } rtc;
        bool nuke_switch : 1;
        bool check       : 1;
    };
} userspace_config_t;

_Static_assert(sizeof(userspace_config_t) <= EECONFIG_USER_DATA_SIZE, "User EECONFIG block is not large enough.");

extern userspace_config_t userspace_config;

#if defined(COMMUNITY_MODULE_DISPLAY_MENU_ENABLE)
#    include "display_menu.h"
#else
typedef struct PACKED {
    bool    is_in_menu;
    uint8_t selected_child;
    uint8_t menu_stack[8];
} menu_state_t;

typedef struct PACKED {
    bool dirty        : 1;
    bool has_rendered : 1;
} menu_state_runtime_t;
#endif // CUSTOM_QUANTUM_PAINTER_ENABLE

typedef struct PACKED {
    uint8_t wpm_peak : 8;
    uint8_t wpm_avg  : 8;
} wpm_sync_data_t;

typedef struct PACKED {
    struct {
        bool  enable        : 1;
        bool  clicky_enable : 1;
        bool  music_enable  : 1;
        float clicky_freq;
        float clicky_rand;
    } audio;
    struct {
        bool tap_toggling         : 1;
        bool swap_hands           : 1;
        bool host_driver_disabled : 1;
        bool is_caps_word         : 1;
    } internals;
    struct {
        uint8_t mode        : 3;
        uint8_t typing_mode : 4;
    } unicode;
    struct {
        menu_state_t         menu_state;
        menu_state_runtime_t menu_state_runtime;
        uint8_t              screensaver_offset;
    } display;
    struct {
        struct {
            bool running : 1;
        } mouse_jiggler;
    } pointing;
    wpm_sync_data_t wpm;
    uint16_t        last_keycode : 16;
    keyevent_t      last_key_event;
} userspace_runtime_state_t;

extern userspace_runtime_state_t userspace_runtime_state;
