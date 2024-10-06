// Copyright 2023 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#undef OLED_DISPLAY_128X64
#define OLED_DISPLAY_128X128
#define OLED_BRIGHTNESS 200

#define CHARYBDIS_MINIMUM_DEFAULT_DPI     1200
#define CHARYBDIS_DEFAULT_DPI_CONFIG_STEP 200
#define CHARYBDIS_MINIMUM_SNIPING_DPI     400
#define CHARYBDIS_SNIPING_DPI_CONFIG_STEP 200

#define BOOTMAGIC_EEPROM_ROW          1
#define BOOTMAGIC_EEPROM_COLUMN       0
#define BOOTMAGIC_EEPROM_ROW_RIGHT    7
#define BOOTMAGIC_EEPROM_COLUMN_RIGHT 5

#define DRV2605L_FB_ERM_LRA   0
#define DRV2605L_RATED_VOLTAGE 3
#define DRV2605L_V_RMS         2.3
#define DRV2605L_V_PEAK        3.30
#define DRV2605L_LIBRARY       2

#define DRV2605L_GREETING     DRV2605L_EFFECT_750_MS_ALERT_100
#define DRV2605L_DEFAULT_MODE DRV2605L_EFFECT_BUZZ_1_100

#define PMW33XX_LIFTOFF_DISTANCE 0b0111

#define DEBOUNCE 15

#define LAYER_MAP_REMAPPING
#define LAYER_MAP_COLS 17
#define LAYER_MAP_ROWS 8

#if defined(KEYBOARD_handwired_tractyl_manuform_5x6_right_f405) || \
    defined(KEYBOARD_handwired_tractyl_manuform_5x6_right_f407)
#    define RGBLIGHT_LIMIT_VAL 255
#    undef RGB_MATRIX_MAXIMUM_BRIGHTNESS
#    define RGB_MATRIX_MAXIMUM_BRIGHTNESS 255
#    define WEAR_LEVELING_BACKING_SIZE    (0x4000)
#    define EECONFIG_USER_DATA_SIZE       64
#elif defined(KEYBOARD_handwired_tractyl_manuform_5x6_right_f411)
#    define EECONFIG_USER_DATA_SIZE 64
#endif

#define QUANTUM_PAINTER_DISPLAY_TIMEOUT 600000
#define RGB_MATRIX_TIMEOUT              600000
#define SPLIT_USB_TIMEOUT               500
#define SPLIT_WATCHDOG_TIMEOUT          700
#define AUDIO_INIT_DELAY
