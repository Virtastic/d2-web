// Hireling screen text rows in 800x600 panel space (origin = panel top-left, retail font16 bitmap text).
// Pure layout so the shell only blits; the frame is invchar6 via the inventory renderer.
export const HIRELING_TEXT = Object.freeze({ x: 24, title: 18, first: 58, step: 72 });
export function hirelingTextRows(rows) {
  const out = [{ text: rows.length ? 'Hireling' : 'No hireling', x: HIRELING_TEXT.x, y: HIRELING_TEXT.title }];
  if (!rows.length) return out;
  rows.forEach((r, i) => {
    const y = HIRELING_TEXT.first + i * HIRELING_TEXT.step;
    out.push({ text: r.name, x: HIRELING_TEXT.x, y }, { text: `Level ${r.level}`, x: HIRELING_TEXT.x, y: y + 22 },
      { text: `${r.life} / ${r.maxLife}`, x: HIRELING_TEXT.x, y: y + 44 });
  });
  return out;
}
