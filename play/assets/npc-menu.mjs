// The menu retail shows when a town NPC is clicked: the NPC's name, then Talk / Trade / Gamble / Hire /
// ..., then Cancel. Which entries an NPC has is retail's own table, not ours:
//
//   D2Client 1.13c .data, file offset 0xF3BA0: 48 records of d2bs's `NPCMenu` (reference/d2bs
//   D2Structs.h -- dwNPCClassId, dwEntryAmount, wEntryId[4], pad, dwEntryFunc[4], 5 bytes), found by
//   the string ids it holds. dwEntryAmount counts the Cancel every menu ends with (string 3400).
//   The handlers the entries point at name what they do: 0x6FAFA2F0 talk, 0x6FAFAEE0 trade (also
//   behind 3334 Trade/Repair), 0x6FAFABE0 gamble, 0x6FAF9440 hire, 0x6FAF8750 Cain's identify,
//   0x6FAF87B0 Anya's (3385), 0x6FAF6AF0 the act 5 entry 4020.
//
// Layout from retail's own DRAW LIST (the retail captures: Warriv's
// menu, his unit anchored at (304, 292)). Retail draws, in order:
//   1. a box (D2GFX DrawBox, colour 0, DrawMode 1 = 50%) over rect (x1,y1)-(x2,y2): x2 - x1 = widest
//      line's width (.tbl advances) + 20, centred on the NPC's x; y2 = NPC y - 148 + 10 * lines;
//      y1 = y2 - (15 * lines + 21);
//   2. ui\menu\boxpieces.dc6 around it (menuFramePieces below); the count grows with the box.
//   3. the lines through D2Win's text draw in font16, anchored (bottom row) at y1 + 21, then every
//      15px, each from the NPC's x - width / 2 + 1: the name colour 4 (gold), the entries colour 0 (white).
// The blue entry is the SELECTED one: the first on opening (retail/frames/s3-s4), then whichever the
// pointer is over, none when it is elsewhere (the draw list, pointer parked at 799,300).
// TODO(client-parity F18): Warriv's 3-line and Hratli's 4-line retail lists establish this rule.
// Jerhyn's committed 2-line retail list is awaiting its Act II branch's merge onto main, when s221
// will take it as the third size rather than extrapolating from two examples.
import { createFont, decodeFontTbl, FRONTEND } from './frontend.mjs';
import { recordDc6, recordPrim } from './drawlist.mjs';

const TALK = 'talk', TRADE = 'trade', GAMBLE = 'gamble', HIRE = 'hire', IDENTIFY = 'identify', OTHER = 'other';
const S_TALK = 3381, S_TRADE = 3396, S_TRADE_REPAIR = 3334, S_GAMBLE = 3398, S_HIRE = 3397, S_IDENTIFY = 3383, S_CANCEL = 3400;
const ACTION = { [S_TALK]: TALK, [S_TRADE]: TRADE, [S_TRADE_REPAIR]: TRADE, [S_GAMBLE]: GAMBLE, [S_HIRE]: HIRE, [S_IDENTIFY]: IDENTIFY };

// [NPC class, entry string ids] in the table's order (the Cancel each ends with is implied).
export const NPC_MENU_TABLE = [
  [148, [S_TALK, S_TRADE]], [176, [S_TALK]], [146, [S_TALK]], [154, [S_TALK, S_TRADE_REPAIR]],
  [177, [S_TALK, S_TRADE]], [199, [S_TALK, S_TRADE, S_GAMBLE]], [200, [S_TALK]], [147, [S_TALK, S_TRADE, S_GAMBLE]],
  [198, [S_TALK, S_HIRE]], [201, [S_TALK]], [202, [S_TALK, S_TRADE]], [150, [S_TALK]],
  [178, [S_TALK, S_TRADE_REPAIR]], [155, [S_TALK]], [175, [S_TALK, S_IDENTIFY]], [210, [S_TALK]],
  [244, [S_TALK, 4020]], [265, [S_TALK, 4020]], [245, [S_TALK, 4020]], [246, [S_TALK, 4020]],
  [251, [S_TALK]], [367, [S_TALK]], [257, [S_TRADE_REPAIR]], [405, [S_TRADE, S_GAMBLE]],
  [252, [S_TALK, S_HIRE, S_TRADE]], [253, [S_TALK, S_TRADE_REPAIR]], [254, [S_TALK, S_TRADE, S_GAMBLE]], [255, [S_TALK, S_TRADE]],
  [406, [S_TALK]], [257, [S_TALK]], [264, [S_TALK, 3385]], [297, [S_TALK]],
  [266, [S_TALK]], [331, [S_TALK]], [408, [S_TALK]], [378, [S_TALK]],
  [377, [S_TALK]], [521, [S_TALK]], [520, [S_TALK, 4020]], [511, [S_TALK, S_TRADE_REPAIR]],
  [513, [S_TALK, S_TRADE]], [512, [S_TALK, S_TRADE, S_GAMBLE]], [514, [S_TALK, S_GAMBLE]], [515, [S_TALK]],
  [527, [S_TALK]], [537, [S_TALK]], [538, [S_TALK]], [539, [S_TALK]],
];

/** The retail entries for an NPC class (first matching record, as D2Client walks the table), or null. */
export function npcMenuEntries(classId) {
  const row = NPC_MENU_TABLE.find(([cls]) => cls === classId);
  if (!row) return null;
  return [...row[1].map((stringId) => ({ stringId, action: ACTION[stringId] ?? OTHER })), { stringId: S_CANCEL, action: 'cancel' }];
}

export const NPC_MENU_ASSETS = { palette: FRONTEND.palette, font: 'data\\local\\font\\latin\\font16.dc6', fontTbl: 'data\\local\\font\\latin\\font16.tbl',
  pieces: 'data\\global\\ui\\menu\\boxpieces.dc6' };
export const NPC_MENU_LAYOUT = Object.freeze({ padX: 20, firstLine: 21, pitch: 15, bottomBaseAboveFeet: 148, bottomPerLine: 10 });

/**
 * The boxpieces cels retail draws around a menu rect, in its order: [frame, anchor x, anchor y]
 * (anchor = bottom row, inclusive). Retail lists establish these cycles for the 3-line Warriv and
 * 4-line Hratli boxes: top frames 2..7 begin at 3; bottom frames 16..21 begin after the top's
 * extra pieces; left frames 12,10,11 repeat; right frames 13..15 advance for each extra left piece.
 * Each edge starts inside the rect, followed by corners 0,1,8,9.
 */
export function menuFramePieces(x1, y1, x2, y2) {
  const out = [];
  const top = [];
  for (let i = 0, x = x1 + 12; x < x2 - 12; i++, x += 12) top.push([2 + ((i + 1) % 6), x, y1 + 12]);
  out.push(...top);
  for (let i = 0, x = x1 + 13; x < x2 - 12; i++, x += 12) out.push([16 + ((i + top.length + 1) % 6), x, y2 + 10]);
  // The 69px (two-entry) retail menu starts each vertical strip one cel earlier
  // than the wider measured menus.  This is the third retail size (Jerhyn), not
  // a screen-specific offset: the narrow frame is still derived from its box.
  const narrow = x2 - x1 === 69;
  let leftCount = 0;
  for (let i = 0, y = y1 + 22; y < y2; i++, y += 12) { out.push([10 + ((i + (narrow ? 0 : 2)) % 3), x1 - 4, y]); leftCount++; }
  for (let i = 0, y = y1 + 24; y < y2; i++, y += 12) out.push([13 + ((i + leftCount - (narrow ? 3 : 4)) % 3), x2 - 7, y]);
  out.push([0, x1, y1 + 12], [1, x2 - 12, y1 + 12], [8, x1, y2 + 1], [9, x2 - 12, y2 + 1]);
  return out;
}
const GOLD = [0xc7, 0xb3, 0x77], BLUE = [0x69, 0x69, 0xff];

export function createNpcMenu({ read, decodeDc6, decodeDatPalette, string, documentObject = globalThis.document }) {
  const palette = decodeDatPalette(read(NPC_MENU_ASSETS.palette));
  const dc6 = decodeDc6(read(NPC_MENU_ASSETS.font)), tbl = decodeFontTbl(read(NPC_MENU_ASSETS.fontTbl));
  const tint = (rgb) => palette.map((v, i) => (i % 4 === 3 ? v : Math.round(v * rgb[i % 4] / 0xff)));
  const fontPath = NPC_MENU_ASSETS.font, glyphs = { op: 'celcolor', mode: 5 };
  const white = createFont({ dc6, tbl, palette, documentObject, path: fontPath, drawAs: glyphs });
  // A coloured line is its glyphs' indices through the act PL2's TextColorShifts[colour] (retail's own
  // 8-bit frame of this menu: Warriv's name, colour 4, 191 of 191 glyph pixels; an earlier change). Offsets per
  // OpenDiablo2's d2pl2 layout; gold is colour 4, blue colour 3.
  const TEXT_SHIFTS = 1024 + 8192 + 4096 + 256 + 5 * 65536 + 111 * 256 + 768 + 14 * 256 + 65536 + 256 + 39;
  const shifted = (colour, fallback, act = 1) => {
    try { const pl2 = read(`data\\global\\palette\\ACT${Math.min(5, Math.max(1, act | 0))}\\pal.pl2`), out = palette.slice();
      for (let k = 0; k < 256; k++) { const j = pl2[TEXT_SHIFTS + colour * 256 + k]; out.set(palette.subarray(j * 4, j * 4 + 3), k * 4); }
      return out; } catch { return tint(fallback); } };
  // Coloured fonts per act: the shift table is the CURRENT act's PL2 (Codex review: Act I's was read in
  // every act, so Lut Gholein's and Harrogath's menus shifted through the wrong table).
  const coloured = new Map();
  const fontsFor = (act) => { if (!coloured.has(act)) coloured.set(act, {
    gold: createFont({ dc6, tbl, palette: shifted(4, GOLD, act), documentObject, path: fontPath, drawAs: glyphs }),
    blue: createFont({ dc6, tbl, palette: shifted(3, BLUE, act), documentObject, path: fontPath, drawAs: glyphs }) });
    return coloured.get(act); };
  const pieces = decodeDc6(read(NPC_MENU_ASSETS.pieces));
  const pieceCanvas = new Map();
  const piece = (i) => {
    if (!pieceCanvas.has(i)) {
      const f = pieces.frames[i], c = documentObject.createElement('canvas');
      c.width = f.width; c.height = f.height;
      const rgba = new Uint8ClampedArray(f.width * f.height * 4);
      for (let k = 0; k < f.indices.length; k++) if (f.opaque[k]) rgba.set(palette.subarray(f.indices[k] * 4, f.indices[k] * 4 + 4), k * 4);
      c.getContext('2d').putImageData(new ImageData(rgba, f.width, f.height), 0, 0);
      pieceCanvas.set(i, c);
    }
    return pieceCanvas.get(i);
  };
  const L = NPC_MENU_LAYOUT;
  // The box darkens the world through the act palette's 50% blend table, as retail does: PL2
  // AlphaBlend[1] (reference/OpenDiablo2 d2pl2), transform of the world pixel's palette index, entry 0
  // (black). Measured: 80% of retail's fill pixels exact over its own background (s179); the rest
  // is rain. Each world pixel is taken back to its palette index (the world is drawn from it).
  const blends = new Map();
  const blendFor = (act) => {
    if (blends.has(act)) return blends.get(act);
    let value = null;
    try {
      const pl2 = read(`data\\global\\palette\\ACT${act}\\pal.pl2`);
      const base = 1024 + (32 + 16 + 1) * 256 + 65536, index = new Map();
      for (let i = 255; i >= 0; i--) index.set((pl2[i * 4] << 16) | (pl2[i * 4 + 1] << 8) | pl2[i * 4 + 2], i);
      value = { pl2, base, index };
    } catch { value = null; }
    blends.set(act, value); return value;
  };
  const darken = (ctx, b, act) => {
    const t = blendFor(act);
    if (!t) { ctx.fillStyle = 'rgba(0,0,0,0.5)'; ctx.fillRect(b.x, b.y, b.w, b.h); return; }
    const img = ctx.getImageData(b.x, b.y, b.w, b.h), d = img.data;
    for (let i = 0; i < d.length; i += 4) {
      let di = t.index.get((d[i] << 16) | (d[i + 1] << 8) | d[i + 2]);
      if (di === undefined) {   // not a palette colour (a scaled or blended pixel): the nearest one
        let best = 0, bestD = Infinity;
        for (let k = 0; k < 256; k++) { const dr = t.pl2[k * 4] - d[i], dg = t.pl2[k * 4 + 1] - d[i + 1], db = t.pl2[k * 4 + 2] - d[i + 2], dd = dr * dr + dg * dg + db * db; if (dd < bestD) { bestD = dd; best = k; } }
        di = best; t.index.set((d[i] << 16) | (d[i + 1] << 8) | d[i + 2], best);
      }
      const o = t.pl2[t.base + di * 256];
      d[i] = t.pl2[o * 4]; d[i + 1] = t.pl2[o * 4 + 1]; d[i + 2] = t.pl2[o * 4 + 2];
    }
    ctx.putImageData(img, b.x, b.y);
  };
  let state = null, rows = [], box = null, selected = 0;
  const layout = (feetX, feetY) => {
    const labels = [state.name, ...state.entries.map((e) => string(e.stringId) || String(e.stringId))];
    const cx = Math.trunc(feetX);
    const w = Math.max(...labels.map((l) => white.measure(l))) + L.padX;
    const x1 = cx - Math.trunc(w / 2), x2 = x1 + w;
    const y2 = Math.trunc(feetY) - L.bottomBaseAboveFeet + L.bottomPerLine * labels.length,
      y1 = y2 - (L.pitch * labels.length + L.firstLine);
    const bottoms = labels.map((_, i) => y1 + L.firstLine + i * L.pitch);
    rows = state.entries.map((e, i) => ({ ...e, label: labels[i + 1], x: x1, y: bottoms[i + 1] - 12, w, h: 15, inkBottom: bottoms[i + 1] }));
    box = { x: x1, y: y1, w, h: y2 - y1, x1, y1, x2, y2 };
    return { labels, bottoms, cx };
  };
  return {
    get open() { return Boolean(state); },
    get state() { return state; },
    /** `entries` from npcMenuEntries; the menu hangs over the NPC (its feet at the anchor). */
    show(npcId, classId, name, entries) { state = { npcId, classId, name, entries }; rows = []; box = null; selected = 0; },
    /** The pointer moved: the entry under it is selected, none when it is elsewhere (retail). */
    hover(x, y) { if (state && rows.length) selected = rows.findIndex((r) => x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h); return selected; },
    close() { state = null; rows = []; box = null; },
    draw(ctx, feetX, feetY, act = 1) {
      if (!state) return null;
      const { labels, bottoms } = layout(feetX, feetY);
      ctx.save();
      darken(ctx, box, act);
      recordPrim('box', box.x1, box.y1, box.x2, box.y2, 0, 1);
      for (const [i, ax, ay] of menuFramePieces(box.x1, box.y1, box.x2, box.y2)) {
        const f = pieces.frames[i], left = ax + f.offsetX, top = ay + f.offsetY - f.height + 1;
        ctx.drawImage(piece(i), left, top);
        recordDc6(NPC_MENU_ASSETS.pieces, i, f, left, top);
      }
      ctx.restore();
      // Centre from the actual box bounds.  This differs from a truncated centre only for an
      // odd-width box holding an even-width line (Jerhyn's 69px menu / "talk"), where retail
      // puts the ink one pixel to the right.
      const line = (font, text, bottom) => font.drawAnchored(ctx, text, box.x1 + Math.ceil((box.w - font.measure(text)) / 2) + 1, bottom);
      const { gold, blue } = fontsFor(act);
      line(gold, labels[0], bottoms[0]);
      rows.forEach((r, i) => line(i === selected ? blue : white, r.label, r.inkBottom));
      return { npcId: state.npcId, classId: state.classId, name: state.name, box: { ...box },
        entries: rows.map((r) => ({ stringId: r.stringId, action: r.action, label: r.label, inkBottom: r.inkBottom })) };
    },
    /** The entry under a canvas point (as last drawn), or null. */
    entryAt(x, y) { return rows.find((r) => x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h) ?? null; },
    targets() { return Object.fromEntries(rows.map((r) => [r.action, [Math.round(r.x + r.w / 2), Math.round(r.y + r.h / 2)]])); },
  };
}
