// Copyright Pablo Martinez (@elpekenin) <elpekenin@elpekenin.dev>
// SPDX-License-Identifier: GPL-2.0-or-later

#include <platforms/progmem.h>

#define NUM_INDICATORS_RAW (sizeof(indicators) / sizeof(indicator_t))

uint8_t indicators_count(void) {
    return NUM_INDICATORS_RAW;
}

indicator_t get_indicator(uint8_t i) {
    return *(indicator_t *)pgm_read_ptr(&(indicators[i]));
}
