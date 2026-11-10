// Joining a game with no page: the real engine, loaded cold in node, is fed a game server's stream through the
// same entry point the page uses. A server packet may be refused (a negative result, the state reset); it may
// never take the engine down. Two things used to:
//   - the act packet (0x03) arriving before the game data archives were mounted (the table loader asserted);
//   - a room reveal or hide (0x07 / 0x08) naming a level the level table has no generator for, such as 0.
// The recorded stream is a stock 1.13c server's send to a joining client (a capture kept outside the repository; skipped when absent).
import test from 'node:test';
import assert from 'node:assert/strict';
import { existsSync, readFileSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { bindEngineProtocol, d2gsJoin } from '../play/assets/realm-client.mjs';
import { haveArchives, haveEngine, loadEngine } from './helpers/node-engine.mjs';

const RECORDED = join(dirname(fileURLToPath(import.meta.url)), '..', 'vectors', 'd2game', 'server-entry-bnet.txt');
const engine = haveEngine(), data = engine && haveArchives(), recorded = data && existsSync(RECORDED);
const skip = (ok, what) => (ok ? false : `needs ${what}`);
const hex = (b) => [...b].map((x) => x.toString(16).padStart(2, '0')).join('');

// The server's sends, one block per write, in order.
const blocks = () => readFileSync(RECORDED, 'utf8').split('\n').filter((l) => /^t\d+\s+blk /.test(l))
  .map((l) => Uint8Array.from(l.replace(/^t\d+\s+blk /, '').trim().split(/\s+/).map((h) => parseInt(h, 16))));

async function open(archives) {
  const lines = [];
  const Module = await loadEngine({ archives, log: (s) => { lines.push(String(s).slice(0, 200)); } });
  return { Module, protocol: bindEngineProtocol(Module), lines };
}

// Feed, turning an engine abort into a failure that names the packet and the engine's last words.
function feed({ protocol, lines }, bytes, what) {
  try { return protocol.d2gsClientPlayerFeed(bytes); }
  catch (e) { assert.fail(`${what}: engine aborted on ${hex(bytes.subarray(0, 24))} (${String(e?.message || e).slice(0, 80)}; ${lines.slice(-2).join(' / ')})`); }
}

// The whole recorded join in the order the join feeds it: the act and the room reveals ahead of the player.
function joinedStream(protocol) {
  const all = Uint8Array.from(blocks().flatMap((b) => [...b])), packets = [];
  for (let at = 0; at < all.length;) { const n = protocol.d2gsPacketSize(all.subarray(at)); assert.ok(n > 0, `recorded packet at ${at}`); packets.push(all.subarray(at, at + n)); at += n; }
  const early = (p) => p[0] === 0x03 || p[0] === 0x07;
  return Uint8Array.from([...packets.filter(early), ...packets.filter((p) => !early(p))].flatMap((p) => [...p]));
}

// The shortest packet the engine's size table accepts for an id, every other byte `fill`. A variable-length id
// carries its length in one of its first bytes; null when the table knows no such server packet.
function minimumPacket(protocol, id, fill) {
  const make = (pos, v) => { const b = new Uint8Array(600).fill(fill); b[0] = id; if (pos) b[pos] = v; return b; };
  let best = null, size = protocol.d2gsPacketSize(make(0, 0));
  if (size > 0) return make(0, 0).subarray(0, size);
  for (let pos = 1; pos <= 3; pos++) for (let v = 1; v <= 80; v++) {
    const b = make(pos, v); size = protocol.d2gsPacketSize(b);
    if (size > 0 && (!best || size < best.length)) best = b.subarray(0, size);
  }
  return best;
}

test('a recorded join fed to a cold engine places the player, its rooms and its neighbours', { skip: skip(recorded, 'the staged engine, the game archives and the recorded stream') }, async () => {
  const eng = await open(true), { Module, protocol } = eng, sends = blocks();
  let onData = () => {};
  const transport = { onData: (f) => { onData = f; }, onClose: () => {}, close() {},
    send: (b) => {
      if (b[0] === 0x68) queueMicrotask(() => { onData(sends[0]); onData(sends[1]); });
      if (b[0] === 0x6b) sends.slice(2).forEach((c, i) => setTimeout(() => onData(c), i));
    } };
  const joining = d2gsJoin(transport, protocol, { character: 'RETAILB', charClass: 4, gameToken: 1, gameHash: 1, expansion: true, timeoutMs: 5000 });
  queueMicrotask(() => onData(Uint8Array.of(0xaf, 0x00)));
  const game = await joining;
  assert.ok(game.playerState.active);
  assert.equal(game.playerState.guid, 1);
  for (;;) {   // the rest of the burst, as the receive loop would feed it
    const frame = await Promise.race([game.readFrame(), new Promise((r) => setTimeout(() => r(null), 300))]);
    if (!frame) break;
    assert.ok(feed(eng, frame, 'burst tail') >= 0, 'burst tail refused');
  }
  const player = protocol.d2gsClientPlayerState(), world = protocol.d2gsClientWorldState();
  assert.equal(player.name, 'RETAILB');
  assert.equal(player.classId, 4);
  assert.ok(player.clientRoomActive, 'the player stands in an active room');
  assert.equal(player.clientRoomLevel, 1);
  assert.equal(world.act, 0);
  assert.equal(world.mapRevealCount, 10);
  const count = (what) => Module.ccall(`d2_realm_client_${what}_count`, 'number', [], []);
  assert.deepEqual({ players: count('player'), monsters: count('monster'), objects: count('object') }, { players: 1, monsters: 11, objects: 22 });
});

test('the act packet with no game data mounted is refused, and loads once the data is there', { skip: skip(data, 'the staged engine and the game archives') }, async () => {
  const bare = await open(false);
  const act = Uint8Array.of(0x03, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0);   // act 1, seed 1, town level 1
  assert.equal(feed(bare, act, 'act with no data'), -5);
  assert.equal(feed(bare, act, 'act with no data, again'), -5);
  const full = await open(true);
  assert.equal(feed(full, act, 'act with data'), 0);   // accepted; the act packet carries no unit record to count
});

for (const fill of [0x00, 0x01, 0xff]) {
  const tag = `filled with 0x${fill.toString(16).padStart(2, '0')}`;
  test(`every server packet id at its minimum length, ${tag}, leaves a cold engine running`, { skip: skip(data, 'the staged engine and the game archives') }, async () => {
    const eng = await open(true);
    let known = 0;
    for (let id = 0; id <= 0xff; id++) {
      const packet = minimumPacket(eng.protocol, id, fill);
      if (packet) known++;
      eng.protocol.d2gsClientPlayerReset();
      feed(eng, packet ?? Uint8Array.of(id), `cold, id 0x${id.toString(16)}`);
    }
    assert.ok(known > 100, `the size table knows ${known} ids`);
  });

  test(`every server packet id at its minimum length, ${tag}, leaves a joined engine running`, { skip: skip(recorded, 'the staged engine, the game archives and the recorded stream') }, async () => {
    const eng = await open(true), stream = joinedStream(eng.protocol);
    let joined = false;
    for (let id = 0; id <= 0xff; id++) {
      if (!joined) { eng.protocol.d2gsClientPlayerReset(); assert.ok(feed(eng, stream, 'recorded join') > 0, 'recorded join refused'); joined = true; }
      const packet = minimumPacket(eng.protocol, id, fill);
      if (feed(eng, packet ?? Uint8Array.of(id), `joined, id 0x${id.toString(16)}`) < 0) joined = false;   // a refusal resets the state
    }
  });
}
