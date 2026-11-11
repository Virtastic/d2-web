import test from 'node:test';
import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { mkdtempSync, rmSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join, resolve } from 'node:path';
import { startD2gsSyncReceive } from '../play/assets/realm-client.mjs';
import { findD2gsDeny } from '../play/assets/d2gs-deny.mjs';

test('the receive pump drains the system queue then the game queue, unbounded, sized like the splitter (native fixture)', (t) => {
  if (spawnSync('g++', ['--version'], { stdio: 'ignore' }).status !== 0) { t.skip('g++ unavailable for the pump fixture'); return; }
  const dir = mkdtempSync(join(tmpdir(), 'd2-net-pump-'));
  try {
    const binary = join(dir, 'net-receive-pump-test');
    const built = spawnSync('g++', ['-std=c++20', '-Wall', '-Wextra', '-Itest/stub', '-Isrc', resolve('test/net-receive-pump.cpp'), '-o', binary], { encoding: 'utf8' });
    assert.equal(built.status, 0, built.stderr || built.stdout);
    const run = spawnSync(binary, [], { encoding: 'utf8' });
    assert.equal(run.status, 0, run.stderr || run.stdout);
  } finally { rmSync(dir, { recursive: true, force: true }); }
});

test('the in-process receive hands over every received packet in arrival order, in one synchronous pass', () => {
  const frames = [new Uint8Array([1]), new Uint8Array([2, 3]), new Uint8Array([4])];
  const seen = [];
  const rx = startD2gsSyncReceive(() => frames.shift() ?? null, null, { onFrame: (f, b) => seen.push([f.count, [...b]]) });
  assert.equal(rx.drain(), 3);                         // returns before any microtask runs: no promise hop
  assert.deepEqual(seen, [[1, [1]], [2, [2, 3]], [3, [4]]]);
  assert.equal(rx.drain(), 3);                         // nothing pending: the count stands, nothing is re-delivered
  assert.equal(seen.length, 3);
  rx.stop();
  frames.push(new Uint8Array([9]));
  rx.drain();
  assert.equal(seen.length, 3);                        // a stopped receiver takes no more
});

test('a refusal (0xB4) is found wherever it sits in a frame, which the retail pump reaches first', () => {
  const size = (b) => ({ 0: 1, 2: 1, 0xb4: 5 }[b[0]] ?? -1);
  assert.equal(findD2gsDeny(new Uint8Array([0, 0xb4, 7, 0, 0, 0, 2]), size), 7);
  assert.equal(findD2gsDeny(new Uint8Array([0, 2]), size), null);
});
