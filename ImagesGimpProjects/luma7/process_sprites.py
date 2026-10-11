"""Turn raw AI-generated sprite sheets into engine-ready sheets.

Keys out the background, slices each sheet into cells, keeps the main subject
of each cell, aligns frames, scales all frames of a sheet uniformly and packs
them into a uniform grid (frameW x frameH, N columns) as resources.txt expects.
"""
import sys
import numpy as np
from PIL import Image
from scipy import ndimage

SRC = sys.argv[1]
DST = sys.argv[2]


def load(name):
    return np.asarray(Image.open(f"{SRC}/{name}").convert("RGB")).astype(np.float32)


def key_color(rgb, bg, lo=0.0, floor=30.0):
    """Key by 'magentaness' m = min(r,b) - g, so whites/oranges/cyans are never touched.
    Partially keyed pixels get the background un-mixed and their magenta spill clamped."""
    m = np.minimum(rgb[..., 0], rgb[..., 2]) - rgb[..., 1]
    m_bg = min(bg[0], bg[2]) - bg[1]
    a = 1 - np.clip((m - lo) / (m_bg - floor - lo), 0, 1)  # floor: bg noise -> fully clear
    safe = np.maximum(a, 1e-3)[..., None]
    col = np.clip((rgb - (1 - a[..., None]) * bg) / safe, 0, 255)
    col = np.where(a[..., None] < 1, col, rgb)
    # spill: never let min(r,b) exceed g by more than lo
    cap = col[..., 1] + 20
    over = np.minimum(col[..., 0], col[..., 2]) > cap
    for ch in (0, 2):
        col[..., ch] = np.where(over, np.minimum(col[..., ch], cap), col[..., ch])
    return np.dstack([col, a * 255])


def key_black(rgb):
    """For additive-looking VFX on black: alpha from brightness, colour un-premultiplied."""
    a = rgb.max(axis=2) / 255.0
    a = np.clip((a - 0.16) / 0.84, 0, 1) ** 1.3
    col = np.clip(rgb / np.maximum(a, 1e-3)[..., None], 0, 255)
    return np.dstack([col, a * 255])


def cells(img, cols, rows, inset=0):
    h, w = img.shape[:2]
    cw, ch = w / cols, h / rows
    out = []
    for r in range(rows):
        for c in range(cols):
            y0, y1 = int(r * ch) + inset, int((r + 1) * ch) - inset
            x0, x1 = int(c * cw) + inset, int((c + 1) * cw) - inset
            out.append(img[y0:y1, x0:x1].copy())
    return out


def main_subject(cell, solid_mask=None):
    """Zero out stray blobs: keep the largest component and anything overlapping its padded bbox."""
    a = cell[..., 3] > 20
    lab, n = ndimage.label(ndimage.binary_dilation(a, iterations=6))
    if n == 0:
        return cell
    sizes = ndimage.sum(a, lab, range(1, n + 1))
    big = np.argmax(sizes) + 1
    ys, xs = np.where(lab == big)
    pad = 30
    y0, y1, x0, x1 = ys.min() - pad, ys.max() + pad, xs.min() - pad, xs.max() + pad
    keep = np.zeros(n + 1, bool)
    for i, sl in enumerate(ndimage.find_objects(lab), 1):
        if sl[0].start >= y0 and sl[0].stop <= y1 and sl[1].start >= x0 and sl[1].stop <= x1:
            keep[i] = True
    keep[big] = True
    cell = cell.copy()
    cell[~keep[lab]] = 0
    return cell


def bbox(alpha, thr=20):
    ys, xs = np.where(alpha > thr)
    return xs.min(), ys.min(), xs.max() + 1, ys.max() + 1


def pack(frames, anchors, frame_h, cols, out_name, pad=4):
    """frames: RGBA arrays; anchors: (ax, ay) per frame, the point aligned across frames.
    Scales all frames by one factor so the tallest subject fits frame_h."""
    boxes = [bbox(f[..., 3]) for f in frames]
    # extents of every subject relative to its anchor
    left = max(ax - b[0] for b, (ax, ay) in zip(boxes, anchors))
    right = max(b[2] - ax for b, (ax, ay) in zip(boxes, anchors))
    up = max(ay - b[1] for b, (ax, ay) in zip(boxes, anchors))
    down = max(b[3] - ay for b, (ax, ay) in zip(boxes, anchors))
    # keep anchor centred horizontally so rotation centre = frame centre
    half_w = max(left, right)
    s = (frame_h - 2 * pad) / (up + down)
    frame_w = int(np.ceil(2 * half_w * s)) + 2 * pad
    frame_w += frame_w % 2
    rows = int(np.ceil(len(frames) / cols))
    sheet = Image.new("RGBA", (frame_w * cols, frame_h * rows), (0, 0, 0, 0))
    for i, (f, (ax, ay)) in enumerate(zip(frames, anchors)):
        im = Image.fromarray(f.astype(np.uint8), "RGBA")
        # premultiply for clean resampling, then un-premultiply
        arr = np.asarray(im).astype(np.float32)
        arr[..., :3] *= arr[..., 3:4] / 255
        im = Image.fromarray(arr.astype(np.uint8), "RGBA")
        im = im.resize((max(1, round(im.width * s)), max(1, round(im.height * s))), Image.LANCZOS)
        arr = np.asarray(im).astype(np.float32)
        arr[..., :3] = np.clip(arr[..., :3] * 255 / np.maximum(arr[..., 3:4], 1), 0, 255)
        im = Image.fromarray(arr.astype(np.uint8), "RGBA")
        ox = (i % cols) * frame_w + round(frame_w / 2 - ax * s)
        oy = (i // cols) * frame_h + round(pad + up * s - ay * s)
        sheet.alpha_composite(im, (max(ox, 0), max(oy, 0))) if ox >= 0 and oy >= 0 else sheet.paste(im, (ox, oy), im)
    sheet.save(f"{DST}/{out_name}")
    print(f"{out_name}: frame {frame_w}x{frame_h}, {len(frames)} frames, {cols} cols, sheet {sheet.size}")
    return frame_w, frame_h


def centroid(f):
    cy, cx = ndimage.center_of_mass(f[..., 3] > 128)
    return cx, cy


# ---- Body: idle (4x2) and thrust (3x2) share one frame size so they can be swapped ----
idle_rgb = load("idle_raw.png")
thrust_rgb = load("thrust_raw.png")
idle = [main_subject(c) for c in cells(key_color(idle_rgb, idle_rgb[5, 5]), 4, 2)]
thrust = [main_subject(c) for c in cells(key_color(thrust_rgb, thrust_rgb[5, 5]), 3, 2)]

def helmet_width(f):
    """Width of the subject 6-10% below its top: the helmet, which is rigid across poses."""
    x0, y0, x1, y1 = bbox(f[..., 3])
    h = y1 - y0
    rows = f[y0 + int(h * 0.06): y0 + int(h * 0.10), :, 3] > 128
    xs = np.where(rows.any(axis=0))[0]
    return xs.max() - xs.min()


def rescale(f, k):
    im = Image.fromarray(f.astype(np.uint8), "RGBA")
    arr = np.asarray(im.resize((round(im.width * k), round(im.height * k)), Image.LANCZOS)).astype(np.float32)
    return arr


k = np.median([helmet_width(f) for f in idle]) / np.median([helmet_width(f) for f in thrust])
print("thrust -> idle scale", k)
thrust = [rescale(f, k) for f in thrust]

# Anchor on the head-top centre horizontally / torso centroid: use centroid for stability.
body = idle + thrust
anchors = [centroid(f) for f in body]
# Compute shared scale/size by packing all together, then split into two sheets with same frame size.
boxes = [bbox(f[..., 3]) for f in body]
left = max(ax - b[0] for b, (ax, ay) in zip(boxes, anchors))
right = max(b[2] - ax for b, (ax, ay) in zip(boxes, anchors))
up = max(ay - b[1] for b, (ax, ay) in zip(boxes, anchors))
down = max(b[3] - ay for b, (ax, ay) in zip(boxes, anchors))
FRAME_H = 192


def pack_shared(frames, anchors_, cols, name):
    # pad each frame with a 1px phantom so pack() sees the shared extents
    fixed = []
    for f, (ax, ay) in zip(frames, anchors_):
        h, w = f.shape[:2]
        L, R, U, D = int(left), int(right), int(up), int(down)
        canvas = np.zeros((U + D + 2, L + R + 2, 4), np.float32)
        x0, y0 = L - int(ax), U - int(ay)
        sx0, sy0 = max(0, -x0), max(0, -y0)
        dx0, dy0 = max(0, x0), max(0, y0)
        cw = min(w - sx0, canvas.shape[1] - dx0)
        chh = min(h - sy0, canvas.shape[0] - dy0)
        canvas[dy0:dy0 + chh, dx0:dx0 + cw] = f[sy0:sy0 + chh, sx0:sx0 + cw]
        canvas[0, 0, 3] = canvas[-1, -1, 3] = 21  # phantom corners pin shared extents
        canvas[0, -1, 3] = canvas[-1, 0, 3] = 21
        fixed.append(canvas)
    a = [(left, up)] * len(fixed)
    w_, h_ = pack(fixed, a, FRAME_H, cols, name)
    # wipe phantom corner pixels
    im = Image.open(f"{DST}/{name}")
    arr = np.asarray(im).copy()
    arr[arr[..., 3] < 24] = 0
    Image.fromarray(arr).save(f"{DST}/{name}")
    return w_, h_


pack_shared(idle, [anchors[i] for i in range(8)], 4, "luma_idle.png")
pack_shared(thrust, [anchors[8 + i] for i in range(6)], 3, "luma_thrust.png")
print("body extents (src px) up/down/left/right:", up, down, left, right)

# ---- Plume (4x2 on black, with white cell borders) ----
plume_rgb = load("plume_raw.png")
pc = cells(plume_rgb, 4, 2, inset=24)  # inset strips the drawn borders
plume = [main_subject(key_black(c)) for c in pc[:6]]  # frames 7-8 have a baked-in halo
# anchor at the top centre of the plume (the nozzle end)
pa = []
for f in plume:
    x0, y0, x1, y1 = bbox(f[..., 3], 60)
    pa.append(((x0 + x1) / 2, y0))
pack(plume, pa, 128, 3, "luma_plume.png")

# ---- Tool (3x2 with black borders); drop frame 1 (missing hazard stripe) ----
tool_rgb = load("tool_raw.png")
tc = cells(tool_rgb, 3, 2, inset=28)
tool = []
for c in tc[1:]:
    bg = c[20, 20]
    tool.append(main_subject(key_color(c, bg)))
# anchor: left edge / vertical middle of the opaque, non-glowing casing
ta = []
for f in tool:
    rgb, a = f[..., :3], f[..., 3]
    glow = (rgb[..., 2] > 180) & (rgb[..., 1] > 160) & (rgb[..., 0] < 200)
    solid = (a > 200) & ~glow
    x0, y0, x1, y1 = bbox(solid.astype(np.float32) * 255)
    ta.append((x0, (y0 + y1) / 2))
# pack with anchor at frame centre would waste space; pack then the code pivots at left-middle
boxes = [bbox(f[..., 3]) for f in tool]
right_ext = max(b[2] - ax for b, (ax, ay) in zip(boxes, ta))
up_ext = max(ay - b[1] for b, (ax, ay) in zip(boxes, ta))
down_ext = max(b[3] - ay for b, (ax, ay) in zip(boxes, ta))
half_h = max(up_ext, down_ext)
TH = 64
s = (TH - 4) / (2 * half_h)
TW = int(np.ceil(right_ext * s)) + 4
TW += TW % 2
sheet = Image.new("RGBA", (TW * len(tool), TH), (0, 0, 0, 0))
for i, (f, (ax, ay)) in enumerate(zip(tool, ta)):
    arr = f.copy()
    arr[..., :3] *= arr[..., 3:4] / 255
    im = Image.fromarray(arr.astype(np.uint8), "RGBA")
    im = im.resize((round(im.width * s), round(im.height * s)), Image.LANCZOS)
    arr = np.asarray(im).astype(np.float32)
    arr[..., :3] = np.clip(arr[..., :3] * 255 / np.maximum(arr[..., 3:4], 1), 0, 255)
    im = Image.fromarray(arr.astype(np.uint8), "RGBA")
    ox, oy = round(i * TW + 2 - ax * s), round(TH / 2 - ay * s)
    tile = Image.new("RGBA", sheet.size, (0, 0, 0, 0))
    tile.paste(im, (ox, oy))
    # clip to this frame's cell
    mask = Image.new("L", sheet.size, 0)
    mask.paste(255, (i * TW, 0, (i + 1) * TW, TH))
    tile.putalpha(Image.fromarray(np.minimum(np.asarray(tile)[..., 3], np.asarray(mask))))
    sheet.alpha_composite(tile)
sheet.save(f"{DST}/luma_tool.png")
print(f"luma_tool.png: frame {TW}x{TH}, {len(tool)} frames, {len(tool)} cols, sheet {sheet.size}")
