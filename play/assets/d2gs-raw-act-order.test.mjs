import test from 'node:test';
import assert from 'node:assert/strict';
import { d2gsJoin } from './realm-client.mjs';

// The stock 1.13c game server sends the player (0x59) before the act (0x03). The engine's client state refuses a
// player assigned with no act, so the join used to fail on the real burst and retry against a spent ticket.
const SIZE = { 0x00: 1, 0x01: 8, 0x02: 1, 0x03: 12, 0x0b: 6, 0x59: 26, 0x07: 6 };
const FIRST = [0x01, 0x00, 0x04, 0x00, 0x10, 0x00, 0x01, 0x00, 0x00, 0x02];

function run(chunks) {
  let onData = () => {};
  const sent = [], fedStreams = [];
  const transport = {
    onData: (f) => { onData = f; }, onClose: () => {},
    send: (b) => {
      const bytes = Uint8Array.from(b); sent.push(bytes);
      if (bytes[0] === 0x68) queueMicrotask(() => onData(Uint8Array.from(FIRST)));
      if (bytes[0] === 0x6b) chunks.forEach((c, i) => setTimeout(() => onData(Uint8Array.from(c)), i));
    },
  };
  const opcodesOf = (b) => { const out = []; for (let at = 0; at < b.length; at += SIZE[b[at]]) out.push(b[at]); return out; };
  let actSeen = false;
  const protocol = {
    greetingParse: (b) => (b[0] === 0xaf ? (2 << 8) | b[1] : -1),
    d2gsGamelogon: () => { const g = new Uint8Array(37); g[0] = 0x68; return g; },
    d2gsPacketSize: (b) => { const n = SIZE[b[0]]; if (n === undefined) return -1; return b.length < n ? 0 : n; },
    d2gsFindPlayerAssign: (r) => { let at = 0; while (at < r.length) { const n = SIZE[r[at]]; if (r.length - at < n) return { status: -1 }; if (r[at] === 0x59) return { status: 1, offset: at }; at += n; } return { status: 0 }; },
    d2gsPlayerAssign: () => ({ name: 'Hero', classId: 5, guid: 1 }),
    d2gsClientPlayerReset: () => {}, d2gsClientSetDifficulty: () => {},
    d2gsClientPlayerFeed: (b) => {
      fedStreams.push(opcodesOf(b));
      actSeen = false;
      for (const op of opcodesOf(b)) { if (op === 0x03) actSeen = true; if (op === 0x59 && !actSeen) return -3; }
      return 1;
    },
    d2gsClientPlayerState: () => ({ active: true, guid: 1 }),
    d2gsPlayerReassign: () => null,
  };
  const joining = d2gsJoin(transport, protocol, { character: 'Hero', charClass: 5, gameToken: 1, gameHash: 1 });
  queueMicrotask(() => onData(Uint8Array.from([0xaf, 0x00])));
  return joining.then((game) => ({ game, sent, fedStreams }));
}
const assign = Array.from({ length: 26 }, (_, i) => (i === 0 ? 0x59 : 0));
const handshake = [0x0b, 0, 1, 0, 0, 0];
const act = [0x03, 0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0];
const reveal = [0x07, 0, 0, 0, 0, 1];

test('a player assigned before the act is fed to the engine after the act', async () => {
  const { game, fedStreams } = await run([[...assign, ...handshake, ...act, ...reveal]]);
  assert.deepEqual(fedStreams.at(-1), [0x01, 0x00, 0x02, 0x03, 0x59, 0x0b]);
  assert.equal(game.playerState.guid, 1);
  assert.equal((await game.readFrame())[0], 0x07, 'the packets after the handshake stay for the receive loop, without the act');
});

test('the join waits for an act that arrives in a later read', async () => {
  const { game, fedStreams } = await run([[...assign, ...handshake], act, reveal]);
  assert.deepEqual(fedStreams[0], [0x01, 0x00, 0x02, 0x59, 0x0b], 'refused without the act');
  assert.deepEqual(fedStreams.at(-1), [0x01, 0x00, 0x02, 0x03, 0x59, 0x0b]);
  assert.equal(game.playerAssignment.name, 'Hero');
});
