// The in-process game connection's JS end against a stand-in engine: only the transport contract
// (what goes to the engine, what comes back, in what order). The engine's own server end is covered
// where the built engine is available.
import test from 'node:test';
import assert from 'node:assert/strict';
import { localGameTransport, openLocalRealm } from './realm-local.mjs';

// A stand-in for the engine's local server exports: a byte queue the test fills, and a log of calls.
function fakeEngine({ game = true } = {}) {
  const heap = new Uint8Array(8192);
  const OUT = 4096;
  let next = 16, queued = [], freed = 0;
  const log = [], refused = [];
  const queue = (...bytes) => queued.push(...bytes);
  const fns = {
    d2_local_d2gs_open: () => { if (!game) return 0; queue(0xaf, 0x00); return 1; },
    d2_local_d2gs_close: () => { log.push('close'); },
    d2_local_d2gs_client: (at, length) => {
      const bytes = Array.from(heap.subarray(Number(at), Number(at) + length));
      log.push(bytes);
      if (bytes[0] === 0x68) { queue(0x01, 0, 0, 0, 0, 0, 1, 0, 0x00, 0x02); return 1; }
      if (bytes[0] === 0x6d) { queue(0x8f, ...Array(32).fill(0)); return 1; }
      refused.push(bytes[0], -1);
      return 0;
    },
    d2_local_d2gs_pump: () => { queue(0x04); return queued.length; },
    d2_local_d2gs_out: () => { heap.set(queued, OUT); return OUT; },
    d2_local_d2gs_take: () => { const n = queued.length; queued = []; return n; },
    d2_local_d2gs_dropped: () => 0,
    d2_local_d2gs_refused: (countAt) => {
      new Int32Array(heap.buffer, Number(countAt), 1)[0] = refused.length / 2;
      new Int32Array(heap.buffer, 2048, refused.length).set(refused);
      return 2048;
    },
    d2_session_tick: (frames) => { log.push(`tick ${frames}`); queue(0x8f, ...Array(32).fill(1)); return 1; },
    d2_session_game_flags: () => 0x010100,   // class 1, expansion, normal
  };
  return { log, freed: () => freed,
    Module: { HEAPU8: heap, ccall: (name, _ret, _types, values) => fns[name](...values),
      _malloc: (size) => { const at = next; next += (size + 7) & ~7; return at; }, _free: () => { freed++; } } };
}

test('nothing is sent or delivered before the connection is open', () => {
  const { Module, log } = fakeEngine();
  const transport = localGameTransport(Module);
  assert.equal(transport.isOpen, false);
  assert.equal(transport.send(Uint8Array.of(0x6d, 1, 2, 3, 4)), false);
  assert.equal(transport.pump(), 0);
  assert.equal(transport.tick(1), 0);
  assert.deepEqual(log, []);
});

test('opening fails when there is no local game', async () => {
  const transport = localGameTransport(fakeEngine({ game: false }).Module);
  await assert.rejects(transport.ready(), /no local game/);
  assert.equal(transport.isOpen, false);
});

test('the greeting, the replies and the tick output reach the client in order', async () => {
  const { Module, log } = fakeEngine();
  const transport = localGameTransport(Module);
  await transport.ready();
  const got = [];
  transport.onData((bytes) => got.push(Array.from(bytes)));   // subscribed after the greeting was said
  assert.deepEqual(got, [[0xaf, 0x00]]);
  assert.equal(transport.send(Uint8Array.of(0x68, 9, 9)), true);
  assert.deepEqual(log[0], [0x68, 9, 9]);
  assert.deepEqual(got[1], [0x01, 0, 0, 0, 0, 0, 1, 0, 0x00, 0x02]);
  assert.equal(transport.tick(2), 34);   // the tick's packet, then the pump's in-game marker
  assert.equal(log[1], 'tick 2');
  assert.equal(got[2].length, 34);
  assert.equal(got[2][0], 0x8f);
  assert.equal(got[2][33], 0x04);
  assert.deepEqual(transport.stats(), { sent: 1, taken: 1, refusedSends: 0, serverBytes: 2 + 10 + 34 });
});

test('a packet the server does not take is counted and listed, and nothing comes back', async () => {
  const { Module } = fakeEngine();
  const transport = localGameTransport(Module);
  await transport.ready();
  const got = [];
  transport.onData((bytes) => got.push(bytes));
  transport.send(Uint8Array.of(0x77));
  assert.equal(got.length, 1);   // the greeting only
  assert.deepEqual(transport.refused(), [{ opcode: 0x77, result: -1 }]);
  assert.equal(transport.stats().refusedSends, 1);
  assert.equal(transport.send(new Uint8Array(2000)), true);   // larger than any client packet: never reaches the engine
  assert.equal(transport.stats().sent, 1);
});

test('closing tells the engine and the client once, and releases the scratch memory once', async () => {
  const { Module, log, freed } = fakeEngine();
  const transport = localGameTransport(Module);
  await transport.ready();
  let closed = 0;
  transport.onClose(() => closed++);
  transport.close();
  transport.close();
  assert.equal(closed, 1);
  assert.deepEqual(log, ['close']);
  assert.equal(freed(), 2);
  assert.equal(transport.send(Uint8Array.of(0x6d)), false);
  assert.deepEqual(transport.refused(), []);
});

test('the local realm flow describes the live session\'s character and opens one connection', () => {
  const { Module } = fakeEngine();
  const flow = openLocalRealm(Module, { tag: 'protocol' }, { name: 'Hero' });
  assert.deepEqual(flow.local.joined.character, { name: 'Hero', charClass: 1, flags: 0x20 });
  assert.equal(flow.local.joined.game.difficulty, 0);
  assert.equal(flow.local.pump(), 0);   // nothing connected yet
  const transport = flow.local.connect();
  assert.equal(typeof transport.ready, 'function');
  assert.equal(flow.mcp, null);
});
