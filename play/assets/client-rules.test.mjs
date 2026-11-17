// The quest log's state bytes as an unjoined game reads them: the 0x52 its own server composes for the client's 0x40,
// walked out of the packet stream by the packet size rule. A stand-in engine; the size rule here is the table's three sizes.
import test from 'node:test';
import assert from 'node:assert/strict';
import { bindClientRules, questLogStatesIn, requestLocalQuestLogStates, staminaColours } from './client-rules.mjs';

const SIZE = { 0x28: 0x67, 0x50: 0x0f, 0x52: 0x2a };
const packet = (op, fill = 0) => { const b = new Uint8Array(SIZE[op]).fill(fill); b[0] = op; return b; };
const join = (...ps) => { const out = new Uint8Array(ps.reduce((n, p) => n + p.length, 0)); let o = 0; for (const p of ps) { out.set(p, o); o += p.length; } return out; };

test('the state bytes are the 41 after the 0x52 header, past the record and the progress packets', () => {
  const states = packet(0x52); for (let i = 1; i < 42; i++) states[i] = i;
  const stream = join(packet(0x28, 0x52), packet(0x50, 0x52), states);
  const got = questLogStatesIn(stream, (at) => SIZE[stream[at]] ?? -1);
  assert.deepEqual(Array.from(got), Array.from({ length: 41 }, (_, i) => i + 1));
});

test('no 0x52, or a stream the size rule cannot walk, gives no bytes', () => {
  const a = join(packet(0x28));
  assert.equal(questLogStatesIn(a, (at) => SIZE[a[at]] ?? -1), null);
  const b = join(Uint8Array.of(0x99), packet(0x52));
  assert.equal(questLogStatesIn(b, (at) => SIZE[b[at]] ?? -1), null);
});

function fakeEngine(answer) {
  const HEAPU8 = new Uint8Array(4096), STREAM = 1024, sent = [];
  let next = 64, stream = new Uint8Array(0);
  const fns = {
    d2_session_client_packet: (p, n) => { sent.push(Array.from(HEAPU8.subarray(Number(p), Number(p) + n))); stream = answer; HEAPU8.set(stream, STREAM); return 0; },
    d2_session_packet_stream_length: () => stream.length,
    d2_session_packet_stream: () => BigInt(STREAM),
    d2_realm_d2gs_packet_size: (p) => SIZE[HEAPU8[Number(p)]] ?? -1,
  };
  return { sent, HEAPU8, _malloc: (n) => { const p = next; next += n; return p; }, _free: () => {},
    _d2_session_client_packet: 1, _d2_session_packet_stream: 1, _d2_realm_d2gs_packet_size: 1,
    ccall: (name, ret, types, args) => fns[name](...args) };
}

test('the log asks with the one-byte 0x40 and reads the answer the server sent', () => {
  const states = packet(0x52); states[1] = 12; states[41] = 4;
  const engine = fakeEngine(join(packet(0x28), states));
  bindClientRules(engine);
  const got = requestLocalQuestLogStates();
  assert.deepEqual(engine.sent, [[0x40]]);
  assert.equal(got.length, 41);
  assert.equal(got[0], 12);
  assert.equal(got[40], 4);
  bindClientRules(null);
});

test('the stamina colours cross the palette into the engine and unpack its three slot bytes', () => {
  const HEAPU8 = new Uint8Array(4096); let seen = null;
  bindClientRules({ HEAPU8, _d2_client_stamina_colours: 1, _malloc: () => 1024n, _free: () => {},
    ccall: (name, ret, types, [p]) => { seen = HEAPU8.slice(Number(p), Number(p) + 1024); return 98 | 109 << 8 | 151 << 16; } });
  const palette = new Uint8Array(1024).map((_, i) => i & 255);
  assert.deepEqual(staminaColours(palette), [98, 109, 151]);
  assert.deepEqual(Array.from(seen), Array.from(palette));
  bindClientRules(null);
});
