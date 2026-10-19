# SignalRGB plugin for this board

This folder is shared (via symlink) by two keymaps: `signalrgb` (SignalRGB
only) and `comboRGB-signalrgb` (SignalRGB + KeyColors + KeyPeek combined).
The plugin itself is identical either way — only the firmware-side
activation differs, see "Switching modes on `comboRGB-signalrgb`" below.

This keymap (`signalrgb`) flashes firmware that speaks the
[SRGBmods QMK_Community_Module](https://github.com/SRGBmods/QMK_Community_Module)
protocol — but SignalRGB's own QMK support
(https://github.com/SRGBmods/qmk-plugins) is a catalog of one `.js` plugin
per specific commercial keyboard model, each hardcoding its own USB
VendorId/ProductId. This board's VID:PID (`fc32:50f1`) isn't a registered
commercial product, so it's not in that catalog and SignalRGB won't detect
it on its own — `SofleChocRP_SignalRGB_Keyboard.js` here is the custom
plugin that makes it work.

## Install

1. Flash both halves with this keymap's firmware
   (`make sofle_choc_rp:signalrgb`, or the `.uf2` from CI/releases).
2. Copy `SofleChocRP_SignalRGB_Keyboard.js` into SignalRGB's user plugin
   folder:
   - Linux: `~/Documents/WhirlwindFX/Plugins/`
   - Windows: `Documents\WhirlwindFX\Plugins\` (unverified — not tested on
     Windows, but matches SignalRGB's own convention)
3. Restart SignalRGB. The keyboard should show up as "Sofle Choc RP
   (SignalRGB)" in the device list.

## Switching modes on `comboRGB-signalrgb`

Only relevant for the combined keymap (`keymaps/comboRGB-signalrgb`), not
plain `signalrgb`.

SignalRGB's own HID-triggered activation (`effectEnable()`, opcode `0x25`)
didn't reliably switch the board into SignalRGB mode on this combined build
(root cause not found — works fine in the standalone `signalrgb` keymap).
As a fallback, two keys on Layer 3 (`_AMBIENT`) switch modes manually,
hardcoded so they survive future Vial re-imports:

| Key              | Layer 3 position | Effect           |
| ---------------- | ----------------- | ---------------- |
| `KEYBIND_USER01` | Esc                | KeyColors        |
| `KEYBIND_USER02` | 1                  | SignalRGB         |

Hold the layer key, tap Esc for KeyColors or 1 for SignalRGB. Both also
call `rgb_matrix_enable_noeeprom()` first, in case a prior brightness/mode
write left the matrix disabled.

## Notes

- Quit VIA/Vial (including closing its browser tab) before running
  SignalRGB and vice versa — both use the same raw HID endpoint, and
  SignalRGB's own docs call this out as a hard conflict.
- The plugin deliberately skips the official template's QMK-version/
  protocol-version/total-LEDs handshake reads in `Initialize()`: those use
  blocking `device.read()` calls that raced against `Render()`'s own
  `device.write()` calls on the same HID pipe once rendering started
  concurrently, corrupting the handshake. Only `effectEnable()` (a
  write-only command, no readback) is actually load-bearing.
- Split-half sync (so the right/slave half lights up too) is handled
  firmware-side, not in this plugin — see the `raw_hid_receive_kb()` /
  `housekeeping_task_user()` block in `../keymap.c`.
- LED positions in the plugin are pulled from this board's own
  `g_led_config` (`../../../sofle_choc_rp.c`), scaled by /4. Re-derive them
  if the physical LED layout ever changes.
