// Bring-your-own-files detection: what a picked file IS, judged by its bytes and never its name.
// Pure functions over Blob/File; nothing is uploaded and nothing here knows about the page.
//
// An archive is read as a plain MPQ v0 (the original game's format; CASC, 1.14+ and D2R are not MPQ and
// are named as unsupported). Its role comes from the shape of its tables: every base archive of the
// original game ships byte-identical across every install and patch level (measured over three installs,
// 1.10-1.13c), so (hash entries, block entries) is a unique, rename-proof signature. Only Patch_D2.mpq
// changes between versions; its SHA-256 over header + block table is looked up in known-mpqs.json
// (hashes and sizes only, never content) to say which version it is.
import { decrypt, hashString } from './mpq-index.mjs';
import { readMpqFile } from './mpq-read.mjs';

const MAGIC = 0x1a51504d;

// role -> [file name the engine opens, hashN, blockN, required]
export const ROLES = {
  d2data:   { file: 'd2data.mpq',   hashN: 65536, blockN: 10851, required: true,  label: 'Game data',        note: 'd2data.mpq' },
  d2char:   { file: 'd2char.mpq',   hashN: 16384, blockN: 10013, required: true,  label: 'Characters',       note: 'd2char.mpq' },
  d2exp:    { file: 'd2exp.mpq',    hashN: 65536, blockN: 9888,  required: true,  label: 'Expansion data',   note: 'd2exp.mpq' },
  d2sfx:    { file: 'd2sfx.mpq',    hashN: 8192,  blockN: 2329,  required: true,  label: 'Sound effects',    note: 'd2sfx.mpq' },
  patch_d2: { file: 'patch_d2.mpq', hashN: 512,   blockN: null,  required: true,  label: 'Patch',            note: 'Patch_D2.mpq' },
  d2speech: { file: 'd2speech.mpq', hashN: 8192,  blockN: 1566,  required: false, label: 'Speech',           note: 'd2speech.mpq' },
  d2music:  { file: 'd2music.mpq',  hashN: 8192,  blockN: 45,    required: false, label: 'Music',            note: 'd2music.mpq' },
  d2xmusic: { file: 'd2xmusic.mpq', hashN: 8192,  blockN: 12,    required: false, label: 'Expansion music',  note: 'd2xmusic.mpq' },
  d2xtalk:  { file: 'd2xtalk.mpq',  hashN: 8192,  blockN: 478,   required: false, label: 'Expansion speech', note: 'd2xtalk.mpq' },
  d2video:  { file: 'd2video.mpq',  hashN: 8192,  blockN: 17,    required: false, label: 'Videos',           note: 'd2video.mpq' },
  d2xvideo: { file: 'd2xvideo.mpq', hashN: 256,   blockN: 6,     required: false, label: 'Expansion videos', note: 'd2xvideo.mpq' },
};
// The engine mounts in this order (play/index.html RETAIL_ORDER): later archives lose to earlier ones.
export const ORDER = ['patch_d2', 'd2exp', 'd2xmusic', 'd2xtalk', 'd2xvideo', 'd2data', 'd2char', 'd2music', 'd2sfx', 'd2speech', 'd2video'];

const u32 = (dv, o) => dv.getUint32(o, true);
const bytes = async (blob, pos, len) => new Uint8Array(await blob.slice(pos, pos + len).arrayBuffer());

async function sha256hex(buf) {
  const d = await crypto.subtle.digest('SHA-256', buf);
  return [...new Uint8Array(d)].map((b) => b.toString(16).padStart(2, '0')).join('');
}

/** Offsets of an 'MPQ\x1a' header at the start, or at 512-byte boundaries (an installer is an exe stub + MPQ). */
export async function findMpqBase(blob, { scanLimit = 8 * 1024 * 1024 } = {}) {
  const head = await bytes(blob, 0, 4);
  if (head.length >= 4 && new DataView(head.buffer).getUint32(0, true) === MAGIC) return 0;
  if (!(head[0] === 0x4d && head[1] === 0x5a)) return -1;          // only an exe is searched
  const end = Math.min(blob.size, scanLimit);
  for (let off = 0; off < end; off += 1 << 20) {
    const chunk = await bytes(blob, off, Math.min(1 << 20, blob.size - off));
    for (let i = 0; i + 4 <= chunk.length; i += 512) {
      if (chunk[i] === 0x4d && chunk[i + 1] === 0x50 && chunk[i + 2] === 0x51 && chunk[i + 3] === 0x1a) return off + i;
    }
  }
  return -1;
}

async function tables(blob, base, hashPos, hashN, blockPos, blockN) {
  const rd = async (pos, n, name) => {
    const b = await bytes(blob, base + pos, n * 16), u = new Uint32Array(n * 4);
    new Uint8Array(u.buffer).set(b.subarray(0, n * 16));
    decrypt(u, hashString(name, 3));
    return u;
  };
  return { hash: await rd(hashPos, hashN, '(hash table)'), block: await rd(blockPos, blockN, '(block table)') };
}
const lookup = (t, hashN, blockN, name) => {
  const a = hashString(name, 1), b = hashString(name, 2), mask = hashN - 1;
  for (let i = hashString(name, 0) & mask, n = 0; n < hashN; i = (i + 1) & mask, n++) {
    const bi = t.hash[i * 4 + 3];
    if (bi === 0xFFFFFFFF) return null;
    if (bi < blockN && t.hash[i * 4] === a && t.hash[i * 4 + 1] === b && (t.block[bi * 4 + 3] & 0x80000000))
      return { pos: t.block[bi * 4], csize: t.block[bi * 4 + 1], size: t.block[bi * 4 + 2], flags: t.block[bi * 4 + 3] };
  }
  return null;
};

/**
 * Judge one picked file. Returns { kind, name, size, ... }:
 *   kind 'mpq'       a plain MPQ (maybe inside an exe: base > 0); role/hashN/blockN/truncated/sha256 filled in
 *   kind 'casc'      a CASC data folder file or the like (1.14+ / D2R): unsupported
 *   kind 'other'     anything else
 */
export async function inspect(file) {
  const name = file.name || '', size = file.size, out = { name, size, kind: 'other' };
  if (/^(data\.\d+|.*\.idx|.*\.index|\.build\.info|.*\.db)$/i.test(name) || /\.casc$/i.test(name)) return { ...out, kind: 'casc' };
  const base = await findMpqBase(file);
  if (base < 0) return out;
  const avail = size - base;
  if (avail < 32) return { ...out, kind: 'other' };
  const h = await bytes(file, base, 32), dv = new DataView(h.buffer);
  const hdrSize = u32(dv, 4), archive = u32(dv, 8), ver = dv.getUint16(12, true);
  const hashPos = u32(dv, 16), blockPos = u32(dv, 20), hashN = u32(dv, 24), blockN = u32(dv, 28);
  const m = { ...out, kind: 'mpq', base, hdrSize, archive, ver, hashN, blockN, shift: dv.getUint16(14, true) };
  if (ver !== 0 || hdrSize !== 32) return { ...m, kind: 'casc', unsupported: 'newer-mpq' };   // v1+ is Warcraft III / StarCraft II era
  if (!hashN || (hashN & (hashN - 1)) || hashN > 1 << 20 || blockN > 1 << 20 || hashPos + hashN * 16 > avail || blockPos + blockN * 16 > avail) {
    return { ...m, truncated: true };
  }
  // An unpatched, whole archive ends where its header says it does; a short download does not.
  m.truncated = archive > avail;
  const t = await tables(file, base, hashPos, hashN, blockPos, blockN);
  m.sha256 = await sha256hex(new Uint8Array([...h, ...new Uint8Array(t.block.buffer)]));
  for (const [role, r] of Object.entries(ROLES)) {
    if (r.hashN === hashN && (r.blockN === null || r.blockN === blockN)) { m.role = role; break; }
  }
  if (m.role === 'patch_d2') {
    // hashN 512 alone could be a mod; a real patch carries the game's tables.
    const looksLikeD2 = ['data\\global\\excel\\Levels.txt', 'data\\global\\excel\\Skills.txt', 'data\\local\\lng\\eng\\patchstring.tbl']
      .some((n) => lookup(t, hashN, blockN, n));
    if (!looksLikeD2) delete m.role;
  }
  // A patch installer carries the patch as a stored/compressed file inside; remember where it sits.
  if (base > 0) m.inner = ['Patch_D2.mpq', 'patch_d2.mpq'].map((n) => lookup(t, hashN, blockN, n)).find(Boolean) || null;
  if (base > 0 && !m.role) m.installer = true;
  m.names = (n) => lookup(t, hashN, blockN, n);
  return m;
}

/**
 * Lift Patch_D2.mpq out of an official patch installer (an exe stub with an MPQ behind it). Returns the
 * inspected patch as a named File, or null when the installer holds none this page can unpack.
 */
export async function extractInnerPatch(file, info) {
  if (!info || !info.inner) return null;
  const blob = await readMpqFile(file, info.base, info.shift, info.inner);
  if (!blob) return null;
  const inner = new File([blob], 'Patch_D2.mpq', { type: 'application/octet-stream' });
  const got = await inspect(inner);
  return got.role === 'patch_d2' && !got.truncated ? Object.assign(inner, { info: got, from: file.name }) : null;
}

/**
 * Fold a set of inspected files into the front door's state.
 * Returns { slots, ready, patch, issues } where each slot is { role, label, required, have, file, problem }.
 */
export function classify(infos, known = { patch_d2: [] }) {
  const slots = {};
  for (const [role, r] of Object.entries(ROLES)) slots[role] = { role, label: r.label, note: r.note, required: r.required, have: false };
  const issues = [];
  for (const f of infos) {
    if (f.kind === 'casc') { issues.push({ code: 'casc', file: f.name, text: 'This looks like a newer Diablo II (1.14 or Resurrected). Only the original game’s .mpq files work.' }); continue; }
    if (f.kind !== 'mpq' || !f.role) {
      if (f.installer) { issues.push({ code: 'installer', file: f.name, text: `${f.name} is an installer without a patch inside it that this page can read.` }); continue; }
      if (f.kind === 'mpq') issues.push({ code: 'unknown-mpq', file: f.name, text: `${f.name} is an MPQ, but not one Diablo II uses.` });
      continue;
    }
    if (f.truncated) { slots[f.role].problem = 'incomplete'; issues.push({ code: 'truncated', file: f.name, text: `${f.name} looks incomplete — copy it again.` }); continue; }
    const s = slots[f.role];
    if (s.have) continue;
    Object.assign(s, { have: true, file: f });
  }
  const p = slots.patch_d2;
  const match = p.have ? (known.patch_d2 || []).find((k) => k.sha256 === p.file.sha256 || (k.size === p.file.size && k.blockN === p.file.blockN)) : null;
  const patch = !p.have ? { level: 'missing' } : match ? { level: match.compat, label: match.label } : { level: 'unknown', label: 'other version' };
  // Any Diablo II Patch_D2.mpq is accepted without a word: the engine patches whatever version it is given.
  const missing = Object.values(slots).filter((s) => s.required && !s.have);
  return { slots, patch, issues, missing: missing.map((s) => s.role), ready: missing.length === 0 };
}
