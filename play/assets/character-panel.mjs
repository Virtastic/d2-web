// The character screen: level, experience, the four attributes, and spending the points a level-up
// gave you.
//
// This is the "the engine already computes it and nothing draws it" gap again (
// P5). `d2_session_advancement` has carried level, experience, next-level experience, the four
// attributes and the remaining stat/skill point counts for several cycles, and
// `d2_session_command_add_stat` -- retail's own 0x3A handler -- has been proved by scenario. Without
// a screen, a character that levels up accumulates points it can never spend, which is a wall in
// front of playing the game rather than a missing decoration.
//
// Nothing here computes a stat. Every number drawn comes from the engine, and every "+" sends
// retail's packet and re-reads the result.

import { decodeDatPalette, decodeDc6, dc6FrameToRgba } from './d2-formats.mjs';
import { createFont, decodeFontTbl, textColourPalette } from './frontend.mjs';
import { BORDER_LEFT, CHROME_ART, createPanelChrome } from './panel-chrome.mjs';
import { characterPanelCloseFrame } from './character-panel-close.mjs';

// Retail's own stat ids (D2StatList.h): the values the 0x3A packet carries.
export const STAT = Object.freeze({ STRENGTH: 0, ENERGY: 1, DEXTERITY: 2, VITALITY: 3 });

// invchar6 is one sheet holding BOTH panels: frames 0..3 are this one (the left-hand character
// screen), 4..7 the inventory. The inventory renderer takes 4..7; taking 0..3 here is why the two
// panels agree on art and palette without a second asset.
const PANEL_FRAMES = [0, 1, 2, 3];
// Retail puts the character screen against the left edge, the inventory against the right.
export const CHARACTER_LAYOUT = Object.freeze({ screenX: 0, screenY: 60, width: 320, height: 441 });

// The panel's OWN slots, read off the retail art rather than invented (s154). Every frame is a
// hollow gold outline; flood-filling those gives 18 of them at a consistent geometry:
//   two at the top, four in the left column (112x23), eight in the right column (151x20) and four
//   resistance slots (138x20). The previous layout put text on a grid of made-up offsets that was
//   not the panel's grid at all -- rows at y 38/62/86/140/170/200/230 against real slots at
//   80/142/228/290 -- which is why it read as lying ON the borders.
//
// Each frame holds a label compartment on the left and a value compartment on the right, so a label
// is drawn inset from the frame's left edge and its value right-aligned to the frame's right edge.
export const CHARACTER_SLOTS = Object.freeze({
  name:       { x: 9, y: 9, w: 174, h: 19 },
  className:  { x: 191, y: 9, w: 120, h: 19 },
  strength:   { x: 9, y: 80, w: 112, h: 23, stat: STAT.STRENGTH },
  dexterity:  { x: 9, y: 142, w: 112, h: 23, stat: STAT.DEXTERITY },
  vitality:   { x: 9, y: 228, w: 112, h: 23, stat: STAT.VITALITY },
  energy:     { x: 9, y: 290, w: 112, h: 23, stat: STAT.ENERGY },
  level:      { x: 160, y: 82, w: 151, h: 20 },
  experience: { x: 160, y: 106, w: 151, h: 20 },
  nextExperience: { x: 160, y: 144, w: 151, h: 20 },
  life:       { x: 160, y: 168, w: 151, h: 20 },
  mana:       { x: 160, y: 192, w: 151, h: 20 },
});
// The point-spending "+" beside an attribute, as RETAIL'S OWN ART rather than a drawn rectangle.
//
// The art and its frames are prior art: OpenDiablo2's button
// table defines the attribute button as `ButtonTypeAddSkill` with `ResourceName =
// d2resource.AddSkillButton` = `PANEL/level.DC6` and `DisabledFrame = 2` (d2core/d2ui/button.go,
// d2common/d2resource/resource_paths.go), and the socket it is drawn into is `HeroStatsPanelSocket`
// = `PANEL/levelsocket.dc6` (`hero_stats_panel.go`, `addStatSocketOffset = -3, 34`).
//
// The two are drawn as a PAIR by retail itself, and the offset between them is measured, not taken
// from OpenDiablo2: `the retail captures` draws `Levelsocket.dc6` frame 0 (35x36) at
// (440,495) and then `Level.DC6` frame 0 (30x30) at (443,491) -- socket 3 px left and 2 px above the
// button, the same pair and offset at (725,440)/(728,436) in `vendor-akara.txt` and `den-outside.txt`.
export const STAT_PLUS_ART = 'data\\global\\ui\\PANEL\\level.DC6';
export const STAT_PLUS_SOCKET_ART = 'data\\global\\ui\\PANEL\\levelsocket.dc6';
export const STAT_PLUS_FRAME = 0;              // retail's enabled frame: a point is waiting
export const STAT_PLUS_PRESSED_FRAME = 1;      // level.dc6's held (sunken) gem, the frame after the lit one
export const STAT_PLUS_DISABLED_FRAME = 2;     // OpenDiablo2's DisabledFrame, recorded for the day a
                                               // capture shows retail drawing the unlit state
export const STAT_PLUS_SOCKET_OFFSET = Object.freeze({ x: -3, y: -2 });

// OpenDiablo2's hero_stats_panel.go places the remaining-stat label as two localized Font6 rows
// beside a separate Font16 count. Its positions are top-left coordinates; drawAnchored uses an
// inclusive bottom-row anchor, so drawStatPoints converts with each decoded font's actual cell height.
export const STAT_POINTS_LAYOUT = Object.freeze({
  labelX: Object.freeze([100, 103]), labelRows: Object.freeze([405, 413]), countX: 190, countY: 405,
  palette: 'data\\global\\palette\\SKY\\pal.dat',
  pl2: 'data\\global\\palette\\SKY\\pal.pl2',
  labelColour: 1,
});

// Retail prints experience as an unsigned 32-bit value.
const u32 = (v) => (v === undefined || v === null ? v : v >>> 0);
// Retail groups the thousands of a 32-bit experience value with commas ("3,500,000,000").
const grouped = (v) => (v === undefined || v === null ? v : String(v).replace(/\B(?=(\d{3})+(?!\d))/g, ','));
// retail T2 x,y is screen minus (80,60) (CHARACTER_LAYOUT.screenY), not the frame-0 origin (80,61).
const T2_OFFSET = Object.freeze({ x: 80, y: 60 });
// Draws the stat-points block and returns its three strings as T2 records (screen minus (80,60)).
export function drawStatPoints(font6, font16, ctx, points, string) {
  if (!(points > 0)) return [];
  const labelAnchor = (top) => top + font6.cellHeight - 1;
  const redFont = font6.red || font6;
  const labels = [[string('strchrstat'), STAT_POINTS_LAYOUT.labelRows[0], STAT_POINTS_LAYOUT.labelX[0]], [string('strchrrema'), STAT_POINTS_LAYOUT.labelRows[1], STAT_POINTS_LAYOUT.labelX[1]]];
  const out = [];
  for (const [text, top, left] of labels) { redFont.drawAnchored(ctx, text, left, labelAnchor(top)); out.push({ text, x: left - T2_OFFSET.x, y: labelAnchor(top) - T2_OFFSET.y, c: STAT_POINTS_LAYOUT.labelColour }); }
  const count = String(points), cx = STAT_POINTS_LAYOUT.countX - Math.trunc(font16.measure(count) / 2), cy = STAT_POINTS_LAYOUT.countY + font16.cellHeight - 1;
  font16.drawAnchored(ctx, count, cx, cy);
  out.push({ text: count, x: cx - T2_OFFSET.x, y: cy - T2_OFFSET.y, c: 0 });
  return out;
}

// Where the button goes, from the panel's own two independent readings: the attribute frames are the
// panel ART's (CHARACTER_SLOTS, s154, and s251 pins the panel at 0 px), and OpenDiablo2's own button
// list is the cross-check -- its four buttons at screen (205, 140/201/286/347) against the art's
// "frame right edge + 4" = 205 exactly, and level with each frame's top row. OpenDiablo2's absolute y
// is 7 px off retail's (its labels are), so the RELATION is what is taken from it, never the number.
const STAT_BUTTON_GAP = 4;
// Measured against retail's own frame (retail/frames/charpanel-points.fb.png, s192): retail's button
// sits 4 px ABOVE its attribute frame's top row -- the "level with the top row" reading was 4 px low.
const STAT_BUTTON_DY = -4;
// InvChar6 frames 0..3 anchored as retail anchors them (draw() below); frame 0's top-left is the
// panel's origin, which is what the slots are relative to.
const PANEL_ORIGIN = Object.freeze({ x: 80, y: 61 });
export const STAT_BUTTON_RECTS = Object.freeze(Object.fromEntries(
  ['strength', 'dexterity', 'vitality', 'energy'].map((key) => {
    const slot = CHARACTER_SLOTS[key];
    return [slot.stat, { x: PANEL_ORIGIN.x + slot.x + slot.w + STAT_BUTTON_GAP, y: PANEL_ORIGIN.y + slot.y + STAT_BUTTON_DY,
      w: 30, h: 30, slot: key }];
  })));

// Which slot holds which field: the four attributes are certain (four left frames, retail's own
// Strength/Dexterity/Vitality/Energy order) and so is the name. The right column assignment is
// INFERRED from retail's field order and is not verified -- the remaining slots at y 230/254/292
// and the four resistance frames are deliberately left empty rather than filled with a guess.
// The label text is RETAIL's, by string key -- the same keys OpenDiablo2's hero_stats_panel.go
// uses (strchrstr, strchrdex, ...). They were hardcoded English before, which is a translation of
// the game rather than the game. Verified by s157: every key resolves.
const ROWS = Object.freeze([
  ['level', 'strchrlvl'], ['experience', 'strchrexp'], ['nextExperience', 'strchrnxtlvl'],
  ['life', 'strchrlif'], ['mana', 'strchrman'],
  ['strength', 'strchrstr'], ['dexterity', 'strchrdex'],
  ['vitality', 'strchrvit'], ['energy', 'strchreng'],
]);
const INSET = 6;

function frameCanvas(frame, palette, documentObject) {
  const canvas = documentObject.createElement('canvas');
  canvas.width = Math.max(1, frame.width); canvas.height = Math.max(1, frame.height);
  if (!frame.width || !frame.height) return canvas;
  canvas.getContext('2d').putImageData(new ImageData(dc6FrameToRgba(frame, palette), frame.width, frame.height), 0, 0);
  return canvas;
}

/**
 * `read` is the MPQ reader. The panel holds no state: what it draws is passed in every frame, so it
 * cannot show a stale character's numbers.
 */
export function createCharacterPanelRenderer(read, documentObject = globalThis.document) {
  const palette = decodeDatPalette(read('data\\global\\palette\\SKY\\pal.dat'));
  const dc6 = decodeDc6(read('data\\global\\ui\\PANEL\\invchar6.DC6'));
  if (dc6.frames.length < 8) throw new Error('unexpected retail invchar6 frame set');
  const panel = PANEL_FRAMES.map((i) => frameCanvas(dc6.frames[i], palette, documentObject));
  const font = createFont({
    dc6: decodeDc6(read('data\\local\\font\\latin\\font16.dc6')),
    tbl: decodeFontTbl(read('data\\local\\font\\latin\\font16.tbl')),
    palette: decodeDatPalette(read('data\\global\\palette\\units\\pal.dat')), documentObject,
  });

  const origin = (width, height) => {
    const scale = Math.min(1, width / 800, height / 600);
    return { scale,
      x: (width - 800 * scale) / 2 + CHARACTER_LAYOUT.screenX * scale,
      y: (height - 600 * scale) / 2 + CHARACTER_LAYOUT.screenY * scale };
  };

  // Retail's own screen: InvChar6 frames 0..3 anchored at
  // (80|336, 316|492), every label in Font6 and every value in Font16, colour 0, each centred on the x
  // its retail sample sits at (sample x + half the sample's width), anchored at the sample's row.
  // [key, font, retail sample text, sample x, anchor y].
  // Fonts through the ACT's palette (the screen is shown in any act), cached per act.
  const f6 = { dc6: decodeDc6(read('data\\local\\font\\latin\\font6.dc6')), tbl: decodeFontTbl(read('data\\local\\font\\latin\\font6.tbl')) };
  const f16 = { dc6: decodeDc6(read('data\\local\\font\\latin\\font16.dc6')), tbl: decodeFontTbl(read('data\\local\\font\\latin\\font16.tbl')) };
  const statPalette = decodeDatPalette(read(STAT_POINTS_LAYOUT.palette));
  const statPl2 = read(STAT_POINTS_LAYOUT.pl2);
  const statPointFonts = {
    6: {
      ...createFont({ ...f6, palette: statPalette, documentObject }),
      red: createFont({ ...f6, palette: textColourPalette(statPl2, statPalette, STAT_POINTS_LAYOUT.labelColour), documentObject }),
    },
    16: createFont({ ...f16, palette: statPalette, documentObject }),
  };
  const faces = new Map();
  const facesFor = (act) => { let f = faces.get(act); if (!f) { const palette = decodeDatPalette(read(`data\\global\\palette\\ACT${Math.min(5, Math.max(1, act | 0))}\\pal.dat`));
    f = { 6: createFont({ ...f6, palette, documentObject }), 16: createFont({ ...f16, palette, documentObject }), c3: createFont({ ...f16, palette: textColourPalette(statPl2, palette, 3), documentObject }) }; faces.set(act, f); } return f; };
  const font6 = facesFor(1)[6], font16 = facesFor(1)[16];
  const F = { 6: font6, 16: font16 };
  // Value-box centres: the intersection of every retail charpanel capture's x = floor(centre - w/2)
  // over all digit counts (a single sample leaves the half-pixel open and was off by one at other widths).
  const CENTRE = { name: 167, level: 113.5, experience: 211, nextExperience: 332, strength: 175, dexterity: 176, vitality: 175, energy: 175,
    defense: 370.5, stamina: 330, staminaMax: 371, life: 330, lifeMax: 371, mana: 330, manaMax: 371,
    fireRes: 370.5, coldRes: 370.5, lightRes: 370.5, poisonRes: 370.5, dmgA: 365.5, dmgB: 365.5,
    // The attack rating is the one value column whose centre differs from defense's (retail's "7485" sits 2 px right of ours at 370.5).
    arA: 372.5, arB: 372.5 };
  // Value boxes retail LEFT-aligns at a fixed x (stamina, life and mana: the 4-digit values sit at T2 x 236/277 for
  // every digit count, where a centred box would move them 2-5 px). Screen x, measured from retail's T2 records.
  // The damage value (retail T2 x 268 for both rows, "13-26") is LEFT-aligned too: screen x 348 = 268 + T2_OFFSET.x.
  const LEFT = { stamina: 316, staminaMax: 357, life: 316, lifeMax: 357, mana: 316, manaMax: 357, dmgA: 348, dmgB: 348 };
  const FIELDS = [
    ['lvlLabel', 6, 'Level', 98, 104], ['expLabel', 6, 'Experience', 175, 104], ['nextLabel', 6, 'Next Level', 303, 104],
    ['strLabel', 6, 'Strength', 99, 157], ['dexLabel', 6, 'Dexterity', 96, 220], ['defLabel', 6, 'Defense', 280, 267],
    ['vitLabel', 6, 'Vitality', 100, 305], ['stmLabel', 6, 'Stamina', 260, 305], ['lifLabel', 6, 'Life', 271, 329],
    ['engLabel', 6, 'Energy', 104, 367], ['manLabel', 6, 'Mana', 267, 367],
    ['fir1', 6, 'Fire', 299, 402], ['fir2', 6, 'Resistance', 280, 410], ['col1', 6, 'Cold', 298, 426], ['col2', 6, 'Resistance', 280, 434],
    ['lit1', 6, 'Lightning', 285, 451], ['lit2', 6, 'Resistance', 280, 459], ['pos1', 6, 'Poison', 292, 475], ['pos2', 6, 'Resistance', 280, 483],
    ['dmgA1', 6, 'ATTACK', 271, 153], ['dmgA2', 6, 'Damage', 270, 161], ['dmgA', 16, '3-7', 355, 158],
    ['arA1', 6, 'ATTACK', 277, 216], ['arA2', 6, 'Rating', 279, 224], ['arA', 16, '85', 362, 220],
    ['dmgB1', 6, 'ATTACK', 271, 177], ['dmgB2', 6, 'Damage', 270, 185], ['dmgB', 16, '3-7', 355, 182],
    ['arB1', 6, 'ATTACK', 277, 240], ['arB2', 6, 'Rating', 279, 248], ['arB', 16, '85', 362, 244],
    ['className', 16, 'Barbarian', 294, 85], ['name', 16, 'Oracle', 167, 85],
    ['level', 16, '1', 111, 119], ['experience', 16, '0', 198, 119], ['nextExperience', 16, '500', 315, 119],
    ['strength', 16, '30', 165, 159], ['dexterity', 16, '20', 164, 221], ['defense', 16, '10', 362, 269], ['vitality', 16, '25', 166, 307],
    ['stamina', 16, '92', 321, 306], ['staminaMax', 16, '92', 362, 306], ['life', 16, '55', 321, 330], ['lifeMax', 16, '55', 362, 330],
    ['energy', 16, '10', 166, 368], ['mana', 16, '10', 321, 368], ['manaMax', 16, '10', 362, 368],
    ['fireRes', 16, '0', 364, 408], ['coldRes', 16, '0', 364, 432], ['lightRes', 16, '0', 364, 456], ['poisonRes', 16, '0', 364, 480],
  ].map(([key, f, sample, x, y]) => ({ key, size: f, centre: CENTRE[key] ?? x + F[f].measure(sample) / 2, y }));
  const chrome = createPanelChrome(read, documentObject);
  const view = (width, height) => { const scale = Math.min(1, width / 800, height / 600); return { scale, ox: (width - 800 * scale) / 2, oy: (height - 600 * scale) / 2 }; };
  const buttonRect = (stat) => STAT_BUTTON_RECTS[stat];
  return {
    get layout() { return CHARACTER_LAYOUT; },
    /** The panel's own rectangle on the 800x600 screen: the art's own extent, anchored where retail
     *  anchors InvChar6 frame 0 (PANEL_ORIGIN, the origin every slot and every "+" is measured from
     *  and what s251 pinned against retail's draw list). The click routing's panel test reads it, the
     *  same way the inventory's and the stash's `panelRect()` are read -- one source for the panel's
     *  rectangle rather than a second guess in the shell. */
    panelRect() { return { left: PANEL_ORIGIN.x, top: PANEL_ORIGIN.y, width: CHARACTER_LAYOUT.width, height: CHARACTER_LAYOUT.height }; },
    buttonAt(px, py, width, height, state) {
      if (!(state?.statPoints > 0)) return null;
      const v = view(width, height), x = (px - v.ox) / v.scale, y = (py - v.oy) / v.scale;
      for (const stat of Object.keys(STAT_BUTTON_RECTS).map(Number)) { const r = buttonRect(stat); if (x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h) return stat; }
      return null;
    },
    /** `state`: a d2_session_advancement read plus { name, className, life, lifeMax, mana, manaMax, screen: d2_session_character_screen's ten numbers, string(key) }. */
    draw(ctx, width, height, state, visible = true) {
      if (!visible) return null;
      const v = view(width, height), S = (k) => state?.string?.(k) || '';
      ctx.save(); ctx.translate(v.ox, v.oy); ctx.scale(v.scale, v.scale);
      for (const [i, x, y] of [[0, 80, 316], [1, 336, 316], [2, 80, 492], [3, 336, 492]]) chrome.cel(ctx, state?.act || 1, 'data\\global\\ui\\PANEL\\invchar6.DC6', i, x, y);
      const two = (k) => S(k).split(/[}\n]/);
      const sc = state?.screen || [];
      const attack = (S('skillname0') || '').toUpperCase(), dmg = sc.length ? `${sc[2]}-${sc[3]}` : '', ar = sc.length ? String(sc[1]) : '';
      const text = {
        lvlLabel: S('strchrlvl'), expLabel: S('strchrexp'), nextLabel: S('strchrnxtlvl'), strLabel: S('strchrstr'), dexLabel: S('strchrdex'),
        defLabel: S('strchrdef'), vitLabel: S('strchrvit'), stmLabel: S('strchrstm'), lifLabel: S('strchrlif'), engLabel: S('strchreng'), manLabel: S('strchrman'),
        fir1: two('strchrfir')[0], fir2: two('strchrfir')[1], col1: two('strchrcol')[0], col2: two('strchrcol')[1],
        lit1: two('strchrlit')[0], lit2: two('strchrlit')[1], pos1: two('strchrpos')[0], pos2: two('strchrpos')[1],
        dmgA1: attack, dmgA2: S('strchrskm'), dmgA: dmg, arA1: attack, arA2: (S('strchrrat').split(/[}\n]/)[1] || ''), arA: ar,
        dmgB1: attack, dmgB2: S('strchrskm'), dmgB: dmg, arB1: attack, arB2: (S('strchrrat').split(/[}\n]/)[1] || ''), arB: ar,
        className: state?.className ?? '', name: state?.name ?? '',
        level: state?.level, experience: grouped(u32(state?.experience)), nextExperience: grouped(u32(sc.length > 10 ? sc[10] : state?.nextExperience)),
        strength: state?.strength, dexterity: state?.dexterity, defense: sc[0], vitality: state?.vitality,
        stamina: sc[4], staminaMax: sc[16] ?? sc[5], life: sc.length > 11 ? sc[11] : state?.life, lifeMax: sc.length > 12 ? sc[12] : state?.lifeMax, energy: state?.energy,
        mana: sc.length > 13 ? sc[13] : state?.mana, manaMax: sc.length > 14 ? sc[14] : state?.manaMax,
        fireRes: sc[6], coldRes: sc[7], lightRes: sc[8], poisonRes: sc[9],
      };
      const face = facesFor(state?.act || 1);
      // Every string drawn, as retail T2 records it: left x and anchor y, in the T2 frame (screen - (80,60)),
      // colour 0, uncentred. The list is the evidence a retail recording-text diffs.
      const texts = [];
      for (const f of FIELDS) { const t = text[f.key]; if (t === undefined || t === null) continue;
        const over = f.key === 'stamina' && text.stamina > text.staminaMax, str = String(t), font = over ? face.c3 : face[f.size], left = LEFT[f.key] ?? Math.floor(f.centre - font.measure(str) / 2);
        font.drawAnchored(ctx, str, left, f.y); texts.push({ text: str, x: left - T2_OFFSET.x, y: f.y - T2_OFFSET.y, c: over ? 3 : 0 }); }
      chrome.cel(ctx, state?.act || 1, CHROME_ART.close, characterPanelCloseFrame(state?.closePressed), 208, 480);
      // The "+" is retail art, blitted as retail blits the pair (socket first, then the button on
      // top), and every blit is carried out to the evidence by the cel's own requested/drawn record.
      const buttons = [];
      const act = state?.act || 1;
      if (state?.statPoints > 0) for (const stat of Object.keys(STAT_BUTTON_RECTS).map(Number)) { const r = buttonRect(stat);
        const socket = chrome.cel(ctx, act, STAT_PLUS_SOCKET_ART, STAT_PLUS_FRAME,
          r.x + STAT_PLUS_SOCKET_OFFSET.x, r.y + STAT_PLUS_SOCKET_OFFSET.y + 36 - 1);
        const plus = chrome.cel(ctx, act, STAT_PLUS_ART, state?.statPressed === stat ? STAT_PLUS_PRESSED_FRAME : STAT_PLUS_FRAME, r.x, r.y + 30 - 1);
        buttons.push({ stat, slot: STAT_BUTTON_RECTS[stat].slot, rect: { ...r }, socket, plus }); }
      texts.push(...drawStatPoints(statPointFonts[6], statPointFonts[16], ctx, state?.statPoints, S));
      chrome.border(ctx, state?.act || 1, BORDER_LEFT);
      ctx.restore();
      return { x: v.ox, y: v.oy, scale: v.scale, buttons, statPoints: state?.statPoints ?? 0, skillPoints: state?.skillPoints ?? 0,
        texts, shown: FIELDS.filter((f) => !/Label|1$|2$/.test(f.key)).map((f) => ({ key: f.key, value: text[f.key] ?? null })) };
    },
  };
}
