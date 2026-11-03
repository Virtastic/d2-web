// Art cut from the player's own archives at run time, cached under userData/derived/. The package never holds it:
// door-art/ (the title-screen art, tools/door-art.mjs) is generated here, once, and regenerated only when the
// archives' sizes or mtimes change. app://d2/door-art/... is answered from this folder (lib/site-protocol.mjs).
//
// What is not derived: widebg/ (the widescreen menu backgrounds) are painted images registered by hand against
// the game's menus (tools/widebg-register.mjs); no archive holds them, so they ship in the package (as in the Docker image)
// The 1.13c patch shim is not needed either: the desktop app serves the player's own
// Patch_D2.mpq as /retail/ (hosted mode); the shim is only the byo page's way of replacing a player's patch.
import { createHash } from 'node:crypto';
import { spawn } from 'node:child_process';
import { existsSync, mkdirSync, readFileSync, renameSync, rmSync, statSync, writeFileSync } from 'node:fs';
import { join } from 'node:path';

const ARCHIVES = ['patch_d2.mpq', 'd2exp.mpq', 'd2data.mpq'];
export const DERIVE_VERSION = 1;   // bump when the generator's output changes

/** Hash of the generator version and each source archive's name, size and mtime. null when an archive is missing. */
export function stampOf(archives) {
  const h = createHash('sha256').update(`v${DERIVE_VERSION}|`);
  for (const n of ARCHIVES) {
    const p = archives.get(n);
    if (!p) return null;
    const st = statSync(p);
    h.update(`${n}:${st.size}:${Math.floor(st.mtimeMs)}|`);
  }
  return h.digest('hex').slice(0, 32);
}

/** Runs `node script args` with the running binary as node. Resolves { code, err }. */
function runNode(script, args) {
  return new Promise((resolve) => {
    const p = spawn(process.execPath, [script, ...args], { env: { ...process.env, ELECTRON_RUN_AS_NODE: '1' }, stdio: ['ignore', 'ignore', 'pipe'], windowsHide: true });
    let err = ''; p.stderr.on('data', (d) => { err += d; });
    p.on('error', (e) => resolve({ code: -1, err: String(e) }));
    p.on('close', (code) => resolve({ code, err }));
  });
}

/**
 * ensureDerived({ derivedRoot, archives, siteRoot, srcDir, log }) resolves to the directory holding door-art/
 * (derivedRoot/current), generating it when the cache is stale or missing. A failure leaves no half-written
 * folder and resolves to null: the page then cuts the art from the archives itself.
 */
export async function ensureDerived({ derivedRoot, archives, siteRoot, srcDir, log = () => {} }) {
  const stamp = stampOf(archives);
  if (!stamp) return null;
  const cur = join(derivedRoot, 'current');
  try { if (JSON.parse(readFileSync(join(cur, 'stamp.json'), 'utf8')).stamp === stamp && existsSync(join(cur, 'door-art', 'meta.json'))) return cur; } catch { /* regenerate */ }
  const script = join(siteRoot, 'derive', 'door-art.mjs');
  if (!existsSync(script)) { log('derive: no generator in the site'); return null; }
  const tmp = join(derivedRoot, 'tmp-' + process.pid);
  rmSync(tmp, { recursive: true, force: true });
  mkdirSync(tmp, { recursive: true });
  const r = await runNode(script, [srcDir, join(tmp, 'door-art')]);
  if (r.code !== 0 || !existsSync(join(tmp, 'door-art', 'meta.json'))) {
    log(`derive: door-art failed (${r.code}) ${r.err.trim().split('\n').pop() || ''}`);
    rmSync(tmp, { recursive: true, force: true });
    return null;
  }
  writeFileSync(join(tmp, 'stamp.json'), JSON.stringify({ stamp }));
  rmSync(cur, { recursive: true, force: true });
  renameSync(tmp, cur);
  return cur;
}
