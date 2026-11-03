// The desktop app's folder detection and run-time art cache (electron/lib/detect.mjs, derive.mjs), run without
// Electron on fake folder trees: which folders are tried per OS, the registry parse, that only a validated
// folder is accepted, the cache stamp, and that door-art/ is answered from the derived folder (never the site).
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { mkdtempSync, mkdirSync, writeFileSync, rmSync, utimesSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { candidateDirs, detectRetailDir, parseRegInstallPath } from '../electron/lib/detect.mjs';
import { stampOf } from '../electron/lib/derive.mjs';
import { createSiteHandler } from '../electron/lib/site-protocol.mjs';

const tmp = () => mkdtempSync(join(tmpdir(), 'd2-detect-test-'));

test('parseRegInstallPath reads the REG_SZ InstallPath value', () => {
  const out = '\r\nHKEY_LOCAL_MACHINE\\Software\\Blizzard Entertainment\\Diablo II\r\n    InstallPath    REG_SZ    C:\\Games\\Diablo II\r\n';
  assert.equal(parseRegInstallPath(out), 'C:\\Games\\Diablo II');
  assert.equal(parseRegInstallPath('ERROR: not found'), null);
});

test('windows: the registry path comes first, then Program Files', () => {
  const run = (c, a) => { assert.equal(c, 'reg'); if (a[1].startsWith('HKLM\\Software\\Blizzard')) return '    InstallPath    REG_SZ    D:\\Diablo II\n'; throw new Error('no key'); };
  const dirs = candidateDirs({ platform: 'win32', env: { ProgramFiles: 'C:\\Program Files' }, home: 'C:\\h', run, ls: () => [] });
  assert.equal(dirs[0], 'D:\\Diablo II');
  assert.ok(dirs.some((d) => /Program Files.Diablo II$/.test(d)));
});

test('mac: /Applications/Diablo II* folders, and a game folder inside an app folder', () => {
  const ls = (d) => (d === '/Applications' ? ['Safari.app', 'Diablo II', 'Diablo II Resurrected'] : []);
  const dirs = candidateDirs({ platform: 'darwin', env: {}, home: '/Users/p', ls });
  assert.ok(dirs.includes('/Applications/Diablo II'));
  assert.ok(dirs.includes('/Applications/Diablo II/Diablo II'));
  assert.ok(!dirs.includes('/Applications/Safari.app'));
});

test('linux: wine prefixes (default, named, WINEPREFIX), Lutris/Games and Steam style paths', () => {
  const ls = (d) => ({ '/h': ['.wine', '.wine-d2', 'Documents'], '/h/Games': ['diablo-ii'], '/h/Games/diablo-ii': [] })[d] || [];
  const dirs = candidateDirs({ platform: 'linux', env: { WINEPREFIX: '/w/pfx' }, home: '/h', ls });
  for (const want of ['/h/.wine/drive_c/Program Files (x86)/Diablo II', '/h/.wine-d2/drive_c/Program Files/Diablo II',
    '/w/pfx/drive_c/Program Files (x86)/Diablo II', '/h/Games/Diablo II', '/h/.local/share/Steam/steamapps/common/Diablo II'])
    assert.ok(dirs.includes(want), want);
  assert.ok(dirs.indexOf('/w/pfx/drive_c/Program Files (x86)/Diablo II') < dirs.indexOf('/h/.wine/drive_c/Program Files (x86)/Diablo II'));
});

test('detectRetailDir returns the first existing folder the check accepts, never one it rejects', () => {
  const have = new Set(['/h/.wine/drive_c/Program Files (x86)/Diablo II', '/h/Games/Diablo II']);
  const opts = { platform: 'linux', env: {}, home: '/h', ls: () => [], exists: (d) => have.has(d) };
  assert.equal(detectRetailDir((d) => ({ ok: d === '/h/Games/Diablo II' }), opts), '/h/Games/Diablo II');
  assert.equal(detectRetailDir(() => ({ ok: false }), opts), null);
  assert.equal(detectRetailDir(() => { throw new Error('unreadable'); }, opts), null);
});

test('the art cache stamp follows the archives size and mtime, and is null when one is missing', () => {
  const dir = tmp();
  try {
    const m = new Map(['patch_d2.mpq', 'd2exp.mpq', 'd2data.mpq'].map((n) => { const p = join(dir, n); writeFileSync(p, 'x'.repeat(10)); return [n, p]; }));
    const a = stampOf(m);
    assert.match(a, /^[0-9a-f]{32}$/);
    assert.equal(stampOf(m), a);
    utimesSync(m.get('d2data.mpq'), new Date(2001, 1, 1), new Date(2001, 1, 1));
    assert.notEqual(stampOf(m), a);
    m.delete('d2exp.mpq');
    assert.equal(stampOf(m), null);
  } finally { rmSync(dir, { recursive: true, force: true }); }
});

test('door-art/ is answered from the derived folder and marked; absent there it is a 404, the site never holds it', () => {
  const root = tmp();
  try {
    mkdirSync(join(root, 'site'), { recursive: true });
    mkdirSync(join(root, 'derived', 'door-art'), { recursive: true });
    writeFileSync(join(root, 'site', 'index.html'), 'hi');
    writeFileSync(join(root, 'derived', 'door-art', 'meta.json'), '{"a":1}');
    writeFileSync(join(root, 'derived', 'secret'), 'no');
    let derived = join(root, 'derived');
    const plan = createSiteHandler({ siteRoot: join(root, 'site'), derivedDir: () => derived });
    const get = (p) => plan({ url: 'app://d2' + p, method: 'GET', headers: {} });
    const ok = get('/door-art/meta.json');
    assert.equal(ok.status, 200);
    assert.equal(ok.file, join(root, 'derived', 'door-art', 'meta.json'));
    assert.equal(ok.headers['X-D2-Derived'], '1');
    assert.equal(get('/door-art/missing.png').status, 404);
    assert.equal(get('/door-art/../secret').status, 404);
    derived = null;
    assert.equal(get('/door-art/meta.json').status, 404);
    assert.equal(get('/index.html').headers['X-D2-Derived'], undefined);
  } finally { rmSync(root, { recursive: true, force: true }); }
});
