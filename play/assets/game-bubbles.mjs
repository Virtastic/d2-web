// Overhead speech bubbles: the text a player says with "!" (or an NPC says) floats over that unit.
// Layout is the client's own: the text wraps at 180 px in the in-game chat font (font 13, the one the
// Enter bar uses), the box is the widest line + 20 wide and 15 per line + 6 high, centred over the unit,
// 50 above the unit's anchor (minus the unit's own offset: 30 for a player, 10 for any other unit) and
// the lines' height; the box is a mode-1 alpha rectangle exactly like the chat bar's. At most the last
// ten lines show. One message per unit: a new one replaces the old.
import { decodeDatPalette, decodeDc6 } from './d2-formats.mjs';
import { createFont, decodeFontTbl } from './frontend.mjs';
import { DIM_ALPHA } from './quest-log.mjs';

export const BUBBLE = { wrap: 180, pad: 0x14, lineHeight: 15, extra: 6, lift: 0x32, base: 3, textX: 10, textY: 0x12, mostLines: 10 };
export const UNIT_LIFT = { player: 0x1e, other: 10 };
export const BUBBLE_MS = 5000;   // capture: the message lifetime is set inside the common library (125 frames assumed)

/** Greedy word wrap to `width` px; a word wider than the line is cut where it no longer fits. */
export function wrapBubble(text, measure, width = BUBBLE.wrap) {
  const lines = []; let line = '';
  for (const word of String(text).split(/\s+/).filter(Boolean)) {
    let w = word;
    while (measure(w) > width && w.length > 1) {
      let n = w.length - 1; while (n > 1 && measure((line ? line + ' ' : '') + w.slice(0, n)) > width) n--;
      if (!line || measure(line + ' ' + w.slice(0, n)) <= width) { lines.push((line ? line + ' ' : '') + w.slice(0, n)); line = ''; w = w.slice(n); }
      else { lines.push(line); line = ''; }
    }
    if (!line) line = w; else if (measure(line + ' ' + w) <= width) line += ' ' + w; else { lines.push(line); line = w; }
  }
  if (line) lines.push(line);
  return lines;
}

/** The box and line positions for one bubble over a unit at screen (x, y). Null when it is off screen. */
export function bubbleLayout(lines, measure, x, y, screenW, screenH, isPlayer) {
  const n = lines.length; if (!n) return null;
  const w = Math.max(...lines.map(measure)) + BUBBLE.pad, h = n * BUBBLE.lineHeight + BUBBLE.extra;
  const anchorY = y - (isPlayer ? UNIT_LIFT.player : UNIT_LIFT.other);
  if (!(anchorY > 0 && anchorY < screenH && x > -100 && x < screenW + 100)) return null;
  const left = x - (w >> 1), top = anchorY - BUBBLE.lift - n * BUBBLE.lineHeight - BUBBLE.base;
  const shown = Math.min(n, BUBBLE.mostLines);
  if (top < 0 || top + h >= screenH) return null;
  return { left, top, w, h, box: [left, top + 4, left + w, top + 4 + h - 5],
    lines: lines.slice(0, shown).map((text, i) => ({ text, x: left + BUBBLE.textX, y: top + (i - shown + n) * BUBBLE.lineHeight + BUBBLE.textY })) };
}

/** Drop bubbles older than their lifetime. */
export function liveBubbles(map, now) {
  for (const [key, b] of map) if (now - b.at > BUBBLE_MS) map.delete(key);
  return map;
}

export function createBubbleRenderer(read, documentObject = globalThis.document) {
  const fonts = new Map();
  const fontFor = (act) => {
    const a = Math.min(5, Math.max(1, act | 0));
    if (!fonts.has(a)) {
      const base = 'data\\local\\font\\latin\\fontingamechat';
      fonts.set(a, createFont({ dc6: decodeDc6(read(base + '.dc6')), tbl: decodeFontTbl(read(base + '.tbl')),
        palette: decodeDatPalette(read(`data\\global\\palette\\ACT${a}\\pal.dat`)), documentObject, path: base + '.dc6', drawAs: { op: 'celcolor', mode: 5, extra: 0 } }));
    }
    return fonts.get(a);
  };
  return {
    /** `units`: [{ key: "type:guid", x, y, player }] screen points (canvas px); `bubbles`: the shell's Map. */
    draw(ctx, w, h, units, bubbles, now, act = 1) {
      liveBubbles(bubbles, now);
      const f = fontFor(act), drawn = [];
      for (const u of units) {
        const b = bubbles.get(u.key); if (!b) continue;
        const L = bubbleLayout(wrapBubble(b.text, (s) => f.measure(s)), (s) => f.measure(s), u.x, u.y, w, h, u.player);
        if (!L) continue;
        ctx.fillStyle = `rgba(0,0,0,${DIM_ALPHA})`; ctx.fillRect(L.box[0], L.box[1], L.box[2] - L.box[0], L.box[3] - L.box[1]);
        for (const l of L.lines) f.drawAnchored(ctx, l.text, l.x, l.y);
        drawn.push({ key: u.key, box: L.box, lines: L.lines.map((l) => l.text) });
      }
      return drawn;
    },
  };
}
