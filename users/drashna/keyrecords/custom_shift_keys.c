// Copyright 2021-2023 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file custom_shift_keys.c
 * @brief Custom Shift Keys implementation
 *
 * For full documentation, see
 * <https://getreuer.info/posts/keyboards/custom-shift-keys>
 */

#include "custom_shift_keys.h"

__attribute__((weak)) const custom_shift_key_t custom_shift_keys[] = {};

const uint8_t NUM_CUSTOM_SHIFT_KEYS = sizeof(custom_shift_keys) / sizeof(custom_shift_key_t);

bool process_custom_shift_keys(uint16_t keycode, keyrecord_t *record) {
    static uint16_t registered_keycode = KC_NO;

    // If a custom shift key is registered, then this event is either releasing
    // it or manipulating another key at the same time. Either way, we release
    // the currently registered key.
    if (registered_keycode != KC_NO) {
        unregister_code16(registered_keycode);
        registered_keycode = KC_NO;
    }

    if (record->event.pressed) { // Press event.
        const uint8_t mods = get_mods();
#ifndef NO_ACTION_ONESHOT
        if ((mods | get_weak_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT) {
#else // NO_ACTION_ONESHOT
        if ((mods | get_weak_mods()) & MOD_MASK_SHIFT) { // Shift is held.
#endif // NO_ACTION_ONESHOT
       // Continue default handling if this is a tap-hold key being held.
            if ((IS_QK_MOD_TAP(keycode) || IS_QK_LAYER_TAP(keycode)) && record->tap.count == 0) {
                return true;
            }

            // Search for a custom shift key whose keycode is `keycode`.
            for (int i = 0; i < NUM_CUSTOM_SHIFT_KEYS; ++i) {
                if (keycode == custom_shift_keys[i].keycode) {
                    registered_keycode = custom_shift_keys[i].shifted_keycode;
                    if (IS_QK_MODS(registered_keycode) && // Should keycode be shifted?
                        (QK_MODS_GET_MODS(registered_keycode) & MOD_LSFT) != 0) {
                        register_code16(registered_keycode); // If so, press it directly.
                    } else {
                        // Otherwise cancel shift mods, press the key, and restore mods.
                        del_weak_mods(MOD_MASK_SHIFT);
#ifndef NO_ACTION_ONESHOT
                        del_oneshot_mods(MOD_MASK_SHIFT);
#endif // NO_ACTION_ONESHOT
                        unregister_mods(MOD_MASK_SHIFT);
                        register_code16(registered_keycode);
                        set_mods(mods);
                    }
                    return false;
                }
            }
        }
    }

    return true; // Continue with default handling.
}
