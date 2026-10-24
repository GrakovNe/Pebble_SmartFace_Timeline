#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
"""Upscale the original 16x16 icons (kept in resources/images_orig) to 32x32
with nearest-neighbour, so the pixel art stays crisp on the 200x228 screen.

Menu logo is the only exception: Pebble caps menu icons at 25x25, so the
original 28x28 logo is resampled to 24x24.

Usage: python3 tools/upscale_icons.py   (run from the repository root)
"""
import os

from PIL import Image

SRC = "resources/images_orig"
DST = "resources/images"
FACTOR = 2

# original file -> built file
MAPPING = {
    "battery_0_normal.png": "battery_0_normal.png",
    "battery_10_normal.png": "battery_10_normal.png",
    "battery_20_normal.png": "battery_20_normal.png",
    "battery_30_normal.png": "battery_30_normal.png",
    "battery_40_normal.png": "battery_40_normal.png",
    "battery_50_normal.png": "battery_50_normal.png",
    "battery_60_normal.png": "battery_60_normal.png",
    "battery_70_normal.png": "battery_70_normal.png",
    "battery_80_normal.png": "battery_80_normal.png",
    "battery_90_normal.png": "battery_90_normal.png",
    "battery_100_normal.png": "battery_100_normal.png",
    "battery_Full_normal.png": "battery_full_normal.png",
    "battery_0_inverted.png": "battery_0_inverted.png",
    "battery_10_inverted.png": "battery_10_inverted.png",
    "battery_20_inverted.png": "battery_20_inverted.png",
    "battery_30_inverted.png": "battery_30_inverted.png",
    "battery_40_inverted.png": "battery_40_inverted.png",
    "battery_50_inverted.png": "battery_50_inverted.png",
    "battery_60_inverted.png": "battery_60_inverted.png",
    "battery_70_inverted.png": "battery_70_inverted.png",
    "battery_80_inverted.png": "battery_80_inverted.png",
    "battery_90_inverted.png": "battery_90_inverted.png",
    "battery_100_inverted.png": "battery_100_inverted.png",
    "battery_Full_inverted.png": "battery_full_inverted.png",
    "bluetooth_normal.png": "bluetooth_yes_normal.png",
    "bluetooth_inverted.png": "bluetooth_yes_inverted.png",
    "bluetooth_NO_normal.png": "bluetooth_no_normal.png",
    "bluetooth_NO_inverted.png": "bluetooth_no_inverted.png",
    "updating_now_normal.png": "sync_active_normal.png",
    "updating_now_inverted.png": "sync_active_inverted.png",
    "updating_Err_normal.png": "sync_error_normal.png",
    "updating_Err_inverted.png": "sync_error_inverted.png",
}


def main():
    os.makedirs(DST, exist_ok=True)
    for source, target in MAPPING.items():
        image = Image.open(os.path.join(SRC, source)).convert("RGBA")
        size = (image.width * FACTOR, image.height * FACTOR)
        image.resize(size, Image.NEAREST).save(os.path.join(DST, target))
        print(target)

    logo = Image.open(os.path.join(SRC, "app_logo.png")).convert("L")
    logo = logo.resize((24, 24), Image.LANCZOS)
    logo.save(os.path.join(DST, "app_logo.png"))
    print("app_logo.png")


if __name__ == "__main__":
    main()
