// Write a plain (stored, version 0) MPQ from named files. This is the patcher's job: a Blizzard patch
// installer (1.14x) carries its files loose with a patch.lst of "<path in the MPQ>;<file in the installer>",
// and the patcher folds them into Patch_D2.mpq. We do the same fold in the browser so the engine reads a
// Patch_D2.mpq made from the files the player supplied.
import { hashString } from './mpq-index.mjs';

const CRYPT = (() => {
  const t = new Uint32Array(0x500); let s = 0x00100001;
  for (let a = 0; a < 0x100; a++) for (let i = 0, b = a; i < 5; i++, b += 0x100) {
    s = (s * 125 + 3) % 0x2AAAAB; const hi = (s & 0xFFFF) << 16;
    s = (s * 125 + 3) % 0x2AAAAB; t[b] = (hi | (s & 0xFFFF)) >>> 0;
  }
  return t;
})();
function encrypt(u, key) {
  let s1 = key >>> 0, s2 = 0xEEEEEEEE;
  for (let i = 0; i < u.length; i++) {
    s2 = (s2 + CRYPT[0x400 + (s1 & 0xFF)]) >>> 0;
    const p = u[i]; u[i] = (p ^ ((s1 + s2) >>> 0)) >>> 0;
    s1 = ((((~s1) << 0x15) + 0x11111111) | (s1 >>> 0x0B)) >>> 0;
    s2 = (p + s2 + (s2 << 5) + 3) >>> 0;
  }
}

/** files = [{name, data: Uint8Array}] (backslash paths); returns the archive bytes. */
export function buildMpq(files) {
  let n = 16; while (n < files.length * 2) n <<= 1;
  const hash = new Uint32Array(n * 4).fill(0xFFFFFFFF), block = new Uint32Array(files.length * 4);
  let pos = 32; const at = [];
  files.forEach((f, i) => {
    at.push(pos); block.set([pos, f.data.length, f.data.length, 0x80000000], i * 4); pos += f.data.length;
    let h = hashString(f.name, 0) & (n - 1);
    while (hash[h * 4 + 3] !== 0xFFFFFFFF) h = (h + 1) & (n - 1);
    hash.set([hashString(f.name, 1), hashString(f.name, 2), 0, i], h * 4);
  });
  const hpos = pos, bpos = hpos + n * 16;
  encrypt(hash, hashString('(hash table)', 3)); encrypt(block, hashString('(block table)', 3));
  const out = new Uint8Array(bpos + block.length * 4), dv = new DataView(out.buffer);
  dv.setUint32(0, 0x1a51504d, true); dv.setUint32(4, 32, true); dv.setUint32(8, out.length, true);
  dv.setUint16(12, 0, true); dv.setUint16(14, 3, true);
  dv.setUint32(16, hpos, true); dv.setUint32(20, bpos, true); dv.setUint32(24, n, true); dv.setUint32(28, files.length, true);
  files.forEach((f, i) => out.set(f.data, at[i]));
  out.set(new Uint8Array(hash.buffer), hpos); out.set(new Uint8Array(block.buffer), bpos);
  return out;
}
