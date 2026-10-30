// 1.13c D2Launch delete-confirm layout: a D2Win text box (type 4) at x 268, width 264, height 120,
// margins (10, 8), Font16, line pitch tbl height + 4, first line anchored at 218. D2Win wraps at 244 px:
// at each character whose prefix width reaches the limit the line breaks at the last space, and the
// remainder loses its leading space. Each line is centred in the 264 px box.
export const DELETE_LAYOUT = { box: { x: 268, top: 175 }, text: { x: 268, w: 264, h: 120, marginX: 10, marginY: 8, anchor: 218, leading: 4 },
  no: { x: 281, top: 306 }, yes: { x: 421, top: 306 } };

export function wrapPixels(text, measure, avail = DELETE_LAYOUT.text.w - 2 * DELETE_LAYOUT.text.marginX) {
  const lines = []; let rest = String(text);
  for (;;) {
    if (measure(rest) < avail) { lines.push(rest); return lines; }
    let cut = -1, lastSpace = -1;
    for (let i = 0; i < rest.length; i++) {
      if (rest[i] === ' ') lastSpace = i;
      if (measure(rest.slice(0, i + 1)) >= avail) { cut = lastSpace >= 0 ? lastSpace : i; break; }
    }
    if (cut < 0) { lines.push(rest); return lines; }
    lines.push(rest.slice(0, cut));
    rest = rest.slice(cut).replace(/^ /, '');
    if (!rest) return lines;
  }
}

export function layoutLines(text, measure, fontHeight) {
  const t = DELETE_LAYOUT.text, pitch = fontHeight + t.leading, maxLines = Math.floor((t.h - 2 * t.marginY) / pitch);
  return wrapPixels(text, measure).slice(0, maxLines).map((line, n) =>
    ({ text: line, x: t.x + Math.max(Math.trunc((t.w - measure(line)) / 2), 0), anchor: t.anchor + n * pitch }));
}
