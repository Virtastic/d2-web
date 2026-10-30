// Read one file out of an MPQ held in a Blob: stored, zlib (mask 0x02) and PKWARE-implode (mask 0x08)
// sectors, which is what a Blizzard patch installer uses for the Patch_D2.mpq it carries. Used only to lift
// that patch out of an installer the player picks; archives themselves are mounted whole and read by the
// engine. Encrypted files are read when the caller names the file (the key comes from its name). Anything
// else (Huffman, bzip2, ADPCM, sparse) returns null -- the caller says so.
import { hashString, decrypt } from './mpq-index.mjs';
const FLAG_FIXKEY = 0x20000, FLAG_IMPLODE = 0x100, FLAG_COMPRESS = 0x200, FLAG_ENCRYPTED = 0x10000, FLAG_SINGLE = 0x1000000, FLAG_CRC = 0x4000000;

// PKWARE DCL "explode" (the format blast.c documents), written from that description.
const LEN_BASE = [2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 16, 24, 40, 72, 136, 264];
const LEN_EXTRA = [0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8];
const LEN_CODE = [0x05, 0x03, 0x01, 0x06, 0x0A, 0x02, 0x0C, 0x14, 0x04, 0x18, 0x08, 0x30, 0x10, 0x20, 0x40, 0x00];
const LEN_LEN = [3, 2, 3, 3, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 7, 7];
const DIST_CODE = [0x03, 0x0D, 0x05, 0x19, 0x09, 0x11, 0x01, 0x3E, 0x1E, 0x2E, 0x0E, 0x36, 0x16, 0x26, 0x06, 0x3A, 0x1A, 0x2A, 0x0A, 0x32, 0x12, 0x22, 0x42, 0x02, 0x7C, 0x3C, 0x5C, 0x1C, 0x6C, 0x2C, 0x4C, 0x0C, 0x74, 0x34, 0x54, 0x14, 0x64, 0x24, 0x44, 0x04, 0x78, 0x38, 0x58, 0x18, 0x68, 0x28, 0x48, 0x08, 0xF0, 0x70, 0xB0, 0x30, 0xD0, 0x50, 0x90, 0x10, 0xE0, 0x60, 0xA0, 0x20, 0xC0, 0x40, 0x80, 0x00];
const DIST_LEN = [2, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8];
// Peel the first set bit's codes by brute force over the (tiny) tables: codes are sent LSB-first.
function decodeSym(bits, codes, lens) {
  let v = 0;
  for (let n = 1; n <= 8; n++) {
    v |= bits.bit() << (n - 1);
    for (let i = 0; i < codes.length; i++) if (lens[i] === n && codes[i] === v) return i;
  }
  throw new Error('implode: bad code');
}
export function explode(src, outSize) {
  if (src.length < 3) throw new Error('implode: short');
  const lit = src[0], dictBits = src[1];
  if (lit > 1 || dictBits < 4 || dictBits > 6) throw new Error('implode: bad header');
  let pos = 2, cur = 0, left = 0;
  const bits = { bit() { if (!left) { if (pos >= src.length) throw new Error('implode: eof'); cur = src[pos++]; left = 8; } const b = cur & 1; cur >>= 1; left--; return b; },
    take(n) { let v = 0; for (let i = 0; i < n; i++) v |= this.bit() << i; return v; } };
  const out = new Uint8Array(outSize); let o = 0;
  while (o < outSize) {
    if (bits.bit()) {
      const ls = decodeSym(bits, LEN_CODE, LEN_LEN);
      const len = LEN_BASE[ls] + bits.take(LEN_EXTRA[ls]);
      if (len === 519) break;
      const ds = decodeSym(bits, DIST_CODE, DIST_LEN);
      const low = len === 2 ? 2 : dictBits;
      const dist = ((ds << low) | bits.take(low)) + 1;
      if (dist > o) throw new Error('implode: distance');
      for (let k = 0; k < len && o < outSize; k++, o++) out[o] = out[o - dist];
    } else {
      if (lit) throw new Error('implode: coded literals unsupported');
      out[o++] = bits.take(8);
    }
  }
  return out;
}

async function zlib(bytes) {
  const s = new Blob([bytes]).stream().pipeThrough(new DecompressionStream('deflate'));
  return new Uint8Array(await new Response(s).arrayBuffer());
}
async function unpack(chunk, outSize) {
  if (chunk.length >= outSize) return chunk.subarray(0, outSize);
  const mask = chunk[0], body = chunk.subarray(1);
  if (mask === 0x02) return zlib(body);
  if (mask === 0x08) return explode(body, outSize);
  throw new Error('compression 0x' + mask.toString(16));
}

/** entry = {pos, csize, size, flags} (positions relative to `base`); returns a Blob or null when unsupported. */
export async function readMpqFile(blob, base, sectorShift, entry, name) {
  const { pos, csize, size, flags } = entry;
  if (flags & FLAG_ENCRYPTED && !name) return null;
  let key = 0;
  if (flags & FLAG_ENCRYPTED) {
    key = hashString(name.slice(name.lastIndexOf('\\') + 1), 3);
    if (flags & FLAG_FIXKEY) key = ((key + pos) ^ size) >>> 0;
  }
  const dec = (u8, k) => { const w = Math.ceil(u8.length / 4), u = new Uint32Array(w); new Uint8Array(u.buffer).set(u8); decrypt(u, k); return new Uint8Array(u.buffer).subarray(0, u8.length); };
  const at = (a, n) => blob.slice(base + a, base + a + n).arrayBuffer().then((b) => new Uint8Array(b));
  try {
    if (!(flags & (FLAG_IMPLODE | FLAG_COMPRESS)) || csize === size && (flags & FLAG_SINGLE)) return blob.slice(base + pos, base + pos + size);
    if (flags & FLAG_SINGLE) {
      let c = await at(pos, csize);
      if (key) c = dec(c, key);
      return new Blob([flags & FLAG_IMPLODE ? explode(c, size) : await unpack(c, size)]);
    }
    const sector = 512 << sectorShift, n = Math.ceil(size / sector);
    let raw = await at(pos, (n + 1 + (flags & FLAG_CRC ? 1 : 0)) * 4);
    if (key) raw = dec(raw, (key - 1) >>> 0);
    const tab = new DataView(raw.buffer, raw.byteOffset, raw.length);
    const parts = [];
    for (let i = 0; i < n; i++) {
      const a = tab.getUint32(i * 4, true), b = tab.getUint32(i * 4 + 4, true), want = Math.min(sector, size - i * sector);
      let c = await at(pos + a, b - a);
      if (key) c = dec(c, (key + i) >>> 0);
      parts.push(flags & FLAG_IMPLODE && c.length < want ? explode(c, want) : await unpack(c, want));
    }
    return new Blob(parts);
  } catch (e) { return null; }
}
