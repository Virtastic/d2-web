import { test } from 'node:test';
import assert from 'node:assert/strict';
import { mkdtempSync, writeFileSync, readFileSync, existsSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { reservePort, releasePort, tryLock } from '../tools/port-reserve.mjs';

const dir = () => mkdtempSync(join(tmpdir(), 'port-reserve-'));
const DEAD = 2 ** 22 + 12345;   // above pid_max's default: never a live process

test('two reservers given the same port by the kernel never share it', () => {
  const d = dir();
  const seq = [45001, 45001, 45002];   // the probe race: both callers are handed 45001
  const probe = (p) => (p ? p : seq.shift());
  const a = reservePort({ dir: d, probe, pid: process.pid, release: false });
  const b = reservePort({ dir: d, probe, pid: process.ppid, release: false });
  assert.equal(a, 45001);
  assert.equal(b, 45002);
});

test('an inherited preferred port held by a live prover is not reused', () => {
  const d = dir();
  writeFileSync(join(d, '8997'), String(process.ppid));
  const p = reservePort({ dir: d, preferred: 8997, probe: (x) => x || 45010, release: false });
  assert.equal(p, 45010);
});

test('a stale lock (its owner gone) is taken over, and release removes only our own', () => {
  const d = dir();
  writeFileSync(join(d, '8997'), String(DEAD));
  assert.equal(reservePort({ dir: d, preferred: 8997, probe: (x) => x, release: false }), 8997);
  assert.equal(readFileSync(join(d, '8997'), 'utf8'), String(process.pid));
  releasePort(d, 8997, process.ppid);
  assert.ok(existsSync(join(d, '8997')));
  releasePort(d, 8997);
  assert.ok(!existsSync(join(d, '8997')));
  assert.ok(tryLock(d, 8997));
});

test('a port that does not bind is released and skipped', () => {
  const d = dir();
  const p = reservePort({ dir: d, preferred: 8997, probe: (x) => (x === 8997 ? 0 : x || 45020), release: false });
  assert.equal(p, 45020);
  assert.ok(!existsSync(join(d, '8997')));
});

test('a real reservation binds and returns a usable port', () => {
  const p = reservePort({ dir: dir() });
  assert.ok(p > 0 && p < 65536);
});
