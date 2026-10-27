// 1.13c D2Client+0xC0590 (life 0..128) and D2Win+0x133A0 (plate boxes).
export function hoverLife(u) {
  if (!u || u.type !== 1 || (u.flags & 0x10000) || u.mode === 0 || u.mode === 0x0C) return 0;
  let life = ((u.hpPct | 0) * 128 / 100) | 0;
  if ((u.hpPct | 0) <= 0) life = u.maxHp ? ((u.hp | 0) << 7) / u.maxHp | 0 : 128;
  return Math.min(128, Math.max(2, life));
}
// Draw-rect records [x1,y1,x2,y2,colour,mode] for a plate box of width W.
export function plateBoxes(x1, y1, W, h, life) {
  const y2 = y1 + h;
  if (!life) return [[x1, y1, x1 + W, y2, 0, 2]];
  const lw = (life * W) >> 7, out = [[x1, y1, x1 + lw, y2, 5, 1]];
  if (W - lw) out.push([x1 + lw, y1, x1 + W, y2, 0, 2]);
  return out;
}

// D2Win 0x133A0's plate rectangles are D2Gfx D2FillArea (ord 10014) calls: the colour is a pal.dat
// INDEX and the mode picks the AlphaBlend bank. The canvas has no indexed surface here, so a rectangle is filled with the
// palette entry's RGB at the mode's alpha -- the same mapping play/assets/options-pages.mjs uses for
// retail boxes. Index 5 is RGB(92,0,0) and index 0 black in ACT1, Units and Sky alike
// (reference/riiablo palettes, decoded B,G,R).
const DRAW_MODE_ALPHA = [0.25, 0.5, 0.75];
export function platePaintOps(boxes, palette) {
  return boxes.map(([x1, y1, x2, y2, colour, mode]) => {
    const alpha = DRAW_MODE_ALPHA[mode] ?? 1;
    const r = palette ? palette[colour * 4] : 0, g = palette ? palette[colour * 4 + 1] : 0,
      b = palette ? palette[colour * 4 + 2] : 0;
    return { x: x1, y: y1, width: x2 - x1, height: y2 - y1, colour, mode, alpha,
      rgb: [r, g, b], style: `rgba(${r},${g},${b},${alpha})` };
  });
}

/**
 * Paint a plate's own rectangle list (D2Win 0x133A0's D2FillArea calls) and hand each one to the
 * draw-list recorder. BOTH plate families go through here -- the top-centre monster plate and the
 * above-NPC plate -- so the bar retail paints cannot be dropped at one site while the other records
 * it: the plate the player sees IS the life bar's fill, at every life value including none (life 0 =
 * the single dark box).
 */
export function paintHoverPlate(c, boxes, palette, recordPrim) {
  const painted = platePaintOps(boxes, palette);
  c.save();
  for (const p of painted) { c.fillStyle = p.style; c.fillRect(p.x, p.y, p.width, p.height); }
  c.restore();
  for (const p of painted) recordPrim('box', p.x, p.y, p.x + p.width, p.y + p.height, p.colour, p.mode);
  return painted;
}
// 1.13c D2Client+0x58FB0 (showitems.cpp) queues the ground-item name through D2Win's popup-text plate:
// the same layout as the NPC plate -- width+8 by 16, centred on x ((w+8)>>1 left of it), text 4px in
// with its bottom row 3px above the plate bottom. `bottom` is the plate's bottom edge.
export function groundLabelPlate(cx, bottom, textWidth) {
  const width = textWidth + 8, left = cx - (width >> 1);
  return { box: { x: left, y: bottom - 16, width, height: 16 }, textX: left + 4, anchorY: bottom - 3 };
}

// Hover-plate name colour, D2Client+0xC0850: flag 0x08 (unique) -> gold (4), then flag 0x04 (champion) -> blue (3)
// overrides it; a fixed list of monster classes is forced gold. Superunique (0x02) and minion (0x10) get no colour.
const GOLD_CLASSES = new Set([0xf3, 0x14d, 0xf2, 0xd3, 0x220, 0x23a, 0xe5, 0x2c4, 0x2c3, 0x2c2, 0x2c1, 0x2c5, 0x2c0]);
export function monsterNameColour(typeFlags, classId = -1) {
  const f = typeFlags | 0;
  return GOLD_CLASSES.has(classId) ? 4 : f & 4 ? 3 : f & 8 ? 4 : 0;
}

// 1.13c D2Client+0x58FB0 switch on D2Common ord 10106 (dwQualityNo 1 low,2 normal,3 superior,4 magic,5 set,6 rare,7 unique,8 crafted,9 tempered): 4->3, 5->2, 6->9, 7->4, 8->8, 9->10, else 0. Retail greys (5) only ethereal/socketed, not low quality; the snapshot carries no flags.
// set green 2, rare yellow 9, unique gold 4, crafted orange 8). Textcolour indices as textColourPalette takes.
export function groundLabelColour(quality, flags = 0) {
  return { 4: 3, 5: 2, 6: 9, 7: 4, 8: 8, 9: 10 }[quality] ?? ((flags & 0x400800) ? 5 : 0); // ethereal 0x400000 / socketed 0x800 -> grey
}

// 1.13c D2Client+0x58FB0 (showitems.cpp): the label under the cursor gets fill colour D2Win #10190(0,0x40,0x80)
// (a pal.dat index), draw mode 5 and white text (colour 0); every other label gets colour 0, mode 1.
// #10190 is the palette entry nearest the RGB (first on ties).
export const ALT_HOVER_RGB = Object.freeze([0, 0x40, 0x80]);
export const ALT_HOVER_MODE = 5;
export function nearestPaletteIndex(palette, r, g, b) {
  let best = 0, bestD = Infinity;
  for (let i = 0; i < 256; i++) {
    const d = (palette[i * 4] - r) ** 2 + (palette[i * 4 + 1] - g) ** 2 + (palette[i * 4 + 2] - b) ** 2;
    if (d < bestD) { bestD = d; best = i; }
  }
  return best;
}
export const altHoverBox = (box, palette) =>
  [[box.x, box.y, box.x + box.width, box.y + box.height, nearestPaletteIndex(palette, ...ALT_HOVER_RGB), ALT_HOVER_MODE]];
