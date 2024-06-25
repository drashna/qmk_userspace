// Copyright 2023 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "action.h"
#include "keyboard.h"
#include "keymap_common.h"
#include "action_layer.h"
#include "layer_map.h"

volatile uint16_t layer_map[LAYER_MAP_ROWS][LAYER_MAP_COLS] = {0};
static bool       layer_map_set                             = false;
extern bool       peek_matrix(uint8_t row_index, uint8_t col_index, bool read_raw);

void set_layer_map(void) {
    layer_map_set = true;
}

void populate_layer_map(void) {
    // xprintf("Layer map set\n\n");
    for (uint8_t i = 0; i < LAYER_MAP_ROWS; i++) {
        for (uint8_t j = 0; j < LAYER_MAP_COLS; j++) {
#ifdef LAYER_MAP_REMAPPING
            keypos_t key = layer_remap[i][j];
#else
            keypos_t key = {.row = i, .col = j};
#endif

#ifdef SWAP_HANDS_ENABLE
            if (is_swap_hands_on()) {
#    ifdef LAYER_MAP_REMAPPING
                key.row = pgm_read_byte(&hand_swap_config[layer_remap[i][j].row][layer_remap[i][j].col].row);
                key.col = pgm_read_byte(&hand_swap_config[layer_remap[i][j].row][layer_remap[i][j].col].col);
#    else
                key.row = pgm_read_byte(&hand_swap_config[i][j].row);
                key.col = pgm_read_byte(&hand_swap_config[i][j].col);
#    endif
            }
#endif
            layer_map[i][j] = keymap_key_to_keycode(layer_switch_get_layer(key), key);
            // xprintf("0x%04x, ", layer_map[i][j]);
        }
        // xprintf("\n");
    }
}

bool peek_matrix_layer_map(uint8_t row, uint8_t col) {
#ifdef LAYER_MAP_REMAPPING
    if (layer_remap[row][col].row >= KEYLOC_DIP_SWITCH_OFF) {
        return false;
    }
    return peek_matrix(layer_remap[row][col].row, layer_remap[row][col].col, false);
#else
    if (row = > KEYLOC_DIP_SWITCH_OFF) {
        return false;
    }
    return peek_matrix(row, col, false);
#endif
}

void housekeeping_task_layer_map(void) {
#ifdef SWAP_HANDS_ENABLE
    static bool swap_hands = false;
    if (is_swap_hands_on() != swap_hands) {
        swap_hands    = is_swap_hands_on();
        layer_map_set = true;
    }
#endif
    if (layer_map_set) {
        populate_layer_map();
        layer_map_set = false;
    }
}

#ifdef DYNAMIC_KEYMAP_ENABLE
#    include "dynamic_keymap.h"

#    ifdef ENCODER_MAP_ENABLE
bool dynamic_keymap_set_encoder_user(uint8_t layer, uint8_t encoder_id, bool clockwise, uint16_t keycode) {
    layer_map_set = true;
    return true;
}
#    endif

bool dynamic_keymap_set_keycode_user(uint8_t layer, uint8_t row, uint8_t column, uint16_t keycode) {
    layer_map_set = true;
    return true;
}
bool dynamic_keymap_set_buffer_user(uint16_t offset, uint16_t size, uint8_t *data) {
    layer_map_set = true;
    return true;
}
#endif
