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
  // Retail puts the "+" beside the attribute frame; the stone right of it is empty in the art.
  const buttonRect = (slot) => ({ x: slot.x + slot.w + 4, y: slot.y + 3, w: 16, h: 16 });

  return {
    get layout() { return CHARACTER_LAYOUT; },

    /**
     * The stat id whose "+" is under a canvas point, or null. Shares buttonRect() with draw(), so a
     * click cannot land on a different row than the one the player sees, and returns null when no
     * points remain -- the button is not drawn then either.
     */
    buttonAt(px, py, width, height, state) {
      if (!(state?.statPoints > 0)) return null;
      const o = origin(width, height);
      const x = (px - o.x) / o.scale, y = (py - o.y) / o.scale;
      for (const [key] of ROWS) {
        const slot = CHARACTER_SLOTS[key];
        if (slot?.stat === undefined) continue;
        const r = buttonRect(slot);
        if (x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h) return slot.stat;
      }
      return null;
    },

    /** `state` is a d2_session_advancement read plus { name, className, life, mana, string(key) }. */
    draw(ctx, width, height, state, visible = true) {
      if (!visible) return null;
      const o = origin(width, height);
      ctx.save(); ctx.translate(o.x, o.y); ctx.scale(o.scale, o.scale);
      // Retail invchar6 direction 0: TL, TR, BL, BR.
      let x = 1, y = 4;
      ctx.drawImage(panel[0], x, y); x += panel[0].width;
      ctx.drawImage(panel[1], x, y); y += panel[1].height;
      ctx.drawImage(panel[3], x, y); x = 1;
      ctx.drawImage(panel[2], x, y);

      const buttons = [];
      const text = (str, slot, align) => {
        const t = String(str);
        const ty = slot.y + Math.round((slot.h - font.lineHeight) / 2);
        font.draw(ctx, t, align === 'right' ? slot.x + slot.w - INSET - font.measure(t) : slot.x + INSET, ty);
      };
      if (state?.name) text(state.name, CHARACTER_SLOTS.name, 'left');
      if (state?.className) text(state.className, CHARACTER_SLOTS.className, 'left');
      for (const [key, stringKey] of ROWS) {
        const slot = CHARACTER_SLOTS[key], value = state?.[key];
        // Retail's own word for the field; fall back to the key so a missing string is visible
        // rather than silently blank.
        const label = (state?.string?.(stringKey) || stringKey).split('\n')[0];
        text(label, slot, 'left');
        text(Number.isFinite(value) ? value : '-', slot, 'right');
        if (slot.stat !== undefined && state?.statPoints > 0) {
          const r = buttonRect(slot);
          ctx.save();
          ctx.fillStyle = 'rgba(150,120,50,.55)';
          ctx.fillRect(r.x, r.y, r.w, r.h);
          ctx.restore();
          font.draw(ctx, '+', r.x + 5, r.y + 1);
          buttons.push(slot.stat);
        }
      }
      // Below the Energy frame (which ends at y=313) and left of the resistance column (x=173),
      // so the remaining-points lines sit on bare stone instead of across the attribute frames.
      const points = state?.statPoints > 0 ? `STAT POINTS ${state.statPoints}` : '';
      if (points) font.draw(ctx, points, INSET + 4, 330);
      const skill = state?.skillPoints > 0 ? `SKILL POINTS ${state.skillPoints}` : '';
      if (skill) font.draw(ctx, skill, INSET + 4, 352);
      ctx.restore();

      return { x: o.x, y: o.y, scale: o.scale, buttons,
        statPoints: state?.statPoints ?? 0, skillPoints: state?.skillPoints ?? 0,
        shown: ROWS.map(([key]) => ({ key, value: state?.[key] ?? null })) };
    },
  };
}
