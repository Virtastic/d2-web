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
// Layout is prior art, not derived: OpenDiablo2 d2game/d2player/game_controls.go puts the buttons
// at (117,550) and (635,550), 50x50, and skill_select_panel.go draws the popup's rows 48px tall,
// the first row's top at y=465 and each further row 48px above it; the left popup starts at x=90,
// the right one ends at x=720. Rows are SkillDesc ListRow 0..4, empty rows skipped; the left popup
// omits skills without Skills.txt `leftskill`, both omit passives and ListRow -1. Left rows are
// ordered by skill id ascending, right rows descending.
import { decodeDatPalette, decodeDc6, dc6FrameToRgba } from './d2-formats.mjs';
import { recordDc6 } from './drawlist.mjs';

export const SESSION_SKILL_CHOICE_BYTES = 8;   // D2SessionSkillChoice
export const SKILL_SELECT_LAYOUT = Object.freeze({
  left: { x: 117, y: 550, w: 50, h: 50 }, right: { x: 635, y: 550, w: 50, h: 50 },
  icon: 48, firstRowTop: 465, leftStartX: 90, rightEndX: 720, rows: 5,
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
      passive: Boolean(flags & 1), leftSkill: Boolean(flags & 2), level: v.getUint8(o + 7) });
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
  const sheets = new Map();
  const sheet = (charClass) => {
    const key = skillSheetAsset(charClass);
    if (!sheets.has(key)) {
      const frames = decodeDc6(read(key)).frames;   // the raw frames: the draw list hashes their bytes
      sheets.set(key, { key, frames, canvases: frames.map((f) => frameCanvas(f, palette, documentObject)) });
    }
    return sheets.get(key);
  };
  const origin = (width, height) => { const scale = Math.min(1, width / 800, height / 600); return { scale, x: (width - 800 * scale) / 2, y: (height - 600 * scale) / 2 }; };
  const toScreen = (x, y, width, height) => { const o = origin(width, height); return [(x - o.x) / o.scale, (y - o.y) / o.scale]; };
  const inRect = (x, y, r) => x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h;
  const renderer = {
    open: null,
    /** What is under a canvas point: { button: side } or { side, skill } in the open popup. */
    hitTest(cx, cy, width, height, choices) {
      const [x, y] = toScreen(cx, cy, width, height), L = SKILL_SELECT_LAYOUT;
      if (this.open) for (const row of skillSelectRows(choices, this.open))
        for (const s of row.skills) if (inRect(x, y, { x: s.x, y: s.y, w: L.icon, h: L.icon })) return { side: this.open, skill: s };
      for (const side of ['left', 'right']) if (inRect(x, y, L[side])) return { button: side };
      return null;
    },
    /** Canvas-pixel centres of the buttons and of every icon in the open popup (for aimed clicks). */
    targets(width, height, choices) {
      const o = origin(width, height), L = SKILL_SELECT_LAYOUT, at = (x, y) => [Math.round(o.x + x * o.scale), Math.round(o.y + y * o.scale)];
      const out = { left: at(L.left.x + L.left.w / 2, L.left.y + L.left.h / 2), right: at(L.right.x + L.right.w / 2, L.right.y + L.right.h / 2), skills: {} };
      if (this.open) for (const row of skillSelectRows(choices, this.open)) for (const s of row.skills) out.skills[s.id] = at(s.x + L.icon / 2, s.y + L.icon / 2);
      return out;
    },
    draw(ctx, width, height, choices, selected) {
      const o = origin(width, height), L = SKILL_SELECT_LAYOUT, drawn = [];
      ctx.save(); ctx.translate(o.x, o.y); ctx.scale(o.scale, o.scale);
      // The blit is recorded, not just counted: retail's own draw lists carry this cel (the socket
      // icon as `Skillicon.dc6` frame 2 at the anchor (117|635, 600)), and a record is what lets a
      // row compare OUR anchor against RETAIL's instead of trusting the layout constant below.
      const icon = (s, x, y, where) => {
        const sh = sheet(s.charClass), f = sh.frames[s.iconCel], c = sh.canvases[s.iconCel];
        if (!f || !c) { drawn.push({ where, id: s.id, sheet: sh.key, cel: s.iconCel, missing: true }); return; }
        ctx.drawImage(c, x, y);
        drawn.push({ where, id: s.id, sheet: sh.key, cel: s.iconCel, blit: recordDc6(sh.key, s.iconCel, f, x, y) });
      };
      for (const side of ['left', 'right']) {
        const s = choices.find((c) => c.id === selected?.[side]);
        // Retail draws the button's icon (SkillIcon, 48x48) anchored at (117|635, 600): top-left (x, 553).
        if (s) icon(s, L[side].x, L[side].y + 3, side + 'Button');
      }
      const rows = this.open ? skillSelectRows(choices, this.open) : [];
      for (const row of rows) for (const s of row.skills) icon(s, s.x, s.y, 'popup');
      ctx.restore();
      return { open: this.open, selected: { ...selected }, rows: rows.map((r) => ({ listRow: r.listRow, ids: r.skills.map((s) => s.id) })), drawn };
    },
  };
  return renderer;
}
