// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#ifdef KEYBOARD_bastardkb_charybdis_3x5_blackpill

#    undef MATRIX_ROW_PINS
#    define MATRIX_ROW_PINS {B12, B13, B14, B15}
#    undef MATRIX_COL_PINS
#    define MATRIX_COL_PINS {A15, B3, B4, B5, B6}

#    undef MATRIX_ROW_PINS_RIGHT
#    define MATRIX_ROW_PINS_RIGHT {B12, B13, B14, B15}
#    undef MATRIX_COL_PINS_RIGHT
#    define MATRIX_COL_PINS_RIGHT {A15, B3, B4, B5, B6}

#    define USB_VBUS_PIN B10
#    undef SPLIT_HAND_PIN
#    define SPLIT_HAND_PIN C14 // high = left, low = right

// WS2812 RGB LED strip input and number of LEDs
#    undef WS2812_DI_PIN
#    define WS2812_DI_PIN A1
#    undef RGB_MATRIX_MAXIMUM_BRIGHTNESS
#    define RGB_MATRIX_MAXIMUM_BRIGHTNESS 150
#    define WS2812_PWM_DRIVER             PWMD2
#    define WS2812_PWM_CHANNEL            2
#    define WS2812_PWM_PAL_MODE           1
#    define WS2812_EXTERNAL_PULLUP
// #define WS2812_PWM_COMPLEMENTARY_OUTPUT
#    undef WS2812_DMA_STREAM
#    define WS2812_DMA_STREAM        STM32_DMA1_STREAM7
#    define WS2812_DMA_CHANNEL       3
#    define WS2812_PWM_TARGET_PERIOD 800000

#    define DEBUG_LED_PIN C13

/* Audio config */
#    define AUDIO_PIN          B1
#    define AUDIO_PWM_DRIVER   PWMD3
#    define AUDIO_PWM_CHANNEL  4
#    define AUDIO_PWM_PAL_MODE 2

/* serial.c configuration for split keyboard */
#    undef SOFT_SERIAL_PIN
#    define SERIAL_USART_FULL_DUPLEX
#    undef SERIAL_USART_TX_PIN
#    define SERIAL_USART_TX_PIN A2
#    undef SERIAL_USART_RX_PIN
#    define SERIAL_USART_RX_PIN      A3
#    define SERIAL_USART_DRIVER      SD2
#    define SERIAL_USART_TX_PAL_MODE 7
#    define SERIAL_USART_RX_PAL_MODE 7
#    define SERIAL_USART_TIMEOUT     100
#    define SERIAL_USART_SPEED       921600
// #define SERIAL_USART_PIN_SWAP // swap RX and TX pins on master
// To use the highest possible baudrate (3.75Mbit/s) uncomment the following
// line, this can result in dropped communications so lower the speed if there
// are many timeouts.
// #define SERIAL_USART_SPEED (STM32_PCLK2 >> 4)

#    define CRC8_USE_TABLE
#    define CRC8_OPTIMIZE_SPEED

/* spi config for eeprom and pmw3360 sensor */
#    define SPI_DRIVER        SPID1
#    define SPI_SCK_PIN       A5
#    define SPI_SCK_PAL_MODE  5
#    define SPI_MOSI_PIN      A7
#    define SPI_MOSI_PAL_MODE 5
#    define SPI_MISO_PIN      A6
#    define SPI_MISO_PAL_MODE 5

/* eeprom config */
#    define EXTERNAL_EEPROM_SPI_SLAVE_SELECT_PIN A4
#    define EXTERNAL_EEPROM_SPI_CLOCK_DIVISOR    64
// #define EXTERNAL_EEPROM_BYTE_COUNT           8196
// #define EXTERNAL_EEPROM_PAGE_SIZE            32
// #define EXTERNAL_EEPROM_ADDRESS_SIZE         2

/* pmw3360 config  */
#    undef PMW33XX_CS_PIN
#    define PMW33XX_CS_PIN B0
#endif

#define CHARYBDIS_MINIMUM_DEFAULT_DPI     1200
#define CHARYBDIS_DEFAULT_DPI_CONFIG_STEP 400
#define CHARYBDIS_MINIMUM_SNIPING_DPI     200
#define CHARYBDIS_SNIPING_DPI_CONFIG_STEP 100
