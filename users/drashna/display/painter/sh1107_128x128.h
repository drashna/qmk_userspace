// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <stdint.h>

void init_display_sh1107_128x128(void);
void sh1107_128x128_display_power(bool on);
void sh1107_128x128_draw_user(void);
void sh1107_128x128_display_shutdown(bool jump_to_bootloader);
void init_display_sh1107_128x128_rotation(void);
void init_display_sh1107_128x128_inversion(void);
