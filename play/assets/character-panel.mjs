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
import { createFont, decodeFontTbl } from './frontend.mjs';
import { BORDER_LEFT, CHROME_ART, createPanelChrome } from './panel-chrome.mjs';

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
  const faces = new Map();
  const facesFor = (act) => { let f = faces.get(act); if (!f) { const palette = decodeDatPalette(read(`data\\global\\palette\\ACT${Math.min(5, Math.max(1, act | 0))}\\pal.dat`));
    f = { 6: createFont({ ...f6, palette, documentObject }), 16: createFont({ ...f16, palette, documentObject }) }; faces.set(act, f); } return f; };
  const font6 = facesFor(1)[6], font16 = facesFor(1)[16];
  const F = { 6: font6, 16: font16 };
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
    ['className', 16, 'Barbarian', 294, 85], ['name', 16, 'Oracle', 141, 85],
    ['level', 16, '1', 111, 119], ['experience', 16, '0', 198, 119], ['nextExperience', 16, '500', 315, 119],
    ['strength', 16, '30', 165, 159], ['dexterity', 16, '20', 164, 221], ['defense', 16, '10', 362, 269], ['vitality', 16, '25', 166, 307],
    ['stamina', 16, '92', 321, 306], ['staminaMax', 16, '92', 362, 306], ['life', 16, '55', 321, 330], ['lifeMax', 16, '55', 362, 330],
    ['energy', 16, '10', 166, 368], ['mana', 16, '10', 321, 368], ['manaMax', 16, '10', 362, 368],
    ['fireRes', 16, '0', 364, 408], ['coldRes', 16, '0', 364, 432], ['lightRes', 16, '0', 364, 456], ['poisonRes', 16, '0', 364, 480],
  ].map(([key, f, sample, x, y]) => ({ key, size: f, centre: x + F[f].measure(sample) / 2, y }));
  const chrome = createPanelChrome(read, documentObject);
  const view = (width, height) => { const scale = Math.min(1, width / 800, height / 600); return { scale, ox: (width - 800 * scale) / 2, oy: (height - 600 * scale) / 2 }; };
  // The "+" beside an attribute when points remain: the art's arrow slot right of the value box.
  // DECLARED: retail's level-up button was not on screen in the capture (no points to spend).
  const STAT_ROWS = { [STAT.STRENGTH]: 159, [STAT.DEXTERITY]: 221, [STAT.VITALITY]: 307, [STAT.ENERGY]: 368 };
  const buttonRect = (stat) => ({ x: 196, y: STAT_ROWS[stat] - 15, w: 16, h: 16 });
  return {
    get layout() { return CHARACTER_LAYOUT; },
    buttonAt(px, py, width, height, state) {
      if (!(state?.statPoints > 0)) return null;
      const v = view(width, height), x = (px - v.ox) / v.scale, y = (py - v.oy) / v.scale;
      for (const stat of Object.keys(STAT_ROWS).map(Number)) { const r = buttonRect(stat); if (x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h) return stat; }
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
        level: state?.level, experience: state?.experience, nextExperience: sc.length > 10 ? sc[10] : state?.nextExperience,
        strength: state?.strength, dexterity: state?.dexterity, defense: sc[0], vitality: state?.vitality,
        stamina: sc[4], staminaMax: sc[5], life: sc.length > 11 ? sc[11] : state?.life, lifeMax: sc.length > 12 ? sc[12] : state?.lifeMax, energy: state?.energy,
        mana: sc.length > 13 ? sc[13] : state?.mana, manaMax: sc.length > 14 ? sc[14] : state?.manaMax,
        fireRes: sc[6], coldRes: sc[7], lightRes: sc[8], poisonRes: sc[9],
      };
      const face = facesFor(state?.act || 1);
      for (const f of FIELDS) { const t = text[f.key]; if (t === undefined || t === null || t === '') continue;
        const str = String(t), font = face[f.size]; font.drawAnchored(ctx, str, Math.floor(f.centre - font.measure(str) / 2), f.y); }
      chrome.cel(ctx, state?.act || 1, CHROME_ART.close, 10, 208, 480);
      const buttons = [];
      if (state?.statPoints > 0) for (const stat of Object.keys(STAT_ROWS).map(Number)) { const r = buttonRect(stat);
        ctx.save(); ctx.fillStyle = 'rgba(150,120,50,.55)'; ctx.fillRect(r.x, r.y, r.w, r.h); ctx.restore(); font16.draw(ctx, '+', r.x + 5, r.y + 1); buttons.push(stat); }
      if (state?.statPoints > 0) font16.drawAnchored(ctx, `${S('strchrstat')} ${state.statPoints}`, 96, 420);
      chrome.border(ctx, state?.act || 1, BORDER_LEFT);
      ctx.restore();
      return { x: v.ox, y: v.oy, scale: v.scale, buttons, statPoints: state?.statPoints ?? 0, skillPoints: state?.skillPoints ?? 0,
        shown: FIELDS.filter((f) => !/Label|1$|2$/.test(f.key)).map((f) => ({ key: f.key, value: text[f.key] ?? null })) };
    },
  };
}
