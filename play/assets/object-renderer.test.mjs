// The Act 4 red portal (class 566, token pp) lists only TR in Objects.txt but its COF carries the TR base and
// the HD swirl; the draw is COF-driven, so the swirl must reach the draw list with its additive blend.
import assert from 'node:assert/strict';
import { existsSync, openSync, readSync } from 'node:fs';
import { fileURLToPath } from 'node:url';
import { hashString, decrypt, mpqTables } from './mpq-index.mjs';
import { readMpqFile } from './mpq-read.mjs';
import { createObjectRenderer } from './object-renderer.mjs';
import { openAsBlob } from 'node:fs';

const archive = fileURLToPath(new URL('../../retail/D2/d2data.mpq', import.meta.url));
if (!existsSync(archive)) { console.log('SKIP object-renderer: retail archive absent'); process.exit(0); }
const fd = openSync(archive, 'r'), blob = await openAsBlob(archive);
const at = (p, l) => { const b = Buffer.alloc(l); readSync(fd, b, 0, l, p); return new Uint8Array(b); };
const head = at(0, 32), [[hp, hn], [bp, bn]] = mpqTables(head), shift = new DataView(head.buffer).getUint16(14, true);
const table = (pos, n, name) => { const u = new Uint32Array(n * 4); new Uint8Array(u.buffer).set(at(pos, n * 16)); decrypt(u, hashString(name, 3)); return u; };
const hash = table(hp, hn, '(hash table)'), block = table(bp, bn, '(block table)');
const files = new Map();
const preload = async (name) => {
  const a = hashString(name, 1), b = hashString(name, 2);
  for (let i = hashString(name, 0) & (hn - 1), n = 0; n < hn; i = (i + 1) & (hn - 1), n++) {
    const bi = hash[i * 4 + 3]; if (bi === 0xFFFFFFFF) return;
    if (bi < bn && hash[i * 4] === a && hash[i * 4 + 1] === b) {
      const entry = { pos: block[bi * 4], csize: block[bi * 4 + 1], size: block[bi * 4 + 2], flags: block[bi * 4 + 3] };
      const f = await readMpqFile(blob, 0, shift, entry, name); if (f) files.set(name.toLowerCase(), new Uint8Array(await f.arrayBuffer())); return;
    }
  }
};
for (const n of ['data\\global\\palette\\ACT4\\pal.dat', 'data\\global\\palette\\ACT4\\pal.pl2', 'data\\global\\excel\\objects.bin'])
  await preload(n);
for (const m of ['on']) for (const [c, w] of [['cof', ''], ['tr', 'tr'], ['hd', 'hd']])
  await preload(c === 'cof' ? `data\\global\\objects\\pp\\cof\\pp${m}hth.cof` : `data\\global\\objects\\pp\\${w}\\pp${w}lit${m}hth.dcc`);
const read = (p) => { const f = files.get(p.toLowerCase()); if (!f) throw new Error('missing ' + p); return f; };

globalThis.ImageData ??= class { constructor(data, width, height) { this.data = data; this.width = width; this.height = height; } };
const canvas = () => ({ width: 800, height: 600, getContext: () => ctx });
const ctx = { canvas: { width: 800, height: 600 }, drawImage() {}, getImageData: (x, y, w, h) => ({ data: new Uint8ClampedArray(w * h * 4) }), putImageData() {} };
const documentObject = { createElement: () => canvas() };

const draw = (componentMask) => {
  const renderer = createObjectRenderer(read, documentObject);
  globalThis.__d2DrawList = [];
  const evidence = renderer.draw(ctx, 400, 300, { id: 1, classId: 566, token: 'pp', mode: 2, direction: 0, componentMask, framePrecise: 0 }, 4);
  const list = globalThis.__d2DrawList; globalThis.__d2DrawList = null;
  return { evidence, list };
};
const open = draw(0xFFFF);
assert.deepEqual(open.list.map((d) => [d.path, d.mode]).sort(),
  [['objects\\pp\\hd\\pphdlitonhth', 3], ['objects\\pp\\tr\\pptrlitonhth', 5]]);
// Negative control: the TR-only mask the table row implies draws the opaque base alone.
assert.deepEqual(draw(0x2).list.map((d) => [d.path, d.mode]), [['objects\\pp\\tr\\pptrlitonhth', 5]]);
console.log('PASS object-renderer red portal');
