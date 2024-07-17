// Copyright 2024 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "custom_key_cancellation.h"

const key_cancellation_t PROGMEM key_cancellation_list[] = {
    [KEY_CANC_AD] = {KC_A, KC_D},
    [KEY_CANC_DA] = {KC_D, KC_A},
};
