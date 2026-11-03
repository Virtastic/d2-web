// The player's Diablo II folder, as the desktop shell sees it: which archives it has, whether the patch is
// the 1.13c Patch_D2.mpq, and the config that remembers the folder. Pure Node, no Electron.
//
// The 1.13c check is tools/patch-shim.mjs's (PATCH_113C, isPatch113c). The package cannot import that file
// (it is not shipped), so the two constants are repeated here and test/desktop-protocol.test.mjs fails if they
// drift. Nothing in this module copies or stores game data: it reads sizes, headers and one hash.
import { createHash } from 'node:crypto';
import { closeSync, openSync, readFileSync, readSync, readdirSync, statSync, writeFileSync, mkdirSync, renameSync } from 'node:fs';
import { dirname, join } from 'node:path';

export const PATCH_113C = { size: 8229959, sha256: '41a5fc95977369e3d5f32127cc4c0d571daad9cf706b6b6523935b705afc5a6d' };
export const REQUIRED = ['d2data.mpq'];

/** Map of lower-case file name -> absolute path for every .mpq directly in dir. Empty when dir is unreadable. */
export function listArchives(dir) {
  const out = new Map();
  let names;
  try { names = readdirSync(dir); } catch { return out; }
  for (const n of names) {
    if (!/\.mpq$/i.test(n)) continue;
    const p = join(dir, n);
    try { if (statSync(p).isFile()) out.set(n.toLowerCase(), p); } catch { /* a dangling link is not an archive */ }
  }
  return out;
}

/**
 * checkRetailDir(dir) -> { ok, problems: string[], archives: Map }
 * ok only when d2data.mpq is an MPQ and the folder's Patch_D2.mpq is exactly the 1.13c patch.
 * Other expansion archives are served when present (the page mounts whichever it finds) and are not required here.
 */
export function checkRetailDir(dir) {
  const archives = listArchives(dir);
  const problems = [];
  for (const n of REQUIRED) if (!archives.has(n)) problems.push(`${n} is missing`);
  const patch = archives.get('patch_d2.mpq');
  if (!patch) problems.push('Patch_D2.mpq is missing');
  else if (!isPatch113c(readFileSync(patch))) problems.push('Patch_D2.mpq is not the 1.13c patch (update the game to 1.13c)');
  for (const n of REQUIRED) {
    const p = archives.get(n);
    if (p && !isMpq(p)) problems.push(`${n} is not an MPQ archive`);
  }
  return { ok: problems.length === 0, problems, archives };
}

/** First four bytes are the MPQ magic. Reads 4 bytes, not the archive (d2data.mpq is large). */
function isMpq(path) {
  const b = Buffer.alloc(4);
  let fd;
  try { fd = openSync(path, 'r'); return readSync(fd, b, 0, 4, 0) === 4 && b.readUInt32LE(0) === 0x1a51504d; }
  catch { return false; } finally { if (fd !== undefined) closeSync(fd); }
}

export function isPatch113c(buf) {
  return buf.length === PATCH_113C.size && createHash('sha256').update(buf).digest('hex') === PATCH_113C.sha256;
}

/** The saved choice: { retailDir: string | null, declined?: true }. Missing or unreadable is "never asked". */
export function readConfig(file) {
  try {
    const c = JSON.parse(readFileSync(file, 'utf8'));
    return c && typeof c === 'object' ? c : {};
  } catch { return {}; }
}

/** Writes via a temporary file so a crash mid-write never leaves a half config behind. */
export function writeConfig(file, config) {
  mkdirSync(dirname(file), { recursive: true });
  const tmp = file + '.tmp';
  writeFileSync(tmp, JSON.stringify(config, null, 2) + '\n');
  renameSync(tmp, file);
}

