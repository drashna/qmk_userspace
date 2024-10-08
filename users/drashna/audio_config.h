// Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#define AUDIO_CLICKY
#define AUDIO_CLICKY_FREQ_RANDOMNESS 1.5f

#if __has_include("user_song_list.h")
#    include "user_song_list.h"
#endif

#ifdef USER_SONG_LIST
#    define STARTUP_SONG        SONG(RICK_ROLL)
#    define GOODBYE_SONG        SONG(SONIC_RING)
#    define DEFAULT_LAYER_SONGS {SONG(QWERTY_SOUND), SONG(COLEMAK_SOUND), SONG(DVORAK_SOUND), SONG(OVERWATCH_THEME)}
#    define UNICODE_SONG_MAC    SONG(MARIO_THEME)
#    define UNICODE_SONG_LNX    SONG(MARIO_POWERUP)
#    define UNICODE_SONG_WIN    SONG(MARIO_ONEUP)
#    define UNICODE_SONG_BSD    SONG(RICK_ROLL)
#    define UNICODE_SONG_WINC   SONG(RICK_ROLL)
#    define CG_NORM_SONG        SONG(ZSA_STARTUP)
#    define CG_SWAP_SONG        SONG(ZSA_GOODBYE)
#else // USER_SONG_LIST
#    define STARTUP_SONG        SONG(STARTUP_SOUND)
#    define GOODBYE_SONG        SONG(GOODBYE_SOUND)
#    define DEFAULT_LAYER_SONGS {SONG(QWERTY_SOUND), SONG(COLEMAK_SOUND), SONG(DVORAK_SOUND), SONG(WORKMAN_SOUND)}
#    define UNICODE_SONG_MAC    SONG(QWERTY_SOUND)
#    define UNICODE_SONG_LNX    SONG(COLEMAK_SOUND)
#    define UNICODE_SONG_WIN    SONG(DVORAK_SOUND)
#    define UNICODE_SONG_BSD    SONG(WORKMAN_SOUND)
#    define UNICODE_SONG_WINC   SONG(PLOVER_GOODBYE_SOUND)
#endif // USER_SONG_LIST
