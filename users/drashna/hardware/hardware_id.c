// Copyright 2022 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include <ch.h>
#include "hardware_id.h"
#include "print.h"

hardware_id_t get_hardware_id(void) {
    hardware_id_t id = {0};
#if defined(RP2040)
    // Forward declare as including "hardware/flash.h" here causes more issues...
    void flash_get_unique_id(uint8_t *);

    flash_get_unique_id((uint8_t *)&id);
#elif defined(UID_BASE)
    uint32_t base[3];
    base[0] = (uint32_t)(*((uint32_t *)UID_BASE));
    base[1] = (uint32_t)(*((uint32_t *)(UID_BASE + 4)));
    base[2] = (uint32_t)(*((uint32_t *)(UID_BASE + 8)));

    // https://github.com/libopencm3/libopencm3/blob/master/lib/stm32/common/desig_common_all.c

    uint8_t uid[12];
    for (uint8_t index = 0; index < 12; index++) {
        uid[index] = (uint8_t)(base[index / 4] >> ((index % 4) * 8));
    }

    id.data[0] = uid[11];
    id.data[1] = uid[10] + uid[2];
    id.data[2] = uid[9];
    id.data[3] = uid[8] + uid[0];
    id.data[4] = uid[7];
    id.data[5] = uid[6];

#endif
    return id;
}
