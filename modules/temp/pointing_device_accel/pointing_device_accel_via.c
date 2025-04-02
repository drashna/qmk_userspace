// Copyright 2024 burkfers (@burkfers)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "pointing_device_accel.h"
#include "pointing_device_internal.h"
#include "via.h"

_Static_assert(sizeof(g_pointing_device_accel_config) <= VIA_EEPROM_CUSTOM_CONFIG_SIZE, "Mismatch in via custom eeprom stored data");

enum via_pointing_device_channel {
    id_maccel = 24,
};

enum via_pointing_device_ids {
    id_pointing_device_takeoff     = 1,
    id_pointing_device_growth_rate = 2,
    id_pointing_device_offset      = 3,
    id_pointing_device_limit       = 4,
    id_pointing_device_enabled     = 5,
};

#define MACCEL_VIA_TKO_MIN 0
#define MACCEL_VIA_TKO_MAX 5
#define MACCEL_VIA_GRO_MIN 0.01
#define MACCEL_VIA_GRO_MAX 5
#define MACCEL_VIA_OFS_MIN 0
#define MACCEL_VIA_OFS_MAX 15
#define MACCEL_VIA_LMT_MIN 0
#define MACCEL_VIA_LMT_MAX 1

#define MACCEL_VIA_UINT16_MIN 0
#define MACCEL_VIA_UINT16_MAX \
    60000 // Not using the full range for historic reasons. Should be changed with breaking change requiring via json
          // update.

#define PROJECT(val, rmin, rmax, tmin, tmax) \
    ((rmax == rmin) ? tmin : ((((float)(val - rmin) / (float)(rmax - rmin)) * (float)(tmax - tmin)) + tmin))
#define PROJECT_TO_VIA(val, rmin, rmax)   PROJECT(val, rmin, rmax, MACCEL_VIA_UINT16_MIN, MACCEL_VIA_UINT16_MAX)
#define PROJECT_FROM_VIA(val, tmin, tmax) PROJECT(val, MACCEL_VIA_UINT16_MIN, MACCEL_VIA_UINT16_MAX, tmin, tmax)

#define COMBINE_UINT8(one, two) (two | (one << 8))

// Handle the data received by the keyboard from the VIA menus
void pointing_device_config_set_value(uint8_t *data) {
    // data = [ value_id, value_data ]
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);

    switch (*value_id) {
        case id_pointing_device_takeoff:
            {
                uint16_t takeoff = COMBINE_UINT8(value_data[0], value_data[1]);

                g_pointing_device_accel_config.takeoff =
                    PROJECT_FROM_VIA(takeoff, MACCEL_VIA_TKO_MIN, MACCEL_VIA_TKO_MAX);
#ifdef MACCEL_DEBUG
                printf("MACCEL:via: TKO: %.3f grw: %.3f ofs: %.3f lmt: %.3f\n", g_pointing_device_accel_config.takeoff,
                       g_pointing_device_accel_config.growth_rate, g_pointing_device_accel_config.offset,
                       g_pointing_device_accel_config.limit);
#endif
                break;
            }
        case id_pointing_device_growth_rate:
            {
                uint16_t growth_rate = COMBINE_UINT8(value_data[0], value_data[1]);

                g_pointing_device_accel_config.growth_rate =
                    PROJECT_FROM_VIA(growth_rate, MACCEL_VIA_GRO_MIN, MACCEL_VIA_GRO_MAX);
#ifdef MACCEL_DEBUG
                printf("MACCEL:via: tko: %.3f GRW: %.3f ofs: %.3f lmt: %.3f\n", g_pointing_device_accel_config.takeoff,
                       g_pointing_device_accel_config.growth_rate, g_pointing_device_accel_config.offset,
                       g_pointing_device_accel_config.limit);
#endif
                break;
            }
        case id_pointing_device_offset:
            {
                uint16_t offset = COMBINE_UINT8(value_data[0], value_data[1]);

                g_pointing_device_accel_config.offset =
                    PROJECT_FROM_VIA(offset, MACCEL_VIA_OFS_MIN, MACCEL_VIA_OFS_MAX);
#ifdef MACCEL_DEBUG
                printf("MACCEL:via: tko: %.3f grw: %.3f OFS: %.3f lmt: %.3f\n", g_pointing_device_accel_config.takeoff,
                       g_pointing_device_accel_config.growth_rate, g_pointing_device_accel_config.offset,
                       g_pointing_device_accel_config.limit);
#endif
                break;
            }
        case id_pointing_device_limit:
            {
                uint16_t limit = COMBINE_UINT8(value_data[0], value_data[1]);

                g_pointing_device_accel_config.limit = PROJECT_FROM_VIA(limit, MACCEL_VIA_LMT_MIN, MACCEL_VIA_LMT_MAX);
#ifdef MACCEL_DEBUG
                printf("MACCEL:via: tko: %.3f grw: %.3f ofs: %.3f LMT: %.3f\n", g_pointing_device_accel_config.takeoff,
                       g_pointing_device_accel_config.growth_rate, g_pointing_device_accel_config.offset,
                       g_pointing_device_accel_config.limit);
#endif
                break;
            }
        case id_pointing_device_enabled:
            {
                g_pointing_device_accel_config.enabled = value_data[0];
                break;
            }
    }
}

// Handle the data sent by the keyboard to the VIA menus
void pointing_device_config_get_value(uint8_t *data) {
    // data = [ value_id, value_data ]
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);

    switch (*value_id) {
        case id_pointing_device_takeoff:
            {
                // uint16_t takeoff = (g_pointing_device_accel_config.takeoff - 0.5) / (4.5 / 6) * 10000;
                uint16_t takeoff =
                    PROJECT_TO_VIA(g_pointing_device_accel_config.takeoff, MACCEL_VIA_TKO_MIN, MACCEL_VIA_TKO_MAX);
                value_data[0] = takeoff >> 8;
                value_data[1] = takeoff & 0xFF;
                break;
            }
        case id_pointing_device_growth_rate:
            {
                uint16_t growth_rate =
                    PROJECT_TO_VIA(g_pointing_device_accel_config.growth_rate, MACCEL_VIA_GRO_MIN, MACCEL_VIA_GRO_MAX);
                value_data[0] = growth_rate >> 8;
                value_data[1] = growth_rate & 0xFF;
                break;
            }
        case id_pointing_device_offset:
            {
                uint16_t offset =
                    PROJECT_TO_VIA(g_pointing_device_accel_config.offset, MACCEL_VIA_OFS_MIN, MACCEL_VIA_OFS_MAX);
                value_data[0] = offset >> 8;
                value_data[1] = offset & 0xFF;
                break;
            }
        case id_pointing_device_limit:
            {
                uint16_t limit =
                    PROJECT_TO_VIA(g_pointing_device_accel_config.limit, MACCEL_VIA_LMT_MIN, MACCEL_VIA_LMT_MAX);
                value_data[0] = limit >> 8;
                value_data[1] = limit & 0xFF;
                break;
            }
        case id_pointing_device_enabled:
            {
                value_data[0] = g_pointing_device_accel_config.enabled;
                break;
            }
    }
}

// Save the data to persistent memory after changes are made
__attribute__((weak)) void pointing_device_config_update(pointing_device_accel_config_t *config) {
    via_update_custom_config(config, 0, sizeof(pointing_device_accel_config_t));
}

__attribute__((weak)) void pointing_device_config_read(pointing_device_accel_config_t *config) {
    via_read_custom_config(&g_pointing_device_accel_config, 0, sizeof(g_pointing_device_accel_config));
}

void via_custom_value_command_kb(uint8_t *data, uint8_t length) {
    uint8_t *command_id     = &(data[0]);
    uint8_t *channel_id     = &(data[1]);
    uint8_t *value_and_data = &(data[2]);

    if (*channel_id == id_maccel) {
        switch (*command_id) {
            case id_custom_set_value:
                pointing_device_config_set_value(value_and_data);
                break;
            case id_custom_get_value:
                pointing_device_config_get_value(value_and_data);
                break;
            case id_custom_save:
                pointing_device_config_update(&g_pointing_device_accel_config);
                break;
            default:
                *command_id = id_unhandled;
                break;
        }
        return;
    }

    *command_id = id_unhandled;
}
