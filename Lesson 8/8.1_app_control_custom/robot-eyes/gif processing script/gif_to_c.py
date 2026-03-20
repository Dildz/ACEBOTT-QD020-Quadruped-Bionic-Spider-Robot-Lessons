#!/usr/bin/env python3
"""
gif_to_c.py  —  Convert robot-eyes GIF animations to C PROGMEM headers
                for the ESP8266 / Adafruit SSD1306 128x64 display.

Usage:
    python gif_to_c.py

Output:
    One header file per animation in the output/ subfolder, e.g.:
        output/anim_sleep.h
        output/anim_angry.h
        ...

    Copy the output/ headers into your firmware source tree and include them
    in the driver that handles custom eye animations.

How it works:
    1. Opens each <name>/<name>.gif file
    2. Reads per-frame delays from the GIF Graphic Control Extension metadata
    3. Resizes each frame to fit within 128x64 (preserving aspect ratio, black padding)
    4. Converts to 1-bit monochrome (hard threshold — clean for cartoon-style graphics)
    5. Packs pixels into bytes: MSB = leftmost pixel (Adafruit drawBitmap format)
    6. Writes PROGMEM arrays + per-frame delay array to a .h file

In firmware, use the generated headers like this:

    #include "anim_angry.h"

    // Get a frame pointer (must use pgm_read_ptr on ESP8266):
    const uint8_t* frame = (const uint8_t*) pgm_read_ptr(&angry_frames[i]);
    uint16_t delay_ms    = pgm_read_word(&angry_delays[i]);
    display.drawBitmap(x, y, frame, ANIM_W, ANIM_H, SSD1306_WHITE);

Requirements:
    pip install Pillow
"""

from PIL import Image, ImageChops, ImageDraw, ImageFont
import math
import pathlib

# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------

# Target display dimensions
DISPLAY_W = 128
DISPLAY_H = 64

# Hard-threshold for 1-bit conversion: pixels >= this value become white (1).
# Uses max(R,G,B) per pixel (not luminance), so any vivid colour counts as bright.
# Set to 100 rather than 128 to capture colour-transition frames where the fill
# is a dark mid-tone (e.g. angry frames 3/23: max channel ~110). Safe to lower
# because backgrounds are pure black (max=0), so no false positives.
THRESHOLD = 100

# GIF folders to process (subfolder name == C identifier prefix)
ANIMATIONS = [
    "sleep",
    "angry",
    "love",
    "sad",
    "curious",
    "wow",
]

# Paths
SCRIPT_DIR     = pathlib.Path(__file__).parent
ROBOT_EYES_DIR = SCRIPT_DIR.parent
OUTPUT_DIR     = SCRIPT_DIR / "output"
PREVIEW_DIR    = SCRIPT_DIR / "preview"

# Preview contact sheet settings
PREVIEW_SCALE   = 3   # upscale factor: 128*3=384, 64*3=192 per frame
FRAMES_PER_ROW  = 4   # frames across each row of the contact sheet
CELL_PADDING    = 6   # pixels between frames
LABEL_HEIGHT    = 16  # pixels reserved below each frame for the label


# ---------------------------------------------------------------------------
# Image processing helpers
# ---------------------------------------------------------------------------

def resize_to_fit(img: Image.Image, target_w: int, target_h: int) -> Image.Image:
    """
    Scale img to fit within target_w x target_h, preserving aspect ratio.
    Centres the result on a black background of exactly target_w x target_h.
    """
    # Convert to RGBA first so transparent GIF frames become black, not white
    rgba = img.convert("RGBA")

    # Paste onto a black RGBA background to flatten transparency
    bg = Image.new("RGBA", rgba.size, (0, 0, 0, 255))
    bg.paste(rgba, mask=rgba.split()[3])  # use alpha as mask
    rgb = bg.convert("RGB")

    # Scale to fit within target dimensions
    rgb.thumbnail((target_w, target_h), Image.LANCZOS)

    # Centre on a black canvas
    canvas = Image.new("RGB", (target_w, target_h), (0, 0, 0))
    x_off = (target_w - rgb.width) // 2
    y_off = (target_h - rgb.height) // 2
    canvas.paste(rgb, (x_off, y_off))

    return canvas


def to_1bit(img_rgb: Image.Image, threshold: int = THRESHOLD) -> Image.Image:
    """
    Convert an RGB image to 1-bit monochrome via a hard threshold.
    Pixels with value >= threshold become white (1), rest black (0).
    No dithering — produces cleaner edges for cartoon-style graphics.

    Uses max(R, G, B) per pixel rather than standard luminance.
    Luminance weights: 0.299R + 0.587G + 0.114B — this makes vivid reds and
    purples very dark (red luminance ~76, purple ~68) causing them to drop below
    the threshold and disappear. max(R,G,B) treats any saturated colour as bright
    regardless of hue, so red/purple/blue eyes all convert to white correctly.
    """
    r, g, b = img_rgb.split()
    value = ImageChops.lighter(ImageChops.lighter(r, g), b)  # max(R,G,B) per pixel
    return value.point(lambda p: 255 if p >= threshold else 0, mode="L").convert("1")


def image_to_progmem_bytes(img_1bit: Image.Image) -> bytes:
    """
    Pack a 1-bit image into bytes for Adafruit drawBitmap().
    Format: row-major, MSB = leftmost pixel of each group of 8.
    For 128px wide image: 16 bytes per row, 64 rows = 1024 bytes total.
    """
    return img_1bit.tobytes()


# ---------------------------------------------------------------------------
# C header generation
# ---------------------------------------------------------------------------

HEADER_TEMPLATE = """\
// Auto-generated by gif_to_c.py — DO NOT EDIT MANUALLY
// Animation : {name}
// Source    : {source}
// Frames    : {frame_count}
// Frame size: {w}x{h} px  ({bytes_per_frame} bytes each)
// Total     : {total_bytes} bytes in Flash

#pragma once
#include <Arduino.h>

#define ANIM_{NAME}_FRAME_COUNT {frame_count}
// Frame dimensions are always 128x64 (display size).
// ANIM_W / ANIM_H are defined once in Display_SSD1306.cpp — not here.

// --- Frame data ---
// Read back with: pgm_read_ptr(&{name}_frames[i])
{frame_arrays}
static const uint8_t* const PROGMEM {name}_frames[{frame_count}] = {{
{frame_pointers}
}};

// --- Per-frame delays (milliseconds) ---
// Read back with: pgm_read_word(&{name}_delays[i])
static const uint16_t PROGMEM {name}_delays[{frame_count}] = {{
  {delays}
}};
"""


def format_bytes(data: bytes, indent: int = 2) -> str:
    """Format a byte sequence as a C hex literal block, 16 bytes per line."""
    pad = " " * indent
    lines = []
    for i in range(0, len(data), 16):
        chunk = data[i:i + 16]
        lines.append(pad + ", ".join(f"0x{b:02X}" for b in chunk) + ",")
    return "\n".join(lines)


def generate_header(name: str, frames_data: list[bytes], delays: list[int],
                    w: int, h: int, source_filename: str) -> str:
    bytes_per_frame = w * h // 8
    total_bytes = bytes_per_frame * len(frames_data)

    # Individual frame arrays
    frame_array_lines = []
    for i, data in enumerate(frames_data):
        var = f"{name}_frame_{i:02d}"
        frame_array_lines.append(
            f"static const uint8_t PROGMEM {var}[{bytes_per_frame}] = {{\n"
            f"{format_bytes(data)}\n"
            f"}};"
        )
    frame_arrays = "\n\n".join(frame_array_lines)

    # Pointer array entries
    frame_pointers = "\n".join(
        f"  {name}_frame_{i:02d}," for i in range(len(frames_data))
    )

    # Delay list
    delays_str = ", ".join(str(d) for d in delays)

    return HEADER_TEMPLATE.format(
        name=name,
        NAME=name.upper(),
        source=source_filename,
        frame_count=len(frames_data),
        bytes_per_frame=bytes_per_frame,
        total_bytes=total_bytes,
        frame_arrays=frame_arrays,
        frame_pointers=frame_pointers,
        delays=delays_str,
    )


# ---------------------------------------------------------------------------
# Preview contact sheet
# ---------------------------------------------------------------------------

def generate_contact_sheet(name: str, frames_1bit: list, delays: list[int]) -> None:
    """
    Save a contact sheet PNG showing all converted frames at PREVIEW_SCALE,
    labelled with frame index and delay in milliseconds.
    Uses NEAREST scaling to keep pixel edges crisp (no blurring of 1-bit data).
    """
    scale   = PREVIEW_SCALE
    fw      = DISPLAY_W * scale   # scaled frame width
    fh      = DISPLAY_H * scale   # scaled frame height
    cell_w  = fw + CELL_PADDING
    cell_h  = fh + LABEL_HEIGHT + CELL_PADDING
    n       = len(frames_1bit)
    cols    = FRAMES_PER_ROW
    rows    = math.ceil(n / cols)

    sheet_w = cols * cell_w - CELL_PADDING
    sheet_h = rows * cell_h - CELL_PADDING

    # Dark grey background — easier to see black areas of the frame
    sheet = Image.new("RGB", (sheet_w, sheet_h), (30, 30, 30))
    draw  = ImageDraw.Draw(sheet)
    font  = ImageFont.load_default()  # built-in font, no external file needed

    for i, (img_1bit, delay) in enumerate(zip(frames_1bit, delays)):
        col = i % cols
        row = i // cols
        x   = col * cell_w
        y   = row * cell_h

        # Convert 1-bit -> greyscale -> RGB, then scale up with NEAREST (pixel-crisp)
        frame_rgb    = img_1bit.convert("L").convert("RGB")
        frame_scaled = frame_rgb.resize((fw, fh), Image.NEAREST)
        sheet.paste(frame_scaled, (x, y))

        # Draw a border around the frame
        draw.rectangle([x, y, x + fw - 1, y + fh - 1], outline=(80, 80, 80))

        # Label: frame number and delay
        label = f"#{i:02d}  {delay}ms"
        draw.text((x + 2, y + fh + 2), label, fill=(160, 160, 160), font=font)

    PREVIEW_DIR.mkdir(exist_ok=True)
    out_path = PREVIEW_DIR / f"preview_{name}.png"
    sheet.save(out_path)
    print(f"           -> {out_path.name}  ({cols}x{rows} grid, {scale}x scale)")


# ---------------------------------------------------------------------------
# Main processing
# ---------------------------------------------------------------------------

def process_gif(gif_path: pathlib.Path, name: str) -> None:
    gif = Image.open(gif_path)
    n_frames = gif.n_frames

    print(f"  {name:8s}  {n_frames} frames  source size: {gif.size[0]}x{gif.size[1]}")

    frames_data  = []   # byte arrays for C headers
    frames_1bit  = []   # Image objects for preview
    delays       = []

    for i in range(n_frames):
        gif.seek(i)

        # GIF stores delay in centiseconds; Pillow converts to milliseconds
        delay_ms = gif.info.get("duration", 100)
        delays.append(int(delay_ms))

        frame = resize_to_fit(gif, DISPLAY_W, DISPLAY_H)
        frame_1bit = to_1bit(frame)
        frames_data.append(image_to_progmem_bytes(frame_1bit))
        frames_1bit.append(frame_1bit.copy())

    print(f"           Generating preview contact sheet...")
    generate_contact_sheet(name, frames_1bit, delays)

    header = generate_header(
        name=name,
        frames_data=frames_data,
        delays=delays,
        w=DISPLAY_W,
        h=DISPLAY_H,
        source_filename=gif_path.name,
    )

    out_path = OUTPUT_DIR / f"anim_{name}.h"
    out_path.write_text(header, encoding="utf-8")

    bytes_per_frame = DISPLAY_W * DISPLAY_H // 8
    total_kb = (bytes_per_frame * n_frames) / 1024
    print(f"           -> {out_path.name}  ({total_kb:.1f} KB in Flash)")


def main():
    OUTPUT_DIR.mkdir(exist_ok=True)

    print(f"gif_to_c.py")
    print(f"Target display : {DISPLAY_W}x{DISPLAY_H}")
    print(f"1-bit threshold: {THRESHOLD}")
    print(f"Output folder  : {OUTPUT_DIR}")
    print()

    total_flash = 0
    bytes_per_frame = DISPLAY_W * DISPLAY_H // 8

    for name in ANIMATIONS:
        gif_path = ROBOT_EYES_DIR / name / f"{name}.gif"
        if not gif_path.exists():
            print(f"  [SKIP] {gif_path.relative_to(ROBOT_EYES_DIR)} — not found")
            continue
        process_gif(gif_path, name)

        gif = Image.open(gif_path)
        total_flash += bytes_per_frame * gif.n_frames

    print()
    print(f"Total Flash used by all animations: {total_flash / 1024:.1f} KB")
    print(f"  (ESP8266 has 4096 KB Flash — {total_flash / 1024 / 4096 * 100:.1f}% used by animations)")
    print()
    print("Done. Copy the output/ headers into your firmware source tree.")


if __name__ == "__main__":
    main()
