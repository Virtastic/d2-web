// DCC decode micro-bench: decodes every direction of the player DCCs the single-player client draws for the seven
// classes in light, medium and heavy armour (every mode and weapon class whose file exists in the retail archives)
// N times and reports the wall-clock cost per direction decode, plus a SHA-256 of every decoded direction (the
// whole decoded record: frame headers, palette map, stream sizes, indexed and opaque pixels). Retail MPQs are read
// in place from D2_RETAIL_DIR (default <repo>/retail/D2); nothing retail is written anywhere.
//
// Usage: node tools/dcc-decode-bench.mjs [--iterations 5] [--retail DIR] [--baseline-module path/to/unit-formats.mjs]
//                                        [--write-hashes out.json] [--check-hashes baseline.json]
// --write-hashes stores {path#direction: sha256}; --check-hashes compares against such a file and exits 1 on any
// difference, so a decoder change can be proved byte-identical to the decoder it replaced. --baseline-module loads
// a second decoder (e.g. the parent commit's unit-formats.mjs), checks its digests equal this one's, and times the
// two interleaved (alternating order each pass) so both see the same host load.
import { readFileSync, writeFileSync, openSync, readSync, existsSync } from 'node:fs';
import { createHash } from 'node:crypto';
import { inflateSync } from 'node:zlib';
import { performance } from 'node:perf_hooks';
import { join, dirname, resolve } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';
import { hashString, decrypt } from '../play/assets/mpq-index.mjs';
import { explode } from '../play/assets/mpq-read.mjs';
import { decodeDcc } from '../play/assets/unit-formats.mjs';

const ROOT = join(dirname(fileURLToPath(import.meta.url)), '..');
export const DEFAULT_RETAIL = process.env.D2_RETAIL_DIR || join(ROOT, 'retail', 'D2');
const ARCHIVES = ['patch_d2.mpq', 'd2exp.mpq', 'd2char.mpq', 'd2data.mpq'];

/** Synchronous MPQ v0 reader over the retail archives (patch first); null when they are absent. */
export function openRetail(dir = DEFAULT_RETAIL) {
  if (!ARCHIVES.every((f) => existsSync(join(dir, f)))) return null;
  const archives = ARCHIVES.map((f) => openMpq(join(dir, f)));
  const cache = new Map();
  return (path) => {
    let v = cache.get(path);
    if (v) return v;
    for (const a of archives) { v = a(path); if (v) break; }
    if (!v) throw new Error('missing ' + path);
    cache.set(path, v); return v;
  };
}

function openMpq(file) {
  const fd = openSync(file, 'r');
  const at = (pos, len) => { const b = Buffer.alloc(len); readSync(fd, b, 0, len, pos); return new Uint8Array(b.buffer, b.byteOffset, len); };
  const h = new DataView(at(0, 32).buffer, 0, 32);
  if (h.getUint32(0, true) !== 0x1a51504d) throw new Error(`${file}: not an MPQ`);
  const sectorShift = h.getUint16(14, true), hashPos = h.getUint32(16, true), blockPos = h.getUint32(20, true), hashN = h.getUint32(24, true), blockN = h.getUint32(28, true);
  const tab = (pos, n, name) => { const u = new Uint32Array(n * 4); new Uint8Array(u.buffer).set(at(pos, n * 16)); decrypt(u, hashString(name, 3)); return u; };
  const hash = tab(hashPos, hashN, '(hash table)'), block = tab(blockPos, blockN, '(block table)');
  const find = (name) => {
    const a = hashString(name, 1), b = hashString(name, 2), mask = hashN - 1;
    for (let i = hashString(name, 0) & mask, n = 0; n < hashN; i = (i + 1) & mask, n++) {
      const bi = hash[i * 4 + 3];
      if (bi === 0xFFFFFFFF) return null;
      if (bi < blockN && hash[i * 4] === a && hash[i * 4 + 1] === b && (block[bi * 4 + 3] & 0x80000000)) return { pos: block[bi * 4], csize: block[bi * 4 + 1], size: block[bi * 4 + 2], flags: block[bi * 4 + 3] };
    }
    return null;
  };
  const unpack = (chunk, want) => {
    if (chunk.length >= want) return chunk.subarray(0, want);
    const mask = chunk[0], body = chunk.subarray(1);
    if (mask === 0x02) return new Uint8Array(inflateSync(body));
    if (mask === 0x08) return explode(body, want);
    throw new Error(`compression 0x${mask.toString(16)}`);
  };
  return (name) => {
    const e = find(name); if (!e) return null;
    if (e.flags & 0x10000) throw new Error('encrypted file ' + name);
    if (!(e.flags & 0x300)) return at(e.pos, e.size);
    if (e.flags & 0x1000000) { const c = at(e.pos, e.csize); return e.flags & 0x100 ? explode(c, e.size) : unpack(c, e.size); }
    const sector = 512 << sectorShift, n = Math.ceil(e.size / sector), tbl = new DataView(at(e.pos, (n + 1) * 4).slice().buffer);
    const out = new Uint8Array(e.size);
    for (let i = 0; i < n; i++) {
      const a = tbl.getUint32(i * 4, true), b = tbl.getUint32(i * 4 + 4, true), want = Math.min(sector, e.size - i * sector), c = at(e.pos + a, b - a);
      out.set(e.flags & 0x100 && c.length < want ? explode(c, want) : unpack(c, want), i * sector);
    }
    return out;
  };
}

// Canonical serialization of a decoded value into a hash: keys sorted, typed arrays as raw bytes, so any change
// to any decoded field (not only the pixels) changes the digest.
function feed(h, v) {
  if (v === null || v === undefined) { h.update(`${v};`); return; }
  if (ArrayBuffer.isView(v)) { h.update(`u8[${v.byteLength}]`); h.update(new Uint8Array(v.buffer, v.byteOffset, v.byteLength)); return; }
  if (Array.isArray(v)) { h.update(`[${v.length}`); for (const x of v) feed(h, x); h.update(']'); return; }
  if (typeof v === 'object') { h.update('{'); for (const k of Object.keys(v).sort()) { h.update(`${k}:`); feed(h, v[k]); } h.update('}'); return; }
  h.update(`${typeof v}:${v};`);
}
/** SHA-256 of one decoded DCC direction record (every field, pixels included). */
export function directionDigest(direction) { const h = createHash('sha256'); feed(h, direction); return h.digest('hex'); }
/** SHA-256 per direction of a decoded DCC, plus its container header. */
export function dccDigests(dcc) {
  const head = createHash('sha256'); feed(head, { version: dcc.version, directions: dcc.directions, framesPerDirection: dcc.framesPerDirection, totalSizeCoded: dcc.totalSizeCoded });
  return { header: head.digest('hex'), directions: dcc.directionRecords.map((d) => (d ? directionDigest(d) : null)) };
}

const CLASSES = ['AM', 'SO', 'NE', 'PA', 'BA', 'DZ', 'AI'];
const COMPONENTS = ['HD', 'TR', 'LG', 'RA', 'LA', 'S1', 'S2'];
const ARMOUR = ['LIT', 'MED', 'HVY'];
const MODES = ['DT', 'NU', 'WL', 'RN', 'GH', 'TN', 'TW', 'A1', 'A2', 'BL', 'SC', 'TH', 'KK', 'S1', 'S2', 'S3', 'S4', 'DD', 'SQ'];
const WEAPON_CLASSES = ['HTH', '1HS', '2HS', '1HT', '2HT', 'BOW', 'XBW', 'STF', 'HT1', 'HT2', '1SS', '1JS', '1ST', '1JT'];
/** The sorted player DCC paths (class x armour component x LIT/MED/HVY x mode x weapon class) present in the archives. */
export function singlePlayerDccPaths(read) {
  const paths = [];
  for (const cl of CLASSES) for (const comp of COMPONENTS) for (const arm of ARMOUR) for (const mode of MODES) for (const wc of WEAPON_CLASSES) {
    const p = `data\\global\\CHARS\\${cl}\\${comp}\\${cl}${comp}${arm}${mode}${wc}.DCC`;
    try { read(p); paths.push(p); } catch {}
  }
  return paths.sort();
}

if (process.argv[1] && import.meta.url === pathToFileURL(process.argv[1]).href) {
  const args = process.argv.slice(2);
  const flag = (n, d) => { const i = args.indexOf(n); return i >= 0 && args[i + 1] !== undefined ? args[i + 1] : d; };
  const ITER = Number(flag('--iterations', 5));
  const read = openRetail(flag('--retail', DEFAULT_RETAIL));
  if (!read) { console.error('retail MPQs not found (set D2_RETAIL_DIR or --retail)'); process.exit(2); }
  const paths = singlePlayerDccPaths(read);
  const baselinePath = flag('--baseline-module', '');
  const baseline = baselinePath ? (await import(pathToFileURL(resolve(baselinePath)).href)).decodeDcc : null;
  const files = paths.map((p) => [p, read(p)]);
  let directions = 0, bytes = 0;
  for (const [, b] of files) { directions += b[2]; bytes += b.length; }
  // warm the JIT once, collect the digests from that pass
  const hashes = {};
  for (const [p, b] of files) {
    const d = dccDigests(decodeDcc(b));
    hashes[`${p}#header`] = d.header;
    d.directions.forEach((x, i) => { hashes[`${p}#${i}`] = x; });
  }
  if (baseline) {
    let diff = 0;
    for (const [p, b] of files) {
      const d = dccDigests(baseline(b));
      if (d.header !== hashes[`${p}#header`]) diff++;
      d.directions.forEach((x, i) => { if (x !== hashes[`${p}#${i}`]) diff++; });
    }
    console.log(`  baseline  ${diff ? `DIFFERS on ${diff} digests` : `identical, ${Object.keys(hashes).length} digests equal`}`);
    if (diff) process.exitCode = 1;
  }
  const runs = [], baseRuns = [];
  const timePass = (fn) => { const t0 = performance.now(); for (const [, b] of files) fn(b); return performance.now() - t0; };
  for (let it = 0; it < ITER; it++) {
    if (baseline && it % 2) { baseRuns.push(timePass(baseline)); runs.push(timePass(decodeDcc)); }
    else { runs.push(timePass(decodeDcc)); if (baseline) baseRuns.push(timePass(baseline)); }
  }
  const sorted = [...runs].sort((a, b) => a - b), median = sorted[Math.floor(sorted.length / 2)];
  const all = createHash('sha256'); for (const k of Object.keys(hashes).sort()) all.update(`${k}=${hashes[k]}\n`);
  console.log(`DCC decode: ${files.length} files, ${directions} directions, ${(bytes / 1048576).toFixed(2)} MiB compressed; ${ITER} timed passes after 1 warm pass`);
  console.log(`  pass ms   median ${median.toFixed(1)}  min ${sorted[0].toFixed(1)}  max ${sorted[sorted.length - 1].toFixed(1)}`);
  console.log(`  per direction  median ${(median / directions * 1000).toFixed(1)} us  min ${(sorted[0] / directions * 1000).toFixed(1)} us`);
  if (baseline) {
    const bs = [...baseRuns].sort((a, b) => a - b), bm = bs[Math.floor(bs.length / 2)];
    console.log(`  baseline per direction  median ${(bm / directions * 1000).toFixed(1)} us  min ${(bs[0] / directions * 1000).toFixed(1)} us  (interleaved; ${(bm / median).toFixed(2)}x)`);
  }
  console.log(`  digests   ${Object.keys(hashes).length} (sha256 of the set ${all.digest('hex')})`);
  const out = flag('--write-hashes', '');
  if (out) writeFileSync(out, JSON.stringify(hashes, null, 1) + '\n');
  const check = flag('--check-hashes', '');
  if (check) {
    const base = JSON.parse(readFileSync(check, 'utf8'));
    const keys = new Set([...Object.keys(base), ...Object.keys(hashes)]);
    const diff = [...keys].filter((k) => base[k] !== hashes[k]);
    console.log(`  identity  ${diff.length ? `DIFFERS on ${diff.length}/${keys.size}: ${diff.slice(0, 5).join(' ')}` : `identical, ${keys.size}/${keys.size} digests equal`}`);
    if (diff.length) process.exitCode = 1;
  }
}
