"""Key and crop the ship module sprites to square tiles. usage: process_modules.py <src_dir> <dst_dir>"""
import sys
import numpy as np
from PIL import Image

src, dst = sys.argv[1], sys.argv[2]
SIZE = 128


def key_color(rgb, bg, floor=30.0):
    # same 'magentaness' key as process_sprites.py
    m = np.minimum(rgb[..., 0], rgb[..., 2]) - rgb[..., 1]
    m_bg = min(bg[0], bg[2]) - bg[1]
    a = 1 - np.clip(m / (m_bg - floor), 0, 1)
    col = np.clip((rgb - (1 - a[..., None]) * bg) / np.maximum(a, 1e-3)[..., None], 0, 255)
    col = np.where(a[..., None] < 1, col, rgb)
    cap = col[..., 1] + 20
    over = np.minimum(col[..., 0], col[..., 2]) > cap
    for ch in (0, 2):
        col[..., ch] = np.where(over, np.minimum(col[..., ch], cap), col[..., ch])
    return np.dstack([col, a * 255])


for name in ("hull", "thruster"):
    rgb = np.asarray(Image.open(f"{src}/{name}_raw.png").convert("RGB")).astype(np.float32)
    rgba = key_color(rgb, rgb[4, 4])
    ys, xs = np.where(rgba[..., 3] > 128)
    x0, x1, y0, y1 = xs.min(), xs.max() + 1, ys.min(), ys.max() + 1
    side = max(x1 - x0, y1 - y0)
    cx, cy = (x0 + x1) / 2, (y0 + y1) / 2
    box = tuple(int(round(v)) for v in (cx - side / 2, cy - side / 2, cx + side / 2, cy + side / 2))
    arr = rgba.copy()
    arr[..., :3] *= arr[..., 3:4] / 255  # premultiply for clean resampling
    im = Image.fromarray(arr.astype(np.uint8), "RGBA").crop(box).resize((SIZE, SIZE), Image.LANCZOS)
    out = np.asarray(im).astype(np.float32)
    out[..., :3] = np.clip(out[..., :3] * 255 / np.maximum(out[..., 3:4], 1), 0, 255)
    Image.fromarray(out.astype(np.uint8), "RGBA").save(f"{dst}/module_{name}.png")
    print(name, "crop", box)
