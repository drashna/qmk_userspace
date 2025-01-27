// Copyright 2024 Harrison Chan (@xelus22)
// SPDX-License-Identifier: GPL-2.0-or-later

#include <stdlib.h>
#include "clap_trap.h"
#include "keycodes.h"
#include "action_util.h"
#include "drashna_layers.h"
#include "drashna_runtime.h"
#include "keyrecords/process_records.h"
#include "wait.h"

// Max 10 since we only have 10 fingers to press keys
#define KEYREPORT_BUFFER_SIZE 10

_Static_assert(KEYREPORT_BUFFER_SIZE <= 16,
               "KEYREPORT_BUFFER_SIZE must be less than or equal to 16 due to bitfield usage");

const clap_trap_t PROGMEM clap_trap_list[] = {
    {KC_A, KC_D},
    {KC_D, KC_A},
};

// Clap Trap up stroke buffer
uint16_t buffer_keyreports[KEYREPORT_BUFFER_SIZE];

// track the next free index
int buffer_keyreport_count = 0;

uint16_t clap_trap_count(void) {
    return sizeof(clap_trap_list) / sizeof(clap_trap_t);
}

clap_trap_t clap_trap_get(uint16_t idx) {
    clap_trap_t ret;
    memcpy_P(&ret, &clap_trap_list[idx], sizeof(clap_trap_t));
    return ret;
}

/**
 * @brief function for querying the enabled state of key cancellation
 *
 * @return true if enabled
 * @return false if disabled
 */
bool clap_trap_is_enabled(void) {
    return userspace_config.gaming.clap_trap_enable;
}

/**
 * @brief Enables key cancellation and saves state to eeprom
 *
 */
void clap_trap_enable(void) {
    userspace_config.gaming.clap_trap_enable = true;
    eeconfig_update_user_datablock(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
}

/**
 * @brief Disables key cancellation and saves state to eeprom
 *
 */
void clap_trap_disable(void) {
    userspace_config.gaming.clap_trap_enable = false;
    eeconfig_update_user_datablock(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
}

/**
 * @brief Toggles key cancellation's status and save state to eeprom
 *
 */
void clap_trap_toggle(void) {
    userspace_config.gaming.clap_trap_enable = !userspace_config.gaming.clap_trap_enable;
    eeconfig_update_user_datablock(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
}

/**
 * @brief handler for user to override whether key cancellation should process this keypress
 *
 * @param keycode Keycode registered by matrix press, per keymap
 * @param record keyrecord_t structure
 * @return true Allow key cancellation
 * @return false Stop processing and escape from key cancellation
 */
bool process_clap_trap_keymap(uint16_t keycode, keyrecord_t *record) {
    return true;
}

/**
 * @brief handler for user to override whether key cancellation should process this keypress
 *
 * @param keycode Keycode registered by matrix press, per keymap
 * @param record keyrecord_t structure
 * @return true Allow key cancellation
 * @return false Stop processing and escape from key cancellation
 */
bool process_clap_trap_user(uint16_t keycode, keyrecord_t *record) {
    if (!process_clap_trap_keymap(keycode, record)) {
        return false;
    }
    return is_gaming_layer_active(layer_state);
}

// check if key already in buffer
int get_key_index_in_buffer(uint16_t keycode) {
    for (int i = 0; i < buffer_keyreport_count; i++) {
        if (buffer_keyreports[i] == keycode) {
            ac_dprintf("Clap Trap: Found Keycode <%d> index <%d>\n", keycode, i);
            return i;
        }
    }
    return -1;
}

void add_key_buffer(uint16_t keycode) {
    if (get_key_index_in_buffer(keycode) > 0) {
        ac_dprintf("Clap Trap: %d Key already in buffer\n", keycode);
        return;
    }

    // sanity check don't write past the buffer size
    if (buffer_keyreport_count >= KEYREPORT_BUFFER_SIZE) {
        ac_dprintf("Clap Trap: Buffer full\n");
        return;
    }

    buffer_keyreports[buffer_keyreport_count] = keycode;
    buffer_keyreport_count++;

    ac_dprintf("Clap Trap: Added <%d>\n", keycode);
}

// remove keycode and shift buffer
void del_key_buffer(uint16_t keycode) {
    for (int i = 0; i < buffer_keyreport_count; i++) {
        if (buffer_keyreports[i] == keycode) {
            for (int j = i; j < buffer_keyreport_count - 1; j++) {
                buffer_keyreports[j] = buffer_keyreports[j + 1];
            }
            buffer_keyreport_count--;
            break;
        }
    }
    ac_dprintf("Clap Trap: Added <%d>\n", keycode);
}

// check if keycode is in the interrupt press list
bool clap_trap_is_key_in_press_list(uint16_t keycode) {
    for (int i = 0; i < clap_trap_count(); i++) {
        if (clap_trap_get(i).press == keycode) {
            ac_dprintf("Clap Trap: Keycode <%d> in clap_trap_list\n", keycode);
            return true;
        }
    }
    return false;
}

/**
 * @brief Process handler for clap_trap feature
 *
 * @param keycode Keycode registered by matrix press, per keymap
 * @param record keyrecord_t structure
 * @return true Continue processing keycodes, and send to host
 * @return false Stop processing keycodes, and don't send to host
 */
bool process_clap_trap(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        switch (keycode) {
            case CLAP_TRAP_ON:
                clap_trap_enable();
                return false;
            case CLAP_TRAP_OFF:
                clap_trap_disable();
                return false;
            case CLAP_TRAP_TOGGLE:
                clap_trap_toggle();
                return false;
            default:
                break;
        }
    }

    if (!userspace_config.gaming.clap_trap_enable) {
        return true;
    }

    // only supports basic keycodes
    if (!IS_BASIC_KEYCODE(keycode)) {
        return true;
    }

    // custom user hook
    if (!process_clap_trap_user(keycode, record)) {
        return true;
    }

    // only Clap Trap to buffer if required
    if (clap_trap_is_key_in_press_list(keycode)) {
        if (record->event.pressed) {
            add_key_buffer(keycode);
        } else {
            del_key_buffer(keycode);
        }
    }

    if (buffer_keyreport_count == 0) {
        return true;
    }

    ac_dprintf("buffer_keyreport_count: %d\n", buffer_keyreport_count);

    if (record->event.pressed) {
        for (int i = 0; i < clap_trap_count(); i++) {
            clap_trap_t clap_trap = clap_trap_get(i);
            if (keycode == clap_trap.press) {
                del_key(clap_trap.unpress);
            }
        }
    } else {
        uint16_t bitfield_keyreports         = 0xFF << (16 - buffer_keyreport_count);
        uint16_t bitfield_keyreports_scratch = bitfield_keyreports;

        for (int j = buffer_keyreport_count - 1; j >= 0; j--) {
            uint16_t current_bitmask = 1 << (16 - j);
            // skip if bit is not set
            if (!(bitfield_keyreports_scratch & current_bitmask)) {
                continue;
            }
            for (int i = 0; i < clap_trap_count(); i++) {
                clap_trap_t clap_trap = clap_trap_get(i);
                // if key in buffer is the same as the Clap Trap press
                if (clap_trap.press == buffer_keyreports[j]) {
                    // if Clap Trap unpress is in buffer
                    int index = get_key_index_in_buffer(clap_trap.unpress);
                    if (index > 0) {
                        // remove Clap Trap unpress from buffer
                        bitfield_keyreports_scratch &= ~(1 << (16 - index));
                    }
                }
            }
        }

        // compare buffer and temp buffer
        uint16_t comparison = bitfield_keyreports ^ bitfield_keyreports_scratch;

        // bits that are 1 means it is different
        // if there are differences and bit in bitfield_keyreports_scratch is 0, then del key
        // if there are no differences then add key

        // compare buffer and temp buffer
        for (int i = 0; i < buffer_keyreport_count; i++) {
            uint16_t bitmask = 1 << (16 - i);
            if (comparison & bitmask) {
                if (~(bitfield_keyreports_scratch & bitmask)) {
                    wait_ms(rand() % 10);
                    del_key(buffer_keyreports[i]);
                }
            } else {
                add_key(buffer_keyreports[i]);
            }
        }
    }

    return true;
}
