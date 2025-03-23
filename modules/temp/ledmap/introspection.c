// Copyright Pablo Martinez (@elpekenin) <elpekenin@elpekenin.dev>
// SPDX-License-Identifier: GPL-2.0-or-later

#include <platforms/progmem.h>

#define NUM_LEDMAP_LAYERS_RAW (sizeof(ledmap) / (MATRIX_ROWS * MATRIX_COLS * sizeof(ledmap_color_t)))

_Static_assert(NUM_KEYMAP_LAYERS_RAW == NUM_LEDMAP_LAYERS_RAW, "Number of ledmap layers doesn't match the number of keymap layers");

uint8_t ledmap_layer_count(void) {
    return NUM_LEDMAP_LAYERS_RAW;
}

ledmap_color_t color_at_ledmap_location(uint8_t layer, uint8_t row, uint8_t col) {
    return pgm_read_byte(&(ledmap[layer][row][col]));
}
