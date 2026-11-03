// D2Client 1.13c FUN_6FAD7590 / FUN_6FAD8050; F227.
// These pins are client UI state. Resource values always come from the engine snapshot.
export function orbAt(width, height, x, y) {
  const scale = Math.min(1, width / 800);
  if (y < height - 75 * scale || y > height - 15 * scale) return null;
  if (x >= 30 * scale && x <= 110 * scale) return 'life';
  if (x >= width - 111 * scale && x <= width - 31 * scale) return 'mana';
  return null;
}

// The finger's aim at an orb (M18): a point off both rings, within `reach` canvas px of one, presses that
// ring's centre (the nearest ring wins). A point already on a ring is delivered raw, and so is one inside
// an open panel (`panels`: {left, top, width, height} rectangles): the mana ring reaches into an open
// inventory's right edge, and the panel's own hit test decides there. Returns [x, y] or null.
export function orbAimPoint(width, height, x, y, reach, panels = []) {
  if (orbAt(width, height, x, y)) return null;
  if (panels.some((p) => x >= p.left && x < p.left + (p.width ?? p.w) && y >= p.top && y < p.top + (p.height ?? p.h))) return null;
  const s = Math.min(1, width / 800), top = height - 75 * s, bottom = height - 15 * s;
  let best = null;
  for (const [left, right] of [[30 * s, 110 * s], [width - 111 * s, width - 31 * s]]) {
    const d = Math.hypot(Math.max(left - x, 0, x - right), Math.max(top - y, 0, y - bottom));
    if (d <= reach && (!best || d < best.d)) best = { d, point: [(left + right) / 2, (top + bottom) / 2] };
  }
  return best ? best.point : null;
}

export function drawOrbReadouts(ctx, font, snapshot, width, height, pointer, pins, line) {
  const hovered = orbAt(width, height, pointer.x, pointer.y), drawn = [];
  const scale = Math.min(1, width / 800), player = snapshot.player;
  ctx.save(); ctx.scale(scale, scale);
  try {
    for (const [orb, index, value, maximum] of [
      ['life', 4165, player.hp, player.hpMax], ['mana', 4166, player.mana, player.manaMax],
    ]) {
      if (!pins[orb] && hovered !== orb) continue;
      let current = value >> 8;
      if (orb === 'life' && snapshot.mode !== 17 && current >= 0 && current < 2) current = 1;
      const values = [current, maximum >> 8];
      const text = line(index).replace(/%d/g, () => String(values.shift()));
      const x = (orb === 'life' ? 65 : width / scale - 80) - Math.trunc(font.measure(text) / 2);
      const y = height / scale - 95, glyphs = [];
      font.drawAnchored(ctx, text, x, y, glyph => glyphs.push(glyph));
      drawn.push({ orb, index, text, x: x * scale, y: y * scale, pinned: Boolean(pins[orb]), hovered: hovered === orb, glyphs });
    }
  } finally { ctx.restore(); }
  return drawn;
}
