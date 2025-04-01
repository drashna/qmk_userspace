// Copyright 2024 burkfers (@burkfers)
// Copyright 2024 Wimads (@wimads)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "action.h"
#include "report.h"

typedef struct PACKED pointing_device_accel_config_t {
    float growth_rate;
    float offset;
    float limit;
    float takeoff;
    bool  enabled;
} pointing_device_accel_config_t;

extern pointing_device_accel_config_t g_pointing_device_accel_config;

void maccel_enabled(bool enable);
bool maccel_get_enabled(void);
void maccel_toggle_enabled(void);

float maccel_get_takeoff(void);
float maccel_get_growth_rate(void);
float maccel_get_offset(void);
float maccel_get_limit(void);
void  maccel_set_growth_rate(float val);
void  maccel_set_growth_rate(float val);
void  maccel_set_offset(float val);
void  maccel_set_limit(float val);
