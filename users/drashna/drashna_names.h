// Copyright 2018-2024 Nick Brassel (@tzarc)
// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <stdint.h>
#include "action_layer.h"

#ifdef OS_DETECTION_ENABLE
#    include "os_detection.h"
#endif // OS_DETECTION_ENABLE
#if defined(HAPTIC_ENABLE) && defined(HAPTIC_DRV2605L)
#    include "drv2605l.h"
#endif // HAPTIC_ENABLE && HAPTIC_DRV2605L

#define layer_name(layer) get_layer_name_string(layer, false, true)
const char *get_layer_name_string(uint8_t layer, bool alt_name, bool is_default);
bool        is_gaming_layer_active(layer_state_t state);
void        format_layer_bitmap_string(char *buffer, layer_state_t state, layer_state_t default_state);

#if defined(HAPTIC_ENABLE) && defined(HAPTIC_DRV2605L)
const char *get_haptic_drv2605l_effect_name(drv2605l_effect_t effect);
#endif // HAPTIC_ENABLE && HAPTIC_DRV2605L
#ifdef OS_DETECTION_ENABLE
const char *os_variant_to_string(os_variant_t os);
#endif // OS_DETECTION_ENABLE
