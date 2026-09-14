#!/usr/bin/env python3
"""Regenerate SmartFace watchface icons for Pebble Time 2 (emery, 200x228, color).

All status icons: 20x20 RGBA, drawn at 4x supersampling, transparent background.
Normal theme = black glyphs, inverted theme = white glyphs.
Menu logo: 48x48 RGBA, white glyph on a black rounded badge.
"""
import math
import os

from PIL import Image, ImageDraw, ImageFont

OUT = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "resources", "images")
SS = 4          # supersampling factor
ICON = 20       # output size of status icons
LOGO = 24       # output size of the menu icon (max 25x25)


def canvas(size=ICON):
    img = Image.new("RGBA", (size * SS, size * SS), (0, 0, 0, 0))
    return img, ImageDraw.Draw(img)


def save(img, name, size=ICON):
    img = img.resize((size, size), Image.LANCZOS)
    img.save(os.path.join(OUT, name))
    print(name)


def s(v):
    return v * SS


def rounded_rect(d, box, radius, outline=None, width=0, fill=None):
    d.rounded_rectangle(
        [s(box[0]), s(box[1]), s(box[2]), s(box[3])],
        radius=s(radius), outline=outline, width=max(1, int(round(width * SS))), fill=fill,
    )


def battery(percent, color):
    """Horizontal battery: body x2..16 y6..13, terminal nub on the right."""
    img, d = canvas()
    outline = color + (255,)
    rounded_rect(d, (1.5, 5.5, 16.5, 14.5), 2, outline=outline, width=1.5)
    d.rounded_rectangle([s(17.2), s(8.2), s(18.6), s(11.8)], radius=s(0.9), fill=outline)
    inner = (3.2, 7.2, 14.8, 12.8)
    if percent > 0:
        w = (inner[2] - inner[0]) * percent / 100.0
        d.rounded_rectangle(
            [s(inner[0]), s(inner[1]), s(inner[0] + w), s(inner[3])],
            radius=s(0.6), fill=outline,
        )
    return img


def battery_full(color):
    """Charged-and-plugged: battery outline with a bolt."""
    img, d = canvas()
    outline = color + (255,)
    rounded_rect(d, (1.5, 5.5, 16.5, 14.5), 2, outline=outline, width=1.5)
    d.rounded_rectangle([s(17.2), s(8.2), s(18.6), s(11.8)], radius=s(0.9), fill=outline)
    bolt = [(11.4, 7.2), (7.2, 11.4), (9.3, 11.4), (8.6, 13.0), (12.8, 8.7), (10.7, 8.7)]
    d.polygon([(s(x), s(y)) for x, y in bolt], fill=outline)
    return img


def bluetooth(color):
    """Classic Bluetooth rune, stroked path."""
    img, d = canvas()
    ink = color + (255,)
    pts = [(6.0, 7.0), (14.0, 13.0), (10.0, 17.0), (10.0, 3.0), (14.0, 7.0), (6.0, 13.0)]
    d.line([(s(x), s(y)) for x, y in pts], fill=ink, width=int(round(1.6 * SS)), joint="curve")
    for x, y in [(10, 3), (10, 17)]:
        r = 0.9 * SS
        d.ellipse([s(x) - r, s(y) - r, s(x) + r, s(y) + r], fill=ink)
    return img


def bluetooth_no(color):
    """Bluetooth rune crossed out by a diagonal slash with a transparent gutter."""
    img = bluetooth(color)
    d = ImageDraw.Draw(img)
    # erase a wider gutter along the slash (paste transparent through a mask)
    gutter = Image.new("L", img.size, 0)
    gd = ImageDraw.Draw(gutter)
    gd.line([s(3.0), s(17.0), s(17.0), s(3.0)], fill=255, width=int(round(4.2 * SS)))
    transparent = Image.new("RGBA", img.size, (0, 0, 0, 0))
    img.paste(transparent, (0, 0), gutter)
    d.line([s(3.0), s(17.0), s(17.0), s(3.0)], fill=color + (255,), width=int(round(2.0 * SS)))
    return img


def sync_active(color):
    """Circular arrow: 300-degree arc with an arrowhead, reads as 'refreshing'."""
    img, d = canvas()
    ink = color + (255,)
    cx, cy, r = 10.0, 10.0, 6.2
    start, end = 60, 340  # PIL angles: 0 = 3 o'clock, clockwise; gap on the right
    bbox = [s(cx - r), s(cy - r), s(cx + r), s(cy + r)]
    d.arc(bbox, start, end, fill=ink, width=int(round(2.0 * SS)))
    # arrowhead at the arc end, pointing along the motion (clockwise tangent)
    a = math.radians(end)
    px, py = cx + r * math.cos(a), cy + r * math.sin(a)
    tx, ty = -math.sin(a), math.cos(a)      # tangent (direction of motion)
    nx, ny = math.cos(a), math.sin(a)       # outward normal
    tip = (px + 3.0 * tx, py + 3.0 * ty)
    b1 = (px + 2.4 * nx, py + 2.4 * ny)
    b2 = (px - 2.4 * nx, py - 2.4 * ny)
    d.polygon(
        [(s(tip[0]), s(tip[1])), (s(b1[0]), s(b1[1])), (s(b2[0]), s(b2[1]))],
        fill=ink,
    )
    return img


def sync_error(color):
    """Exclamation mark."""
    img, d = canvas()
    ink = color + (255,)
    d.rounded_rectangle([s(8.9), s(3.2), s(11.1), s(11.8)], radius=s(1.1), fill=ink)
    r = s(1.5)
    d.ellipse([s(10) - r, s(15.2) - r, s(10) + r, s(15.2) + r], fill=ink)
    return img


def logo():
    img = Image.new("RGBA", (LOGO * SS, LOGO * SS), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.rounded_rectangle(
        [s(2), s(2), s(46), s(46)], radius=s(10), fill=(0, 0, 0, 255),
    )
    font = None
    for path in (
        "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
        "/usr/share/fonts/dejavu/DejaVuSans-Bold.ttf",
    ):
        if os.path.exists(path):
            font = ImageFont.truetype(path, int(30 * SS))
            break
    if font is None:
        raise SystemExit("DejaVuSans-Bold.ttf not found")
    box = d.textbbox((0, 0), "5", font=font)
    w, h = box[2] - box[0], box[3] - box[1]
    d.text(
        ((LOGO * SS - w) / 2 - box[0], (LOGO * SS - h) / 2 - box[1]),
        "5", font=font, fill=(255, 255, 255, 255),
    )
    return img


def main():
    os.makedirs(OUT, exist_ok=True)
    themes = {"normal": (0, 0, 0), "inverted": (255, 255, 255)}
    for theme, color in themes.items():
        for pct in range(0, 101, 10):
            save(battery(pct, color), f"battery_{pct}_{theme}.png")
        save(battery_full(color), f"battery_full_{theme}.png")
        save(bluetooth(color), f"bluetooth_yes_{theme}.png")
        save(bluetooth_no(color), f"bluetooth_no_{theme}.png")
        save(sync_active(color), f"sync_active_{theme}.png")
        save(sync_error(color), f"sync_error_{theme}.png")
    save(logo(), "app_logo.png", size=LOGO)


if __name__ == "__main__":
    main()
