import { test } from 'node:test';
import assert from 'node:assert/strict';
import { spawn, spawnSync } from 'node:child_process';
import { mkdtempSync, readFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';

const lib = join(import.meta.dirname, '../ci/build-slot.sh');
const run = (dir, key, body) => spawn('bash', ['-c', `source ${lib}; d2_build_key_lock "$1" "$2" && ${body}`, 'x', dir, key]);

test('dirty or empty keys are never deduped', () => {
  const dir = mkdtempSync(join(tmpdir(), 'bkl-'));
  for (const k of ['', 'abc-dirty', 'a/b']) assert.equal(spawnSync('bash', ['-c', `source ${lib}; d2_build_key_lock "$1" "$2"`, 'x', dir, k]).status, 1);
});

test('two builds of the same key run one after the other, different keys in parallel', async () => {
  const dir = mkdtempSync(join(tmpdir(), 'bkl-'));
  const log = join(dir, 'log');
  const body = (n) => `echo start-${n} >> ${log}; sleep 1; echo end-${n} >> ${log}`;
  const done = (p) => new Promise((r) => p.on('close', r));
  await Promise.all([done(run(dir, 'k1', body('a'))), done(run(dir, 'k1', body('b'))), done(run(dir, 'k2', body('c')))]);
  const l = readFileSync(log, 'utf8').trim().split('\n');
  const idx = (s) => l.indexOf(s);
  const serial = idx('end-a') < idx('start-b') || idx('end-b') < idx('start-a');
  assert.ok(serial, `same key overlapped: ${l}`);
  assert.ok(idx('start-c') < Math.max(idx('end-a'), idx('end-b')), `other key waited: ${l}`);
});
