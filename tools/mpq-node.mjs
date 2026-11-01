// Minimal read-only MPQ v1/v2 reader for the front-end art extractor.
// zlib + PKWARE implode only (what D2's DC6/DAT/TBL use); other compressions throw.
import fs from 'node:fs';
import zlib from 'node:zlib';

const T = new Uint32Array(0x500);
{ let seed = 0x100001;
  for (let i = 0; i < 0x100; i++) for (let j = i, k = 0; k < 5; k++, j += 0x100) {
    seed = (seed * 125 + 3) % 0x2AAAAB; const a = (seed & 0xFFFF) << 16;
    seed = (seed * 125 + 3) % 0x2AAAAB; T[j] = (a | (seed & 0xFFFF)) >>> 0;
  } }
export const hash = (s, type) => { let a = 0x7FED7FED, b = 0xEEEEEEEE;
  for (const ch of s.toUpperCase().replace(/\//g, '\\')) { const c = ch.charCodeAt(0);
    a = (T[type * 0x100 + c] ^ (a + b)) >>> 0; b = (c + a + b + (b << 5) + 3) >>> 0; }
  return a; };
const decrypt = (u32, key) => { let seed = 0xEEEEEEEE;
  for (let i = 0; i < u32.length; i++) { seed = (seed + T[0x400 + (key & 0xFF)]) >>> 0;
    const v = (u32[i] ^ (key + seed)) >>> 0; u32[i] = v;
    key = ((((~key) << 21) + 0x11111111) | (key >>> 11)) >>> 0; seed = (v + seed + (seed << 5) + 3) >>> 0; } };

// PKWARE DCL explode (port of zlib's blast.c).
const LIT = [11,124,8,7,28,7,188,13,76,4,10,8,12,10,12,10,8,23,8,9,7,6,7,8,7,6,55,8,23,24,12,11,7,9,11,12,6,7,22,5,7,24,6,11,9,6,7,22,7,11,38,7,9,8,25,11,8,11,9,12,8,12,5,38,5,38,5,11,7,5,6,21,6,10,53,8,7,24,10,27,44,253,253,253,252,252,252,13,12,45,12,45,12,61,12,45,44,173];
const LEN = [2,35,36,53,38,23], DIST = [2,20,53,230,247,151,248];
const BASE = [3,2,4,5,6,7,8,9,10,12,16,24,40,72,136,264], EXTRA = [0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8];
function construct(rep) { const lens = []; for (const r of rep) { const l = (r & 15), n = (r >> 4) + 1; for (let i = 0; i < n; i++) lens.push(l); }
  const count = new Array(17).fill(0); for (const l of lens) if (l) count[l]++;
  const offs = [0, 0]; for (let l = 1; l < 16; l++) offs[l + 1] = offs[l] + count[l];
  const sym = []; lens.forEach((l, s) => { if (l) sym[offs[l]++] = s; }); return { count, sym }; }
const hLit = construct(LIT), hLen = construct(LEN), hDist = construct(DIST);
export function explode(src, outSize) {
  let p = 0, bb = 0, bc = 0; const out = Buffer.alloc(outSize); let o = 0;
  const bits = (n) => { let v = bb; while (bc < n) { v |= src[p++] << bc; bc += 8; } bb = v >> n; bc -= n; return v & ((1 << n) - 1); };
  const dec = (h) => { let code = 0, first = 0, idx = 0;
    for (let len = 1; len <= 15; len++) { code |= bits(1) ^ 1; const c = h.count[len];
      if (code < first + c) return h.sym[idx + code - first]; idx += c; first = (first + c) << 1; code <<= 1; } throw new Error('bad code'); };
  const lit = bits(8), dict = bits(8); if (lit > 1 || dict < 4 || dict > 6) throw new Error('bad pk header');
  for (;;) {
    if (bits(1)) { let s = dec(hLen); const len = BASE[s] + bits(EXTRA[s]); if (len === 519) break;
      s = dec(hDist); const sh = len === 2 ? 2 : dict, d = ((s << sh) | bits(sh)) + 1;
      for (let i = 0; i < len; i++) { out[o] = out[o - d]; o++; }
    } else out[o++] = lit ? dec(hLit) : bits(8);
    if (o >= outSize) break;
  }
  return out;
}
const unpack = (buf, size, imp) => { if (buf.length >= size) return buf;
  if (imp) return explode(buf, size);
  const m = buf[0]; const d = buf.subarray(1);
  if (m === 2) return zlib.inflateSync(d);
  if (m === 8) return explode(d, size);
  throw new Error('compression 0x' + m.toString(16)); };

export function openMpq(file) {
  const f = fs.readFileSync(file); let hdr = 0;
  while (f.readUInt32LE(hdr) !== 0x1A51504D) hdr += 512;
  const ss = 512 << f.readUInt16LE(hdr + 14);
  const ht = f.readUInt32LE(hdr + 16) + hdr, bt = f.readUInt32LE(hdr + 20) + hdr, hn = f.readUInt32LE(hdr + 24), bn = f.readUInt32LE(hdr + 28);
  const H = new Uint32Array(f.buffer.slice(f.byteOffset + ht, f.byteOffset + ht + hn * 16)); decrypt(H, hash('(hash table)', 3));
  const B = new Uint32Array(f.buffer.slice(f.byteOffset + bt, f.byteOffset + bt + bn * 16)); decrypt(B, hash('(block table)', 3));
  return { read(name) {
    const a = hash(name, 1), b = hash(name, 2);
    for (let i = hash(name, 0) & (hn - 1), n = 0; n < hn; n++, i = (i + 1) & (hn - 1)) {
      if (H[i * 4 + 3] === 0xFFFFFFFF) return null;
      if (H[i * 4] !== a || H[i * 4 + 1] !== b || H[i * 4 + 3] >= 0xFFFFFFFE) continue;
      const k = H[i * 4 + 3] * 4, off = B[k] + hdr, csize = B[k + 1], size = B[k + 2], fl = B[k + 3];
      if (fl & 0x10000) throw new Error('encrypted ' + name);
      const imp = !(fl & 0x200) && (fl & 0x100);
      if (!(fl & 0x200) && !imp) return f.subarray(off, off + size);
      if (fl & 0x1000000) return unpack(f.subarray(off, off + csize), size, imp); // single unit
      const ns = Math.ceil(size / ss), out = [];
      for (let s = 0; s < ns; s++) { const s0 = f.readUInt32LE(off + s * 4), s1 = f.readUInt32LE(off + s * 4 + 4);
        const want = Math.min(ss, size - s * ss), chunk = f.subarray(off + s0, off + s1);
        out.push(chunk.length === want ? chunk : unpack(chunk, want, imp)); }
      return Buffer.concat(out);
    }
    return null; } };
}
