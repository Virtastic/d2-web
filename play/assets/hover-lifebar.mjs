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
