// The widescreen HUD and open-panel background, laid out as SGD2FreeRes + D2GL draw them (measured against
// D2GL PrintScreens at 1280x720 / 1920x1080 / 2560x1080: the retail captures).
// The geometry here is PURE (no art needed) so a row can compare every anchor to the measurement; the art is
// SGD2FreeRes's own DC6 files (a third-party mod's art, not Blizzard's), tracked in play/freeres/ and staged as
// /freeres/*.dc6 by ci/stage.sh. test/wide-hud-art.test.mjs holds every file to the SIZE table.
import { decodeDatPalette, decodeDc6, dc6FrameToRgba } from './d2-formats.mjs';

export const SIZE = { // [width, height] per frame, from the DC6 headers
  D2MRStoneBack: [[256, 256], [256, 256], [256, 256], [256, 256]],
  D2MRFancyVerticalBar: [[60, 256], [60, 256], [60, 35]],
  D2MRFancyHorizontalBar: [[256, 60], [256, 60], [35, 60]],
  D2MRFancyPanelInvertLeft: [[238, 47]], D2MRFancyPanelInvertRight: [[238, 47]], D2MRFancyPanelInvertHorizontalBar: [[254, 47]],
  NeoD2MRFancyBorderInterfaceLeft: [[80, 256], [80, 256], [80, 42]], NeoD2MRFancyBorderInterfaceRight: [[80, 256], [80, 256], [80, 42]],
  NeoD2MRFancyBorderTopLeft: [[256, 60], [28, 60]], NeoD2MRFancyBorderTopRight: [[256, 60], [29, 60]],
  NeoD2MRFancyBorderBottomLeft: [[256, 62], [28, 62]], NeoD2MRFancyBorderBottomRight: [[256, 62], [28, 62]],
  NeoD2MRFancyBorderCornerTopLeft: [[58, 60]], NeoD2MRFancyBorderCornerTopRight: [[59, 60]],
  NeoD2MRFancyBorderCornerBottomLeft: [[59, 62]], NeoD2MRFancyBorderCornerBottomRight: [[59, 62]],
};
export const FREERES_FILES = Object.keys(SIZE);
const part = (name, frame, x, y, clip) => ({ name, frame, x, y, w: SIZE[name][frame][0], h: SIZE[name][frame][1], clip });

// The bottom bar's ornate strips either side of the 470 px centre console (orbs are at the screen edges, the
// skill buttons sit against them at x 117..167 / W-167..W-117): InvertLeft, then bars tiled inward, clipped at the
// console; the right strip mirrors it from its own end piece. Strip top = H-48.
export function barParts(w, h) {
  if (w <= 800) return [];
  const y = h - 48, c = w / 2, out = [], L = [165, c - 235], R = [c + 235, w - 165];
  out.push(part('D2MRFancyPanelInvertLeft', 0, 165, y, L));
  for (let x = 403; x < L[1]; x += 254) out.push(part('D2MRFancyPanelInvertHorizontalBar', 0, x, y, L));
  const ir = w - 403;   // the end piece sits 1 px further right at 2560 (measured 2158, its bar chain ends at 2157; unexplained)
  out.push(part('D2MRFancyPanelInvertRight', 0, ir + Math.floor(w / 2560), y, R));
  for (let x = ir - 254; x + 254 > R[0]; x -= 254) out.push(part('D2MRFancyPanelInvertHorizontalBar', 0, x, y, R));
  return out;
}

// A horizontal run of D2MRFancyHorizontalBar frames 0,1,2 (strides 256, 256, 36) from x0 rightwards, or ending at x1.
function hRun(y, from, to, dir, w0) {
  const out = [], gap = Math.floor(w0 / 2560), step = [256, 256, 35 + gap], sizes = [256, 256, 35];   // gap: a 1 px seam after the end tile at 2560 (measured 2228, 1920 has none)
  if (dir > 0) for (let x = from, i = 0; x < to; x += step[i % 3], i++) out.push(part('D2MRFancyHorizontalBar', i % 3, x, y));
  else for (let x = to, i = 2; x - sizes[i] > from - 256; i = (i + 2) % 3) { x -= sizes[i]; out.push(part('D2MRFancyHorizontalBar', i, x, y)); x -= i === 0 ? gap : 0; }
  return out;
}
// The stone half and its frame. side 'right' = the right half is stone (panel on the right); measured for the right
// half, the left half is the mirror (its frame pieces were measured in the vendor screen).
export function halfParts(w, h, side) {
  const c = w / 2, T = Math.round(h / 2 - 301), B = T + 492, out = [], r = side === 'right', s = r ? 1 : -1;
  const vx = r ? [c, c + 340] : [c - 400, c - 60];
  for (const x of vx) {
    let y = T - 35, f = 2;   // above the frame: ...,1,0,1,2 ending at T (frame 2 is the 35 px end)
    while (y + SIZE.D2MRFancyVerticalBar[f][1] > 0) { out.push(part('D2MRFancyVerticalBar', f, x, y)); f = f === 2 ? 1 : 1 - f; y -= 256; }
    for (let yy = B + 62, i = 0; yy < h; yy += 256, i++) out.push(part('D2MRFancyVerticalBar', i % 2, x, yy));
  }
  const edge = r ? c + 400 : c - 400;
  for (const y of [T, T + 494]) out.push(...(r ? hRun(y, edge, w, 1, w) : hRun(y, 0, edge, -1, w)));
  if (r) {
    out.push(part('NeoD2MRFancyBorderCornerTopRight', 0, c, T), part('NeoD2MRFancyBorderTopRight', 0, c + 59, T), part('NeoD2MRFancyBorderTopRight', 1, c + 314, T),
      part('NeoD2MRFancyBorderCornerBottomRight', 0, c, B), part('NeoD2MRFancyBorderBottomRight', 0, c + 59, B), part('NeoD2MRFancyBorderBottomRight', 1, c + 314, B));
    [0, 1, 2].forEach((i) => out.push(part('NeoD2MRFancyBorderInterfaceRight', i, c + 320, T + 256 * i)));
  } else {
    out.push(part('NeoD2MRFancyBorderCornerTopLeft', 0, c - 58, T), part('NeoD2MRFancyBorderTopLeft', 0, c - 342, T), part('NeoD2MRFancyBorderTopLeft', 1, c - 86, T),
      part('NeoD2MRFancyBorderCornerBottomLeft', 0, c - 59, B), part('NeoD2MRFancyBorderBottomLeft', 0, c - 343, B), part('NeoD2MRFancyBorderBottomLeft', 1, c - 87, B));
    [0, 1, 2].forEach((i) => out.push(part('NeoD2MRFancyBorderInterfaceLeft', i, c - 400, T + 256 * i)));
  }
  return out;
}
// Stone lattice: frame = (col & 1) + 2 * (row & 1), tile (col,row) at (W/2 + 256 col, 256 row), for both halves.
export function stoneTiles(w, h, side) {
  const c = w / 2, out = [], x0 = side === 'right' ? c : 0, x1 = side === 'right' ? w : c;
  for (let col = Math.floor((x0 - c) / 256); c + 256 * col < x1; col++) for (let row = 0; 256 * row < h; row++)
    out.push({ name: 'D2MRStoneBack', frame: (col & 1) + 2 * (row & 1), x: c + 256 * col, y: 256 * row, w: 256, h: 256, clip: [x0, x1] });
  return out;
}

/** Load the art. `read` is the retail MPQ reader (palette only). `ready` turns true when every DC6 is in; a missing file leaves it false. */
export function createFreeResArt(read, documentObject = globalThis.document, base = '/freeres/') {
  const palette = decodeDatPalette(read('data\\global\\palette\\ACT1\\pal.dat')), canvases = new Map();
  const art = { ready: false, error: '', load: async () => {
    try {
      await Promise.all(FREERES_FILES.map(async (name) => {
        const res = await fetch(`${base}${name}.dc6`); if (!res.ok) throw new Error(`${name}: ${res.status}`);
        decodeDc6(new Uint8Array(await res.arrayBuffer())).frames.forEach((f, i) => {
          const c = documentObject.createElement('canvas'); c.width = f.width; c.height = f.height;
          c.getContext('2d').putImageData(new ImageData(dc6FrameToRgba(f, palette), f.width, f.height), 0, 0); canvases.set(`${name}#${i}`, c);
        });
      }));
      art.ready = true;
    } catch (error) { art.error = String(error); }
  }, canvas: (p) => canvases.get(`${p.name}#${p.frame}`) };
  return art;
}
export function drawParts(ctx, art, parts) {
  for (const p of parts) {
    const c = art.canvas(p); if (!c) continue;
    if (p.clip) { ctx.save(); ctx.beginPath(); ctx.rect(p.clip[0], 0, p.clip[1] - p.clip[0], 1 << 15); ctx.clip(); ctx.drawImage(c, p.x, p.y); ctx.restore(); }
    else ctx.drawImage(c, p.x, p.y);
  }
}
