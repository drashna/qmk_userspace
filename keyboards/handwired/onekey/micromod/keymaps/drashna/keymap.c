// Copyright 2020 QMK
// SPDX-License-Identifier: GPL-3.0-or-later

#include "drashna.h"

enum custom_keycodes {
    FUN_TIME = USER_SAFE_RANGE,
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_ortho_1x1(FUN_TIME)
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] =  { ENCODER_CCW_CW(RGB_MOD, RGB_RMOD) },
};
#endif

#ifdef RGB_MATRIX_ENABLE
const is31_led g_is31_leds[RGB_MATRIX_LED_COUNT] = {
    {0,  CS3_SW9,   CS2_SW9 ,  CS1_SW9 },
    {0,  CS5_SW9,   CS4_SW9 ,  CS6_SW9 },
    {0,  CS9_SW9,   CS8_SW9 ,  CS7_SW9 },
    {0,  CS11_SW9,  CS10_SW9,  CS12_SW9 },
    {0,  CS15_SW9,  CS14_SW9,  CS13_SW9 },
    {0,  CS17_SW9,  CS16_SW9,  CS18_SW9 },
    {0,  CS21_SW9,  CS20_SW9,  CS19_SW9 },
    {0,  CS23_SW9,  CS22_SW9,  CS24_SW9 },
    {0,  CS27_SW9,  CS26_SW9,  CS25_SW9 },
    {0,  CS29_SW9,  CS28_SW9,  CS30_SW9 },
    {0,  CS33_SW9,  CS32_SW9,  CS31_SW9 },
    {0,  CS35_SW9,  CS34_SW9,  CS36_SW9 },
    {0,  CS38_SW9,  CS37_SW9,  CS39_SW9 },
    {0,  CS3_SW6,   CS2_SW6 ,  CS1_SW6  },
    {0,  CS5_SW6,   CS4_SW6 ,  CS6_SW6  },
    {0,  CS9_SW6,   CS8_SW6 ,  CS7_SW6  },
    {0,  CS11_SW6,  CS10_SW6,  CS12_SW6 },
    {0,  CS15_SW6,  CS14_SW6,  CS13_SW6 },
    {0,  CS17_SW6,  CS16_SW6,  CS18_SW6 },
    {0,  CS21_SW6,  CS20_SW6,  CS19_SW6 },
    {0,  CS23_SW6,  CS22_SW6,  CS24_SW6 },
    {0,  CS27_SW6,  CS26_SW6,  CS25_SW6 },
    {0,  CS29_SW6,  CS28_SW6,  CS30_SW6 },
    {0,  CS33_SW6,  CS32_SW6,  CS31_SW6 },
    {0,  CS35_SW6,  CS34_SW6,  CS36_SW6 },
    {0,  CS38_SW6,  CS37_SW6,  CS39_SW6 },
    {0,  CS3_SW5,   CS2_SW5 ,  CS1_SW5  },
    {0,  CS5_SW5,   CS4_SW5 ,  CS6_SW5  },
    {0,  CS9_SW5,   CS8_SW5 ,  CS7_SW5  },
    {0,  CS11_SW5,  CS10_SW5,  CS12_SW5 },
    {0,  CS15_SW5,  CS14_SW5,  CS13_SW5 },
    {0,  CS17_SW5,  CS16_SW5,  CS18_SW5 },
    {0,  CS21_SW5,  CS20_SW5,  CS19_SW5 },
    {0,  CS23_SW5,  CS22_SW5,  CS24_SW5 },
    {0,  CS27_SW5,  CS26_SW5,  CS25_SW5 },
    {0,  CS29_SW5,  CS28_SW5,  CS30_SW5 },
    {0,  CS33_SW5,  CS32_SW5,  CS31_SW5 },
    {0,  CS35_SW5,  CS34_SW5,  CS36_SW5 },
    {0,  CS38_SW5,  CS37_SW5,  CS39_SW5 },
    {0,  CS3_SW4,   CS2_SW4 ,  CS1_SW4  },
    {0,  CS5_SW4,   CS4_SW4 ,  CS6_SW4  },
    {0,  CS9_SW4,   CS8_SW4 ,  CS7_SW4  },
    {0,  CS11_SW4,  CS10_SW4,  CS12_SW4 },
    {0,  CS15_SW4,  CS14_SW4,  CS13_SW4 },
    {0,  CS17_SW4,  CS16_SW4,  CS18_SW4 },
    {0,  CS21_SW4,  CS20_SW4,  CS19_SW4 },
    {0,  CS23_SW4,  CS22_SW4,  CS24_SW4 },
    {0,  CS27_SW4,  CS26_SW4,  CS25_SW4 },
    {0,  CS29_SW4,  CS28_SW4,  CS30_SW4 },
    {0,  CS33_SW4,  CS32_SW4,  CS31_SW4 },
    {0,  CS35_SW4,  CS34_SW4,  CS36_SW4 },
    {0,  CS38_SW4,  CS37_SW4,  CS39_SW4 },
    {0,  CS3_SW3,   CS2_SW3 ,  CS1_SW3  },
    {0,  CS5_SW3,   CS4_SW3 ,  CS6_SW3  },
    {0,  CS9_SW3,   CS8_SW3 ,  CS7_SW3  },
    {0,  CS11_SW3,  CS10_SW3,  CS12_SW3 },
    {0,  CS15_SW3,  CS14_SW3,  CS13_SW3 },
    {0,  CS17_SW3,  CS16_SW3,  CS18_SW3 },
    {0,  CS21_SW3,  CS20_SW3,  CS19_SW3 },
    {0,  CS23_SW3,  CS22_SW3,  CS24_SW3 },
    {0,  CS27_SW3,  CS26_SW3,  CS25_SW3 },
    {0,  CS29_SW3,  CS28_SW3,  CS30_SW3 },
    {0,  CS33_SW3,  CS32_SW3,  CS31_SW3 },
    {0,  CS35_SW3,  CS34_SW3,  CS36_SW3 },
    {0,  CS38_SW3,  CS37_SW3,  CS39_SW3 },
    {0,  CS3_SW2,   CS2_SW2 ,  CS1_SW2  },
    {0,  CS5_SW2,   CS4_SW2 ,  CS6_SW2  },
    {0,  CS9_SW2,   CS8_SW2 ,  CS7_SW2  },
    {0,  CS11_SW2,  CS10_SW2,  CS12_SW2 },
    {0,  CS15_SW2,  CS14_SW2,  CS13_SW2 },
    {0,  CS17_SW2,  CS16_SW2,  CS18_SW2 },
    {0,  CS21_SW2,  CS20_SW2,  CS19_SW2 },
    {0,  CS23_SW2,  CS22_SW2,  CS24_SW2 },
    {0,  CS27_SW2,  CS26_SW2,  CS25_SW2 },
    {0,  CS29_SW2,  CS28_SW2,  CS30_SW2 },
    {0,  CS33_SW2,  CS32_SW2,  CS31_SW2 },
    {0,  CS35_SW2,  CS34_SW2,  CS36_SW2 },
    {0,  CS38_SW2,  CS37_SW2,  CS39_SW2 },
    {0,  CS3_SW1,   CS2_SW1 ,  CS1_SW1  },
    {0,  CS5_SW1,   CS4_SW1 ,  CS6_SW1  },
    {0,  CS9_SW1,   CS8_SW1 ,  CS7_SW1  },
    {0,  CS11_SW1,  CS10_SW1,  CS12_SW1 },
    {0,  CS15_SW1,  CS14_SW1,  CS13_SW1 },
    {0,  CS17_SW1,  CS16_SW1,  CS18_SW1 },
    {0,  CS21_SW1,  CS20_SW1,  CS19_SW1 },
    {0,  CS23_SW1,  CS22_SW1,  CS24_SW1 },
    {0,  CS27_SW1,  CS26_SW1,  CS25_SW1 },
    {0,  CS29_SW1,  CS28_SW1,  CS30_SW1 },
    {0,  CS33_SW1,  CS32_SW1,  CS31_SW1 },
    {0,  CS35_SW1,  CS34_SW1,  CS36_SW1 },
    {0,  CS38_SW1,  CS37_SW1,  CS39_SW1 },
    {0,  CS3_SW8,   CS2_SW8 ,  CS1_SW8  },
    {0,  CS5_SW8,   CS4_SW8 ,  CS6_SW8  },
    {0,  CS9_SW8,   CS8_SW8 ,  CS7_SW8  },
    {0,  CS11_SW8,  CS10_SW8,  CS12_SW8 },
    {0,  CS15_SW8,  CS14_SW8,  CS13_SW8 },
    {0,  CS17_SW8,  CS16_SW8,  CS18_SW8 },
    {0,  CS21_SW8,  CS20_SW8,  CS19_SW8 },
    {0,  CS23_SW8,  CS22_SW8,  CS24_SW8 },
    {0,  CS27_SW8,  CS26_SW8,  CS25_SW8 },
    {0,  CS29_SW8,  CS28_SW8,  CS30_SW8 },
    {0,  CS33_SW8,  CS32_SW8,  CS31_SW8 },
    {0,  CS35_SW8,  CS34_SW8,  CS36_SW8 },
    {0,  CS38_SW8,  CS37_SW8,  CS39_SW8 },
    {0,  CS3_SW7,   CS2_SW7 ,  CS1_SW7  },
    {0,  CS5_SW7,   CS4_SW7 ,  CS6_SW7  },
    {0,  CS9_SW7,   CS8_SW7 ,  CS7_SW7  },
    {0,  CS11_SW7,  CS10_SW7,  CS12_SW7 },
    {0,  CS15_SW7,  CS14_SW7,  CS13_SW7 },
    {0,  CS17_SW7,  CS16_SW7,  CS18_SW7 },
    {0,  CS21_SW7,  CS20_SW7,  CS19_SW7 },
    {0,  CS23_SW7,  CS22_SW7,  CS24_SW7 },
    {0,  CS27_SW7,  CS26_SW7,  CS25_SW7 },
    {0,  CS29_SW7,  CS28_SW7,  CS30_SW7 },
    {0,  CS33_SW7,  CS32_SW7,  CS31_SW7 },
    {0,  CS35_SW7,  CS34_SW7,  CS36_SW7 },
    {0,  CS38_SW7,  CS37_SW7,  CS39_SW7 },
    {1,   CS3_SW9,   CS2_SW9 ,  CS1_SW9 },
    {1,   CS5_SW9,   CS4_SW9 ,  CS6_SW9 },
    {1,   CS9_SW9,   CS8_SW9 ,  CS7_SW9 },
    {1,   CS11_SW9,  CS10_SW9,  CS12_SW9 },
    {1,   CS15_SW9,  CS14_SW9,  CS13_SW9 },
    {1,   CS17_SW9,  CS16_SW9,  CS18_SW9 },
    {1,   CS21_SW9,  CS20_SW9,  CS19_SW9 },
    {1,   CS23_SW9,  CS22_SW9,  CS24_SW9 },
    {1,   CS27_SW9,  CS26_SW9,  CS25_SW9 },
    {1,   CS29_SW9,  CS28_SW9,  CS30_SW9 },
    {1,   CS33_SW9,  CS32_SW9,  CS31_SW9 },
    {1,   CS35_SW9,  CS34_SW9,  CS36_SW9 },
    {1,   CS38_SW9,  CS37_SW9,  CS39_SW9 },
    {1,   CS3_SW6,   CS2_SW6 ,  CS1_SW6  },
    {1,   CS5_SW6,   CS4_SW6 ,  CS6_SW6  },
    {1,   CS9_SW6,   CS8_SW6 ,  CS7_SW6  },
    {1,   CS11_SW6,  CS10_SW6,  CS12_SW6 },
    {1,   CS15_SW6,  CS14_SW6,  CS13_SW6 },
    {1,   CS17_SW6,  CS16_SW6,  CS18_SW6 },
    {1,   CS21_SW6,  CS20_SW6,  CS19_SW6 },
    {1,   CS23_SW6,  CS22_SW6,  CS24_SW6 },
    {1,   CS27_SW6,  CS26_SW6,  CS25_SW6 },
    {1,   CS29_SW6,  CS28_SW6,  CS30_SW6 },
    {1,   CS33_SW6,  CS32_SW6,  CS31_SW6 },
    {1,   CS35_SW6,  CS34_SW6,  CS36_SW6 },
    {1,   CS38_SW6,  CS37_SW6,  CS39_SW6 },
    {1,   CS3_SW5,   CS2_SW5 ,  CS1_SW5  },
    {1,   CS5_SW5,   CS4_SW5 ,  CS6_SW5  },
    {1,   CS9_SW5,   CS8_SW5 ,  CS7_SW5  },
    {1,   CS11_SW5,  CS10_SW5,  CS12_SW5 },
    {1,   CS15_SW5,  CS14_SW5,  CS13_SW5 },
    {1,   CS17_SW5,  CS16_SW5,  CS18_SW5 },
    {1,   CS21_SW5,  CS20_SW5,  CS19_SW5 },
    {1,   CS23_SW5,  CS22_SW5,  CS24_SW5 },
    {1,   CS27_SW5,  CS26_SW5,  CS25_SW5 },
    {1,   CS29_SW5,  CS28_SW5,  CS30_SW5 },
    {1,   CS33_SW5,  CS32_SW5,  CS31_SW5 },
    {1,   CS35_SW5,  CS34_SW5,  CS36_SW5 },
    {1,   CS38_SW5,  CS37_SW5,  CS39_SW5 },
    {1,   CS3_SW4,   CS2_SW4 ,  CS1_SW4  },
    {1,   CS5_SW4,   CS4_SW4 ,  CS6_SW4  },
    {1,   CS9_SW4,   CS8_SW4 ,  CS7_SW4  },
    {1,   CS11_SW4,  CS10_SW4,  CS12_SW4 },
    {1,   CS15_SW4,  CS14_SW4,  CS13_SW4 },
    {1,   CS17_SW4,  CS16_SW4,  CS18_SW4 },
    {1,   CS21_SW4,  CS20_SW4,  CS19_SW4 },
    {1,   CS23_SW4,  CS22_SW4,  CS24_SW4 },
    {1,   CS27_SW4,  CS26_SW4,  CS25_SW4 },
    {1,   CS29_SW4,  CS28_SW4,  CS30_SW4 },
    {1,   CS33_SW4,  CS32_SW4,  CS31_SW4 },
    {1,   CS35_SW4,  CS34_SW4,  CS36_SW4 },
    {1,   CS38_SW4,  CS37_SW4,  CS39_SW4 },
    {1,   CS3_SW3,   CS2_SW3 ,  CS1_SW3  },
    {1,   CS5_SW3,   CS4_SW3 ,  CS6_SW3  },
    {1,   CS9_SW3,   CS8_SW3 ,  CS7_SW3  },
    {1,   CS11_SW3,  CS10_SW3,  CS12_SW3 },
    {1,   CS15_SW3,  CS14_SW3,  CS13_SW3 },
    {1,   CS17_SW3,  CS16_SW3,  CS18_SW3 },
    {1,   CS21_SW3,  CS20_SW3,  CS19_SW3 },
    {1,   CS23_SW3,  CS22_SW3,  CS24_SW3 },
    {1,   CS27_SW3,  CS26_SW3,  CS25_SW3 },
    {1,   CS29_SW3,  CS28_SW3,  CS30_SW3 },
    {1,   CS33_SW3,  CS32_SW3,  CS31_SW3 },
    {1,   CS35_SW3,  CS34_SW3,  CS36_SW3 },
    {1,   CS38_SW3,  CS37_SW3,  CS39_SW3 },
    {1,   CS3_SW2,   CS2_SW2 ,  CS1_SW2  },
    {1,   CS5_SW2,   CS4_SW2 ,  CS6_SW2  },
    {1,   CS9_SW2,   CS8_SW2 ,  CS7_SW2  },
    {1,   CS11_SW2,  CS10_SW2,  CS12_SW2 },
    {1,   CS15_SW2,  CS14_SW2,  CS13_SW2 },
    {1,   CS17_SW2,  CS16_SW2,  CS18_SW2 },
    {1,   CS21_SW2,  CS20_SW2,  CS19_SW2 },
    {1,   CS23_SW2,  CS22_SW2,  CS24_SW2 },
    {1,   CS27_SW2,  CS26_SW2,  CS25_SW2 },
    {1,   CS29_SW2,  CS28_SW2,  CS30_SW2 },
    {1,   CS33_SW2,  CS32_SW2,  CS31_SW2 },
    {1,   CS35_SW2,  CS34_SW2,  CS36_SW2 },
    {1,   CS38_SW2,  CS37_SW2,  CS39_SW2 },
    {1,   CS3_SW1,   CS2_SW1 ,  CS1_SW1  },
    {1,   CS5_SW1,   CS4_SW1 ,  CS6_SW1  },
    {1,   CS9_SW1,   CS8_SW1 ,  CS7_SW1  },
    {1,   CS11_SW1,  CS10_SW1,  CS12_SW1 },
    {1,   CS15_SW1,  CS14_SW1,  CS13_SW1 },
    {1,   CS17_SW1,  CS16_SW1,  CS18_SW1 },
    {1,   CS21_SW1,  CS20_SW1,  CS19_SW1 },
    {1,   CS23_SW1,  CS22_SW1,  CS24_SW1 },
    {1,   CS27_SW1,  CS26_SW1,  CS25_SW1 },
    {1,   CS29_SW1,  CS28_SW1,  CS30_SW1 },
    {1,   CS33_SW1,  CS32_SW1,  CS31_SW1 },
    {1,   CS35_SW1,  CS34_SW1,  CS36_SW1 },
    {1,   CS38_SW1,  CS37_SW1,  CS39_SW1 },
    {1,   CS3_SW8,   CS2_SW8 ,  CS1_SW8  },
    {1,   CS5_SW8,   CS4_SW8 ,  CS6_SW8  },
    {1,   CS9_SW8,   CS8_SW8 ,  CS7_SW8  },
    {1,   CS11_SW8,  CS10_SW8,  CS12_SW8 },
    {1,   CS15_SW8,  CS14_SW8,  CS13_SW8 },
    {1,   CS17_SW8,  CS16_SW8,  CS18_SW8 },
    {1,   CS21_SW8,  CS20_SW8,  CS19_SW8 },
    {1,   CS23_SW8,  CS22_SW8,  CS24_SW8 },
    {1,   CS27_SW8,  CS26_SW8,  CS25_SW8 },
    {1,   CS29_SW8,  CS28_SW8,  CS30_SW8 },
    {1,   CS33_SW8,  CS32_SW8,  CS31_SW8 },
    {1,   CS35_SW8,  CS34_SW8,  CS36_SW8 },
    {1,   CS38_SW8,  CS37_SW8,  CS39_SW8 },
    {1,   CS3_SW7,   CS2_SW7 ,  CS1_SW7  },
    {1,   CS5_SW7,   CS4_SW7 ,  CS6_SW7  },
    {1,   CS9_SW7,   CS8_SW7 ,  CS7_SW7  },
    {1,   CS11_SW7,  CS10_SW7,  CS12_SW7 },
    {1,   CS15_SW7,  CS14_SW7,  CS13_SW7 },
    {1,   CS17_SW7,  CS16_SW7,  CS18_SW7 },
    {1,   CS21_SW7,  CS20_SW7,  CS19_SW7 },
    {1,   CS23_SW7,  CS22_SW7,  CS24_SW7 },
    {1,   CS27_SW7,  CS26_SW7,  CS25_SW7 },
    {1,   CS29_SW7,  CS28_SW7,  CS30_SW7 },
    {1,   CS33_SW7,  CS32_SW7,  CS31_SW7 },
    {1,   CS35_SW7,  CS34_SW7,  CS36_SW7 },
    {1,   CS38_SW7,  CS37_SW7,  CS39_SW7 }
};


led_config_t g_led_config = { {
  // Key Matrix to LED Index
  { 58 }
}, {
    // LED Index to Physical Position
    {  0,  0 }, {  8,  0 }, { 16,  0 }, { 24,  0 }, { 32,  0 }, { 40,  0 }, { 48,  0 }, { 56,  0 }, { 64,  0 }, { 72,  0 }, { 80,  0 },  { 88,  0 }, { 96,  0 },
    {  0,  8 }, {  8,  8 }, { 16,  8 }, { 24,  8 }, { 32,  8 }, { 40,  8 }, { 48,  8 }, { 56,  8 }, { 64,  8 }, { 72,  8 }, { 80,  8 },  { 88,  8 }, { 96,  8 },
    {  0, 16 }, {  8, 16 }, { 16, 16 }, { 24, 16 }, { 32, 16 }, { 40, 16 }, { 48, 16 }, { 56, 16 }, { 64, 16 }, { 72, 16 }, { 80, 16 },  { 88, 16 }, { 96, 16 },
    {  0, 24 }, {  8, 24 }, { 16, 24 }, { 24, 24 }, { 32, 24 }, { 40, 24 }, { 48, 24 }, { 56, 24 }, { 64, 24 }, { 72, 24 }, { 80, 24 },  { 88, 24 }, { 96, 24 },

    {  0, 32 }, {  8, 32 }, { 16, 32 }, { 24, 32 }, { 32, 32 }, { 40, 32 }, { 48, 32 }, { 56, 32 }, { 64, 32 }, { 72, 32 }, { 80, 32 },  { 88, 32 }, { 96, 32 },

    {  0, 40 }, {  8, 40 }, { 16, 40 }, { 24, 40 }, { 32, 40 }, { 40, 40 }, { 48, 40 }, { 56, 40 }, { 64, 40 }, { 72, 40 }, { 80, 40 },  { 88, 40 }, { 96, 40 },
    {  0, 48 }, {  8, 48 }, { 16, 48 }, { 24, 48 }, { 32, 48 }, { 40, 48 }, { 48, 48 }, { 56, 48 }, { 64, 48 }, { 72, 48 }, { 80, 48 },  { 88, 48 }, { 96, 48 },
    {  0, 56 }, {  8, 56 }, { 16, 56 }, { 24, 56 }, { 32, 56 }, { 40, 56 }, { 48, 56 }, { 56, 56 }, { 64, 56 }, { 72, 56 }, { 80, 56 },  { 88, 56 }, { 96, 56 },
    {  0, 64 }, {  8, 64 }, { 16, 64 }, { 24, 64 }, { 32, 64 }, { 40, 64 }, { 48, 64 }, { 56, 64 }, { 64, 64 }, { 72, 64 }, { 80, 64 },  { 88, 64 }, { 96, 64 },

    {104,  0 }, {112,  0 }, {120,  0 }, {128,  0 }, {136,  0 }, {144,  0 }, {152,  0 }, {160,  0 }, {168,  0 }, {176,  0 }, {184,  0 },  {192,  0 }, {200,  0 },
    {104,  8 }, {112,  8 }, {120,  8 }, {128,  8 }, {136,  8 }, {144,  8 }, {152,  8 }, {160,  8 }, {168,  8 }, {176,  8 }, {184,  8 },  {192,  8 }, {200,  8 },
    {104, 16 }, {112, 16 }, {120, 16 }, {128, 16 }, {136, 16 }, {144, 16 }, {152, 16 }, {160, 16 }, {168, 16 }, {176, 16 }, {184, 16 },  {192, 16 }, {200, 16 },
    {104, 24 }, {112, 24 }, {120, 24 }, {128, 24 }, {136, 24 }, {144, 24 }, {152, 24 }, {160, 24 }, {168, 24 }, {176, 24 }, {184, 24 },  {192, 24 }, {200, 24 },

    {104, 32 }, {112, 32 }, {120, 32 }, {128, 32 }, {136, 32 }, {144, 32 }, {152, 32 }, {160, 32 }, {168, 32 }, {176, 32 }, {184, 32 },  {192, 32 }, {200, 32 },

    {104, 40 }, {112, 40 }, {120, 40 }, {128, 40 }, {136, 40 }, {144, 40 }, {152, 40 }, {160, 40 }, {168, 40 }, {176, 40 }, {184, 40 },  {192, 40 }, {200, 40 },
    {104, 48 }, {112, 48 }, {120, 48 }, {128, 48 }, {136, 48 }, {144, 48 }, {152, 48 }, {160, 48 }, {168, 48 }, {176, 48 }, {184, 48 },  {192, 48 }, {200, 48 },
    {104, 56 }, {112, 56 }, {120, 56 }, {128, 56 }, {136, 56 }, {144, 56 }, {152, 56 }, {160, 56 }, {168, 56 }, {176, 56 }, {184, 56 },  {192, 56 }, {200, 56 },
    {104, 64 }, {112, 64 }, {120, 64 }, {128, 64 }, {136, 64 }, {144, 64 }, {152, 64 }, {160, 64 }, {168, 64 }, {176, 64 }, {184, 64 },  {192, 64 }, {200, 64 },

}, {

  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4
} };
#endif
// clang-format off


#if defined(OLED_ENABLE) && defined(OLED_DISPLAY_128X128)
#include "oled/oled_stuff.h"

void oled_render_large_display(bool side) {
#if 1
        render_rgb_hsv(1, 6);
        render_rgb_mode(1, 7);
        render_arasaka_logo(0, 8);

        oled_render_time(1, 13);
        render_unicode_mode(1, 12);
#else
        render_rgb_hsv(1, 6);
        render_rgb_mode(1, 7);

        render_arasaka_logo(0, 8);
        render_wpm_graph(23, 107, 25, 96);

#endif

}
#endif

bool process_record_keymap(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case FUN_TIME: {
            uint32_t my_callback(uint32_t trigger_time, void *cb_arg) {
                rgb_matrix_step_noeeprom();
                void last_matrix_activity_trigger(void);
                last_matrix_activity_trigger();
                return 5000;
            }
            static deferred_token my_token = INVALID_DEFERRED_TOKEN;
            if (record->event.pressed) {
                if (my_token == INVALID_DEFERRED_TOKEN) {
                    my_token = defer_exec(500, my_callback, NULL);
                } else {
                    if (cancel_deferred_exec(my_token)) {
                        my_token = INVALID_DEFERRED_TOKEN;
                    }
                }
            }
            break;
        }
    }
    return true;
}
