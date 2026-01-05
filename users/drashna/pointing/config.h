// Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#define MOUSE_EXTENDED_REPORT
#define WHEEL_EXTENDED_REPORT
#ifndef MOUSE_SHARED_EP
#    define POINTING_DEVICE_HIRES_SCROLL_ENABLE
#endif // MOUSE_SHARED_EP
#define POINTING_DEVICE_AUTO_MOUSE_ENABLE

#define POINTING_DEVICE_MODES_ENABLE
#define POINTING_MODE_MAP_COUNT 4

#ifndef AUTO_MOUSE_TIME
#    define AUTO_MOUSE_TIME 1250
#endif
