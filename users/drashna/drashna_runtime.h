// Copyright 2024 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <util.h>
#include <eeconfig.h>
#include <quantum/color.h>
#include <action_layer.h>
#include <keycode_config.h>
#include <led.h>
#include "debug.h"

typedef struct PACKED {
    hsv_t primary;
    hsv_t secondary;
} dual_hsv_t;

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
            uint8_t brightness  : 8;
            bool    screen_lock : 1;
        } oled;
        struct {
            bool i2c_scanner_enable : 1;
            bool matrix_scan_print  : 1;
        } debug;
        struct {
            bool  enable_acceleration : 1;
            bool  auto_mouse_layer_enable : 1;
            float growth_rate;
            float offset;
            float limit;
            float takeoff;
        } pointing;
        struct {
            uint8_t    display_mode : 3;
            uint8_t    display_logo : 4;
            dual_hsv_t hsv;
            bool       inverted : 1;
            uint8_t    rotation : 2;
        } painter;
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

typedef struct PACKED {
    bool    dirty;
    bool    is_in_menu;
    uint8_t selected_child;
    uint8_t menu_stack[8];
} menu_state_t;

typedef struct PACKED {
    uint8_t mods;
    uint8_t weak_mods;
    uint8_t oneshot_mods;
    uint8_t oneshot_locked_mods;
} sync_mods_t;

typedef struct PACKED {
    layer_state_t layer_state;
    layer_state_t default_layer_state;
} sync_layer_t;

typedef struct PACKED {
    uint32_t matrix_timestamp;
    uint32_t encoder_timestamp;
    uint32_t pointing_device_timestamp;
} sync_activity_t;

typedef struct PACKED {
    struct {
        bool enable        : 1;
        bool clicky_enable : 1;
    } audio;
    struct {
        bool tap_toggling         : 1;
        bool swap_hands           : 1;
        bool host_driver_disabled : 1;
        bool is_caps_word         : 1;
        bool is_device_suspended  : 1;
    } internals;
    struct {
        uint8_t mode        : 3;
        uint8_t typing_mode : 4;
    } unicode;
    menu_state_t    menu_state;
    sync_mods_t     mods;
    sync_layer_t    layers;
    led_t           leds;
    uint8_t         wpm_count;
    sync_activity_t activity;
    keymap_config_t keymap_config;
    debug_config_t  debug_config;
} user_runtime_config_t;

extern user_runtime_config_t user_runtime_state;
