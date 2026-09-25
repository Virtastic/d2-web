// The client's draw list: the same records retail's renderer is handed, so a screen can be compared
// draw by draw with retail's own.
//
// Off unless `globalThis.__d2DrawList` is an array: then every recorded draw is pushed to it. Records
// use RETAIL's conventions, not the canvas's, so they line up with the hook's:
//   * `x, y` is the anchor retail passes to its cel draw. A DC6 frame lands with its left edge at
//     x + offsetX and its BOTTOM row at y + offsetY (inclusive): top = y + offsetY - height + 1.
//     Measured on retail's title frame: gameselectscreenEXP frame 0 is drawn at y=256 and row 0 of
//     the screen stays black; frame 8 (88 rows) at y=599 fills rows 512..599.
//   * `op` is the renderer entry retail uses: 'cel' (pfCelDraw, slot 33) or 'celcolor' (pfCelDrawColor,
//     slot 34, the font glyph path); `mode` is D2MOO's DrawMode (3 modulate/add, 4 burn, 5 normal).
//   * `hash` is FNV-1a over the frame's encoded DC6 bytes, the same bytes the hook hashes in memory,
//     so a record names the exact frame drawn, not just the file it came from.

export function fnv1a(bytes) {
  let h = 0x811c9dc5;
  for (let i = 0; i < bytes.length; i++) { h ^= bytes[i]; h = Math.imul(h, 0x01000193) >>> 0; }
  return h >>> 0;
}

const hashes = new WeakMap();
export function dc6FrameHash(frame) {
  if (!frame?.encoded) return 0;
  let h = hashes.get(frame);
  if (h === undefined) { h = fnv1a(frame.encoded); hashes.set(frame, h); }
  return h;
}

/** Normalise a retail path the way the vector writes it: lower case, no `data\`, no extension. */
export function drawPath(path) {
  return String(path).replace(/\//g, '\\').toLowerCase().replace(/^data\\/, '').replace(/\.dc6$/, '');
}

export function drawListActive() { return Array.isArray(globalThis.__d2DrawList); }

/**
 * Record that `frame` (frame `index` of the DC6 at `path`) was just drawn with its top-left corner
 * at (left, top) on the 800x600 screen.
 */
export function recordDc6(path, index, frame, left, top, { op = 'cel', mode = 5 } = {}) {
  const list = globalThis.__d2DrawList;
  if (!Array.isArray(list) || !frame) return;
  list.push({
    op, path: drawPath(path), frame: index,
    x: Math.round(left - frame.offsetX), y: Math.round(top + frame.height - 1 - frame.offsetY),
    mode, w: frame.width, h: frame.height, hash: dc6FrameHash(frame),
  });
}

/** Record a renderer primitive (retail's box / line / rect records): op, corners, colour, mode. */
export function recordPrim(op, x1, y1, x2, y2, color, mode) {
  const list = globalThis.__d2DrawList;
  if (!Array.isArray(list)) return;
  list.push({ op, path: `${x1},${y1},${x2},${y2}`, frame: color, x: x1, y: y1, mode, hash: 0 });
}

/** Parse a the retail captures (tools/drawlist.py) into the same record shape. */
export function parseDrawListVector(text) {
  const out = [];
  for (const line of String(text).split('\n')) {
    if (!line || line.startsWith('#')) continue;
    const [op, path, ...rest] = line.split(' ');
    const f = Object.fromEntries(rest.map((kv) => kv.split('=')));
    if (['box', 'boxalpha', 'line', 'rect', 'fillrect', 'diamond'].includes(op)) {
      // `box x1 y1 x2 y2 color=C mode=M`: identity is the rect and colour, like recordPrim.
      const [x1, y1, x2, y2] = [path, ...rest.slice(0, 3)].map(Number);
      const g = Object.fromEntries(rest.slice(3).map((kv) => kv.split('=')));
      out.push({ op, path: `${x1},${y1},${x2},${y2}`, frame: Number(g.color), x: x1, y: y1, mode: Number(g.mode), hash: 0 });
      continue;
    }
    if (op !== 'cel' && op !== 'celcolor') { out.push({ op, raw: line }); continue; }
    out.push({ op, path: drawPath(path), frame: Number(f.frame), x: Number(f.x), y: Number(f.y), mode: Number(f.mode),
      w: Number(f.w), h: Number(f.h), hash: parseInt(f.hash, 16) >>> 0 });
  }
  return out;
}

/**
 * Diff two draw lists record by record. Records are paired on identity (op, path, frame, frame-bytes
 * hash): first in call order with a longest-common-subsequence walk, then -- among what is left --
 * out of order (the same draw issued at a different point in the frame). Every retail record counts
 * at most once: unpaired on either side, or paired but out of order, at another anchor, or in another
 * draw mode.
 */
export function diffDrawLists(retail, ours) {
  const key = (r) => `${r.op} ${r.path} ${r.frame} ${r.hash ?? ''}`;
  const a = retail.map(key), b = ours.map(key);
  const n = a.length, m = b.length;
  const L = Array.from({ length: n + 1 }, () => new Uint16Array(m + 1));
  for (let i = n - 1; i >= 0; i--) for (let j = m - 1; j >= 0; j--) L[i][j] = a[i] === b[j] ? L[i + 1][j + 1] + 1 : Math.max(L[i + 1][j], L[i][j + 1]);
  const pairs = [];
  let missing = [], extra = [];
  let i = 0, j = 0;
  while (i < n && j < m) {
    if (a[i] === b[j]) { pairs.push([retail[i], ours[j], true]); i++; j++; }
    else if (L[i + 1][j] >= L[i][j + 1]) missing.push(retail[i++]);
    else extra.push(ours[j++]);
  }
  while (i < n) missing.push(retail[i++]);
  while (j < m) extra.push(ours[j++]);
  // Out-of-order pairs: the nearest-anchored unused draw of ours with the same identity.
  const left = [];
  for (const r of missing) {
    let best = -1;
    for (let k = 0; k < extra.length; k++) {
      if (!extra[k] || key(extra[k]) !== key(r)) continue;
      if (best < 0 || Math.abs(extra[k].x - r.x) + Math.abs(extra[k].y - r.y) < Math.abs(extra[best].x - r.x) + Math.abs(extra[best].y - r.y)) best = k;
    }
    if (best < 0) { left.push(r); continue; }
    pairs.push([r, extra[best], false]);
    extra[best] = null;
  }
  missing = left; extra = extra.filter(Boolean);
  const differing = [];
  for (const [r, o, inOrder] of pairs) {
    const d = [];
    if (!inOrder) d.push('out of call order');
    if (r.x !== o.x || r.y !== o.y) d.push(`at ${o.x},${o.y} not ${r.x},${r.y} (${o.x - r.x >= 0 ? '+' : ''}${o.x - r.x},${o.y - r.y >= 0 ? '+' : ''}${o.y - r.y})`);
    if (r.mode !== o.mode) d.push(`mode ${o.mode} not ${r.mode}`);
    if (d.length) differing.push({ draw: key(r), diff: d.join(', ') });
  }
  const identical = pairs.length - differing.length;
  return { paired: pairs.length, identical, missing, extra, differing, diffs: missing.length + extra.length + differing.length };
}
