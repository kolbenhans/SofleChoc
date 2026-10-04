// Copyright 2026 Kolbenhans (@github)
// SPDX-License-Identifier: GPL-2.0-or-later
#include "sofle_choc_rp.h"

#ifdef ENCODER_ENABLE

#endif

#ifdef OLED_ENABLE
    // nothing here yet
#endif

#ifdef RGB_MATRIX_ENABLE
  // LED-Matrix nach offiziellem QMK sofle_choc: 29 Tasten-LEDs pro Seite, kein Underglow.
  // Kette je Seite: innen oben [0,5] -> Spalte 5 runter -> Daumen -> Spalte 4 hoch -> ... -> aussen.
  // Matrix-Position [4,5] / [9,5] hat keine LED.

led_config_t g_led_config = {
    {
        // Left half
        { 28, 21, 20, 11, 10,  0 },
        { 27, 22, 19, 12,  9,  1 },
        { 26, 23, 18, 13,  8,  2 },
        { 25, 24, 17, 14,  7,  3 },
        { 16, 15,  6,  5,  4, NO_LED },

        // Right half: col 0 = outer (high x), col 5 = inner (low x), matches LAYOUT
        { 57, 50, 49, 40, 39, 29 },
        { 56, 51, 48, 41, 38, 30 },
        { 55, 52, 47, 42, 37, 31 },
        { 54, 53, 46, 43, 36, 32 },
        { 45, 44, 35, 34, 33, NO_LED }
    },
    {
        // Left half: chain starts inner top, serpentine towards the outer column
        { 95,  7}, { 95, 21}, { 95, 36}, { 95, 50}, {115, 64}, { 95, 64},
        { 76, 64}, { 76, 48}, { 76, 33}, { 76, 19}, { 76,  5}, { 57,  5},
        { 57, 19}, { 57, 33}, { 57, 48}, { 57, 64}, { 38, 64}, { 38, 48},
        { 38, 33}, { 38, 19}, { 38,  5}, { 19,  7}, { 19, 21}, { 19, 36},
        { 19, 50}, {  0, 50}, {  0, 36}, {  0, 21}, {  0,  7},
        // Right half: chain starts inner top, serpentine towards the outer column
        {129,  7}, {129, 21}, {129, 36}, {129, 50}, {119, 64}, {129, 64},
        {148, 64}, {148, 48}, {148, 33}, {148, 19}, {148,  5}, {167,  5},
        {167, 19}, {167, 33}, {167, 48}, {167, 64}, {186, 64}, {186, 48},
        {186, 33}, {186, 19}, {186,  5}, {209,  7}, {209, 21}, {209, 36},
        {209, 50}, {224, 50}, {224, 36}, {224, 21}, {224,  7},
    },
    {
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT,
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT,
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT,
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT,
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT,
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT,
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT,
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT,
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT,
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT
    }
};
#endif
