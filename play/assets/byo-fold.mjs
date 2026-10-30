// Fold an official patch installer's loose files into a Patch_D2.mpq, the way Blizzard's patcher does:
// the installer is an MPQ whose patch.lst lists "<path in the patch>;<file in the installer>;<flags>".
// Only entries under data\ go into the patch (the DLLs and exes are the patcher's own business). A file the
// installer stores in pieces (name.000, name.001, ...) is joined in order. Each file starts with a 24-byte header
// (0x18, 0, 4, flag, ...): flag 1 is the whole file, which is kept; flag 0 is a delta against the installed
// archives, whose format is not decoded here, so that entry is skipped and reported.
import { readMpqFile } from './mpq-read.mjs';
import { buildMpq } from './mpq-build.mjs';

const text = async (file, info, name) => {
  const e = info.names(name); if (!e) return null;
  const b = await readMpqFile(file, info.base, info.shift, e, name); return b ? b.text() : null;
};

/** Returns { bytes, files, skipped } for an installer inspected as `info`, or null when it carries no patch.lst. */
export async function foldInstaller(file, info) {
  const lst = await text(file, info, 'patch.lst'); if (!lst) return null;
  const files = [], skipped = [];
  for (const line of lst.split(/\r?\n/)) {
    const [dest, src] = line.split(';'); if (!dest || !src || !/^data\\/i.test(dest)) continue;
    const parts = [];
    const whole = info.names(src);
    if (whole) parts.push([src, whole]);
    else for (let i = 0; ; i++) { const n = `${src}.${String(i).padStart(3, '0')}`, e = info.names(n); if (!e) break; parts.push([n, e]); }
    const blobs = [];
    for (const [n, e] of parts) { const b = await readMpqFile(file, info.base, info.shift, e, n); if (!b) { blobs.length = 0; break; } blobs.push(b); }
    if (!blobs.length) { skipped.push(dest); continue; }
    const data = new Uint8Array(await new Blob(blobs).arrayBuffer());
    if (data.length < 24 || data[0] !== 0x18 || data[2] !== 4 || data[3] !== 1) { skipped.push(dest); continue; }
    files.push({ name: dest, data: data.subarray(24) });
  }
  return files.length ? { bytes: buildMpq(files), files: files.length, skipped } : null;
}
