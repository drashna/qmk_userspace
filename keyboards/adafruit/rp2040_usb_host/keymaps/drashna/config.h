// Copyright 2023 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#define OLED_DISPLAY_128X128
#define OLED_UPDATE_PROCESS_LIMIT 2
#define I2C1_CLOCK_SPEED          1000000

#define DRV2605L_FB_ERM_LRA     0
#define DRV2605L_FB_BRAKEFACTOR 3 /* For 1x:0, 2x:1, 3x:2, 4x:3, 6x:4, 8x:5, 16x:6, Disable Braking:7 */
#define DRV2605L_FB_LOOPGAIN    1 /* For  Low:0, Medium:1, High:2, Very High:3 */

/* Please refer to your datasheet for the optimal setting for your specific motor. */
#define DRV2605L_RATED_VOLTAGE 3
#define DRV2605L_V_PEAK        3

#define DRV2605L_GREETING     DRV2605L_EFFECT_750_MS_ALERT_100
#define DRV2605L_DEFAULT_MODE DRV2605L_EFFECT_BUZZ_1_100

#define SPI_DRIVER   SPID1
#define SPI_SCK_PIN  GP14
#define SPI_MOSI_PIN GP15
#define SPI_MISO_PIN GP8

#define OLED_DC_PIN      GP1
#define OLED_CS_PIN      GP0
#define OLED_RESET_PIN   GP4
#define OLED_SPI_DIVISOR 4

#define DISPLAY_RST_PIN       GP1
#define DISPLAY_DC_PIN        GP0
#define DISPLAY_CS_PIN        GP4
#define DISPLAY_SPI_DIVIDER   1
#define BACKLIGHT_PIN         GP9
#define BACKLIGHT_PWM_DRIVER  PWMD4
#define BACKLIGHT_PWM_CHANNEL RP2040_PWM_CHANNEL_B

#define PAINTER_SCREENSAVER_NEON_GENESIS
#define PAINTER_SCREENSAVER_CYBERPUNK_2077
#define PAINTER_SCREENSAVER_ANIME
#define PAINTER_SCREENSAVER_STARRYXMARIE
#define PAINTER_SCREENSAVER_SUCCUBI
#define PAINTER_SCREENSAVER_GOTH
#define PAINTER_SCREENSAVER_EGIRLS

#define DYNAMIC_KEYMAP_LAYER_COUNT 4
