// The desktop app's answer rules (electron/lib), run without Electron: the headers the engine needs, HTTP Range,
// the .br sibling, path traversal refused, retail archives resolved from the player's folder, and the 1.13c
// check. electron/main.mjs is a thin wrapper over these; a bug here is a bug in every desktop build.
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { brotliCompressSync } from 'node:zlib';
import { mkdtempSync, mkdirSync, readFileSync, writeFileSync, rmSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { createSiteHandler, HOST, PRIVILEGES, SCHEME } from '../electron/lib/site-protocol.mjs';
import { PATCH_113C, checkRetailDir, isPatch113c, listArchives, readConfig, writeConfig } from '../electron/lib/retail.mjs';
import { brMayAnswer } from '../play/br-check.mjs';
import { PATCH_113C as SHIM_PATCH, isPatch113c as shimIsPatch } from '../tools/patch-shim.mjs';

const tmp = () => mkdtempSync(join(tmpdir(), 'd2-desktop-test-'));
const url = (p) => `${SCHEME}://${HOST}${p}`;
const get = (plan, path, headers = {}, method = 'GET') => plan({ url: url(path), method, headers });
/** The bytes a plan sends (file slice or text), the way main.mjs streams them. */
function bytesOf(p) {
  if (p.head) return Buffer.alloc(0);
  if (p.text !== undefined) return Buffer.from(p.text);
  if (!p.file) return Buffer.alloc(0);
  return readFileSync(p.file).subarray(p.start, p.end + 1);
}

function fixtureSite() {
  const root = tmp();
  const site = join(root, 'site');
  mkdirSync(join(site, 'e', 'v1'), { recursive: true });
  const body = Buffer.from(Array.from({ length: 1000 }, (_, i) => i % 251));
  writeFileSync(join(site, 'index.html'), '<!doctype html>hello');
  writeFileSync(join(site, 'streamfs.js'), 'export const x = 1;\n'.repeat(40));
  writeFileSync(join(site, 'blob.bin'), body);
  writeFileSync(join(root, 'secret.txt'), 'outside the site root');
  return { root, site, body, cleanup: () => rmSync(root, { recursive: true, force: true }) };
}

test('the scheme is privileged the way the engine needs (fetch, streams, secure)', () => {
  assert.deepEqual(PRIVILEGES, { standard: true, secure: true, supportFetchAPI: true, stream: true });
});

test('a site file carries the isolation headers and an accept-ranges answer', () => {
  const f = fixtureSite();
  try {
    const plan = createSiteHandler({ siteRoot: f.site });
    const p = get(plan, '/streamfs.js');
    assert.equal(p.status, 200);
    assert.equal(p.headers['Cross-Origin-Opener-Policy'], 'same-origin');
    assert.equal(p.headers['Cross-Origin-Embedder-Policy'], 'require-corp');
    assert.equal(p.headers['Cross-Origin-Resource-Policy'], 'cross-origin');
    assert.equal(p.headers['Accept-Ranges'], 'bytes');
    assert.equal(p.headers['Content-Type'], 'text/javascript');
    assert.equal(p.headers['Content-Length'], readFileSync(join(f.site, 'streamfs.js')).length);
    assert.equal(p.headers['Cache-Control'], 'no-cache');
    // The root answers index.html; a directory-looking path too.
    assert.equal(get(plan, '/').file, join(f.site, 'index.html'));
    assert.equal(get(plan, '/').headers['Content-Type'], 'text/html');
  } finally { f.cleanup(); }
});

test('e/<hash>/ is immutable, as serve.mjs answers it', () => {
  const f = fixtureSite();
  try {
    writeFileSync(join(f.site, 'e', 'v1', 'd2.wasm'), Buffer.from([0, 97, 115, 109]));
    const p = get(createSiteHandler({ siteRoot: f.site }), '/e/v1/d2.wasm');
    assert.equal(p.headers['Cache-Control'], 'public, max-age=31536000, immutable');
    assert.equal(p.headers['Content-Type'], 'application/wasm');
  } finally { f.cleanup(); }
});

test('HEAD answers the headers with no body (the page sizes archives this way)', () => {
  const f = fixtureSite();
  try {
    const p = get(createSiteHandler({ siteRoot: f.site }), '/blob.bin', {}, 'HEAD');
    assert.equal(p.status, 200);
    assert.equal(p.headers['Content-Length'], 1000);
    assert.equal(p.head, true);
    assert.equal(bytesOf(p).length, 0);
    assert.equal(get(createSiteHandler({ siteRoot: f.site }), '/blob.bin', {}, 'POST').status, 405);
  } finally { f.cleanup(); }
});

test('Range: 206 with the exact slice, suffix and open-ended forms, and 416 past the end', () => {
  const f = fixtureSite();
  try {
    const plan = createSiteHandler({ siteRoot: f.site });
    const a = get(plan, '/blob.bin', { Range: 'bytes=10-19' });
    assert.equal(a.status, 206);
    assert.equal(a.headers['Content-Range'], 'bytes 10-19/1000');
    assert.equal(a.headers['Content-Length'], 10);
    assert.deepEqual(bytesOf(a), f.body.subarray(10, 20));

    const b = get(plan, '/blob.bin', { Range: 'bytes=-5' });
    assert.equal(b.status, 206);
    assert.equal(b.headers['Content-Range'], 'bytes 995-999/1000');
    assert.deepEqual(bytesOf(b), f.body.subarray(995));

    const c = get(plan, '/blob.bin', { range: 'bytes=990-' });   // header names are case-insensitive
    assert.equal(c.status, 206);
    assert.equal(c.headers['Content-Range'], 'bytes 990-999/1000');
    assert.deepEqual(bytesOf(c), f.body.subarray(990));

    const clipped = get(plan, '/blob.bin', { Range: 'bytes=900-5000' });
    assert.equal(clipped.headers['Content-Range'], 'bytes 900-999/1000');

    const bad = get(plan, '/blob.bin', { Range: 'bytes=5000-6000' });
    assert.equal(bad.status, 416);
    assert.equal(bad.headers['Content-Range'], 'bytes */1000');

    // A form this server does not parse is the whole file, as serve.mjs answers it.
    assert.equal(get(plan, '/blob.bin', { Range: 'bytes=0-1,5-6' }).status, 200);
  } finally { f.cleanup(); }
});

test('the .br sibling is chosen only when accepted, matching, and the request is not a Range', () => {
  const f = fixtureSite();
  try {
    const plain = readFileSync(join(f.site, 'streamfs.js'));
    writeFileSync(join(f.site, 'streamfs.js.br'), brotliCompressSync(plain));
    const plan = createSiteHandler({ siteRoot: f.site, canUseBr: brMayAnswer });
    const br = get(plan, '/streamfs.js', { 'Accept-Encoding': 'gzip, deflate, br, zstd' });
    assert.equal(br.headers['Content-Encoding'], 'br');
    assert.equal(br.file, join(f.site, 'streamfs.js.br'));
    assert.equal(br.headers['Vary'], 'Accept-Encoding');
    assert.equal(get(plan, '/streamfs.js', { 'Accept-Encoding': 'gzip' }).headers['Content-Encoding'], undefined);
    assert.equal(get(plan, '/streamfs.js', {}).headers['Content-Encoding'], undefined);
    // Range asks for bytes of the plain file, never of the compressed one.
    const ranged = get(plan, '/streamfs.js', { 'Accept-Encoding': 'br', Range: 'bytes=0-9' });
    assert.equal(ranged.file, join(f.site, 'streamfs.js'));
    assert.equal(ranged.status, 206);
  } finally { f.cleanup(); }
});

test('a .br that is not the file beside it is never served (content check, not mtime)', () => {
  const f = fixtureSite();
  try {
    writeFileSync(join(f.site, 'streamfs.js.br'), brotliCompressSync(Buffer.from('an older engine')));
    const plan = createSiteHandler({ siteRoot: f.site, canUseBr: brMayAnswer });
    const p = get(plan, '/streamfs.js', { 'Accept-Encoding': 'br' });
    assert.equal(p.headers['Content-Encoding'], undefined);
    assert.equal(p.file, join(f.site, 'streamfs.js'));
  } finally { f.cleanup(); }
});

test('traversal is refused: nothing outside the site root is answered', () => {
  const f = fixtureSite();
  try {
    const plan = createSiteHandler({ siteRoot: f.site });
    const attempts = ['/../secret.txt', '/%2e%2e/secret.txt', '/%2E%2E/secret.txt', '/..%2fsecret.txt', '/..%5csecret.txt',
      '/a/%2e%2e/%2e%2e/secret.txt', '/%5c..%5csecret.txt', '/x%00.html', '/a//b', '/%ZZ'];
    for (const path of attempts) {
      const p = get(plan, path);
      assert.ok(p.status === 404 || p.status === 400, `${path} answered ${p.status}`);
      assert.equal(p.file, undefined, `${path} resolved to a file`);
    }
    // The URL parser folds a leading ../ to the root, so the outside file is only reachable by name from inside: still refused.
    const folded = get(plan, '/../secret.txt');
    assert.notEqual(folded.file, join(f.root, 'secret.txt'));
  } finally { f.cleanup(); }
});

test('requests for another origin or host are refused', () => {
  const f = fixtureSite();
  try {
    const plan = createSiteHandler({ siteRoot: f.site });
    assert.equal(plan({ url: 'https://d2/index.html', method: 'GET', headers: {} }).status, 404);
    assert.equal(plan({ url: 'app://evil/index.html', method: 'GET', headers: {} }).status, 404);
    assert.equal(plan({ url: 'not a url', method: 'GET', headers: {} }).status, 400);
  } finally { f.cleanup(); }
});

test('retail archives resolve from the player folder, case-insensitively, and nothing else', () => {
  const f = fixtureSite();
  const game = tmp();
  try {
    writeFileSync(join(game, 'D2DATA.MPQ'), Buffer.from('MPQ\x1a' + 'x'.repeat(2000), 'latin1'));
    writeFileSync(join(game, 'readme.txt'), 'not an archive');
    mkdirSync(join(game, 'sub'));
    const map = listArchives(game);
    assert.deepEqual([...map.keys()], ['d2data.mpq']);
    const plan = createSiteHandler({ siteRoot: f.site, retailFiles: () => map, siteConfig: () => ({ assets: 'hosted' }) });

    const p = get(plan, '/retail/d2data.mpq');
    assert.equal(p.status, 200);
    assert.equal(p.file, join(game, 'D2DATA.MPQ'));
    assert.equal(p.headers['Content-Length'], 2004);
    assert.equal(p.headers['Content-Type'], 'application/octet-stream');
    assert.equal(p.headers['Cache-Control'], 'no-cache');
    assert.equal(p.headers['Content-Encoding'], undefined);

    const upper = get(plan, '/retail/D2Data.MPQ', { Range: 'bytes=0-3' });
    assert.equal(upper.status, 206);
    assert.equal(bytesOf(upper).toString('latin1'), 'MPQ\x1a');
    assert.equal(upper.headers['Content-Range'], 'bytes 0-3/2004');

    assert.equal(get(plan, '/retail/d2exp.mpq').status, 404);
    assert.equal(get(plan, '/retail/readme.txt').status, 404);
    assert.equal(get(plan, '/retail/sub').status, 404);
    assert.equal(get(plan, '/retail/sub/d2data.mpq').status, 404);
    assert.equal(get(plan, '/retail/%2e%2e/d2data.mpq').status, 404);
    assert.equal(get(plan, '/retail/..%2fsecret.txt').status, 404);
  } finally { f.cleanup(); rmSync(game, { recursive: true, force: true }); }
});

test('no retail folder: /retail/ is 404 and site-config.json says byo', () => {
  const f = fixtureSite();
  try {
    const plan = createSiteHandler({ siteRoot: f.site, retailFiles: () => null, siteConfig: () => ({ assets: 'byo' }) });
    assert.equal(get(plan, '/retail/d2data.mpq').status, 404);
    const cfg = get(plan, '/site-config.json');
    assert.equal(cfg.status, 200);
    assert.equal(JSON.parse(cfg.text).assets, 'byo');
  } finally { f.cleanup(); }
});

test('with a retail folder, site-config.json says hosted (the page then mounts /retail/)', () => {
  const f = fixtureSite();
  try {
    const plan = createSiteHandler({ siteRoot: f.site, retailFiles: () => new Map(), siteConfig: () => ({ assets: 'hosted' }) });
    assert.equal(JSON.parse(get(plan, '/site-config.json').text).assets, 'hosted');
    assert.equal(get(plan, '/site-config.json', {}, 'HEAD').text, undefined);
  } finally { f.cleanup(); }
});

test('the 1.13c constants repeated in the package match tools/patch-shim.mjs', () => {
  assert.deepEqual(PATCH_113C, SHIM_PATCH);
  assert.equal(isPatch113c(Buffer.from('not a patch')), false);
  assert.equal(shimIsPatch(Buffer.from('not a patch')), false);
});

test('a folder without d2data.mpq or with a wrong patch is refused, with the reasons', () => {
  const game = tmp();
  try {
    const empty = checkRetailDir(game);
    assert.equal(empty.ok, false);
    assert.ok(empty.problems.includes('d2data.mpq is missing'));
    assert.ok(empty.problems.includes('Patch_D2.mpq is missing'));

    writeFileSync(join(game, 'd2data.mpq'), 'not an archive at all');
    writeFileSync(join(game, 'Patch_D2.mpq'), 'MPQ\x1a older patch bytes');
    const bad = checkRetailDir(game);
    assert.equal(bad.ok, false);
    assert.ok(bad.problems.includes('d2data.mpq is not an MPQ archive'));
    assert.ok(bad.problems.some((p) => p.startsWith('Patch_D2.mpq is not the 1.13c patch')));
  } finally { rmSync(game, { recursive: true, force: true }); }
});

test('a real 1.13c folder passes (set D2_TEST_RETAIL_DIR to a folder with the player files; skipped otherwise)', { skip: !process.env.D2_TEST_RETAIL_DIR }, () => {
  const r = checkRetailDir(process.env.D2_TEST_RETAIL_DIR);
  assert.deepEqual(r.problems, []);
  assert.equal(r.ok, true);
});

test('the saved folder round-trips and a missing config is "never asked"', () => {
  const dir = tmp();
  try {
    const file = join(dir, 'userdata', 'config.json');
    assert.deepEqual(readConfig(file), {});
    writeConfig(file, { retailDir: '/games/d2' });
    assert.deepEqual(readConfig(file), { retailDir: '/games/d2' });
    writeConfig(file, { retailDir: null, declined: true });
    assert.deepEqual(readConfig(file), { retailDir: null, declined: true });
    writeFileSync(file, '{ broken');
    assert.deepEqual(readConfig(file), {});
  } finally { rmSync(dir, { recursive: true, force: true }); }
});
