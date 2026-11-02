// Archives the player picks: a .zip or .7z is opened in this browser and the entries the game can use
// (.mpq, .exe installers, PlugY's files: the same names a direct pick takes) are handed on one at a time.
// Nothing is uploaded and nothing is kept whole: each entry is streamed to `place`, which copies it to the
// origin's private file system or keeps it as a File.
//   zip  read here: central directory (zip64 included), stored and deflate entries (DecompressionStream
//        'deflate-raw'), CRC-32 and size checked as the bytes stream through. Encrypted entries are skipped.
//   7z   read by libarchive.js (vendor/libarchive/, loaded only when a .7z is picked). That build reads the
//        whole archive into its wasm heap, so a 7z is limited by what that heap can hold, and a solid 7z
//        decodes from its start for each entry it yields.
// A file that is not an archive, or an entry that fails, is skipped; the caller then sees the usual missing files.

const SIG_EOCD = 0x06054b50, SIG_EOCD64 = 0x06064b50, SIG_LOC64 = 0x07064b50;
const SIG_CD = 0x02014b50, SIG_LOCAL = 0x04034b50;
const U32_MAX = 0xFFFFFFFF, U16_MAX = 0xFFFF;

export const isArchiveName = (n) => /\.(zip|7z)$/i.test(n || '');
const archiveKind = (n) => (/\.zip$/i.test(n || '') ? 'zip' : /\.7z$/i.test(n || '') ? '7z' : null);

const CRC_TABLE = (() => {
  const t = new Uint32Array(256);
  for (let n = 0; n < 256; n++) {
    let c = n;
    for (let k = 0; k < 8; k++) c = c & 1 ? 0xEDB88320 ^ (c >>> 1) : c >>> 1;
    t[n] = c >>> 0;
  }
  return t;
})();
/** CRC-32 (zip's), continuing from `crc` so a stream can be checked chunk by chunk. */
export function crc32(bytes, crc = 0) {
  let c = (crc ^ 0xFFFFFFFF) >>> 0;
  for (let i = 0; i < bytes.length; i++) c = CRC_TABLE[(c ^ bytes[i]) & 0xFF] ^ (c >>> 8);
  return (c ^ 0xFFFFFFFF) >>> 0;
}

const corrupt = (what) => Object.assign(new Error(`archive: ${what}`), { corrupt: true });
const u16 = (dv, o) => dv.getUint16(o, true);
const u32 = (dv, o) => dv.getUint32(o, true);
const u64 = (dv, o) => u32(dv, o) + u32(dv, o + 4) * 0x100000000;
const bytesAt = async (file, pos, len) => {
  if (pos < 0 || len < 0 || pos + len > file.size) throw corrupt('read past the end');
  return new DataView(await file.slice(pos, pos + len).arrayBuffer());
};

// The end-of-central-directory record sits in the last 64 KiB plus its comment; a zip64 locator (20 bytes)
// sits directly before it. Scan backwards for the signature and accept the first one that fits.
async function centralDirectory(file) {
  const span = Math.min(file.size, 22 + U16_MAX + 20);
  const tail = await bytesAt(file, file.size - span, span);
  let eocd = -1;
  for (let i = span - 22; i >= 0; i--) {
    if (u32(tail, i) === SIG_EOCD && i + 22 + u16(tail, i + 20) <= span) { eocd = i; break; }
  }
  if (eocd < 0) throw corrupt('no zip directory');
  let count = u16(tail, eocd + 10), size = u32(tail, eocd + 12), offset = u32(tail, eocd + 16);
  if (count === U16_MAX || size === U32_MAX || offset === U32_MAX) {
    const loc = eocd - 20;
    if (loc < 0 || u32(tail, loc) !== SIG_LOC64) throw corrupt('zip64 locator missing');
    const rec = await bytesAt(file, u64(tail, loc + 8), 56);
    if (u32(rec, 0) !== SIG_EOCD64) throw corrupt('zip64 directory missing');
    count = u64(rec, 32); size = u64(rec, 40); offset = u64(rec, 48);
  }
  const cd = await bytesAt(file, offset, size);
  const entries = [];
  let p = 0;
  for (let n = 0; n < count; n++) {
    if (p + 46 > cd.byteLength || u32(cd, p) !== SIG_CD) throw corrupt('bad directory entry');
    const e = {
      flags: u16(cd, p + 8), method: u16(cd, p + 10), crc: u32(cd, p + 16),
      csize: u32(cd, p + 20), usize: u32(cd, p + 24), offset: u32(cd, p + 42),
    };
    const nl = u16(cd, p + 28), el = u16(cd, p + 30), cl = u16(cd, p + 32);
    e.path = new TextDecoder().decode(new Uint8Array(cd.buffer, cd.byteOffset + p + 46, nl));
    // zip64: the 64-bit values, in this order, stand in for each 0xFFFFFFFF field.
    let q = p + 46 + nl;
    const end = q + el;
    while (q + 4 <= end) {
      const id = u16(cd, q), len = u16(cd, q + 2);
      if (id === 1) {
        let r = q + 4;
        if (e.usize === U32_MAX) { e.usize = u64(cd, r); r += 8; }
        if (e.csize === U32_MAX) { e.csize = u64(cd, r); r += 8; }
        if (e.offset === U32_MAX) { e.offset = u64(cd, r); r += 8; }
      }
      q += 4 + len;
    }
    entries.push(e);
    p += 46 + nl + el + cl;
  }
  return entries;
}

// The entry's data starts after its own local header (whose name and extra lengths may differ from the directory's).
async function dataStart(file, e) {
  const lh = await bytesAt(file, e.offset, 30);
  if (u32(lh, 0) !== SIG_LOCAL) throw corrupt('bad local header');
  return e.offset + 30 + u16(lh, 26) + u16(lh, 28);
}

// Stream an entry's bytes, checking the size and CRC the directory promised. A mismatch errors the stream.
function entryStream(file, e, start) {
  let src = file.slice(start, start + e.csize).stream();
  if (e.method === 8) {
    if (typeof DecompressionStream === 'undefined') throw new Error('archive: this browser cannot inflate zip entries');
    src = src.pipeThrough(new DecompressionStream('deflate-raw'));
  }
  let crc = 0, n = 0;
  return src.pipeThrough(new TransformStream({
    transform(chunk, c) { crc = crc32(chunk, crc); n += chunk.byteLength; c.enqueue(chunk); },
    flush() { if (n !== e.usize || crc !== e.crc) throw corrupt(`${e.path} does not match its directory entry`); },
  }));
}

// Entries that are not the player's files: macOS's resource-fork shadows and any folder of them.
const junk = (path) => path.split(/[\\/]/).some((s, i, a) => s === '__MACOSX' || (i === a.length - 1 && s.startsWith('._')));
const leaf = (path) => path.split(/[\\/]/).pop();

async function unpackZip(file, wanted, place) {
  for (const e of await centralDirectory(file)) {
    if (e.path.endsWith('/') || e.flags & 1 || (e.method !== 0 && e.method !== 8)) continue;
    const name = leaf(e.path);
    if (!name || junk(e.path) || !wanted(name)) continue;
    const start = await dataStart(file, e);
    await place(name, e.usize, () => entryStream(file, e, start));
  }
}

// libarchive.js loads on the first .7z only. Its entries come out as File objects; the stream is
// re-made on demand so `place` can call it again after a failed copy.
async function unpack7z(file, wanted, place) {
  const { Archive } = await import('./vendor/libarchive/libarchive.js');
  const arc = await Archive.open(file);
  try {
    if ((await arc.hasEncryptedData()) === true) return;
    // getFilesArray() hides each entry's full path; the worker's listing keeps it, and extractSingleFile needs it.
    const listing = await arc.client.listFiles();
    for (const f of listing) {
      if (f.type !== 'FILE' || junk(f.path)) continue;
      const name = leaf(f.path);
      if (!wanted(name)) continue;
      await place(name, f.size, async () => (await arc.extractSingleFile(f.path)).stream());
    }
  } finally {
    await arc.close();
  }
}

/**
 * Hand each game entry of a .zip or .7z to `place(name, size, open)`. `open()` returns a fresh ReadableStream
 * of the entry's bytes. `place` returns whatever it keeps. An entry whose place() throws is skipped. Returns
 * nothing: the caller collects what place() kept. A file that is not an archive does nothing.
 */
export async function unpackArchive(file, { wanted, place }) {
  const kind = archiveKind(file.name);
  const safePlace = async (name, size, open) => {
    try { await place(name, size, open); } catch { /* one bad entry does not stop the rest */ }
  };
  try {
    if (kind === 'zip') await unpackZip(file, wanted, safePlace);
    else if (kind === '7z') await unpack7z(file, wanted, safePlace);
  } catch { /* not an archive this browser can read: the missing-files path reports it */ }
}
