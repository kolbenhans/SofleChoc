# Flashing Guide

## Linux: USB permissions

See [QMK's udev rules guide](https://docs.qmk.fm/faq_build#linux-udev-rules).

## Flash

Use a prebuilt file from [`firmware/`](../firmware/) or build one (`.build/sofle_choc_rp_<keymap>.uf2`), then copy it onto the keyboard's UF2 drive. **Both halves, individually.**

## Entering bootloader mode

- Double-press the reset button on the PCB, or
- Press a key bound to `QK_BOOT` (assignable in Vial)

The half appears as a USB drive — drop the `.uf2` file on it.

## Clear EEPROM

After switching from another firmware or after LED matrix changes: hold the outer top key of the half while plugging it in (Bootmagic), on both halves. Then set your colors again in the WebGUI.

## Split notes

- Only plug or unplug the TRRS cable while USB is disconnected.
- The half connected to USB becomes master; left/right is detected via `GP4`, so USB works on either side.
