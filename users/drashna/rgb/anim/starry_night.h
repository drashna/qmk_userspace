#if defined(RGB_MATRIX_FRAMEBUFFER_EFFECTS)
RGB_MATRIX_EFFECT(STARRY_NIGHT)
#    ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

#        include "matrix.h"

//    %----------------------------------%
//    |   ANIMATION CONFIGURATIONS  (can be adjusted)
//    %-----------------------------%

// Decrease this to increase the density of rain drops.
#        ifndef RGB_STARRY_NIGHT_RAIN_DROPS
#            define RGB_STARRY_NIGHT_RAIN_DROPS 26
#        endif

// Fade timing for heatmap
#        ifndef RGB_MATRIX_STARRY_NIGHT_HEATMAP_FADE_MS
#            define RGB_MATRIX_STARRY_NIGHT_HEATMAP_FADE_MS 25
#        endif

// Heat continuously added to a key held down
#        ifndef RGB_MATRIX_STARRY_NIGHT_INCREASE_HOLD
#            define RGB_MATRIX_STARRY_NIGHT_INCREASE_HOLD 5
#        endif

// Heat continuously added to nearby keys of the one held down
#        ifndef RGB_MATRIX_STARRY_NIGHT_INCREASE_HOLD_NEAR
#            define RGB_MATRIX_STARRY_NIGHT_INCREASE_HOLD_NEAR 2
#        endif

// Heat added to the key if it was just a single tap
#        ifndef RGB_MATRIX_STARRY_NIGHT_INCREASE_TAP
#            define RGB_MATRIX_STARRY_NIGHT_INCREASE_TAP 5
#        endif

// Heat added to nearby keys if it was just a single tap
#        ifndef RGB_MATRIX_STARRY_NIGHT_INCREASE_TAP_NEAR
#            define RGB_MATRIX_STARRY_NIGHT_INCREASE_TAP_NEAR 2
#        endif

//    %--------------------------%
//    |   COLOR HELPERS FUNCTIONS
//    %----------------------%

void set_color_val_cap_to_rgb(uint8_t index, rgb_t rgb, uint8_t max_intensity) {
    // Find the brightest channel
    const uint8_t max_value = MAX(MAX(rgb.r, rgb.g), rgb.b);

    // If already inside limits, send directly
    if (max_value <= max_intensity) {
        rgb_matrix_set_color(index, rgb.r, rgb.g, rgb.b);
        return;
    }

    // Scale uniformly

    float   scale = (float)max_intensity / (float)max_value;
    uint8_t r     = (uint8_t)((float)rgb.r * scale);
    uint8_t g     = (uint8_t)((float)rgb.g * scale);
    uint8_t b     = (uint8_t)((float)rgb.b * scale);

    rgb_matrix_set_color(index, r, g, b);
}

//    %--------------------------%
//    |   HELPERS FUNCTIONS
//    %----------------------%

// heatmap framebuffer separate from rain framebuffer
static uint8_t heatmap_fb[MATRIX_ROWS][MATRIX_COLS];

// A timer to track last time we decremented heatmap values
static uint16_t heatmap_decrease_timer;
static bool     decrease_heatmap_values;

// previous pressed matrix used to detect rising edge (tap)
bool prev_starry_pressed[MATRIX_ROWS][MATRIX_COLS];

void starry_heatmap_increase(uint8_t row, uint8_t col, uint8_t increase_heat, uint8_t increase_heat_near) {
#        ifdef RGB_MATRIX_STARRY_NIGHT_SLIM
    heatmap_fb[row][col] = qadd8(heatmap_fb[row][col], increase_heat);
#        else
    if (g_led_config.matrix_co[row][col] == NO_LED) return;

    for (uint8_t r = 0; r < MATRIX_ROWS; r++) {
        for (uint8_t c = 0; c < MATRIX_COLS; c++) {
            if (g_led_config.matrix_co[r][c] == NO_LED) continue;

            if (r == row && c == col) {
                heatmap_fb[r][c] = qadd8(heatmap_fb[r][c], increase_heat);

            } else {
                // Spread only to orthogonal neighbors (+ shape)
                if ((r == row && (c == col + 1 || c == col - 1)) || (c == col && (r == row + 1 || r == row - 1))) {
                    heatmap_fb[r][c] = qadd8(heatmap_fb[r][c], increase_heat_near);
                }
            }
        }
    }
#        endif
}

//    %------------------------------%
//    |   ACTUAL COLOR-CHANGING FUNCTION
//    %------------------------%

bool STARRY_NIGHT(effect_params_t *params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    const uint8_t drop_ticks    = 28;
    const uint8_t max_intensity = rgb_matrix_config.hsv.v;
    const uint8_t decay_ticks   = 0xff / max_intensity;

    static uint8_t drop  = 0;
    static uint8_t decay = 0;

    // Init
    if (params->init) {
        rgb_matrix_set_color_all(0, 0, 0);
        memset(g_rgb_frame_buffer, 0, sizeof(g_rgb_frame_buffer));
        memset(heatmap_fb, 0, sizeof(heatmap_fb));
        heatmap_decrease_timer = timer_read();
        drop                   = 0;
    }

    decay++;

    // Update heatmap decay timer
    if (params->iter == 0) {
        decrease_heatmap_values = timer_elapsed(heatmap_decrease_timer) >= RGB_MATRIX_STARRY_NIGHT_HEATMAP_FADE_MS;

        if (decrease_heatmap_values) {
            // Restart timer if we will decrease this frame
            heatmap_decrease_timer = timer_read();
        }
    }

    // Matrix Scan
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
        for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
            /* -------------------------------------------
            |     1. Map row/col → LED index
            * ------------------------------------------- */
            uint8_t leds[LED_HITS_TO_REMEMBER];
            uint8_t led_count = rgb_matrix_map_row_column_to_led(row, col, leds);
            if (!led_count) continue;

            uint8_t led = leds[0];

            /* -------------------------------------------
            |    2. DIGITAL RAIN behavior (frame buffer update)
            * ------------------------------------------- */
            if (row == 0 && drop == 0 && rand() < RAND_MAX / RGB_STARRY_NIGHT_RAIN_DROPS) {
                g_rgb_frame_buffer[row][col] = max_intensity;
            } else if (g_rgb_frame_buffer[row][col] > 0 && g_rgb_frame_buffer[row][col] < max_intensity) {
                if (decay == decay_ticks) g_rgb_frame_buffer[row][col]--;
            }

            /* -------------------------------------------
            |     3. Compute base rain color for this LED
            * ------------------------------------------- */
            uint8_t rain_v = g_rgb_frame_buffer[row][col];
            // If there is no rain intensity and no heat, leave it off
            uint8_t r = 0, g = 0, b = 0;

            if (rain_v) {
                float   s    = (float)rain_v / (float)max_intensity;
                uint8_t temp = 150.0f * s;
                r            = (temp > 60) ? (temp - 60) : 0;
                g            = 0;
                b            = (uint8_t)(255.0f * s);
            }

            /* -------------------------------------------
            |     4. Apply heatmap
            * ------------------------------------------- */

            uint8_t heat = heatmap_fb[row][col];

            // If the key is being held, add HOLD heat continuously
            // If the key was just pressed, add TAP heat to previously applied heat
            if (matrix_is_on(row, col)) {
                if (prev_starry_pressed[row][col]) {
                    starry_heatmap_increase(row, col, RGB_MATRIX_STARRY_NIGHT_INCREASE_HOLD,
                                            RGB_MATRIX_STARRY_NIGHT_INCREASE_HOLD_NEAR);
                } else {
                    starry_heatmap_increase(
                        row, col, RGB_MATRIX_STARRY_NIGHT_INCREASE_TAP + RGB_MATRIX_STARRY_NIGHT_INCREASE_HOLD,
                        RGB_MATRIX_STARRY_NIGHT_INCREASE_TAP_NEAR + RGB_MATRIX_STARRY_NIGHT_INCREASE_HOLD_NEAR);
                }
            }

            // Update prev matrix for next frame
            prev_starry_pressed[row][col] = matrix_is_on(row, col);

            /* -------------------------------------------
            |     5. Apply heatmap overlay blending
            * ------------------------------------------- */

            // Blend heat on top of rain
            if (heat) {
                float   scale_stars = (float)heat / 255.0f;
                uint8_t star_r      = heat;
                uint8_t star_g      = (uint8_t)(120.0f * scale_stars * scale_stars + 100.0f * scale_stars);

                r = qadd8(r, star_r);
                g = qadd8(g, star_g);
            }

            rgb_t rgb = {r, g, b};
            set_color_val_cap_to_rgb(led, rgb, max_intensity);

            /* -------------------------------------------
            |     6. Decrease heatmap values over time
            * ------------------------------------------- */
            if (decrease_heatmap_values) heatmap_fb[row][col] = qsub8(heat, 1);
        }
    }

    // Reset decay
    if (decay == decay_ticks) decay = 0;

    /* -------------------------------------------
    |     7. Move raindrops downward
    * ------------------------------------------- */
    if (++drop > drop_ticks) {
        drop = 0;
        for (int8_t row = MATRIX_ROWS - 1; row > 0; row--) {
            for (uint8_t col = 0; col < MATRIX_COLS; col++) {
                // if ths is on the bottom row and bright allow decay
                if (row == MATRIX_ROWS - 1 && g_rgb_frame_buffer[row][col] == max_intensity) {
                    g_rgb_frame_buffer[row][col]--;
                }
                // check if the pixel above is bright
                if (g_rgb_frame_buffer[row - 1][col] >=
                    max_intensity) { // Note: can be larger than max_intensity if val was recently decreased
                    // allow old bright pixel to decay
                    g_rgb_frame_buffer[row - 1][col] = max_intensity - 1;
                    // make this pixel bright
                    g_rgb_frame_buffer[row][col] = max_intensity;
                }
            }
        }
    }

    return false;
}

#    endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#endif
