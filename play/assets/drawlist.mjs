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

/** Normalise a retail path the way the vector writes it: lower case, no `data\`, no extension.
 *  Retail's hook logs a DCC cel (a character composite layer) with no extension either. */
export function drawPath(path) {
  return String(path).replace(/\//g, '\\').toLowerCase().replace(/^data\\/, '').replace(/\.(dc6|dcc)$/, '');
}

/** G emitter: one retail-format D2Gfx call line `<tick> G <ordinal> <ecx>,<edx>[,<arg>...]` (hex; values >= 0x100000 are pointers, masked `*`)
 *  into `globalThis.__d2G` when it is an array. The tick is `globalThis.__d2GTick`. Compared with the retail captures */
export function gCall(ord, ecx, edx, ...args) {
  const g = globalThis.__d2G;
  if (Array.isArray(g)) g.push(`${globalThis.__d2GTick ?? 0} G ${ord} ${[ecx, edx, ...args].map((v) => (v >>> 0 < 0x100000 ? (v >>> 0).toString(16) : '*')).join(',')}`);
}

/** G lines from the C++ side (`<ord> <ecx>,<edx>,...`, already retail-format), tick-prefixed here. */
export function gLines(text) {
  const g = globalThis.__d2G;
  if (Array.isArray(g)) for (const l of String(text).split('\n')) if (l) g.push(`${globalThis.__d2GTick ?? 0} G ${l}`);
}

/** G emitter for a unit draw (player/monster/object/missile/item sprite), at its place in the draw order: a retail G 10041 line's `u=<type>:<class>:<mode>`
 *  (hex) without the cel fields, which a unit's layers repeat. One line per unit, not per layer; compare with the retail trace collapsed the same way. */
/** Tick 1 of a G trace: the loading-screen walk of D2Client+0x6D010 (a retail recording-g records 0-105): frame 0, the player's 6C490 restore
 *  (D2Gfx 10083 arg 1), one D2Gfx 10043 (ecx/edx 1,1) per monster the client holds (`monsters`: 4 main / 14 holdout, = the tick's 0xAC assigns; NOT C1760
 *  skips), then frames 1..8 = the 8 C1760(1) calls of the walk's 9-room list (D2Client+0x6D010 gate: ebp&0x1f != 0, ebp from 0 per walk, so rooms 1..8; the second
 *  walk runs with edi=1 and D2Client+0x11C054 clear, so it calls none). `frame(ecx, edx, first)` is d2_client_loading_g; the entry ecx/edx of each frame are
 *  D2Common-thunk leftovers read off the retail trace (not modelled). */
export function gLoadingWalk(frame, monsters, pairs) {
  gLines(frame(0, 0, 1));
  gCall(10083, 0, 9, 1); for (let i = 0; i < monsters; i++) gCall(10043, 1, 1);
  for (const [e, d] of pairs) gLines(frame(e, d, 0));   // D2Client+0x6C3F0's register leftovers per room slot (d2_client_loading_pairs)
  gLines('10022 0,0,*\n10061 0,0,*\n10061 0,0,*');   // loading ends: palette reload (D2Gfx 10022, 10061 x2)
}

/** D2Client+0x44230's D2Gfx calls (`d2_client_frame_g`, src/core/d2client/frame_44230_g.cpp): phase 0 up to the room walk, phase 1 after it.
 *  The register leftovers (a D2Common thunk's ecx/edx) are not recoverable from the decompile and are the retail values: 8B110's 0x164c0,
 *  A8800's 0x22/0x1720, 14050's 8; the 2A9F0 10013 pair varies per tick and is left 0. */
export function gFrame(Module, phase, gateEcx = 0) {
  gLines(Module.ccall('d2_client_frame_g', 'string', Array(12).fill('number'), [phase, 1, 0, 1, 0xff, 0, 0, 0x164c0, 0x22, 0x1720, 8, gateEcx]));
}

export function gUnit(u, wx, wy) {
  const g = globalThis.__d2G;
  if (u && !globalThis.__d2GLoading && Array.isArray(g)) g.push(`${globalThis.__d2GTick ?? 0} G 10041 unit u=${u.map((v) => (v >>> 0).toString(16)).join(':')} at=${Math.round(wx)},${Math.round(wy)}`);
}

export function drawListActive() { return Array.isArray(globalThis.__d2DrawList); }

/**
 * Record that `frame` (frame `index` of the DC6 at `path`) was just drawn with its top-left corner
 * at (left, top) on the 800x600 screen.
 *
 * `extra` is retail's own per-draw argument (the decompile: the colour shift a
 * pfCelDrawColor carries -- 4 on the trademark screen's gold copyright, 0 on the menu's labels).
 * diffDrawLists does NOT compare it: it is recorded so a row can assert the colour shift for a
 * draw whose identity is otherwise identical (s198).
 */
export function recordDc6(path, index, frame, left, top, { op = 'cel', mode = 5, extra = 0 } = {}) {
  const list = globalThis.__d2DrawList;
  if (!frame) return null;
  const record = {
    op, path: drawPath(path), frame: index,
    x: Math.round(left - frame.offsetX), y: Math.round(top + frame.height - 1 - frame.offsetY),
    mode, extra, w: frame.width, h: frame.height, hash: dc6FrameHash(frame),
  };
  if (Array.isArray(list)) list.push(record);
  gCall(10041, 0, 0, 1 << 20, record.x, record.y, 1 << 20, mode, 0);   // DrawCelContext: cel*, x, y, palette*, mode, 0
  return record;
}

/**
 * Record a DCC cel draw (a character composite layer, the only DCC path the front end draws). Retail
 * takes the same (x, y) anchor for it as for a DC6 cel -- x + frame.xOffset is the first column,
 * y + frame.yOffset + 1 - frame.height the first row -- but the hook cannot hash a frame decoded from
 * a DCC bitstream, so retail's own records carry `hash=0`. The anchor is passed in rather than
 * re-derived from the frame: a DCC frame has no `offsetX`/`offsetY` and its `bottomUp` orientation
 * changes which of its own fields is the anchor.
 */
export function recordDcc(path, index, anchorX, anchorY, { op = 'cel', mode = 5 } = {}) {
  const list = globalThis.__d2DrawList;
  if (!Array.isArray(list)) return;
  list.push({ op, path: drawDccPath(path), frame: index, x: Math.round(anchorX), y: Math.round(anchorY), mode, hash: 0 });
}

/** A DCC cel's path the way the hook writes it (tools/drawlist.py `line()`): retail has no cell file
 *  for one, so its identity is the unit's own tokens -- `dcc:{kind}\{class}\{type}{class}{armour}
 *  {mode}{hitclass}` under the game's data root, e.g. `dcc:chars\BA\LG\BALGlitTNhth`. Our own path
 *  spells the same thing as `data\global\chars\BA\LG\BALGlitTNhth.dcc`, so the two differences are the
 *  `data\global\` root and the extension. */
export function drawDccPath(path) {
  return 'dcc:' + drawPath(path).replace(/^global\\/, '');
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
      extra: Number(f.extra ?? 0), w: Number(f.w), h: Number(f.h), hash: parseInt(f.hash, 16) >>> 0 });
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
