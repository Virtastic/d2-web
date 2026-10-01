// The skill buttons either side of the belt, and the popup of learned skills a click on one opens.
//
// Retail: the left button (beside the life orb) and the right button (beside the mana orb) show the
// selected left- and right-hand skills; clicking one opens rows of the skills the player can put in
// that hand, and clicking an icon selects it (client packet 0x3C, D2Game's SelectSkill handler).
//
// What is listed comes from the engine (`d2_session_skill_choices`: every skill the player HAS, with
// SkillDesc's ListRow and IconCel and the Skills.txt passive/leftskill flags), what is selected from
// the engine (`d2_session_selected_skills`, the unit's own left/right skill), and selecting goes
// through the engine (`d2_session_command_select_skill`). Nothing here decides a skill exists.
//
// Layout is prior art, cross-checked against retail's own frame: OpenDiablo2 d2game/d2player/
// game_controls.go puts the buttons at (117,550) and (635,550), 50x50, and skill_select_panel.go
// draws the popup's rows 48px tall, the first row's top at y=465 and each further row 48px above it,
// the right popup ending at x=720. The left popup's start x is retail's OWN measured value, not
// OpenDiablo2's: `s341-drawlist-skill-popup-layout` reads it off `the retail captures
// v9b-skill-popup-left.txt` (retail 1.13c, retail-capture 2026-09-28) and found x=80, 10px left of
// OpenDiablo2's 90 -- icons at 80,128,176. Rows are SkillDesc ListRow 0..4, empty rows skipped; the
// left popup omits skills without Skills.txt `leftskill`, both omit passives and ListRow -1. Left
// rows are ordered by skill id ascending, right rows descending.
import { decodeDatPalette, decodeDc6, dc6FrameToRgba } from './d2-formats.mjs';
import { recordDc6 } from './drawlist.mjs';
import { hudOrigin } from './hud-origin.mjs';
import { createFont, decodeFontTbl, textColourPalette } from './frontend.mjs';
import { drawPlate, buildSkillPlate } from './skill-panel.mjs';

export const SESSION_SKILL_CHOICE_BYTES = 8;   // D2SessionSkillChoice
export const SKILL_SELECT_LAYOUT = Object.freeze({
  left: { x: 117, y: 550, w: 50, h: 50 }, right: { x: 635, y: 550, w: 50, h: 50 },
  icon: 48, firstRowTop: 465, leftStartX: 80, rightEndX: 720, rows: 5,
});
const CLASS_TOKEN = ['am', 'so', 'ne', 'pa', 'ba', 'dr', 'as'];
// The per-class sheet (<token>skillicon.dc6) holds the class's icons; the general skills (Attack,
// Kick, Throw, ...; Skills.txt charclass empty) index the shared SPELLS\Skillicon.dc6.
export const skillSheetAsset = (charClass) => charClass >= 0 && charClass < CLASS_TOKEN.length
  ? `data\\global\\ui\\SPELLS\\${CLASS_TOKEN[charClass]}skillicon.dc6` : 'data\\global\\ui\\SPELLS\\Skillicon.dc6';

export function parseSkillChoices(bytes, count) {
  const b = bytes instanceof Uint8Array ? bytes : new Uint8Array(bytes);
  if (!Number.isInteger(count) || count < 0 || b.length < count * SESSION_SKILL_CHOICE_BYTES) throw new RangeError('invalid skill choice extent');
  const v = new DataView(b.buffer, b.byteOffset, b.byteLength), out = [];
  for (let i = 0; i < count; i++) {
    const o = i * SESSION_SKILL_CHOICE_BYTES, flags = v.getUint8(o + 6);
    out.push({ id: v.getUint16(o, true), listRow: v.getInt8(o + 2), charClass: v.getInt8(o + 3), iconCel: v.getUint16(o + 4, true),
      passive: Boolean(flags & 1), leftSkill: Boolean(flags & 2), usable: !(flags & 4), townBlocked: Boolean(flags & 8), level: v.getUint8(o + 7) });
  }
  return out;
}

/** The popup's rows for one hand, as screen rectangles in 800x600 coordinates. */
export function skillSelectRows(choices, side) {
  const L = SKILL_SELECT_LAYOUT, rows = [];
  for (let r = 0; r < L.rows; r++) {
    const skills = choices.filter((s) => s.listRow === r && !s.passive && s.level > 0 && (side === 'right' || s.leftSkill))
      .sort((a, b) => side === 'left' ? a.id - b.id : b.id - a.id);
    if (!skills.length) continue;
    const width = skills.length * L.icon, top = L.firstRowTop - rows.length * L.icon;
    const x = side === 'left' ? L.leftStartX : L.rightEndX - width;
    rows.push({ listRow: r, top, x, width, skills: skills.map((s, i) => ({ ...s, x: x + i * L.icon, y: top })) });
  }
  return rows;
}

function frameCanvas(frame, palette, documentObject) {
  const canvas = documentObject.createElement('canvas');
  canvas.width = Math.max(1, frame.width); canvas.height = Math.max(1, frame.height);
  if (frame.width && frame.height) canvas.getContext('2d').putImageData(new ImageData(dc6FrameToRgba(frame, palette), frame.width, frame.height), 0, 0);
  return canvas;
}

export function createSkillSelectRenderer(read, documentObject = globalThis.document) {
  const palette = decodeDatPalette(read('data\\global\\palette\\SKY\\pal.dat'));
  // Retail draws a popup icon the unit cannot use right now through colour shift 1 (RED): `celcolor
  // Skillicon.dc6 ... extra=1` in the retail captures (Throw and Kick, unarmed).
  const red = (() => { try { return textColourPalette(read('data\\global\\palette\\SKY\\pal.pl2'), palette, 1); } catch { return palette; } })();
  const sheets = new Map();
  const sheet = (charClass, tint = false) => {
    const key = skillSheetAsset(charClass), id = tint ? key + '#red' : key;
    if (!sheets.has(id)) {
      const frames = decodeDc6(read(key)).frames;   // the raw frames: the draw list hashes their bytes
      sheets.set(id, { key, frames, canvases: frames.map((f) => frameCanvas(f, tint ? red : palette, documentObject)) });
    }
    return sheets.get(id);
  };
  const fonts = new Map();
  const fontFor = (act) => { const a = Math.min(5, Math.max(1, act | 0)); if (!fonts.has(a)) fonts.set(a, createFont({ dc6: decodeDc6(read('data\\local\\font\\latin\\font16.dc6')),
    tbl: decodeFontTbl(read('data\\local\\font\\latin\\font16.tbl')), palette: decodeDatPalette(read(`data\\global\\palette\\ACT${a}\\pal.dat`)), documentObject })); return fonts.get(a); };
  const labelFonts = new Map(), FONT = 'data\\local\\font\\latin\\font16';
  const labelFont = (act) => { const a = Math.min(5, Math.max(1, act | 0)); if (!labelFonts.has(a)) labelFonts.set(a, createFont({ dc6: decodeDc6(read(FONT + '.dc6')), tbl: decodeFontTbl(read(FONT + '.tbl')),
    palette: textColourPalette(read('data\\global\\palette\\SKY\\pal.pl2'), decodeDatPalette(read('data\\global\\palette\\SKY\\pal.dat')), 4), documentObject, path: FONT + '.dc6', drawAs: { op: 'celcolor', mode: 5, extra: 4 } })); return labelFonts.get(a); };
  // Each hand's button and popup are anchored to their own screen edge and the bottom (hud-origin.mjs).
  const origin = (width, height, side) => hudOrigin(width, height, side === 'right' ? 'bottom-right' : 'bottom-left');
  const toScreen = (x, y, width, height, side) => { const o = origin(width, height, side); return [(x - o.x) / o.scale, (y - o.y) / o.scale]; };
  const inRect = (x, y, r) => x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h;
  const renderer = {
    open: null, hoverSide: null, hoverIcon: null,
    /** The pointer moved: which button (if any) it is over -- retail shows the hand's skill plate there. */
    hover(cx, cy, width, height, choices) { const h = this.hitTest(cx, cy, width, height, choices); this.hoverSide = h?.button || null; this.hoverIcon = h?.skill ? h : null; },
    /** What is under a canvas point: { button: side } or { side, skill } in the open popup. */
    hitTest(cx, cy, width, height, choices) {
      const L = SKILL_SELECT_LAYOUT;
      if (this.open) { const [x, y] = toScreen(cx, cy, width, height, this.open);
        for (const row of skillSelectRows(choices, this.open))
          for (const s of row.skills) if (inRect(x, y, { x: s.x, y: s.y, w: L.icon, h: L.icon })) return { side: this.open, skill: s }; }
      for (const side of ['left', 'right']) { const [x, y] = toScreen(cx, cy, width, height, side); if (inRect(x, y, L[side])) return { button: side }; }
      return null;
    },
    /** Canvas-pixel centres of the buttons and of every icon in the open popup (for aimed clicks). */
    targets(width, height, choices) {
      const L = SKILL_SELECT_LAYOUT, at = (side, x, y) => { const o = origin(width, height, side); return [Math.round(o.x + x * o.scale), Math.round(o.y + y * o.scale)]; };
      const out = { left: at('left', L.left.x + L.left.w / 2, L.left.y + L.left.h / 2), right: at('right', L.right.x + L.right.w / 2, L.right.y + L.right.h / 2), skills: {} };
      if (this.open) for (const row of skillSelectRows(choices, this.open)) for (const s of row.skills) out.skills[s.id] = at(this.open, s.x + L.icon / 2, s.y + L.icon / 2);
      return out;
    },
    draw(ctx, width, height, choices, selected, extra = {}) {
      const L = SKILL_SELECT_LAYOUT, drawn = [];
      // The blit is recorded, not just counted: retail's own draw lists carry this cel (the socket
      // icon as `Skillicon.dc6` frame 2 at the anchor (117|635, 600)), and a record is what lets a
      // row compare OUR anchor against RETAIL's instead of trusting the layout constant below.
      const icon = (s, x, y, where, side) => {
        const tint = s.usable === false || (where.endsWith('Button') && s.townBlocked), sh = sheet(s.charClass, tint), f = sh.frames[s.iconCel], c = sh.canvases[s.iconCel];
        if (!f || !c) { drawn.push({ where, id: s.id, sheet: sh.key, cel: s.iconCel, missing: true }); return; }
        const o = origin(width, height, side); ctx.save(); ctx.translate(o.x, o.y); ctx.scale(o.scale, o.scale);
        ctx.drawImage(c, x, y); ctx.restore();
        drawn.push({ where, id: s.id, red: tint, townBlocked: Boolean(s.townBlocked), sheet: sh.key, cel: s.iconCel, origin: { x: o.x, y: o.y }, blit: recordDc6(sh.key, s.iconCel, f, x, y, { op: 'celcolor', extra: tint ? 1 : 0 }) });   // retail's own op: celcolor
      };
      for (const side of ['left', 'right']) {
        const s = choices.find((c) => c.id === selected?.[side]);
        // Retail draws the button's icon (SkillIcon, 48x48) anchored at (117|635, 600): top-left (x, 553).
        if (s) icon(s, L[side].x, L[side].y + 3, side + 'Button', side);
      }
      // Retail labels a hand's button and the icons of its open popup 'F<n>' (Font16, gold shift) for every
      // hotkey bound to that skill in that hand (V9b). ONE rule for all four places, measured off retail's
      // own draw lists: the label's last pixel + 1 is the icon cell's RIGHT edge, and its bottom row is the
      // icon's bottom anchor minus 35 (the cell is drawn at `y0`, its 48-tall cel bottom-anchored there).
      //   left button   v9b-hotkey-f5-r29:      'F5' at 147,565 on the icon at 117,600  -> 117+48-1-17
      //   left popup    v9b-popup-hotkey-r29:   'F5' at 110,431 on the icon at  80,466
      //   right button  rightpopup-fkey-caps:   'F1'..'F8' at 665..669,565 on the icon at 635,600
      //   right popup   rightpopup-fkey-caps:   'F6' at 607,431 on the icon at 576,466
      // The x is right-aligned, NOT a fixed +30: the 30 that fits 'F5' is 48-1-17, and 48-1-width moves with
      // the text (s570's own F5 check could not tell the two apart; the F1..F8 group can).
      const keysFor = (id, side) => (extra.hotkeys || []).map((k, i) => (k.skill === id && k.left === (side === 'left') ? 'F' + (i + 1) : null)).filter(Boolean);
      const labels = [];
      const label = (side, ids, x0, y0, text) => {
        const f = labelFont(extra.act), tx = x0 + L.icon - 1 - f.measure(text), ty = y0 - 35;
        f.drawAnchored(ctx, text, tx, ty); labels.push({ where: ids, side, text, x: tx, y: ty });
      };
      { const o = origin(width, height, 'left'), r = origin(width, height, 'right');
        for (const side of ['left', 'right']) { if (!(selected?.[side] >= 0)) continue; const oo = side === 'left' ? o : r;
          ctx.save(); ctx.translate(oo.x, oo.y); ctx.scale(oo.scale, oo.scale);
          for (const t of keysFor(selected[side], side)) label(side, side + 'Button', L[side].x, L[side].y + 50, t);
          ctx.restore(); } }
      const rows = this.open ? skillSelectRows(choices, this.open) : [];
      for (const row of rows) for (const s of row.skills) icon(s, s.x, s.y, 'popup', this.open);
      if (this.open) { const oo = origin(width, height, this.open); ctx.save(); ctx.translate(oo.x, oo.y); ctx.scale(oo.scale, oo.scale);
        for (const row of rows) for (const s of row.skills) for (const t of keysFor(s.id, this.open)) label(this.open, 'popup:' + s.id, s.x, s.y + 49, t);
        ctx.restore(); }
      // Retail's hover plate over a hand's button (v9b-skill-popup-left/right: box 117|635,563 to +125,595,
      // "Attack" over "normal attack" = SkillDesc's wStrName and wStrShort): the same plate as the tree's.
      let plate = null;
      const hs = this.hoverSide, sel = hs && selected?.[hs] >= 0 ? selected[hs] : null, strs = sel !== null && extra.skillStrings ? extra.skillStrings(sel) : null;
      if (strs && extra.line) {
        const lines = [strs.name, strs.short].map((i) => extra.line(i)).filter(Boolean).map((text) => ({ text }));
        if (lines.length) {
          const o = origin(width, height, hs); ctx.save(); ctx.translate(o.x, o.y); ctx.scale(o.scale, o.scale);
          const box = drawPlate(ctx, fontFor(extra.act), { x: L[hs].x, y: L[hs].y + 2, w: L.icon, h: L.icon }, lines); ctx.restore();
          plate = { side: hs, id: sel, box, lines: lines.map((l) => l.text) };
        }
      }
      // Retail's plate over an icon INSIDE the popup (v9b-popup-iconhover-r29): the tree's plate for that skill, above the icon.
      let iconPlate = null;
      const hi = this.open && this.hoverIcon && this.hoverIcon.side === this.open ? this.hoverIcon.skill : null;
      if (hi && extra.plateState && extra.line) {
        const ps = extra.plateState, rec = ps.record ? ps.record(hi.id) : null;
        let lines = rec ? buildSkillPlate(rec, ps, true).lines : null;
        if (!lines) { const st = extra.skillStrings?.(hi.id); lines = st ? [st.name, st.short].map((i) => extra.line(i)).filter(Boolean).map((text) => ({ text })) : []; }
        lines = lines.map((l) => ({ ...l }));
        if (lines.length) {
          const o = origin(width, height, this.open); ctx.save(); ctx.translate(o.x, o.y); ctx.scale(o.scale, o.scale);
          const box = drawPlate(ctx, fontFor(extra.act), { x: hi.x, y: hi.y, w: L.icon, h: L.icon }, lines); ctx.restore();
          iconPlate = { side: this.open, id: hi.id, box, lines: lines.map((l) => l.text) };
        }
      }
      return { open: this.open, plate, iconPlate, labels, selected: { ...selected }, rows: rows.map((r) => ({ listRow: r.listRow, ids: r.skills.map((s) => s.id) })), drawn };
    },
  };
  return renderer;
}
