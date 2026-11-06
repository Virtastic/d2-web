// The Hireling screen (UI 0x24, O): 1.13c D2Client+0x9AE80 and the draws it calls. It is a LEFT
// panel and is drawn only in an expansion game whose player has a merc; without one retail draws
// nothing. The draw list is pure (hirelingPanelDrawList) so a layout test can
// compare it with the retail DLL's own tables; the renderer only blits it.

import { decodeDatPalette, decodeDc6, dc6FrameToRgba } from './d2-formats.mjs';
import { createFont, decodeFontTbl, textColourPalette } from './frontend.mjs';
import { CHROME_ART, createPanelChrome } from './panel-chrome.mjs';
import { mercSlotCelAt } from './item-cel-place.mjs';

export const HIRELING_ART = Object.freeze({
  panel: 'data\\global\\ui\\PANEL\\NPCInv.dc6',
  armor: 'data\\global\\ui\\PANEL\\inv_armor.dc6',
  helm: 'data\\global\\ui\\PANEL\\inv_helm_glove.dc6',
  weapons: 'data\\global\\ui\\PANEL\\inv_weapons.dc6',
});
// Screen origin: x + 80 (D2Client 0x6fbcb9a0), y + 60 (-0x6fbcb9a4) at 800x600.
export const HIRELING_ORIGIN = Object.freeze({ x: 80, y: 60 });
// NPCInv frames 0..3: left 80 / 80+0x100, bottom 600-60-0xe0 / 600-60-0x30 (+0x9AE80).
export const HIRELING_PANEL_CELS = Object.freeze([[0, 80, 316], [1, 336, 316], [2, 80, 492], [3, 336, 492]]);
// buysellbtn frame 10 (+1 while held, 0x6fbcbef0) at 80+0x110, 600-60-0x3f.
export const HIRELING_CLOSE = Object.freeze({ frame: 10, x: 352, y: 477 });
// +0x90EB0: Font6, x 80+0x0f, y 0xd6+60, the name cut from its end while wider than 0xa0.
export const HIRELING_NAME = Object.freeze({ x: 95, y: 274, maxWidth: 160 });
// +0x8C8B0, table 0x6fb81398 (14 bytes): x, x2, y, string id. Font6, colour 0.
export const HIRELING_LABELS = Object.freeze([
  [0x0f, 0, 0xec, 0xfda], [0x91, 0, 0xec, 0xfd9], [0xc8, 0, 0xec, 0xfdb], [0x0f, 0, 0x119, 0xfdc],
  [0x0f, 0, 0x131, 0xfde], [0x0f, 0, 0x149, 0xfdd], [0x0f, 0, 0x161, 0xfe0], [0xb4, 0xf5, 0x119, 0xfe7],
  [0xb4, 0xf5, 0x131, 0xfe8], [0xb4, 0xf5, 0x149, 0xfe9], [0xb4, 0xf5, 0x161, 0xfea], [0xb4, 0, 0xd6, 0xfe4],
].map(Object.freeze));
// +0x8EA20, table 0x6fb91210 (20 bytes): x0, x1, y, width, stat. Font8 (Font6 and y+1 past width).
export const HIRELING_VALUES = Object.freeze([
  [0, 120, 254, 0, 13], [0, 175, 254, 0, 12], [0, 305, 254, 0, 30], [0, 154, 282, 48, 0],
  [0, 154, 306, 48, 2], [0, 154, 330, 48, 21], [0, 154, 354, 48, 31], [0, 309, 282, 48, 39],
  [0, 309, 306, 48, 43], [0, 309, 330, 48, 41], [0, 309, 354, 48, 45], [210, 305, 215, 0, 6],
].map(Object.freeze));
// +0x8B2F0: the empty-slot pictures at a rect's left/bottom plus 0x6fb91118 / 0x6fb91130 /
// 0x6fb91158 / 0x6fb91160. Body locations: 1 head, 3 torso, 4 right arm, 5 left arm.
export const HIRELING_SILHOUETTE = Object.freeze({ torso: [2, -2], head: [0, -1], right: [0, -2], left: [0, -2] });
// Inventory.txt row 0xd (Hireling; the 800x600 page is Hireling2), as +0x908C0 loads it.
export const HIRELING_INVENTORY_RECORD = 13;
export const HIRELING_CELL = 29;
const RESIST = Object.freeze({ 39: 40, 43: 44, 41: 42, 45: 46 });   // resistance -> its max stat
const FONT8 = 0, FONT6 = 6;

const fit = (text, measure) => { let s = String(text); while (s && measure(FONT6, s) > HIRELING_NAME.maxWidth) s = s.slice(0, -1); return s; };

// A value's text and colour (+0x8EA20's switch on stat - 6). Unknown inputs (null) draw nothing.
function valueText(merc, stat) {
  const v = merc.stats?.[stat];
  if (stat === 6) {
    if (!merc.life) return null;
    let cur = Math.trunc(merc.life.pct * merc.life.max / 100);
    if (!merc.dead && cur < 0) cur = 1;
    return { text: `${cur} / ${merc.life.max}`, colour: 0 };
  }
  if (RESIST[stat] !== undefined) {
    if (v == null) return null;
    const cap = Math.min(95, 75 + (merc.stats?.[RESIST[stat]] | 0));
    const value = Math.max(-100, v + (merc.resistPenalty | 0));
    return value >= cap ? { text: String(cap), colour: 4 } : { text: String(value), colour: value < 0 ? 1 : 0 };
  }
  if (stat === 21 || stat === 31) return null;   // ponytail: damage and defense need D2Common's formulas
  return v == null ? null : { text: String(v), colour: 0 };
}

/**
 * Retail's draw list for the open Hireling screen, in 800x600 screen pixels.
 * merc: null (no merc: nothing) or { name, dead, stats: {stat: value}, life: {pct, max},
 *   resistPenalty, slots: {bodyLoc: {left, right, top, bottom, w, h}}, items: [{bodyLoc, invFile,
 *   invWidth, invHeight}], twoHanded, inventory }. env: { string(id), measure(font, text) }.
 * Records: {cel, path, frame, x, y} (left x, bottom row y), {text, font, x, y, colour}, {item, invFile, x, y}.
 */
export function hirelingPanelDrawList(merc, env, closePressed = false) {
  if (!merc) return [];
  const out = [], { x: ox, y: oy } = HIRELING_ORIGIN;
  for (const [frame, x, y] of HIRELING_PANEL_CELS) out.push({ cel: true, path: HIRELING_ART.panel, frame, x, y });
  out.push({ cel: true, path: CHROME_ART.close, frame: HIRELING_CLOSE.frame + (closePressed ? 1 : 0), x: HIRELING_CLOSE.x, y: HIRELING_CLOSE.y });
  out.push({ text: fit(merc.name || '', env.measure), font: FONT6, x: HIRELING_NAME.x, y: HIRELING_NAME.y, colour: 0 });
  for (const [lx, lx2, ly, id] of HIRELING_LABELS) {
    const str = String(env.string(id) ?? ''), nl = str.indexOf('\n'), x = lx + ox, room = lx2 - lx + 1;
    const lines = nl < 0 ? [str] : [str.slice(0, nl), str.slice(nl + 1)];   // only the first newline splits
    lines.forEach((line, i) => {
      const w = env.measure(FONT6, line);
      out.push({ text: line, font: FONT6, x: w < room ? x + ((room - w) >> 1) : x, y: ly + oy + (lines.length > 1 ? (i ? 4 : -4) : 0), colour: 0 });
    });
  }
  for (const [x0, x1, vy, width, stat] of HIRELING_VALUES) {
    const v = valueText(merc, stat);
    if (!v) continue;
    let font = FONT8, y = vy + oy - 2;
    if (width && env.measure(FONT8, v.text) > width) { font = FONT6; y += 1; }
    const w = env.measure(font, v.text), room = x1 - x0 + 1;
    const x = x0 === 0 ? x1 + ox - w : x1 === 0 ? x0 + ox : (w < room ? x0 + ox + ((room - w) >> 1) : x0 + ox);
    out.push({ text: v.text, font, x, y, colour: v.colour });
  }
  const worn = new Set((merc.items || []).map((i) => i.bodyLoc)), s = merc.slots || {};
  const pic = (path, frame, loc, [dx, dy]) => { const r = s[loc]; if (r) out.push({ cel: true, path, frame, x: r.left + dx, y: r.bottom + dy }); };
  const inv = merc.inventory !== false;
  if (!inv || !worn.has(3)) pic(HIRELING_ART.armor, 0, 3, HIRELING_SILHOUETTE.torso);
  if (!inv || !worn.has(1)) pic(HIRELING_ART.helm, 1, 1, HIRELING_SILHOUETTE.head);
  const r = inv && worn.has(4), l = inv && worn.has(5), two = inv && Boolean(merc.twoHanded);
  if (!l && (!r || !two)) pic(HIRELING_ART.weapons, 0, 5, HIRELING_SILHOUETTE.left);
  if (!r && (!l || !two)) pic(HIRELING_ART.weapons, 0, 4, HIRELING_SILHOUETTE.right);
  // +0x952A0: each worn item's cell box centred in its slot (signed division), bodyLoc order.
  for (const item of [...(merc.items || [])].sort((a, b) => a.bodyLoc - b.bodyLoc)) {
    const r2 = s[item.bodyLoc];
    if (!r2 || !item.invFile) continue;
    const at = mercSlotCelAt({ left: r2.left, top: r2.top, width: r2.w, height: r2.h }, item.invWidth || 1, item.invHeight || 1, HIRELING_CELL);
    out.push({ item: true, invFile: item.invFile, x: at.left, y: at.top - 1 });
  }
  return out;
}

function rows(bytes) {
  const lines = new TextDecoder('latin1').decode(bytes).replace(/\r/g, '').split('\n').filter(Boolean);
  const header = lines.shift().split('\t').map((n) => n.trim().toLowerCase());
  return lines.map((line) => { const v = line.split('\t'), row = {}; header.forEach((h, i) => { row[h] = (v[i] || '').trim(); }); return row; });
}

// The bodyLoc of an ITEMS_SerializeItem(server, save) stream: 'JM' 16, flags 32, format 10, mode 3,
// then (not on the ground) the body location in 4 bits, LSB first.
export function serializedBodyLoc(bits) {
  let v = 0;
  for (let i = 0; i < 4; i++) { const b = 61 + i; v |= ((bits[b >> 3] >> (b & 7)) & 1) << i; }
  return v;
}

export function createHirelingPanelRenderer(read, documentObject = globalThis.document) {
  const chrome = createPanelChrome(read, documentObject);
  const fontData = Object.fromEntries([[FONT6, 'font6'], [FONT8, 'font8']].map(([id, n]) => [id,
    { dc6: decodeDc6(read(`data\\local\\font\\latin\\${n}.dc6`)), tbl: decodeFontTbl(read(`data\\local\\font\\latin\\${n}.tbl`)) }]));
  const fonts = new Map();
  const palOf = (act) => `data\\global\\palette\\ACT${Math.min(5, Math.max(1, act | 0))}\\`;
  const fontFor = (act, id, colour) => {
    const key = `${act}:${id}:${colour}`;
    let f = fonts.get(key);
    if (!f) {
      const palette = decodeDatPalette(read(palOf(act) + 'pal.dat'));
      f = createFont({ ...fontData[id], palette: colour ? textColourPalette(read(palOf(act) + 'pal.pl2'), palette, colour) : palette, documentObject });
      fonts.set(key, f);
    }
    return f;
  };
  const measure = (id, s) => fontFor(1, id, 0).measure(s);
  let tables = null;
  const itemRows = () => (tables ??= ['weapons', 'armor', 'misc'].flatMap((n) => rows(read(`data\\global\\excel\\${n}.txt`)).filter((r) => r.code).map((r) => ({ ...r, weapon: n === 'weapons' }))));
  let penalties = null;
  const penalty = (difficulty) => (penalties ??= rows(read('data\\global\\excel\\DifficultyLevels.txt')).map((r) => Number(r.resistpenalty) | 0))[difficulty | 0] | 0;
  const icons = new Map();
  const icon = (act, invFile) => {
    const key = act + ':' + invFile;
    if (!icons.has(key)) {
      const f = decodeDc6(read(`data\\global\\items\\${invFile}.dc6`)).frames[0], c = documentObject.createElement('canvas');
      c.width = Math.max(1, f.width); c.height = Math.max(1, f.height);
      c.getContext('2d').putImageData(new ImageData(dc6FrameToRgba(f, decodeDatPalette(read(palOf(act) + 'pal.dat'))), f.width, f.height), 0, 0);
      icons.set(key, { c, f });
    }
    return icons.get(key);
  };
  const view = (w, h) => { const scale = Math.min(1, w / 800, h / 600); return { scale, ox: (w - 800 * scale) / 2, oy: (h - 600 * scale) / 2 }; };
  return {
    panelRect() { return { left: HIRELING_ORIGIN.x, top: HIRELING_ORIGIN.y, width: 320, height: 432 }; },
    /** The merc as +0x9AE80 reads it, or null (no merc unit: nothing is drawn). */
    merc(Module, roster, difficulty = 0) {
      const pet = (roster || []).find((p) => p.unitPresent);
      if (!pet) return null;
      const stat = (n) => { const v = Module.ccall('d2_session_unit_stat', 'number', ['number', 'number', 'number', 'number'], [1, pet.guid, n, 0]); return v === -2147483648 ? null : v; };
      const stats = {};
      for (const n of [0, 2, 12, 13, 30, 39, 40, 41, 42, 43, 44, 45, 46]) stats[n] = stat(n);
      const max = (stat(7) ?? 0) >> 8, life = stat(6) ?? 0;
      // ponytail: retail's percentage is the client pet entry +0x1c (D2Game's update); taken from the unit here.
      const pct = max > 0 ? Math.trunc((life >> 8) * 100 / max) : 0;
      const items = [];
      const countAt = Module._malloc(4);
      try {
        const p = Number(Module.ccall('d2_session_inventory_items', 'number', ['number', 'number', 'bigint'], [1, pet.guid, BigInt(countAt)]));
        const n = Module.HEAPU32[countAt >>> 2], txt = itemRows();
        for (let i = 0; p && i < n && i < 16; i++) {
          const o = p + i * 108, v = new DataView(Module.HEAPU8.buffer, o, 108), row = txt[v.getUint32(4, true)], q = v.getUint32(8, true);
          if (!row) continue;
          const invFile = (q === 7 && row.uniqueinvfile) || (q === 5 && row.setinvfile) || row.invfile;
          items.push({ bodyLoc: serializedBodyLoc(Module.HEAPU8.subarray(o + 28, o + 108)), invFile, invWidth: Number(row.invwidth) || 1, invHeight: Number(row.invheight) || 1, twoHanded: row.weapon && row['2handed'] === '1' });
        }
      } finally { Module._free(countAt); }
      const slots = {}, sp = Module._malloc(64 * 4);
      try {
        if (Module.ccall('d2_session_inventory_components', 'number', ['number', 'number', 'bigint', 'number'], [HIRELING_INVENTORY_RECORD, 1, BigInt(sp), 60])) {
          const c = new Int32Array(Module.HEAPU8.buffer.slice(sp, sp + 240));
          // Component order rArm, torso, lArm, head (Inventory.txt) -> body locations 4, 3, 5, 1.
          [4, 3, 5, 1].forEach((loc, k) => { slots[loc] = { left: c[k * 6], right: c[k * 6 + 1], top: c[k * 6 + 2], bottom: c[k * 6 + 3], w: c[k * 6 + 4], h: c[k * 6 + 5] }; });
        }
      } finally { Module._free(sp); }
      return { name: pet.name || '', dead: Boolean(pet.flags & 1), stats, life: { pct, max }, resistPenalty: penalty(difficulty),
        slots, items: items.filter((i) => i.bodyLoc), twoHanded: items.some((i) => (i.bodyLoc === 4 || i.bodyLoc === 5) && i.twoHanded), inventory: true };
    },
    draw(ctx, w, h, merc, { act = 1, string, closePressed = false } = {}) {
      const list = hirelingPanelDrawList(merc, { string, measure }, closePressed);
      if (!list.length) return { open: false, records: [] };
      const v = view(w, h), records = [];
      ctx.save(); ctx.setTransform(1, 0, 0, 1, 0, 0); ctx.translate(v.ox, v.oy); ctx.scale(v.scale, v.scale);
      for (const r of list) {
        if (r.cel) chrome.cel(ctx, act, r.path, r.frame, r.x, r.y);
        else if (r.item) { const { c, f } = icon(act, r.invFile); ctx.drawImage(c, r.x + f.offsetX, r.y + 1 + f.offsetY); }
        else fontFor(act, r.font, r.colour).drawAnchored(ctx, r.text, r.x, r.y);
        records.push(r);
      }
      ctx.restore();
      return { open: true, records, bounds: { x: v.ox + 80 * v.scale, y: v.oy + 60 * v.scale, width: 320 * v.scale, height: 432 * v.scale } };
    },
  };
}
