// Minimal MPQ v1 writer, independent of StormLib. Uncompressed, unencrypted files; encrypted
// hash/block tables per the format. Exists so the wasm MPQ reader is proven against an archive
// WE constructed before any retail data is around (M0a), same trick as openmw-web's
// build-assetpack.py --selftest. Also handy for test fixtures later.
//
//   node tools/mkmpq.mjs out.mpq name1=file1 name2=file2 ...      (CLI)
//   import { buildMpq, hashString } from './mkmpq.mjs'            (library)
import { readFileSync, writeFileSync } from 'node:fs';

// --- StormLib-compatible crypt table + hashes (all u32 arithmetic) --------------------------
export const CRYPT = new Uint32Array(0x500);
{
  let seed = 0x00100001;
  for (let i1 = 0; i1 < 0x100; i1++) {
    for (let i = 0, i2 = i1; i < 5; i++, i2 += 0x100) {
      seed = (seed * 125 + 3) % 0x2AAAAB; const t1 = (seed & 0xFFFF) << 16;
      seed = (seed * 125 + 3) % 0x2AAAAB; const t2 = seed & 0xFFFF;
      CRYPT[i2] = (t1 | t2) >>> 0;
    }
  }
}
// type: 0 = table index, 1 = name A, 2 = name B, 3 = file key
export function hashString(s, type) {
  let s1 = 0x7FED7FED, s2 = 0xEEEEEEEE;
  for (const ch of s.toUpperCase().replace(/\//g, '\\')) {
    const c = ch.charCodeAt(0);
    s1 = (CRYPT[(type << 8) + c] ^ ((s1 + s2) >>> 0)) >>> 0;
    s2 = (c + s1 + s2 + (s2 << 5) + 3) >>> 0;
  }
  return s1;
}
function encrypt(u32, key) {
  let s1 = key >>> 0, s2 = 0xEEEEEEEE;
  for (let i = 0; i < u32.length; i++) {
    s2 = (s2 + CRYPT[0x400 + (s1 & 0xFF)]) >>> 0;
    const plain = u32[i];
    u32[i] = (plain ^ ((s1 + s2) >>> 0)) >>> 0;
    s1 = ((((~s1) << 0x15) + 0x11111111) | (s1 >>> 0x0B)) >>> 0;
    s2 = (plain + s2 + (s2 << 5) + 3) >>> 0;   // PLAINTEXT feeds the key chain (decrypt uses the same value, post-decrypt)
  }
}

// files: [{ name, data: Uint8Array }] -> Uint8Array of the whole archive
export function buildMpq(files) {
  let hashSize = 4; while (hashSize < files.length * 2) hashSize <<= 1;
  const HDR = 32, SECTOR_SHIFT = 3;
  const hash = new Uint32Array(hashSize * 4).fill(0xFFFFFFFF);
  const block = new Uint32Array(files.length * 4);
  let pos = HDR;
  const chunks = [];
  files.forEach((f, bi) => {
    const nameA = hashString(f.name, 1), nameB = hashString(f.name, 2);
    let hi = hashString(f.name, 0) & (hashSize - 1);
    while (hash[hi * 4 + 3] !== 0xFFFFFFFF) hi = (hi + 1) & (hashSize - 1);
    hash.set([nameA, nameB, 0 /* locale | platform */, bi], hi * 4);
    block.set([pos, f.data.length, f.data.length, 0x80000000 /* MPQ_FILE_EXISTS */], bi * 4);
    chunks.push(f.data); pos += f.data.length;
  });
  const hashPos = pos, blockPos = hashPos + hash.byteLength, total = blockPos + block.byteLength;
  encrypt(hash, hashString('(hash table)', 3));
  encrypt(block, hashString('(block table)', 3));
  const out = new Uint8Array(total), dv = new DataView(out.buffer);
  out.set([0x4D, 0x50, 0x51, 0x1A], 0);            // 'MPQ\x1A'
  dv.setUint32(4, HDR, true); dv.setUint32(8, total, true);
  dv.setUint16(12, 0, true); dv.setUint16(14, SECTOR_SHIFT, true);
  dv.setUint32(16, hashPos, true); dv.setUint32(20, blockPos, true);
  dv.setUint32(24, hashSize, true); dv.setUint32(28, files.length, true);
  let p = HDR; for (const c of chunks) { out.set(c, p); p += c.length; }
  out.set(new Uint8Array(hash.buffer), hashPos);
  out.set(new Uint8Array(block.buffer), blockPos);
  return out;
}

export function fnv1a(bytes) {
  let h = 0x811C9DC5;
  for (const b of bytes) { h ^= b; h = Math.imul(h, 0x01000193) >>> 0; }
  return h.toString(16).padStart(8, '0');
}

if (process.argv[1] && import.meta.url.endsWith(process.argv[1].replace(/\\/g, '/').split('/').pop())) {
  const [out, ...specs] = process.argv.slice(2);
  if (!out || !specs.length) { console.error('usage: mkmpq.mjs out.mpq name=file ...'); process.exit(2); }
  const files = specs.map((s) => { const [name, path] = s.split('='); return { name, data: readFileSync(path) }; });
  writeFileSync(out, buildMpq(files));
  console.log(`${out}: ${files.length} file(s)`);
}
