// Copyright Pablo Martinez (@elpekenin) <elpekenin@elpekenin.dev>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "keylogger.h"
#include "keylogger_helper.h"
#include "keyrecords/process_records.h"
#include "drashna_util.h"
#include "utf8.h"

#include <ctype.h>
#include <quantum/quantum.h>
#include <quantum/util.h>
#include <string.h>
#include <tmk_core/protocol/host.h> // keyboard_led_state

#define BIT(x) ((uint8_t)1 << (x))

static bool    keylog_dirty                                           = true;
static int32_t display_keylogger_string[DISPLAY_KEYLOGGER_LENGTH + 1] = {
    [0 ... DISPLAY_KEYLOGGER_LENGTH - 1] = '_',
    [DISPLAY_KEYLOGGER_LENGTH]           = '\0',
}; // extra space for terminator

typedef enum {
    NO_MODS,
    SHIFT,
    AL_GR,
    // ... implement more when needed
    N_MODS,
} active_mods_t;

typedef struct PACKED {
    const char *raw;
    const char *strings[N_MODS];
} replacements_t;

#define replacement(r, no_mods, shift, al_gr) \
    (replacements_t) {                        \
        .raw     = (r),                       \
        .strings = {                          \
            [NO_MODS] = (no_mods),            \
            [SHIFT]   = (shift),              \
            [AL_GR]   = (al_gr),              \
        },                                    \
    }

// clang-format off
static const replacements_t replacements[] = {
    replacement("0",        NULL,  ")",  NULL),
    replacement("1",        NULL,  "!",  NULL),
    replacement("2",        NULL,  "@",  NULL),
    replacement("3",        NULL,  "#",  NULL),
    replacement("4",        NULL,  "$",  NULL),
    replacement("5",        NULL,  "%",  NULL),
    replacement("6",        NULL,  "^",  NULL),
    replacement("7",        NULL,  "&",  NULL),
    replacement("8",        NULL,  "*",  NULL),
    replacement("9",        NULL,  "(",  NULL),
    replacement("_______", "__",  NULL, NULL),
    replacement("AT",      "@",   NULL, NULL),
    replacement("BSLS",    "\\",  "|",  NULL),
    replacement("BSPC",    "⇤",   NULL, NULL),
    replacement("CAPS",    "↕",   NULL, NULL),
    replacement("COMM",    ",",   "<",  NULL),
    replacement("DB_TOGG", "DBG", NULL, NULL),
    replacement("DOT",     ".",   ">",  NULL),
    replacement("ENT",     "↲",   NULL, NULL),
    replacement("GRV",     "`",   "~",  NULL),
    replacement("HASH",    "#",   NULL, NULL),
    replacement("LBRC",    "[",   "{",  NULL),
    replacement("LCBR",    "{",   NULL, NULL),
    replacement("MINS",    "-",   "_",  NULL),

    replacement("RBRC",    "]",   "}", NULL),
    replacement("RCBR",    "}",   NULL, NULL),
    replacement("PLUS",    "+",   NULL, NULL),
    replacement("PIPE",    "|",   NULL, NULL),
    replacement("QUOT",    "'",   "\"", NULL),
    replacement("SPC",     " ",   NULL, NULL),
    replacement("SCLN",    ";",   ":",  NULL),
    replacement("SLSH",    "/",   "?",  NULL),
    replacement("EQL",     "=",   "+",  NULL),
    replacement("TAB",     "⇥",   NULL, NULL),
    replacement("DEL",     "⇥",   NULL, NULL),
    replacement("TILD",    "~",   NULL, NULL),
    replacement("LEFT",    "←",   NULL, NULL),

    replacement("DOWN",    "↓",   NULL, NULL),
    replacement("RGHT",    "→",   NULL, NULL),
    replacement("UP",      "↑",   NULL, NULL),
    replacement("HOME",    "◀",   NULL, NULL),
    replacement("END",     "▶",   NULL, NULL),
    replacement("PGUP",    "▲",   NULL, NULL),
    replacement("PGDN",    "▼",   NULL, NULL),
    replacement("PSCR",    "P",   NULL, NULL),

    replacement("LOWR",    "▼",   NULL, NULL),
    replacement("UPPR",    "▲",   NULL, NULL),

    replacement("MUTE",    "♫",   "♫",  NULL),
    replacement("VOLU",    "♪",   "♪",  NULL),
    replacement("VOLD",    "♪",   "♪",  NULL),
    replacement("MNXT",    "⏭",   NULL, NULL),
    replacement("MPRV",    "⏮",   NULL, NULL),
    replacement("MSTP",    "⏹",   NULL, NULL),
    replacement("MPLY",    "⏯",   NULL, NULL),

    replacement("IRNY",    "⸮",   NULL, NULL),
    replacement("CLUE",    "‽",   NULL, NULL),
    replacement("SH_TT",   "S",   NULL, NULL),

    replacement("ESC",     "‼",   NULL, NULL),

    replacement("LSFT",    "↑",   NULL, NULL),
    replacement("RSFT",    "↑",   NULL, NULL),
    replacement("LALT",    "A",   NULL, NULL),
    replacement("RALT",    "A",   NULL, NULL),
    replacement("LCTL",    "^",   NULL, NULL),
    replacement("RCTL",    "^",   NULL, NULL),
    replacement("LGUI",    "G",   NULL, NULL),
    replacement("RGUI",    "G",   NULL, NULL),

    replacement("F1",      "F",   NULL, NULL),
    replacement("F2",      "F",   NULL, NULL),
    replacement("F3",      "F",   NULL, NULL),
    replacement("F4",      "F",   NULL, NULL),
    replacement("F5",      "F",   NULL, NULL),
    replacement("F6",      "F",   NULL, NULL),
    replacement("F7",      "F",   NULL, NULL),
    replacement("F8",      "F",   NULL, NULL),
    replacement("F9",      "F",   NULL, NULL),
    replacement("F10",     "F",   NULL, NULL),
    replacement("F11",     "F",   NULL, NULL),
    replacement("F12",     "F",   NULL, NULL),

    replacement("MS_BTN1", "⸁",   NULL, NULL),
    replacement("MS_BTN2", "⸂",   NULL, NULL),
    replacement("MS_BTN3", "⸀",   NULL, NULL),
    replacement("MS_BTN4", "⸀",   NULL, NULL),
    replacement("MS_BTN5", "⸀",   NULL, NULL),
    replacement("MS_BTN6", "⸀",   NULL, NULL),
    replacement("MS_BTN7", "⸀",   NULL, NULL),
    replacement("MS_BTN8", "⸀",   NULL, NULL),
    replacement("MS_WHLD", "⸀",   NULL, NULL),
    replacement("MS_WHLL", "⸀",   NULL, NULL),
    replacement("MS_WHLR", "⸀",   NULL, NULL),
    replacement("MS_WHLU", "⸀",   NULL, NULL),
    replacement("DPI_DEC", "⸀",   NULL, NULL),
    replacement("DPI_INC", "⸀",   NULL, NULL),
    replacement("SNI_DEC", "⸀",   NULL, NULL),
    replacement("SNI_INC", "⸀",   NULL, NULL),

};
// clang-format on

static void skip_prefix(const char **str) {
    char *prefixes[] = {"KC_", "RGB_", "QK_", "TD_", "TL_", "UC_"};

    for (size_t i = 0; i < ARRAY_SIZE(prefixes); ++i) {
        char   *prefix = prefixes[i];
        uint8_t len    = strlen(prefix);

        if (strncmp(prefix, *str, len) == 0) {
            *str += len;
            return;
        }
    }
}

OptionImpl(replacements_t);

static Option(replacements_t) find_replacement(const char *str) {
    for (size_t i = 0; i < ARRAY_SIZE(replacements); ++i) {
        const replacements_t replacement = replacements[i];

        if (strcmp(replacement.raw, str) == 0) {
            return Some(replacements_t, replacement);
        }
    }

    return None(replacements_t);
}

OptionImpl(uintptr_t);

static void maybe_symbol(const char **str, uint8_t mods) {
    const Option(replacements_t) maybe_replacement = find_replacement(*str);
    if (!maybe_replacement.is_some) {
        return;
    }

    replacements_t replacement = unwrap(maybe_replacement);

    const char *target = NULL;
    switch (mods) {
        case 0:
            target = replacement.strings[NO_MODS];
            break;

        case MOD_BIT_LSHIFT:
        case MOD_BIT_RSHIFT:
            if (replacement.strings[SHIFT] == NULL) {
                target = replacement.strings[NO_MODS];
            } else {
                target = replacement.strings[SHIFT];
            }
            break;

        case MOD_BIT_RALT:
            if (replacement.strings[AL_GR] == NULL) {
                target = replacement.strings[NO_MODS];
            } else {
                target = replacement.strings[AL_GR];
            }
            break;

        default:
            // nothing to be done here
            return;
    }

    // we may get here with a combination with no replacement, eg shift+arrows
    // dont want to assign str to NULL
    if (target != NULL) {
        *str = target;
    }
}

// convert to lowercase based on shift/caps
// overengineered so it can also work on strings and whatnot on future
static void apply_casing(const char **str) {
    // not a single char
    if (strlen(*str) > 1) {
        return;
    }

    // not a letter
    if (!isalpha((unsigned char)**str)) {
        return;
    }

    uint8_t mods = get_mods();
#ifndef NO_ACTION_ONESHOT
    mods |= get_oneshot_mods();
#endif
    bool shift = mods & MOD_MASK_SHIFT;
    bool caps  = host_keyboard_led_state().caps_lock;

    // if writing uppercase, string already is, just return
    if (shift ^ caps) {
        return;
    }

    char *lowercase_letters[] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
                                 "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"};

    *str = lowercase_letters[**str - 'A'];
}

static void keylog_clear(void) {
    // spaces (not 0) so `qp_drawtext` actually renders something
    for (int i = 0; i < DISPLAY_KEYLOGGER_LENGTH; i++) {
        display_keylogger_string[i] = '_';
    }
    display_keylogger_string[DISPLAY_KEYLOGGER_LENGTH] = '\0';
}

static void keylog_shift_right_one_char(void) {
    memmove(display_keylogger_string + 1, display_keylogger_string, (DISPLAY_KEYLOGGER_LENGTH - 1) * sizeof(int32_t));
    display_keylogger_string[0] = '_';
}

void keylog_shift_right(void) {
    // With int32_t, each element is a complete character, so just shift one position
    keylog_shift_right_one_char();
}

static void keylog_shift_left(uint8_t len) {
    memmove(display_keylogger_string, display_keylogger_string + len,
            (DISPLAY_KEYLOGGER_LENGTH - len) * sizeof(int32_t));

    // Fill the end with spaces
    for (int i = DISPLAY_KEYLOGGER_LENGTH - len; i < DISPLAY_KEYLOGGER_LENGTH; i++) {
        display_keylogger_string[i] = ' ';
    }
}

static void keylog_append(const char *str) {
    // Convert UTF-8 string to Unicode code points
    uint8_t char_count = 0;
    int32_t unicode_chars[12]; // Temporary buffer for conversion

    const char *ptr = str;
    while (*ptr) {
        ptr = decode_utf8(ptr, &unicode_chars[char_count++]);
    }

    keylog_shift_left(char_count);
    for (uint8_t i = 0; i < char_count; i++) {
        display_keylogger_string[DISPLAY_KEYLOGGER_LENGTH - char_count + i] = unicode_chars[i];
    }
}

bool is_keylogger_dirty(void) {
    return keylog_dirty;
}

void keylogger_set_dirty(bool dirty) {
    keylog_dirty = dirty;
}

/**
 * @brief Converts Unicode code points to UTF-8 encoded string for display
 *
 * This function takes an array of Unicode code points stored in display_keylogger_string
 * and converts them to a UTF-8 encoded C string suitable for display purposes.
 * The conversion handles all valid Unicode ranges:
 * - ASCII (U+0000 to U+007F): 1 byte encoding
 * - U+0080 to U+07FF: 2 byte encoding
 * - U+0800 to U+FFFF: 3 byte encoding
 * - U+10000 to U+10FFFF: 4 byte encoding
 *
 * Invalid Unicode code points are replaced with '?' character.
 * The function uses a static buffer to store the converted string, so the
 * returned pointer remains valid until the next call to this function.
 *
 * @return const char* Pointer to null-terminated UTF-8 encoded string.
 *                     The string is valid until the next call to this function.
 *
 * @note The function assumes display_keylogger_string contains valid Unicode
 *       code points and DISPLAY_KEYLOGGER_LENGTH defines the array size.
 * @note Maximum output string length is DISPLAY_KEYLOGGER_LENGTH * 4 bytes
 *       plus null terminator.
 */
const char *get_keylogger_str(void) {
    // Convert int32_t Unicode code points back to UTF-8 char* for display
    static char converted_string[DISPLAY_KEYLOGGER_LENGTH * 4 + 1]; // Allow up to 4 bytes per Unicode character
    int         pos = 0;

    for (int i = 0; i < DISPLAY_KEYLOGGER_LENGTH && pos < sizeof(converted_string) - 4; i++) {
        int32_t codepoint = display_keylogger_string[i];

        if (codepoint <= 0x7F) {
            // ASCII (1 byte)
            converted_string[pos++] = (char)codepoint;
        } else if (codepoint <= 0x7FF) {
            // 2-byte UTF-8
            converted_string[pos++] = 0xC0 | (codepoint >> 6);
            converted_string[pos++] = 0x80 | (codepoint & 0x3F);
        } else if (codepoint <= 0xFFFF) {
            // 3-byte UTF-8
            converted_string[pos++] = 0xE0 | (codepoint >> 12);
            converted_string[pos++] = 0x80 | ((codepoint >> 6) & 0x3F);
            converted_string[pos++] = 0x80 | (codepoint & 0x3F);
        } else if (codepoint <= 0x10FFFF) {
            // 4-byte UTF-8
            converted_string[pos++] = 0xF0 | (codepoint >> 18);
            converted_string[pos++] = 0x80 | ((codepoint >> 12) & 0x3F);
            converted_string[pos++] = 0x80 | ((codepoint >> 6) & 0x3F);
            converted_string[pos++] = 0x80 | (codepoint & 0x3F);
        } else {
            // Invalid Unicode code point, use replacement character
            converted_string[pos++] = '?';
        }
    }
    converted_string[pos] = '\0';

    return converted_string;
}

const int32_t *get_keylogger_str_raw(void) {
    // Return the raw int32_t array for direct access
    return display_keylogger_string;
}

void keycode_repr(const char **str, const uint8_t mods) {
    skip_prefix(str);
    maybe_symbol(str, mods);
}

/**
 * Processes key events for the keylogger.
 *
 * This function handles keypress events, determines whether the key should be logged,
 * and updates the keylogger display accordingly. It skips certain keycodes, processes
 * modifiers, and applies casing and symbol replacements to the logged key string.
 *
 * @param keycode The keycode of the key being processed.
 * @param record  The key event record containing information about the key event.
 */
void keylogger_process(uint16_t code, keyrecord_t *record) {
    // nothing on release (for now)
    if (!record->event.pressed) {
        return;
    }

    uint16_t keycode = extract_basic_keycode(code, record, true);

    // dont want to show some keycodes
    // clang-format off
    if ((IS_QK_LAYER_TAP(keycode) && !record->tap.count)
        || (keycode >= QK_USER && (keycode != UC_IRNY && keycode != UC_CLUE))
        || IS_RGB_KEYCODE(keycode)
        || IS_QK_ONE_SHOT_LAYER(keycode)
        || IS_QK_ONE_SHOT_MOD(keycode)
        || IS_QK_LIGHTING(keycode)
        || IS_QK_AUDIO(keycode)
        || IS_QK_LAYER_MOD(keycode)
        || IS_QK_MOMENTARY(keycode)
        || IS_QK_DEF_LAYER(keycode)
        || IS_MODIFIER_KEYCODE(keycode)
       )
    {
        // clang-format on
        return;
    }

    keylog_dirty = true;
    uint8_t mods = get_mods();
#ifndef NO_ACTION_ONESHOT
    mods |= get_oneshot_mods();
#endif // NO_ACTION_ONESHOT
    const char *str = get_keycode_string(keycode);

    if (IS_QK_MODS(code) && QK_MODS_GET_MODS(code) & MOD_LSFT) {
        str = get_keycode_string(QK_MODS_GET_BASIC_KEYCODE(code));
        mods |= QK_MODS_GET_MODS(code);
    }

    bool ctrl = mod_config(mods) & MOD_MASK_CTRL;

    // delete from tail
    if (strstr(str, "BSPC") != NULL) {
        // ctrl + backspace clears whole log
        if (ctrl) {
            keylog_clear();
        }
        // backspace = remove last char
        else {
            keylog_shift_right();
        }
        return;
    }

    // unknown keycode, quit
    if (str == NULL) {
        return;
    }

    // convert string into symbols
    keycode_repr(&str, mods);

    // casing is separate so that drawing keycodes on screen is always uppercase
    apply_casing(&str);

    keylog_append(str);
}

void split_sync_keylogger_str(const uint8_t *data, uint8_t size) {
    // Convert incoming data to int32_t
    if (memcmp(data, display_keylogger_string, size * sizeof(int32_t)) != 0) {
        memcpy(display_keylogger_string, data, sizeof(display_keylogger_string));
        keylog_dirty = true;
    }
}

const char *get_keyode_character(uint16_t keycode, keypos_t *key) {
    const char *str = get_keycode_string(extract_basic_keycode(keycode, NULL, false));
    if (str == NULL) {
        return NULL;
    }
    switch (keycode) {
        case KC_NO:
            if (key->row == 255 && key->col == 255) {
                return " ";
            } else {
                return "X";
            }
            break;
        case MAGIC_KEYCODE_RANGE:
        case QK_LIGHTING ... QK_LIGHTING_MAX:
        case QK_LAYER_MOD ... QK_LAYER_MOD_MAX:
        case QK_TO ... QK_TO_MAX:
        case QK_MOMENTARY ... QK_MOMENTARY_MAX:
        case QK_DEF_LAYER ... QK_DEF_LAYER_MAX:
        case QK_TOGGLE_LAYER ... QK_TOGGLE_LAYER_MAX:
        case QK_ONE_SHOT_LAYER ... QK_ONE_SHOT_LAYER_MAX:
        case QK_PERSISTENT_DEF_LAYER ... QK_PERSISTENT_DEF_LAYER_MAX:
        case QK_COMMUNITY_MODULE ... QK_COMMUNITY_MODULE_MAX:
        case QK_TAP_DANCE ... QK_TAP_DANCE_MAX:
        case USER_KEYCODE_RANGE:
            if (keycode == UC_IRNY || keycode == UC_CLUE) {
                break;
            }
        case AUDIO_KEYCODE_RANGE:
        case QUANTUM_KEYCODE_RANGE:
            return "x";
        default:
            break;
    }

    skip_prefix(&str);
    maybe_symbol(&str, IS_QK_MODS(keycode) ? QK_MODS_GET_MODS(keycode) : 0);
    if (KC_A <= keycode && keycode <= KC_Z) {
        // converts uppercase to lowercase if not shifted
        apply_casing(&str);
    }

    return str;
}

void add_keycode_to_keylogger_str(uint8_t keycode, uint8_t mods) {
    keylogger_set_dirty(true);
    const char *str = get_keycode_string(keycode);
    // unknown keycode, quit
    if (str == NULL) {
        return;
    }

    // convert string into symbols
    keycode_repr(&str, mods);

    // casing is separate so that drawing keycodes on screen is always uppercase
    apply_casing(&str);

    keylog_append(str);
}
