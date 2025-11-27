// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H

// Column pin bitmasks (pins we read)
#define GPIOA_BITMASK (1 << 0 | 1 << 1 | 1 << 2) // A0, A1, A2 (columns)
#define GPIOA_OFFSET  0
#define GPIOA_COUNT   3
#define GPIOC_BITMASK (1 << 1 | 1 << 2 | 1 << 3) // C1, C2, C3 (columns)
#define GPIOC_OFFSET  1

// Pin definitions
static const pin_t row_pins[MATRIX_ROWS] = MATRIX_ROW_PINS;
static const pin_t col_pins[MATRIX_COLS] = MATRIX_COL_PINS;

void matrix_wait_for_pin(pin_t pin, uint8_t target_state) {
    rtcnt_t start = chSysGetRealtimeCounterX();
    rtcnt_t end   = start + 5000;
    while (chSysIsCounterWithinX(chSysGetRealtimeCounterX(), start, end)) {
        if (gpio_read_pin(pin) == target_state) {
            break;
        }
    }
}

void matrix_wait_for_port(stm32_gpio_t *port, uint32_t target_bitmask) {
    rtcnt_t start = chSysGetRealtimeCounterX();
    rtcnt_t end   = start + 5000;
    while (chSysIsCounterWithinX(chSysGetRealtimeCounterX(), start, end)) {
        if ((palReadPort(port) & target_bitmask) == target_bitmask) {
            break;
        }
    }
}

static void dummy_vt_callback(virtual_timer_t *vtp, void *p) {}

void matrix_init_custom(void) {
    for (int i = 0; i < MATRIX_ROWS; ++i) {
        gpio_set_pin_input_high(row_pins[i]);
    }
    for (int i = 0; i < MATRIX_COLS; ++i) {
        gpio_set_pin_input_high(col_pins[i]);
    }

    // Start a virtual timer so we'll still get periodic wakeups, now that USB SOF doesn't wake up the main loop
    static virtual_timer_t vt;
    chVTObjectInit(&vt);
    chVTSetContinuous(&vt, TIME_MS2I(10), dummy_vt_callback, NULL);
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    static matrix_row_t temp_matrix[MATRIX_ROWS] = {0};

    for (int current_row = 0; current_row < MATRIX_ROWS; ++current_row) {
        // Keep track of the pin we're working with
        pin_t curr_row_pin = row_pins[current_row];

        // Setup the output row pin
        gpio_set_pin_output(curr_row_pin);
        gpio_write_pin_low(curr_row_pin);
        matrix_wait_for_pin(curr_row_pin, 0);

        // Read the column ports
        uint32_t gpio_a = palReadPort(GPIOA);
        uint32_t gpio_c = palReadPort(GPIOC);

        // Unselect the row pin
        gpio_set_pin_input_high(curr_row_pin);

        // Construct the packed bitmask for the column pins
        // Columns: C1, C2, C3, A0, A1, A2 (in that order for matrix cols 0-5)
        uint32_t col_readback = 0;
        col_readback |= ((gpio_c & GPIOC_BITMASK) >> GPIOC_OFFSET) << 0;           // C1, C2, C3 -> bits 0,1,2
        col_readback |= ((gpio_a & GPIOA_BITMASK) >> GPIOA_OFFSET) << GPIOA_COUNT; // A0, A1, A2 -> bits 3,4,5

        // Invert the readback (active low)
        col_readback = ~col_readback;

        // Inject values into the matrix
        temp_matrix[current_row] = col_readback & ((1ul << MATRIX_COLS) - 1);

        // Wait for readback of the unselected row to go high
        matrix_wait_for_pin(curr_row_pin, 1);

        // Wait for readback of each port to go high -- unselecting the row would have been completed
        matrix_wait_for_port(GPIOA, GPIOA_BITMASK);
        matrix_wait_for_port(GPIOC, GPIOC_BITMASK);
    }

    // Check if we've changed, return the last-read data
    bool changed = memcmp(current_matrix, temp_matrix, sizeof(temp_matrix)) != 0;
    if (changed) {
        memcpy(current_matrix, temp_matrix, sizeof(temp_matrix));
    }
    return changed;
}

void matrix_wait_for_interrupt(void) {
    // Set up row/col pins and attach callback
    for (int i = 0; i < ARRAY_SIZE(row_pins); ++i) {
        gpio_set_pin_output(row_pins[i]);
        gpio_write_pin_low(row_pins[i]);
    }
    for (int i = 0; i < ARRAY_SIZE(col_pins); ++i) {
        gpio_set_pin_input_high(col_pins[i]);
        palEnableLineEvent(col_pins[i], PAL_EVENT_MODE_BOTH_EDGES);
    }

    // Wait for an interrupt
    __WFI();

    // Now that the interrupt has woken us up, reset all the row/col pins back to defaults
    for (int i = 0; i < ARRAY_SIZE(col_pins); ++i) {
        palDisableLineEvent(col_pins[i]);
        gpio_write_pin_high(col_pins[i]);
        gpio_set_pin_input_high(col_pins[i]);
    }
    for (int i = 0; i < ARRAY_SIZE(row_pins); ++i) {
        gpio_write_pin_high(row_pins[i]);
        gpio_set_pin_input_high(row_pins[i]);
    }
}
