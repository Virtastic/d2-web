// The consumer of the C++ draw list (src/core/d2client/draw_sink.cpp): the ported D2Client draw functions emit D2Gfx draw calls in
// retail's order, and this replays them onto a canvas. Nothing here decides what is drawn or how much of it.
export const DRAW_CEL = 0xd096, DRAW_CELEX = 0xd1b6;
export const CELL_FLUID = 1, CELL_OVERLAP = 2;
const FIELDS = 8;

/** Decode `count` records from an Int32 view at word offset `at`: { rva, cell, frame, x, y, a, rows, mode }; y is the cel's bottom row. */
export function decodeDrawList(heap32, at, count) {
  const out = [];
  for (let i = 0; i < count; i++) {
    const o = at + i * FIELDS;
    out.push({ rva: heap32[o], cell: heap32[o + 1], frame: heap32[o + 2], x: heap32[o + 3], y: heap32[o + 4], a: heap32[o + 5], rows: heap32[o + 6], mode: heap32[o + 7] });
  }
  return out;
}

/**
 * Replay orb records. `cels[cell][frame]` = { image, dc6, path, frame }. `edge` is how far each orb moves out in a wide view (the
 * left orb by -edge, the right by +edge). `record(path, frame, dc6, x, top, opts)` receives each blit as the per-blit log wants it.
 * A celex shows the bottom `rows` rows of its cel; a plain cel is whole. `originY` is the screen row of the canvas's origin.
 */
export function replayOrbs(ctx, list, cels, edge, record, originY = 496) {
  for (const r of list) {
    const c = cels[r.cell]?.[r.frame];
    if (!c) continue;
    const h = c.image.height, x = r.x + (r.x >= 400 ? edge : -edge), top = r.y - h + 1;
    if (r.rva === DRAW_CELEX) {
      const rows = Math.min(h, Math.max(0, r.rows));
      if (rows > 0) ctx.drawImage(c.image, 0, h - rows, c.image.width, rows, x, top - originY + h - rows, c.image.width, rows);
      record(c.path, c.frame, c.dc6, x, top, { op: 'celex', extra: rows });
    } else if (r.rva === DRAW_CEL) {
      ctx.drawImage(c.image, x, top - originY);
      record(c.path, c.frame, c.dc6, x, top);
    }
  }
}
