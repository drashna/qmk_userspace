// Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#define SPLIT_TRANSPORT_MIRROR
#if (defined(__AVR__) && defined(SPLIT_USB_DETECT)) || (!defined(__AVR__) && !defined(USB_VBUS_PIN))
#    define SPLIT_WATCHDOG_ENABLE
#endif // (defined(__AVR__) && defined(SPLIT_USB_DETECT)) || (!defined(__AVR__) && !defined(USB_VBUS_PIN))
#define SPLIT_DETECTED_OS_ENABLE
#define SPLIT_HAPTIC_ENABLE
#define SPLIT_ACTIVITY_ENABLE
#define SPLIT_WPM_ENABLE
#define SPLIT_LAYER_STATE_ENABLE
#define SPLIT_MODS_ENABLE
#define SPLIT_LED_STATE_ENABLE

#if defined(__AVR__) && !defined(SELECT_SOFT_SERIAL_SPEED)
#    define SELECT_SOFT_SERIAL_SPEED 1
#endif // defined(__AVR__) && !defined(SELECT_SOFT_SERIAL_SPEED)

#ifdef CUSTOM_SPLIT_TRANSPORT_SYNC
#    define RPC_M2S_BUFFER_SIZE 128
#    define RPC_S2M_BUFFER_SIZE 32
#    undef SPLIT_OLED_ENABLE
#    define SPLIT_TRANSACTION_IDS_USER RPC_ID_EXTENDED_SYNC_TRANSPORT
#endif // CUSTOM_SPLIT_TRANSPORT_SYNC
