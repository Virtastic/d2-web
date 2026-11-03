// The desktop app's startup decisions (electron/lib/startup.mjs), run without Electron on fake folders.
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { resolveStartup, folderFromPick, validatePick, configFor, NOT_FOUND } from '../electron/lib/startup.mjs';

const okDirs = (...dirs) => (d) => (dirs.includes(d) ? { ok: true, problems: [] } : { ok: false, problems: ['d2data.mpq is missing'] });

test('saved folder first: a valid saved folder wins and detection never runs', () => {
  let detected = 0;
  const r = resolveStartup({ saved: '/s', check: okDirs('/s', '/d'), detect: () => { detected++; return '/d'; } });
  assert.deepEqual(r, { dir: '/s', source: 'saved' });
  assert.equal(detected, 0);
});

test('a stale saved folder falls through to detection, every startup', () => {
  const r = resolveStartup({ saved: '/gone', check: okDirs('/d'), detect: () => '/d' });
  assert.deepEqual(r, { dir: '/d', source: 'detected' });
  assert.deepEqual(resolveStartup({ saved: null, check: okDirs(), detect: () => '/d' }), { dir: '/d', source: 'detected' });
});

test('a check that throws counts as invalid; nothing found gives null', () => {
  assert.equal(resolveStartup({ saved: '/x', check: () => { throw new Error('io'); }, detect: () => null }), null);
  assert.equal(resolveStartup({ saved: undefined, check: okDirs(), detect: () => null }), null);
});

const isDir = (p) => p.endsWith('/dir');

test('one folder picked is that folder', () => {
  assert.deepEqual(folderFromPick(['/g/dir'], { isDir }), { dir: '/g/dir' });
});

test('several .mpq files from one folder pick that folder', () => {
  assert.deepEqual(folderFromPick(['/g/d2data.mpq', '/g/Patch_D2.MPQ', '/g/d2exp.mpq'], { isDir }), { dir: '/g' });
});

test('files from two folders, non-mpq files, a mix of folder and files, and nothing are refused with a reason', () => {
  assert.match(folderFromPick(['/a/d2data.mpq', '/b/patch_d2.mpq'], { isDir }).error, /one folder/);
  assert.match(folderFromPick(['/a/readme.txt'], { isDir }).error, /\.mpq files only/);
  assert.match(folderFromPick(['/a/dir', '/a/d2data.mpq'], { isDir }).error, /not both/);
  assert.match(folderFromPick([], { isDir }).error, /Nothing/);
});

test('validatePick runs the 1.13c check on the folder and reports every problem', () => {
  const check = (d) => (d === '/g' ? { ok: true, problems: [] } : { ok: false, problems: ['Patch_D2.mpq is not the 1.13c patch (update the game to 1.13c)'] });
  assert.deepEqual(validatePick(['/g/d2data.mpq'], check, { isDir }), { ok: true, dir: '/g' });
  const bad = validatePick(['/h/d2data.mpq'], check, { isDir });
  assert.equal(bad.ok, false);
  assert.match(bad.message, /not the 1\.13c patch/);
  assert.equal(validatePick(['/a/x.txt'], check, { isDir }).ok, false);
});

test('config records detection only for a detected folder; the modal text is the approved wording', () => {
  assert.deepEqual(configFor('/d', 'detected'), { retailDir: '/d', detected: true });
  assert.deepEqual(configFor('/c', 'chosen'), { retailDir: '/c' });
  assert.equal(NOT_FOUND.title, 'Diablo II files not found');
  assert.equal(NOT_FOUND.choose, 'Choose files…');
  assert.equal(NOT_FOUND.notNow, 'Not now');
  assert.match(NOT_FOUND.body, /^D2-Web needs your own copy of Diablo II \(1\.13c\)\. Choose the folder or the \.mpq files; they stay on this computer and nothing is uploaded or copied into the app\.$/);
});
