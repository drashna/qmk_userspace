// Copyright 2023 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "sendchar.h"

#ifdef SEGGER_RTT_ENABLE
#    include "SEGGER_RTT.h"
#endif
#ifdef CONSOLE_ENABLE
int8_t console_write(uint8_t c);
#endif
#ifdef VIRTSER_ENABLE
#    include "virtser.h"
#endif

int8_t sendchar(uint8_t c) {
#ifdef CONSOLE_ENABLE
    c = console_write(c);
#endif
#ifdef SEGGER_RTT_ENABLE
    c = SEGGER_RTT_PutChar(0, (char)c);
#endif
#ifdef VIRTSER_ENABLE
    virtser_send(c);
#endif
    return c;
}
