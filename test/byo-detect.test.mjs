// Bring-your-own-files detection: roles by table shape, version ladder, installer unpack, PKWARE reader.
import test from 'node:test';
import assert from 'node:assert/strict';
import { existsSync, readFileSync, openAsBlob } from 'node:fs';
import { buildMpq } from '../tools/mkmpq.mjs';
import { inspect, classify, extractInnerPatch, ROLES } from '../play/assets/byo-detect.mjs';
import { explode } from '../play/assets/mpq-read.mjs';

const known = JSON.parse(readFileSync(new URL('../play/assets/known-mpqs.json', import.meta.url)));
const file = (parts, name) => new File(parts, name);

// A 512-slot archive holding a Levels.txt: the shape of a Patch_D2.mpq, with none of its content.
function fakePatch(extra = []) {
  const files = [{ name: 'data\\global\\excel\\Levels.txt', data: Buffer.from('x') }, { name: 'data\\global\\excel\\compcode.bin', data: Buffer.from('x') }, ...extra];
  for (let i = 0; files.length < 200; i++) files.push({ name: `pad\\${i}`, data: Buffer.from([i]) });
  return Buffer.from(buildMpq(files));
}

test('a patch-shaped archive is recognised by its tables, whatever it is called', async () => {
  const i = await inspect(file([fakePatch()], 'renamed.bin'));
  assert.equal(i.kind, 'mpq');
  assert.equal(i.role, 'patch_d2');
  assert.equal(i.truncated, false);
});

test('an MPQ with the patch shape but no game tables is not a patch', async () => {
  const files = [{ name: 'readme.txt', data: Buffer.from('x') }];
  for (let i = 0; files.length < 200; i++) files.push({ name: `pad\\${i}`, data: Buffer.from([i]) });
  const i = await inspect(file([Buffer.from(buildMpq(files))], 'mod.mpq'));
  assert.equal(i.role, undefined);
});

test('a short copy is reported incomplete', async () => {
  const p = fakePatch();
  const i = await inspect(file([p.subarray(0, p.length - 100)], 'patch_d2.mpq'));
  assert.equal(i.truncated, true);
});

test('wrong magic and CASC names are not MPQs', async () => {
  assert.equal((await inspect(file([Buffer.alloc(4096, 7)], 'patch_d2.mpq'))).kind, 'other');
  assert.equal((await inspect(file([Buffer.alloc(64)], 'data.001'))).kind, 'casc');
  const c = classify([await inspect(file([Buffer.alloc(64)], '.build.info'))], known);
  assert.equal(c.issues[0].code, 'casc');
  assert.equal(c.ready, false);
});

test('an unknown patch version is accepted without a warning', async () => {
  const c = classify([await inspect(file([fakePatch()], 'Patch_D2.mpq'))], known);
  assert.equal(c.patch.level, 'unknown');
  assert.equal(c.issues.length, 0);
  assert.deepEqual(c.missing.includes('patch_d2'), false);
});

test('the patch inside an official installer is lifted out and detected', async () => {
  const inner = fakePatch();
  const archive = Buffer.from(buildMpq([{ name: 'Patch_D2.mpq', data: inner }]));
  const exe = Buffer.concat([Buffer.from('MZ'), Buffer.alloc(1024 - 2), archive]);
  const i = await inspect(file([exe], 'D2-1.13c-patch.exe'));
  assert.equal(i.installer, true);
  assert.equal(i.base, 1024);
  const got = await extractInnerPatch(file([exe], 'D2-1.13c-patch.exe'), i);
  assert.ok(got);
  assert.equal(got.size, inner.length);
  assert.equal(got.info.role, 'patch_d2');
});

test('PKWARE explode reproduces a StormLib-imploded stream', () => {
  const plain = Buffer.from('abcabcabcabc hello hello hello hello 0123456789'.repeat(3));
  // imploded by StormLib (binary, dict 1024); see tools/mkmpq.mjs for the writer used elsewhere
  const hex = readFileSync(new URL('./fixtures/pkware-1024.hex', import.meta.url), 'utf8').trim();
  assert.deepEqual(Buffer.from(explode(Buffer.from(hex, 'hex'), plain.length)), plain);
});

const real = existsSync(new URL('../retail/D2/patch_d2.mpq', import.meta.url));
test('retail: every archive gets its own role and 1.13c is verified', { skip: !real }, async () => {
  const dir = new URL('../retail/D2/', import.meta.url);
  const infos = [];
  for (const r of Object.values(ROLES)) {
    const p = new URL(r.file, dir);
    if (existsSync(p)) infos.push(await inspect(new File([await openAsBlob(p.pathname)], 'x' + infos.length)));
  }
  const c = classify(infos, known);
  assert.equal(c.ready, true);
  assert.equal(c.patch.level, 'verified');
  assert.equal(new Set(infos.map((x) => x.role)).size, infos.length);
});
