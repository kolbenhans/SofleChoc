# Build Guide

**Windows:** [QMK MSYS](https://msys.qmk.fm/).
**Linux/macOS:** `pip install qmk`, [QMK Getting Started](https://docs.qmk.fm/newbs_getting_started).

```bash
git clone --depth 1 https://github.com/vial-kb/vial-qmk ~/projects/vial-qmk
cd ~/projects/vial-qmk
git submodule update --init --recursive --depth 1
python3 -m pip install -r requirements.txt

git clone https://github.com/kolbenhans/SofleChoc.git ~/projects/SofleChoc
ln -s ~/projects/SofleChoc/sofle_choc_rp ~/projects/vial-qmk/keyboards/sofle_choc_rp
```

Windows without QMK MSYS: `mklink /D` instead of `ln -s` (admin).

## Community Modules

`keyColors` needs the `key_colors` module from
[kolbenhans/qmk-modules](https://github.com/kolbenhans/qmk-modules) — install steps
there. `vial` needs none.

## Build

```bash
cd ~/projects/vial-qmk
qmk compile -kb sofle_choc_rp -km <vial|keyColors>
```

Output: `.build/sofle_choc_rp_<keymap>.uf2`. Split — flash **both halves**, see [Flashing Guide](flashing.md).

## After editing `config.h`

Remove the build cache first, otherwise a stale generated `info_config.h` can override your changes:

```bash
rm -rf .build/obj_sofle_choc_rp_*
```

## LED positions

The LED map lives in `g_led_config` in `sofle_choc_rp/sofle_choc_rp.c` and is mirrored in the `rgb_matrix` block of each keymap's `vial.json` (read by the WebGUI). Change both together.
