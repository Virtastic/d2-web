// MPQ v0 hash/block table lookup: where (file offset, packed size) a named file sits in an archive.
// Read-only and side-effect free; the streaming prefetcher uses it to know which byte ranges a file
// will need BEFORE the engine reads it. readAt(pos, len) -> Uint8Array supplies the archive bytes.
const CRYPT = new Uint32Array(0x500);
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
export function hashString(s, type) {
  let s1 = 0x7FED7FED, s2 = 0xEEEEEEEE;
  for (let i = 0; i < s.length; i++) {
    let c = s.charCodeAt(i); if (c === 47) c = 92; else if (c >= 97 && c <= 122) c -= 32;
    s1 = (CRYPT[(type << 8) + c] ^ ((s1 + s2) >>> 0)) >>> 0;
    s2 = (c + s1 + s2 + (s2 << 5) + 3) >>> 0;
  }
  return s1;
}
function decrypt(u32, key) {
  let s1 = key >>> 0, s2 = 0xEEEEEEEE;
  for (let i = 0; i < u32.length; i++) {
    s2 = (s2 + CRYPT[0x400 + (s1 & 0xFF)]) >>> 0;
    const plain = (u32[i] ^ ((s1 + s2) >>> 0)) >>> 0;
    u32[i] = plain;
    s1 = ((((~s1) << 0x15) + 0x11111111) | (s1 >>> 0x0B)) >>> 0;
    s2 = (plain + s2 + (s2 << 5) + 3) >>> 0;
  }
}
/** From the 32-byte header: [[hashPos, hashN], [blockPos, blockN]], or null when it is not a plain v0 MPQ. */
export function mpqTables(h) {
  const hv = new DataView(h.buffer, h.byteOffset, 32);
  return hv.getUint32(0, true) === 0x1a51504d ? [[hv.getUint32(16, true), hv.getUint32(24, true)], [hv.getUint32(20, true), hv.getUint32(28, true)]] : null;
}
/** Returns lookup(name) -> {pos, size} | null, or null when the archive is not a plain v0 MPQ. */
export function openMpqIndex(readAt) {
  const t = mpqTables(readAt(0, 32)); if (!t) return null;
  const [[hashPos, hashN], [blockPos, blockN]] = t;
  const tab = (pos, n, name) => { const b = readAt(pos, n * 16), u = new Uint32Array(n * 4); new Uint8Array(u.buffer).set(b.subarray(0, n * 16)); decrypt(u, hashString(name, 3)); return u; };
  const hash = tab(hashPos, hashN, '(hash table)'), block = tab(blockPos, blockN, '(block table)');
  return (name) => {
    const a = hashString(name, 1), b = hashString(name, 2), mask = hashN - 1;
    for (let i = hashString(name, 0) & mask, n = 0; n < hashN; i = (i + 1) & mask, n++) {
      const bi = hash[i * 4 + 3];
      if (bi === 0xFFFFFFFF) return null;
      if (bi < blockN && hash[i * 4] === a && hash[i * 4 + 1] === b && (block[bi * 4 + 3] & 0x80000000)) return { pos: block[bi * 4], size: block[bi * 4 + 1] };
    }
    return null;
  };
}
