// What the packet client sends must be something the local game takes, or be named here as not served yet.
import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import { packets } from './realm-input.mjs';
import { exitLocalGame } from './realm-local.mjs';

const read = (p) => readFileSync(new URL(p, import.meta.url), 'utf8');
const server = read('../../src/core/realm/d2gs_local_server.cpp');
const session = read('../../src/core/monspawn/monspawn_check.cpp');
const served = new Set();
for (const m of server.matchAll(/case 0x([0-9A-Fa-f]{2}):/g)) served.add(parseInt(m[1], 16));
const clientPacket = session.slice(session.indexOf('int d2_session_client_packet('), session.indexOf('A player stat as the game reads it'));
for (const m of clientPacket.matchAll(/case 0x([0-9A-Fa-f]{2}):/g)) served.add(parseInt(m[1], 16));

// Opcodes the local game has no entry for, with the reason.
const unserved = new Map([
  [0x14, 'overhead chat: the local game has no chat room; the shell echoes the line'],
  [0x15, 'chat: same'],
]);

test('every opcode the input packet builders write has an entry in the local game or a named reason', () => {
  const sent = new Set();
  for (const build of Object.values(packets)) {
    const bytes = build(1, 2, 3);
    sent.add(bytes[0]);
  }
  for (const code of [0x32, 0x33, 0x35, 0x36, 0x37, 0x4f]) sent.add(code);   // the NPC and button packet builders
  const missing = [...sent].filter((c) => !served.has(c) && !unserved.has(c));
  assert.deepEqual(missing.map((c) => '0x' + c.toString(16)), []);
});

test('leaving a local game releases the connection before the character is saved, and saves even when the release fails', async () => {
  const order = [];
  await exitLocalGame({ release: async () => { order.push('release'); }, saveAndExit: () => order.push('save') });
  assert.deepEqual(order, ['release', 'save']);
  order.length = 0;
  await assert.rejects(exitLocalGame({ release: async () => { order.push('release'); throw new Error('x'); }, saveAndExit: () => order.push('save') }));
  assert.deepEqual(order, ['release', 'save']);
});
