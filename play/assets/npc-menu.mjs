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
// Layout and colours MEASURED from retail's own frame (retail/frames/s4.png: Warriv's menu, 1.13c
// under Wine, the retail capture tool/frame-ingame-linux.sh): a translucent box with a 1px gold border, box
// width = widest line's advance + 20, 64px tall for three lines, bottom 120px above the NPC's feet, centred on
// them; lines in font16 (retail's ink widths: talk 33, cancel 48 -- font16's exactly) with ink bottoms at box top + 19, then every 15.5px (floored): the name
// in gold, the first entry in blue, the rest white. TODO(client-parity F18): one frame, one NPC --
// the blue entry is the first one with the pointer elsewhere; whether it follows the pointer, and the
// gold/blue/white translations (these are OpenDiablo2's colour tokens), are not verified. The border
// is retail's (154,136,80) taken back through the display ramp; the fill halves the world under it
// (retail's is a palette blend table, so ours differs by a shade where the table rounds).
import { createFont, decodeFontTbl, FRONTEND } from './frontend.mjs';

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

export const NPC_MENU_ASSETS = { palette: FRONTEND.palette, font: 'data\\local\\font\\latin\\font16.dc6', fontTbl: 'data\\local\\font\\latin\\font16.tbl' };
export const NPC_MENU_LAYOUT = Object.freeze({ padX: 10, firstInkBottom: 19, pitch: 15.5, bottomPad: 14, aboveFeet: 120 });
const GOLD = [0xc7, 0xb3, 0x77], BLUE = [0x69, 0x69, 0xff];

export function createNpcMenu({ read, decodeDc6, decodeDatPalette, string, documentObject = globalThis.document }) {
  const palette = decodeDatPalette(read(NPC_MENU_ASSETS.palette));
  const dc6 = decodeDc6(read(NPC_MENU_ASSETS.font)), tbl = decodeFontTbl(read(NPC_MENU_ASSETS.fontTbl));
  const tint = (rgb) => palette.map((v, i) => (i % 4 === 3 ? v : Math.round(v * rgb[i % 4] / 0xff)));
  const white = createFont({ dc6, tbl, palette, documentObject });
  const gold = createFont({ dc6, tbl, palette: tint(GOLD), documentObject });
  const blue = createFont({ dc6, tbl, palette: tint(BLUE), documentObject });
  // Where a capital's ink ends inside the glyph cell, so a line is placed by its ink bottom.
  const capital = dc6.frames[tbl.chars.get(65)?.imageIndex ?? 65];
  let inkBottom = capital ? capital.height - 1 : 0;
  while (capital && inkBottom > 0 && !capital.opaque.subarray(inkBottom * capital.width, (inkBottom + 1) * capital.width).some(Boolean)) inkBottom--;
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
  let state = null, rows = [], box = null;
  const layout = (feetX, feetY) => {
    const labels = [state.name, ...state.entries.map((e) => string(e.stringId) || String(e.stringId))];
    const w = Math.max(...labels.map((l) => white.measure(l))) + L.padX * 2;
    const h = L.firstInkBottom + Math.floor((labels.length - 1) * L.pitch) + L.bottomPad;
    const x = Math.round(feetX - w / 2), y = Math.round(feetY - L.aboveFeet - h);
    const bottoms = labels.map((_, i) => y + L.firstInkBottom + Math.floor(i * L.pitch));
    rows = state.entries.map((e, i) => ({ ...e, label: labels[i + 1], x, y: bottoms[i + 1] - 12, w, h: 15, inkBottom: bottoms[i + 1] }));
    box = { x, y, w, h };
    return { labels, bottoms };
  };
  return {
    get open() { return Boolean(state); },
    get state() { return state; },
    /** `entries` from npcMenuEntries; the menu hangs over the NPC (its feet at the anchor). */
    show(npcId, classId, name, entries) { state = { npcId, classId, name, entries }; rows = []; box = null; },
    close() { state = null; rows = []; box = null; },
    draw(ctx, feetX, feetY, act = 1) {
      if (!state) return null;
      const { labels, bottoms } = layout(feetX, feetY);
      ctx.save();
      darken(ctx, box, act);
      ctx.strokeStyle = 'rgb(99,88,52)'; ctx.lineWidth = 1; ctx.strokeRect(box.x + 0.5, box.y + 0.5, box.w - 1, box.h - 1);
      ctx.restore();
      const line = (font, text, bottom) => font.draw(ctx, text, Math.round(box.x + (box.w - font.measure(text)) / 2), bottom - inkBottom);
      line(gold, labels[0], bottoms[0]);
      rows.forEach((r, i) => line(i === 0 ? blue : white, r.label, r.inkBottom));
      return { npcId: state.npcId, classId: state.classId, name: state.name, box: { ...box },
        entries: rows.map((r) => ({ stringId: r.stringId, action: r.action, label: r.label, inkBottom: r.inkBottom })) };
    },
    /** The entry under a canvas point (as last drawn), or null. */
    entryAt(x, y) { return rows.find((r) => x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h) ?? null; },
    targets() { return Object.fromEntries(rows.map((r) => [r.action, [Math.round(r.x + r.w / 2), Math.round(r.y + r.h / 2)]])); },
  };
}
