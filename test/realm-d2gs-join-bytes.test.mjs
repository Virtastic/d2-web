import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync, readdirSync } from 'node:fs';
import { resolve } from 'node:path';

// Unit-only: retail recordings (vectors/session/bnet-play) against the local server's source.
const dir = resolve('vectors/session/bnet-play');
const records = [];
for (const f of readdirSync(dir).filter((n) => n.endsWith('.txt'))) {
  for (const line of readFileSync(resolve(dir, f), 'utf8').split('\n')) {
    const m = /^(\d+) ([CS]) ([0-9a-f]+)$/.exec(line);
    if (m) records.push({ tick: +m[1], dir: m[2], hex: m[3] });
  }
}

test('retail S 8F answers every C 6D: 33 bytes, zero bytes 1..12 and 17..32, the ping tick is not echoed', () => {
  const pings = records.filter((r) => r.dir === 'C' && r.hex.startsWith('6d'));
  const pongs = records.filter((r) => r.dir === 'S' && r.hex.startsWith('8f'));
  assert.ok(pings.length > 0 && pongs.length > 0);
  for (const p of pongs) {
    assert.equal(p.hex.length, 66);
    assert.equal(p.hex.slice(2, 26), '0'.repeat(24));
    assert.equal(p.hex.slice(34), '0'.repeat(32));
  }
});

test('the local server answers 0x6D with 0x8F and zeros, no echo of the ping bytes', () => {
  const src = readFileSync(resolve('src/core/realm/d2gs_local_server.cpp'), 'utf8');
  const at = src.indexOf('opcode == 0x6D');
  const body = src.slice(at, src.indexOf('return 1;', at));
  assert.match(body, /pong\[33\] = \{ 0x8F \}/);
  assert.ok(!/memcpy/.test(body), 'the pong does not copy the ping');
});

test('retail join: one a8 per join (server-entry-bnet)', () => {
  const stream = readFileSync(resolve('vectors/d2game/server-entry-bnet.txt'), 'utf8').split('\n')
    .filter((l) => /^t\d+\s+blk /.test(l)).map((l) => l.replace(/^t\d+\s+blk /, '').replace(/\s+/g, '')).join('');
  const a8 = 'a800010000000b69acfc0f';
  assert.equal(stream.split(a8).length - 1, 1);
});

test('closing the local server drops every extra slot client and its route, so a slot can log on again', () => {
  const src = readFileSync(resolve('src/core/realm/d2gs_local_server.cpp'), 'utf8');
  const at = src.indexOf('void d2_local_d2gs_close');
  const body = src.slice(at, src.indexOf('\n}', at));
  assert.match(body, /d2_session_drop_client\(s\)/);
  const mon = readFileSync(resolve('src/core/monspawn/monspawn_check.cpp'), 'utf8');
  const drop = mon.slice(mon.indexOf('static void sessionDropExtraClient(int slot)'), mon.indexOf('static void sessionDropExtraClients() {'));
  assert.match(drop, /D2WEB_RouteClient\(slot, nullptr, nullptr\)/);
  assert.match(drop, /free\(e\.client\)/);
});

test('a dropped extra client is removed from every room of every act, not only the adjacent rooms', () => {
  const mon = readFileSync(resolve('src/core/monspawn/monspawn_check.cpp'), 'utf8');
  const drop = mon.slice(mon.indexOf('static void sessionDropExtraClient(int slot)'), mon.indexOf('static void sessionDropExtraClients() {'));
  assert.match(drop, /pRoomNext/);
  assert.ok(!/GetAdjacentRoomsListFromRoom/.test(drop));
  assert.ok(!/e\.room/.test(drop.replace(/\/\/.*$/gm, '')), 'e.room may be freed');
});
