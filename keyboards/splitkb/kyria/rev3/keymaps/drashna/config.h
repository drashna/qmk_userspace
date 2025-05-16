// Copyright 2023 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#define EE_HANDS

#ifdef OLED_ENABLE
#    undef OLED_DISPLAY_128X64
#    define OLED_DISPLAY_128X128
#    define OLED_PRE_CHARGE_PERIOD 0x22
#    define OLED_VCOM_DETECT       0x35
#    define OLED_DISPLAY_FORCE_SCREENSAVER
#endif

#ifdef RGBLIGHT_ENABLE
#    define RGBLIGHT_SLEEP
#    undef RGBLIGHT_LIMIT_VAL
#    define RGBLIGHT_LIMIT_VAL 120
#    define RGBLIGHT_HUE_STEP  8
#    define RGBLIGHT_SAT_STEP  8
#    define RGBLIGHT_VAL_STEP  8
#    define RGBLIGHT_SPLIT
#endif

#define QMK_ESC_INPUT  D4
#define QMK_ESC_OUTPUT B2

#ifdef KEYBOARD_splitkb_kyria_rev3
#    define SERIAL_USART_FULL_DUPLEX     // Enable full duplex operation mode.
#    define SERIAL_USART_PIN_SWAP        // Swap TX and RX pins if keyboard is master halve.
#    define SERIAL_USART_DRIVER      SD1 // USART driver of TX pin. default: SD1
#    define SERIAL_USART_TX_PAL_MODE \
        7 // Pin "alternate function", see the respective datasheet for the appropriate values for your MCU. default: 7
#    undef SOFT_SERIAL_PIN
#    define SERIAL_USART_TX_PIN D3
#    define SERIAL_USART_RX_PIN D2

#    undef WS2812_DI_PIN
#    define WS2812_DI_PIN       PAL_LINE(GPIOA, 3)
#    define WS2812_PWM_DRIVER   PWMD15
#    define WS2812_PWM_CHANNEL  2
#    define WS2812_PWM_PAL_MODE 9
#    define WS2812_DMA_STREAM   STM32_DMA1_STREAM5
#    define WS2812_DMA_CHANNEL  5

#    define BOOTMAGIC_ROW          0
#    define BOOTMAGIC_COLUMN       6
#    define BOOTMAGIC_ROW_RIGHT    4
#    define BOOTMAGIC_COLUMN_RIGHT 6

#    define BOOTMAGIC_EEPROM_ROW          1
#    define BOOTMAGIC_EEPROM_COLUMN       6
#    define BOOTMAGIC_EEPROM_ROW_RIGHT    5
#    define BOOTMAGIC_EEPROM_COLUMN_RIGHT 6
#    define SECURE_UNLOCK_SEQUENCE        {{1, 5}, {1, 4}, {1, 3}, {1, 2}}

#    define ENCODER_RESOLUTION  2
// faster i2c? 1MHz, maybe?
#    define I2C1_TIMINGR_PRESC  0U
#    define I2C1_TIMINGR_SCLDEL 10U
#    define I2C1_TIMINGR_SDADEL 1U
#    define I2C1_TIMINGR_SCLH   13U
#    define I2C1_TIMINGR_SCLL   31U

#else
#    define BOOTMAGIC_ROW          0
#    define BOOTMAGIC_COLUMN       7
#    define BOOTMAGIC_ROW_RIGHT    4
#    define BOOTMAGIC_COLUMN_RIGHT 7

#    define BOOTMAGIC_EEPROM_ROW          1
#    define BOOTMAGIC_EEPROM_COLUMN       7
#    define BOOTMAGIC_EEPROM_ROW_RIGHT    5
#    define BOOTMAGIC_EEPROM_COLUMN_RIGHT 7
#endif

#define SERIAL_USART_SPEED 921600

#if defined(KEYBOARD_splitkb_kyria_rev1_proton_c) || defined(KEYBOARD_splitkb_kyria_rev3)
#    define WEAR_LEVELING_BACKING_SIZE 2048
#    define WEAR_LEVELING_LOGICAL_SIZE 1024
#endif

#define LAYER_MAP_REMAPPING
#define LAYER_MAP_COLS 18
#define LAYER_MAP_ROWS 6
