// Stage-time: writes the BYO front door's art (play/assets/door-art-build.mjs) as PNGs and a meta.json, so
// the door looks like the game before the player has given it any files. Output goes to the staged site only
// (build-wasm/site/door-art/), never into git.
//   node tools/door-art.mjs <dir holding patch_d2.mpq, d2exp.mpq, d2data.mpq> <out dir>
import fs from 'node:fs';
import path from 'node:path';
import zlib from 'node:zlib';
import { buildDoorArt } from '../play/assets/door-art-build.mjs';
import { openMpq } from './mpq-node.mjs';

const [src, out] = process.argv.slice(2);
if (!src || !out) { console.error('usage: node tools/door-art.mjs <mpq dir> <out dir>'); process.exit(2); }
const listing = fs.readdirSync(src);
const find = (n) => { const f = listing.find((x) => x.toLowerCase() === n); if (!f) throw new Error(`no ${n} in ${src}`); return path.join(src, f); };
const mpqs = ['patch_d2.mpq', 'd2exp.mpq', 'd2data.mpq'].map((n) => openMpq(find(n)));
const rd = (n) => { for (const m of mpqs) { const b = m.read(n); if (b) return b; } throw new Error('missing ' + n); };

// A minimal RGBA PNG writer.
const crcT = Int32Array.from({ length: 256 }, (_, n) => { let c = n; for (let k = 0; k < 8; k++) c = c & 1 ? 0xEDB88320 ^ (c >>> 1) : c >>> 1; return c; });
const crc = (b) => { let c = -1; for (const x of b) c = crcT[(c ^ x) & 255] ^ (c >>> 8); return (c ^ -1) >>> 0; };
const chunk = (t, d) => { const l = Buffer.alloc(4); l.writeUInt32BE(d.length); const td = Buffer.concat([Buffer.from(t), d]); const c = Buffer.alloc(4); c.writeUInt32BE(crc(td)); return Buffer.concat([l, td, c]); };
function png({ w, h, px }) {
  const raw = Buffer.alloc((w * 4 + 1) * h);
  for (let y = 0; y < h; y++) Buffer.from(px.buffer, y * w * 4, w * 4).copy(raw, y * (w * 4 + 1) + 1);
  const ih = Buffer.alloc(13); ih.writeUInt32BE(w, 0); ih.writeUInt32BE(h, 4); ih[8] = 8; ih[9] = 6;
  return Buffer.concat([Buffer.from([137, 80, 78, 71, 13, 10, 26, 10]), chunk('IHDR', ih), chunk('IDAT', zlib.deflateSync(raw, { level: 9 })), chunk('IEND', Buffer.alloc(0))]);
}

const { meta, images } = await buildDoorArt(rd);
fs.mkdirSync(out, { recursive: true });
for (const [n, s] of Object.entries(images)) fs.writeFileSync(path.join(out, n + '.png'), png(s));
fs.writeFileSync(path.join(out, 'meta.json'), JSON.stringify(meta));
console.log(`door-art: ${Object.keys(images).length} images -> ${out}`);
