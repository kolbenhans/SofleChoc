// Keymap-level Raw HID glue for the SRGBmods/QMK_Community_Module
// (signalrgb). Called from key_colors_hid.c's raw_hid_receive_kb() chain,
// same pattern as audio_visualizer_hid_handle_command().
//
// SRGBmods/QMK_Community_Module only defines via_command_kb() under #if
// VIA_ENABLE -- a hook name this fork's via.c never calls (only the weak
// raw_hid_receive_kb() is actually invoked for unhandled VIA commands, see
// via.c's raw_hid_receive()). Without a direct call to srgb_raw_hid_rx()
// here, the module's writes succeed at the USB layer but the firmware
// never processes them.
#include QMK_KEYBOARD_H

#ifdef RAW_ENABLE

// signalrgb.h doesn't declare this (header omission in the module itself).
bool srgb_raw_hid_rx(uint8_t *data, uint8_t length);

// Split sync for streamed colors -- the module itself has no split
// awareness: its led_streaming() calls rgb_matrix_set_color() directly,
// which only ever runs on the master (raw HID only reaches the half with
// the USB connection), so the slave half never gets touched on its own.
// Same problem audio_visualizer.c solves for VIALRGB_DIRECT, same fix:
// mirror every incoming color into a full-board buffer, push just the
// slave's half over the split link on a throttled housekeeping tick (not
// synchronously per incoming chunk -- an unthrottled per-packet forward
// caused visible flicker on the slave, worse at low brightness).
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

// Module-scoped hook (auto-chained by QMK's community-module code
// generator as keyboard_post_init_signalrgb_user) -- NOT the generic
// keyboard_post_init_user, which would collide if another module in this
// keymap also defines one.
void keyboard_post_init_signalrgb_user(void) {
    transaction_register_rpc(SIGNALRGB_SYNC_RGB, signalrgb_sync_handler);
}

// Same idea: housekeeping_task_signalrgb_user, not housekeeping_task_user.
void housekeeping_task_signalrgb_user(void) {
    if (!is_keyboard_master()) return;
    if (rgb_matrix_get_mode() != RGB_MATRIX_COMMUNITY_MODULE_SIGNALRGB) return;
    static uint32_t last_sync = 0;
    if (timer_elapsed32(last_sync) < 20) return;
    last_sync = timer_read32();

    uint8_t remote_base = is_keyboard_left() ? SIGNALRGB_HALF_SIZE : 0;
    transaction_rpc_send(SIGNALRGB_SYNC_RGB, SIGNALRGB_HALF_SIZE * 3, &g_signalrgb_mirror[remote_base * 3]);
}
#endif // SPLIT_KEYBOARD

// Mode-guarded: only let SignalRGB touch the LEDs (and update the mirror
// buffer feeding the slave) while its own effect is the one actually
// active. SignalRGB the app keeps streaming colors continuously once
// connected, regardless of what's active on the board -- without this
// guard, switching to KeyColors on the keyboard wouldn't actually stop the
// fight over the LEDs: both would keep writing rgb_matrix_set_color()
// every tick, racing visibly, unless you also quit the SignalRGB app.
// Every other opcode (version/protocol/identifier queries, the mode
// enable/disable commands themselves) always passes through regardless of
// current mode, same as before -- those are how you get *back* into
// SignalRGB mode in the first place.
bool signalrgb_hid_handle_command(uint8_t *data, uint8_t length) {
    if (length < 1) return false;
    if (data[0] == 0x25 /* SET_SIGNALRGB_MODE_ENABLE */) {
        // The vendored module's own signalrgb_mode_enable() only calls
        // rgb_matrix_mode_noeeprom(), never rgb_matrix_enable_noeeprom() --
        // same gap key_colors/audio_visualizer's own mode-switch opcodes had
        // before we patched *their* handlers. If rgb_matrix happens to be
        // disabled (e.g. a leftover EEPROM state from testing), the mode
        // switches internally but nothing lights up. Fix it here instead of
        // touching the vendored modules/signalrgb/signalrgb.c.
        rgb_matrix_enable_noeeprom();
    }
    if (data[0] == 0x24 /* STREAM_RGB_DATA */) {
        if (rgb_matrix_get_mode() != RGB_MATRIX_COMMUNITY_MODULE_SIGNALRGB) {
            return true; // consumed, deliberately ignored
        }
#ifdef SPLIT_KEYBOARD
        uint8_t index = data[1];
        uint8_t count = data[2];
        for (uint8_t i = 0; i < count && (index + i) < RGB_MATRIX_LED_COUNT; i++) {
            g_signalrgb_mirror[(index + i) * 3 + 0] = data[3 + i * 3 + 0];
            g_signalrgb_mirror[(index + i) * 3 + 1] = data[3 + i * 3 + 1];
            g_signalrgb_mirror[(index + i) * 3 + 2] = data[3 + i * 3 + 2];
        }
#endif
    }
    return srgb_raw_hid_rx(data, length);
}

#endif // RAW_ENABLE
