import test from 'node:test';
import assert from 'node:assert/strict';
import { d2gsJoin } from './realm-client.mjs';

// The stock 1.13c game server (captured from a live create-game): after the edge's `AF 00` greeting and the
// client's 37-byte GAMELOGON it answers with the bare, unframed run 01 00 04 00 10 00 01 00 | 00 | 02
// (game flags, loading, load success), then waits for the client's one-byte 0x6B before it sends the world.
const SIZE = { 0x00: 1, 0x01: 8, 0x02: 1, 0x0b: 6, 0x59: 26 };
const FIRST = [0x01, 0x00, 0x04, 0x00, 0x10, 0x00, 0x01, 0x00, 0x00, 0x02];

function harness() {
  let onData = () => {}, onClose = () => {};
  const sent = [];
  const transport = {
    onData: (f) => { onData = f; }, onClose: (f) => { onClose = f; },
    send: (b) => {
      const bytes = Uint8Array.from(b); sent.push(bytes);
      if (bytes[0] === 0x68) queueMicrotask(() => onData(Uint8Array.from(FIRST)));
      if (bytes[0] === 0x6b) queueMicrotask(() => {
        const assign = new Uint8Array(26); assign[0] = 0x59;
        onData(assign.subarray(0, 13)); onData(assign.subarray(13));   // a packet split across two reads
        onData(Uint8Array.from([0x0b, 0, 1, 0, 0, 0]));
      });
    },
  };
  const protocol = {
    greetingParse: (b) => (b[0] === 0xaf ? (2 << 8) | b[1] : -1),
    d2gsGamelogon: () => { const g = new Uint8Array(37); g[0] = 0x68; return g; },
    d2gsPacketSize: (b) => { const n = SIZE[b[0]]; if (n === undefined) return -1; return b.length < n ? 0 : n; },
    d2gsFindPlayerAssign: (r) => { let at = 0; while (at < r.length) { const n = SIZE[r[at]]; if (r.length - at < n) return { status: -1 }; if (r[at] === 0x59) return { status: 1, offset: at }; at += n; } return { status: 0 }; },
    d2gsPlayerAssign: () => ({ name: 'Hero', classId: 1, guid: 1 }),
    d2gsClientPlayerReset: () => {}, d2gsClientSetDifficulty: () => {},
    d2gsClientPlayerFeed: () => 1,
    d2gsClientPlayerState: () => ({ active: true, guid: 1 }),
    d2gsPlayerReassign: () => null,
  };
  return { transport, protocol, sent, push: (b) => onData(Uint8Array.from(b)) };
}

test('an AF 00 game server stream is read unframed and the join answers 0x02 with 0x6B', async () => {
  const h = harness();
  const joining = d2gsJoin(h.transport, h.protocol, { logonDelayMs: 0, character: 'Hero', charClass: 1, gameToken: 0xa3, gameHash: 19 });
  h.push([0xaf, 0x00]);
  const game = await joining;
  assert.equal(game.greeting.mode, 0);
  assert.deepEqual(h.sent.map((b) => b[0]), [0x68, 0x6b]);
  assert.equal(h.sent[1].length, 1);
  assert.deepEqual(game.opcodes.length, 5);   // 01, 00, 02, 59, 0b
  assert.equal(game.playerAssignment.name, 'Hero');
});

test('a stream that is not a D2GS greeting is rejected', async () => {
  const h = harness();
  const joining = d2gsJoin(h.transport, h.protocol, { logonDelayMs: 0, character: 'Hero', charClass: 1, gameToken: 1, gameHash: 1 });
  h.push([0x01, 0x00]);
  await assert.rejects(joining, /not a D2GS greeting/);
});
