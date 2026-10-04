# SofleChoc

Custom **Vial-QMK firmware for the PandaKB Sofle Choc** (split, Choc switches, RP2040 Pro Micro) with per-key RGB lighting configurable from the browser.

* **`vial`** — standard Vial firmware
* **`keyColors`** — per-key RGB colors configurable through a browser-based WebGUI

Precompiled `.uf2` files are included in [`firmware/`](firmware/), so no QMK build environment is needed.

---

## Hardware

| | |
|---|---|
| PCB | PandaKB Sofle Choc V1 |
| Controller | RP2040 Pro Micro (both halves) |
| Split | TRRS, half-duplex serial on `GP1` |
| Handedness | `GP4` (low = left) |
| RGB | 58 per-key LEDs (29 per half), WS2812 on `GP0`, no underglow |
| Encoders | one per half (`GP28`/`GP29`) |
| OLED | 128x32 per half (I²C `GP2`/`GP3`) |

The LED matrix follows the official QMK [`sofle_choc`](https://github.com/qmk/qmk_firmware/tree/master/keyboards/sofle_choc) definition. The y positions of the highest-staggered column and the two inner thumb keys are aligned with their rows, so row-based effects (e.g. Gradient Up/Down) color each row evenly.

---

## Keymaps

| Keymap      | Description |
| ----------- | ----------- |
| `vial`      | Standard Vial configuration, RGB Matrix effects, OLED animation |
| `keyColors` | Per-key RGB colors per layer, configured through the WebGUI |

`keyColors` uses community modules from [kolbenhans/qmk-modules](https://github.com/kolbenhans/qmk-modules). The prebuilt firmware already contains them.

---

## Quick Start

```bash
git clone --depth 1 https://github.com/vial-kb/vial-qmk ~/projects/vial-qmk
cd ~/projects/vial-qmk
git submodule update --init --recursive --depth 1
python3 -m pip install -r requirements.txt

git clone https://github.com/kolbenhans/SofleChoc.git ~/projects/SofleChoc
ln -s ~/projects/SofleChoc/sofle_choc_rp ~/projects/vial-qmk/keyboards/sofle_choc_rp

qmk compile -kb sofle_choc_rp -km keyColors   # or: vial
```

Output: `.build/sofle_choc_rp_<keymap>.uf2`. Details: [Build Guide](docs/build.md).

---

## Flashing

Copy the `.uf2` onto the UF2 drive that appears after double-pressing the reset button. **Both halves need to be flashed separately.** See the [Flashing Guide](docs/flashing.md).

After changing the LED matrix (or switching from another firmware), clear the EEPROM once — stored per-key colors are indexed by LED number.

---

## WebGUI

`keyColors` provides a browser-based interface for per-key colors, see the [WebGUI Usage Guide](docs/webgui-usage.md).

---

## Repository Structure

```text
SofleChoc/
├── docs/            build, flashing, WebGUI
├── firmware/        prebuilt *.uf2
└── sofle_choc_rp/   keyboard definition and keymaps (link into vial-qmk/keyboards/)
```

---

## Related

* [kolbenhans/SofleRGB](https://github.com/kolbenhans/SofleRGB) — the same firmware concept for the Sofle RGB
* [kolbenhans/qmk-modules](https://github.com/kolbenhans/qmk-modules) — community modules used by `keyColors`
* [kolbenhans/qmk-webgui](https://github.com/kolbenhans/qmk-webgui) — self-hostable WebGUI
