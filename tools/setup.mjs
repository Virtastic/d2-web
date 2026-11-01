#!/usr/bin/env node
// One-step setup for building and playing from source: finds your Diablo II files, checks the patch is
// 1.13c, links them into retail/D2/ (where the build and the local server look) and checks the build tools.
//   node tools/setup.mjs "/path/to/Diablo II"                  your installed game folder
//   node tools/setup.mjs "/path/to/Diablo II" --patch <file>   take Patch_D2.mpq from somewhere else
//   node tools/setup.mjs                                        re-check what is already set up
// Safe to re-run. It never copies game files into git (retail/ is ignored) and never overwrites a real file.
import fs from 'node:fs';
import path from 'node:path';
import { spawnSync } from 'node:child_process';
import { fileURLToPath } from 'node:url';
import { isPatch113c } from './patch-shim.mjs';

const ROOT = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const DEST = path.join(ROOT, 'retail', 'D2');
const tty = process.stdout.isTTY;
const c = (n, s) => (tty ? `\x1b[${n}m${s}\x1b[0m` : s);
const OK = c(32, '✓'), BAD = c(31, '✗'), WARN = c(33, '!');
const say = (mark, what, note = '') => console.log(`  ${mark} ${what.padEnd(16)} ${note}`);

// The game needs every file; the build itself needs only the first three (the title screen's art and the patch).
const FILES = [
  ['patch_d2.mpq', 'The 1.13c patch', true], ['d2exp.mpq', 'Lord of Destruction', true],
  ['d2data.mpq', 'Maps, monsters, items', true], ['d2char.mpq', 'The heroes'], ['d2sfx.mpq', 'Sound effects'],
  ['d2speech.mpq', 'Speech'], ['d2music.mpq', 'Music'], ['d2video.mpq', 'Cinematics'],
  ['d2xmusic.mpq', 'Expansion music'], ['d2xtalk.mpq', 'Expansion speech'], ['d2xvideo.mpq', 'Expansion cinematics'],
];

const args = process.argv.slice(2);
const pi = args.indexOf('--patch');
const patchArg = pi >= 0 ? args.splice(pi, 2)[1] : null;
if (args[0] === '-h' || args[0] === '--help' || (pi >= 0 && !patchArg)) {
  console.log('usage: node tools/setup.mjs ["/path/to/Diablo II"] [--patch /path/to/Patch_D2.mpq]');
  process.exit(0);
}
const src = args[0] ? path.resolve(args[0]) : null;
if (src && !fs.statSync(src, { throwIfNoEntry: false })?.isDirectory()) {
  console.error(`${BAD} ${src} is not a folder. Give the folder Diablo II is installed in (the one holding d2data.mpq).`);
  process.exit(1);
}

// Case-insensitive lookup: installs ship D2DATA.MPQ, d2data.mpq or D2Data.mpq.
const listing = (dir) => {
  const m = new Map();
  for (const n of fs.readdirSync(dir, { throwIfNoEntry: false }) ?? []) m.set(n.toLowerCase(), path.join(dir, n));
  return m;
};

// Link (or, where links are not allowed, copy) one file into retail/D2/ under its lower-case name.
function place(from, name) {
  const to = path.join(DEST, name);
  if (path.resolve(from) === path.resolve(to)) return 'here';
  const st = fs.lstatSync(to, { throwIfNoEntry: false });
  if (st && !st.isSymbolicLink()) return fs.realpathSync(from) === fs.realpathSync(to) ? 'here' : 'kept';
  if (st) fs.unlinkSync(to);
  try { fs.symlinkSync(from, to); return 'linked'; } catch {}
  try { fs.linkSync(from, to); return 'linked'; } catch {}
  fs.copyFileSync(from, to); return 'copied';
}

console.log(`\n${c(1, 'd2-web setup')}\n`);
fs.mkdirSync(DEST, { recursive: true });
const have = src ? listing(src) : new Map();
if (patchArg) {
  if (!fs.existsSync(patchArg)) { console.error(`${BAD} --patch ${patchArg}: no such file`); process.exit(1); }
  have.set('patch_d2.mpq', path.resolve(patchArg));
}
// Files set up on an earlier run stay counted even when this run's folder lacks them.
const placed = listing(DEST);

console.log(c(1, `Game files${src ? ` from ${src}` : ' in retail/D2'}`));
let buildReady = true, playReady = true;
for (const [name, what, forBuild] of FILES) {
  const from = have.get(name);
  let how = null;
  if (from) {
    try { how = place(from, name); } catch (e) { say(BAD, name, `could not link or copy: ${e.message}`); buildReady &&= !forBuild; playReady = false; continue; }
  } else if (!placed.has(name) || !fs.existsSync(placed.get(name))) {
    say(forBuild ? BAD : WARN, name, `${what} -- not found${forBuild ? '' : ' (the build works without it; the game needs it to play)'}`);
    if (forBuild) buildReady = false;
    playReady = false;
    continue;
  }
  const at = path.join(DEST, name);
  if (name === 'patch_d2.mpq' && !isPatch113c(fs.readFileSync(at))) {
    say(BAD, name, `not the 1.13c patch. Update the game to 1.13c, or run again with --patch /path/to/a/1.13c/Patch_D2.mpq${how === 'kept' ? ` (retail/D2/${name} is a real file: move it aside first)` : ''}`);
    buildReady = playReady = false;
    continue;
  }
  const note = { linked: 'linked', copied: 'copied', kept: 'already in retail/D2 (kept)', here: '' }[how] ?? 'already set up';
  say(OK, name, `${what}${note ? c(2, `  ${note}`) : ''}`);
}

console.log(`\n${c(1, 'Build tools')}`);
const major = +process.versions.node.split('.')[0];
say(major >= 24 ? OK : WARN, 'Node.js', major >= 24 ? process.versions.node : `${process.versions.node} -- Node 24 or newer is recommended (https://nodejs.org)`);
const docker = spawnSync('docker', ['version', '--format', '{{.Server.Version}}'], { encoding: 'utf8' });
if (docker.error) { say(BAD, 'Docker', 'not installed -- the build compiles inside Docker (https://docs.docker.com/get-docker/)'); buildReady = false; }
else if (docker.status !== 0) { say(BAD, 'Docker', 'installed but not running, or this user cannot use it -- start Docker, then run this again'); buildReady = false; }
else say(OK, 'Docker', docker.stdout.trim());
const storm = path.join(ROOT, 'third_party', 'StormLib', 'CMakeLists.txt');
if (!fs.existsSync(storm)) {
  console.log(`  ${c(2, '…')} StormLib         fetching (git submodule update --init)`);
  spawnSync('git', ['submodule', 'update', '--init'], { cwd: ROOT, stdio: 'inherit' });
}
if (fs.existsSync(storm)) say(OK, 'StormLib', 'third_party/StormLib');
else { say(BAD, 'StormLib', 'missing -- run: git submodule update --init'); buildReady = false; }

console.log('');
if (!buildReady) {
  console.log(`${BAD} Not ready yet. Fix the ${BAD} lines above, then run this again.\n`);
  process.exit(1);
}
console.log(`${OK} ${c(1, 'Ready to build.')} Next:\n`);
console.log('    bash ci/build.sh');
console.log('    D2_RETAIL_DIR=retail/D2 node play/serve.mjs 8920 build-wasm/site');
console.log('\n  then open http://localhost:8920 in Chrome.\n');
if (!playReady) console.log(`${WARN} Some game files are missing: the build works, but the game needs every file to play.\n`);
