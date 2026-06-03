// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include QMK_KEYBOARD_H

// Button order in matrix: bottom left, top left, top right, bottom right
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // clang-format off
    [0] = LAYOUT(MS_BTN3, DRAG_SCROLL,
                 MS_BTN1, MS_BTN2
    ),
    // clang-format on
};

// Set up a few combos to do things with multiple buttons.
enum combos {
    BACK,
    FORWARD,
};

// Top right button, followed by top left.
const uint16_t PROGMEM back_combo[] = {DRAG_SCROLL, MS_BTN3, COMBO_END};
// Top left button, followed by top right.
const uint16_t PROGMEM forward_combo[] = {MS_BTN3, DRAG_SCROLL, COMBO_END};

combo_t key_combos[] = {
    [BACK]    = COMBO(back_combo, MS_BTN4),
    [FORWARD] = COMBO(forward_combo, MS_BTN5),
};
