// Copyright 2023 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "process_key_cancellation.h"

enum {
    KEY_CANC_AD,
    KEY_CANC_DA,
    KEY_CANCELLATION_MAX,
};

const key_cancellation_t PROGMEM key_cancellation_list[KEY_CANCELLATION_MAX];
