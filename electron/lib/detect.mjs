// Finds the player's Diablo II folder without asking: the Windows registry's Blizzard InstallPath, Program Files,
// the macOS Applications folders, and the usual places a Linux player keeps it (wine prefixes, Lutris, Steam,
// Bottles, ~/Games). Every candidate is validated by the caller's check (lib/retail.mjs checkRetailDir), so a
// folder that merely has the right name is never trusted. Pure Node, no Electron. Reads names and sizes only.
import { execFileSync } from 'node:child_process';
import { existsSync, readdirSync } from 'node:fs';
import { join } from 'node:path';

const REG_KEYS = [
  ['HKCU', 'Software\\Blizzard Entertainment\\Diablo II'],
  ['HKLM', 'Software\\Blizzard Entertainment\\Diablo II'],
  ['HKLM', 'Software\\WOW6432Node\\Blizzard Entertainment\\Diablo II'],
];

/** The InstallPath value out of `reg query` output (REG_SZ lines), or null. */
export function parseRegInstallPath(text) {
  const m = /^\s*InstallPath\s+REG_SZ\s+(.+?)\s*$/im.exec(String(text || ''));
  return m ? m[1] : null;
}

function registryCandidates(run) {
  const out = [];
  for (const [hive, key] of REG_KEYS) {
    try { const p = parseRegInstallPath(run('reg', ['query', `${hive}\\${key}`, '/v', 'InstallPath'])); if (p) out.push(p); } catch { /* key absent */ }
  }
  return out;
}

/** Child directories of dir whose name matches re (case-insensitive), as full paths. */
function kids(dir, re, ls) {
  try { return ls(dir).filter((n) => re.test(n)).map((n) => join(dir, n)); } catch { return []; }
}

/**
 * candidateDirs({ platform, env, home, run, ls }) gives the ordered list of folders that might hold the game.
 * run and ls are injectable so a test needs no registry and no real folders.
 */
export function candidateDirs({ platform = process.platform, env = process.env, home = env.HOME || env.USERPROFILE || '',
  run = (c, a) => execFileSync(c, a, { encoding: 'utf8', stdio: ['ignore', 'pipe', 'ignore'], windowsHide: true }),
  ls = (d) => readdirSync(d) } = {}) {
  const out = [];
  const d2 = /^diablo ?(ii|2)\b/i;
  const fromBase = (base) => {
    out.push(join(base, 'Diablo II'), ...kids(base, d2, ls));
    for (const b of kids(base, /^blizzard/i, ls)) out.push(...kids(b, d2, ls));
  };
  if (platform === 'win32') {
    out.push(...registryCandidates(run));
    for (const base of [env.ProgramFiles, env['ProgramFiles(x86)'], env.ProgramW6432, 'C:\\Program Files (x86)', 'C:\\Program Files', 'C:\\Games', 'D:\\Games']) {
      if (base) fromBase(base);
    }
  } else if (platform === 'darwin') {
    for (const base of ['/Applications', join(home, 'Applications'), '/Users/Shared']) {
      for (const k of kids(base, d2, ls)) out.push(k, join(k, 'Diablo II'));   // the folder, or an app folder holding it
    }
  } else {
    const prefixes = [join(home, '.wine'), ...kids(home, /^\.wine/i, ls), ...kids(join(home, '.local/share/wineprefixes'), /./, ls),
      ...kids(join(home, 'Games'), /./, ls), ...kids(join(home, '.var/app/com.usebottles.bottles/data/bottles/bottles'), /./, ls),
      ...kids(join(home, '.local/share/bottles/bottles'), /./, ls)];
    if (env.WINEPREFIX) prefixes.unshift(env.WINEPREFIX);
    for (const pre of new Set(prefixes)) {
      for (const pf of ['Program Files (x86)', 'Program Files', 'GOG Games', 'Games']) fromBase(join(pre, 'drive_c', pf));
    }
    for (const base of [join(home, 'Games'), join(home, 'games'), join(home, '.local/share/Steam/steamapps/common'),
      join(home, '.steam/steam/steamapps/common'), '/opt', '/usr/local/games', '/mnt/games']) {
      out.push(join(base, 'Diablo II'), ...kids(base, d2, ls));
    }
    out.push(join(home, 'Diablo II'));
  }
  return [...new Set(out)];
}

/** First candidate that check() accepts (check is checkRetailDir), or null. Folders that do not exist are skipped. */
export function detectRetailDir(check, opts = {}) {
  const exists = opts.exists || existsSync;
  for (const dir of candidateDirs(opts)) {
    if (!exists(dir)) continue;
    let r; try { r = check(dir); } catch { continue; }
    if (r && r.ok) return dir;
  }
  return null;
}
