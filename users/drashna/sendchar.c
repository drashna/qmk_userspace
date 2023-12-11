// Copyright 2023 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "sendchar.h"
#include "SEGGER_RTT.h"
int8_t console_write(uint8_t c);

int8_t sendchar(uint8_t c) {
#ifdef CONSOLE_ENABLE
    c = console_write(c);
#endif
#ifdef SEGGER_RTT_ENABLE
    c = SEGGER_RTT_PutChar(0, (char)c);
#endif
    return c;
}
