#include <quantum/util.h>
#include <stdbool.h>
#include <sys/cdefs.h>
#include <ctype.h>

#include "printf/printf.h"

#define _Result(T, E) result___##T##___##E

/**
 * Define a new result type.
 *
 * Args:
 *     T: The type of values.
 *     E: The type of errors.
 */
#define ResultImpl(T, E)    \
    typedef struct PACKED { \
        bool is_ok;         \
        union {             \
            T __value;      \
            E __error;      \
        };                  \
    } Result(T, E)

/**
 * Result type.
 *
 * :c:member:`is_ok`: Whether this result is a value or error.
 */
#define Result(T, E) _Result(T, E)

/**
 * Create a result with the given value.
 */
#define Ok(T, E, v)                    \
    (Result(T, E)) {                   \
        .is_ok = true, .__value = (v), \
    }

/**
 * Create a result instance with the given error.
 */
#define Err(T, E, e)                    \
    (Result(T, E)) {                    \
        .is_ok = false, .__error = (e), \
    }

/**
 * ----
 */

// -- barrier --

#define _Option(T) option___##T

/**
 * Define a new option type.
 *
 * Args:
 *     T: The type of values.
 */
#define OptionImpl(T)       \
    typedef struct PACKED { \
        bool is_some;       \
        T    __value;       \
    } Option(T)

/**
 * Option type.
 *
 * :c:member:`is_some`: Whether this option contains a value.
 */
#define Option(T) _Option(T)


/**
 * Create an option with the given value.
 */
#define Some(T, v)                       \
    (Option(T)) {                        \
        .is_some = true, .__value = (v), \
    }

/**
 * Create an empty option.
 */
#define None(T)           \
    (Option(T)) {         \
        .is_some = false, \
    }


/**
 * ----
 */

// -- barrier --

_Noreturn static inline void raise_error(const char *msg) {
    printf("[ERROR] %s\n", msg);
    while (true) {
    }
}

/**
 * Get the inner value from an `Ok`/`Some` value. Panic if `Err`/`None`.
 */
#define unwrap(v)                                              \
    ({                                                         \
        /* is_ok / is_some */                                  \
        /* relies on them being first element of struct */     \
        bool *ptr = (bool *)&(v);                              \
        if (!(*ptr)) {                                         \
            raise_error("called `unwrap` on `Err` or `None`"); \
        }                                                      \
                                                               \
        (v).__value;                                           \
    })

/**
 * Get the inner value from an `Err`. Panic if `Ok`.
 */
#define unwrap_err(v)                                   \
    ({                                                  \
        if ((v).is_ok) {                                \
            raise_error("called `unwrap_err` on `Ok`"); \
        }                                               \
                                                        \
        (v).__error;                                    \
    })


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
    replacement("INS",     "I",   NULL, NULL),
    replacement("HOME",    "◀",   NULL, NULL),
    replacement("END",     "▶",   NULL, NULL),
    replacement("PGUP",    "▲",   NULL, NULL),
    replacement("PGDN",    "▼",   NULL, NULL),
    replacement("PSCR",    "P",   NULL, NULL),

    replacement("SCRL",    "S",   NULL, NULL),
    replacement("NUM",     "N",   NULL, NULL),
    replacement("LCAP",    "↕",   NULL, NULL),

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

    replacement("KP_0",    "0",   NULL,  NULL),
    replacement("KP_1",    "1",   NULL,  NULL),
    replacement("KP_2",    "2",   NULL,  NULL),
    replacement("KP_3",    "3",   NULL,  NULL),
    replacement("KP_4",    "4",   NULL,  NULL),
    replacement("KP_5",    "5",   NULL,  NULL),
    replacement("KP_6",    "6",   NULL,  NULL),
    replacement("KP_7",    "7",   NULL,  NULL),
    replacement("KP_8",    "8",   NULL,  NULL),
    replacement("KP_9",    "9",   NULL,  NULL),
    replacement("PDOT",    ".",   NULL,  NULL),
    replacement("PEQL",    "=",   NULL,  NULL),
    replacement("PSLS",    "/",   NULL,  NULL),
    replacement("PAST",    "*",   NULL,  NULL),
    replacement("PMNS",    "-",   NULL,  NULL),
    replacement("PPLS",    "+",   NULL,  NULL),
    replacement("PENT",    "↲",   NULL,  NULL),

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

    replacement("BOOT",    "B",   NULL, NULL),
    replacement("PAUS",    "⏸",   NULL, NULL),
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


static const char *get_keycode_character(uint16_t keycode, keypos_t *key) {
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
        case QK_LAYER_TAP_TOGGLE ... QK_LAYER_TAP_TOGGLE_MAX:
        case QK_PERSISTENT_DEF_LAYER ... QK_PERSISTENT_DEF_LAYER_MAX:
        case QK_COMMUNITY_MODULE ... QK_COMMUNITY_MODULE_MAX:
        case QK_TAP_DANCE ... QK_TAP_DANCE_MAX:
        case USER_KEYCODE_RANGE:
        case KB_KEYCODE_RANGE:
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
