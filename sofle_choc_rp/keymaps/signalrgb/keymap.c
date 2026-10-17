#include QMK_KEYBOARD_H

// SRGBmods/QMK_Community_Module (modules/signalrgb) only defines
// via_command_kb() under #if VIA_ENABLE -- a hook name this fork's via.c
// never calls (only the weak raw_hid_receive_kb() is actually invoked for
// unhandled VIA commands, see via.c's raw_hid_receive()). Without this, the
// module's srgb_raw_hid_rx() is linked in but never runs: SignalRGB's writes
// succeed at the USB layer but the firmware never processes them, so the
// board never leaves its default rgb_matrix effect.
#ifdef SIGNALRGB_SUPPORT_ENABLE
// signalrgb.h doesn't declare this (header omission in the module itself).
bool srgb_raw_hid_rx(uint8_t *data, uint8_t length);

// Split sync for streamed colors -- the module itself has no split
// awareness (no transaction_rpc/is_keyboard_master anywhere in it): its
// led_streaming() calls rgb_matrix_set_color() directly, which only ever
// runs on the master (raw HID only reaches the half with the USB
// connection), so the slave half never gets touched on its own. Same
// problem audio_visualizer.c solves for VIALRGB_DIRECT, same fix: mirror
// every incoming color into a full-board buffer here, and push just the
// slave's half to it over the split link on a throttled housekeeping tick
// -- not synchronously per incoming chunk. First attempt forwarded each
// raw STREAM_RGB_DATA packet immediately (up to ~7 times per render frame,
// unthrottled) and caused visible flicker on the slave half, worse at
// lower brightness -- same bus-contention problem audio_visualizer.c's
// 20ms-throttled single push already avoids.
//
// LED indices 0..HALF_SIZE-1 are always the physical left half and
// HALF_SIZE..RGB_MATRIX_LED_COUNT-1 the right half (fixed by g_led_config,
// independent of which side is plugged in as master) -- is_keyboard_left()
// picks the right offset regardless of which physical side is master.
#ifdef SPLIT_KEYBOARD
#include "transactions.h"

#define SIGNALRGB_HALF_SIZE (RGB_MATRIX_LED_COUNT / 2)
static uint8_t g_signalrgb_mirror[RGB_MATRIX_LED_COUNT * 3];

static void signalrgb_sync_handler(uint8_t in_buflen, const void *in_data, uint8_t out_buflen, void *out_data) {
    (void)out_buflen;
    (void)out_data;
    if (in_buflen != SIGNALRGB_HALF_SIZE * 3 || in_data == NULL) return;
    const uint8_t *rgb  = (const uint8_t *)in_data;
    uint8_t        base = is_keyboard_left() ? 0 : SIGNALRGB_HALF_SIZE;
    for (uint8_t i = 0; i < SIGNALRGB_HALF_SIZE; i++) {
        rgb_matrix_set_color(base + i, rgb[i * 3], rgb[i * 3 + 1], rgb[i * 3 + 2]);
    }
}

void housekeeping_task_user(void) {
    if (!is_keyboard_master()) return;
    static uint32_t last_sync = 0;
    if (timer_elapsed32(last_sync) < 20) return;
    last_sync = timer_read32();

    uint8_t remote_base = is_keyboard_left() ? SIGNALRGB_HALF_SIZE : 0;
    transaction_rpc_send(SIGNALRGB_SYNC_RGB, SIGNALRGB_HALF_SIZE * 3, &g_signalrgb_mirror[remote_base * 3]);
}
#endif

void raw_hid_receive_kb(uint8_t *data, uint8_t length) {
#ifdef SPLIT_KEYBOARD
    if (data[0] == 0x24 /* STREAM_RGB_DATA */) {
        uint8_t index = data[1];
        uint8_t count = data[2];
        for (uint8_t i = 0; i < count && (index + i) < RGB_MATRIX_LED_COUNT; i++) {
            g_signalrgb_mirror[(index + i) * 3 + 0] = data[3 + i * 3 + 0];
            g_signalrgb_mirror[(index + i) * 3 + 1] = data[3 + i * 3 + 1];
            g_signalrgb_mirror[(index + i) * 3 + 2] = data[3 + i * 3 + 2];
        }
    }
#endif
    srgb_raw_hid_rx(data, length);
}

#ifdef SPLIT_KEYBOARD
void keyboard_post_init_user(void) {
    transaction_register_rpc(SIGNALRGB_SYNC_RGB, signalrgb_sync_handler);
}
#endif
#endif // SIGNALRGB_SUPPORT_ENABLE

#ifdef OLED_ENABLE
    #if defined(OLED_EFFECT_SOUNDMONSTER)
    #include "oled/soundmonster.c"
    #elif defined(OLED_EFFECT_LUNA)
    #include "oled/luna.c"
    #elif defined(OLED_EFFECT_LUNABONGO)
    #include "oled/lunabongo.c"
    #elif defined(OLED_EFFECT_BONGOCAT)
    #include "oled/bongocat.c"
    #elif defined(OLED_EFFECT_SNAKEY)
    #include "oled/snakey.c"
    #elif defined(OLED_EFFECT_OLED)
    #include "oled/oled.c"
    #elif defined(OLED_EFFECT_PANDA)
    #include "oled/pandakb.c"
    #elif defined(OLED_EFFECT_MECHABOARDS)
    #include "oled/mechaboards.c"
    #endif
#endif

enum custom_keycodes {
    KC_QWERTZ = SAFE_RANGE,
    KC_RAISE,
    KC_NUMPAD,
    KC_AMBIENT,
    KC_GAMING,
};

enum sofle_layers {
    _QWERTZ = 0,
    _RAISE,
    _NUMPAD,
    _AMBIENT,
    _GAMING
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_QWERTZ] = LAYOUT(KC_ESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC, MO(3), LGUI_T(KC_A), LALT_T(KC_S), LCTL_T(KC_D), LSFT_T(KC_F), KC_G, KC_H, RSFT_T(KC_J), RCTL_T(KC_K), RALT_T(KC_L), RGUI_T(KC_SCLN), KC_QUOT, KC_NUBS, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_MUTE, KC_NO, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, RALT(KC_NUBS), KC_NO, KC_LGUI, MO(2), MO(1), KC_SPC, KC_ENT, KC_BSPC, KC_DEL, KC_NO, TG(4)),
    [_RAISE] = LAYOUT(KC_ESC, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_TAB, KC_GRV, KC_NO, LGUI(KC_LCTL), KC_NO, KC_NO, KC_HOME, KC_PGDN, KC_PGUP, KC_END, RALT(KC_MINS), KC_F12, RALT(KC_7), LGUI(KC_SLSH), LGUI(KC_N), LCTL(LSFT(KC_LGUI)), LGUI(KC_LSFT), LCTL(LSFT(KC_TAB)), KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, LSFT(KC_8), RALT(KC_8), RALT(KC_0), RALT(KC_NUBS), RSFT(KC_RBRC), LSFT(KC_EQL), KC_EQL, LCTL(KC_TAB), KC_TRNS, KC_TRNS, KC_NUBS, LCTL(KC_LEFT), LCTL(KC_RGHT), LSFT(KC_NUBS), LSFT(KC_9), RALT(KC_9), KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_LGUI, KC_TRNS, KC_TRNS, KC_NO),
    [_NUMPAD] = LAYOUT(KC_NO, KC_NO, KC_NO, KC_NO, LGUI(KC_NO), KC_NO, KC_NUM, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_GRV, KC_GRV, KC_NO, RALT(KC_E), KC_NO, KC_PAUS, KC_PPLS, KC_P7, KC_P8, KC_P9, KC_PAST, KC_NO, KC_NO, KC_NO, LSFT(KC_TAB), KC_NO, KC_NO, KC_PSCR, KC_PMNS, KC_P4, KC_P5, KC_P6, KC_PSLS, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, LCA(KC_DEL), KC_NO, KC_NO, KC_PCMM, KC_P1, KC_P2, KC_P3, KC_PDOT, LSFT(KC_GRV), KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_P0, KC_TRNS, KC_NO),
    [_AMBIENT] = LAYOUT(KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, RM_TOGG, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, RM_SATU, RM_HUEU, RM_NEXT, RM_SPDU, RM_VALU, KC_MUTE, MS_BTN1, MS_UP, MS_BTN2, KC_TRNS, KC_TRNS, KC_TRNS, RM_SATD, RM_HUED, RM_PREV, RM_SPDD, RM_VALD, KC_VOLU, MS_LEFT, MS_DOWN, MS_RGHT, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_NO, KC_NO, KC_NO, UG_TOGG, KC_TRNS, KC_TRNS, KC_VOLD, KC_MPRV, KC_MPLY, KC_MSTP, KC_MNXT, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS),
    [_GAMING] = LAYOUT(KC_NO, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, (KC_1), LGUI(KC_2), LGUI(KC_3), LGUI(KC_4), LGUI(KC_5), RGUI(KC_6), KC_LALT, KC_H, KC_Q, KC_W, KC_E, KC_R, LCTL(KC_W), LCTL(KC_T), KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TAB, KC_LSFT, KC_A, KC_S, KC_D, KC_F, LCTL(KC_TAB), KC_WBAK, KC_WFWD, LCTL(LSFT(KC_TAB)), KC_TRNS, KC_TRNS, KC_LGUI, KC_LCTL, KC_Z, KC_X, KC_C, KC_G, KC_TRNS, KC_TRNS, KC_LEFT, KC_UP, KC_DOWN, KC_RGHT, KC_TRNS, KC_TRNS, KC_NO, KC_Y, KC_M, KC_B, KC_SPC, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, TG(4))
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
#ifdef KEYBOARD_PET
    if (record->event.pressed) {
        oled_timer = timer_read32();
    #endif
    switch (keycode) {
        case KC_QWERTZ:
            if (record->event.pressed) {
                set_single_persistent_default_layer(_QWERTZ);
            }
            return false;
        case KC_RAISE:
            if (record->event.pressed) {
                uprintf("KC_RAISE\n");
                layer_on(_RAISE);
                update_tri_layer(_RAISE, _NUMPAD, _AMBIENT);
            } else {
                layer_off(_RAISE);
                update_tri_layer(_RAISE, _NUMPAD, _AMBIENT);
            }
            return false;
        case KC_NUMPAD:
            if (record->event.pressed) {
                uprintf("KC_NUM\n");
                layer_on(_NUMPAD);
                update_tri_layer(_RAISE, _NUMPAD, _AMBIENT);
            } else {
                layer_off(_NUMPAD);
                update_tri_layer(_RAISE, _NUMPAD, _AMBIENT);
            }
            return false;
        case KC_AMBIENT:
            if (record->event.pressed) {
                layer_on(_AMBIENT);
            } else {
                layer_off(_AMBIENT);
            }
            return false;
        case KC_GAMING:
            if (record->event.pressed) {
                register_mods(mod_config(MOD_MEH));
                register_code(KC_UP);
            } else {
                unregister_mods(mod_config(MOD_MEH));
                unregister_code(KC_UP);
            }
            return false;
    }
    return true;
};

#ifdef ENCODER_MAP_ENABLE
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [_QWERTZ] = { ENCODER_CCW_CW(KC_VOLU, KC_VOLD), ENCODER_CCW_CW(KC_MPRV, KC_MNXT) },
    [_RAISE]  = { ENCODER_CCW_CW(KC_TRNS, KC_TRNS), ENCODER_CCW_CW(KC_TRNS, KC_TRNS) },
    [_NUMPAD] = { ENCODER_CCW_CW(KC_TRNS, KC_TRNS), ENCODER_CCW_CW(KC_TRNS, KC_TRNS) },
    [_AMBIENT] = { ENCODER_CCW_CW(KC_TRNS, KC_TRNS), ENCODER_CCW_CW(KC_TRNS, KC_TRNS) },
    [_GAMING] = { ENCODER_CCW_CW(RM_VALU, RM_VALD), ENCODER_CCW_CW(RM_NEXT, RM_PREV) },
};
#endif

