// UNIT-ONLY: the 0x9B resurrect offer and 0x61 film queue of the joined-game client state
// (src/core/realm/d2gs_client_game_packets.h, compiled natively with g++), and the Resurrect menu row built from them.
// Nothing here runs the wasm build or a game; the packet bytes are written from the decompiled handlers.
import test from 'node:test';
import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { mkdtempSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join, resolve } from 'node:path';
import { createRealmNpc } from './realm-npc-state.mjs';
import { withResurrectRow, resurrectLabel, S_RESURRECT } from './npc-menu.mjs';

const FIXTURE = String.raw`
#include "d2gs_client_game_packets.h"
#include <iostream>
#include <sstream>
#include <string>
using namespace d2::realm;
int main() {
    MercResurrect merc; MovieQueue films; std::string line;
    while (std::getline(std::cin, line)) {
        std::istringstream in(line); std::string cmd; in >> cmd;
        std::vector<uint8_t> p; unsigned b = 0;
        while (in >> std::hex >> b) p.push_back(static_cast<uint8_t>(b));
        const int size = static_cast<int>(p.size());
        bool ok = cmd == "merc" ? mercResurrectApply(merc, p.data(), size) : movieQueueApply(films, p.data(), size);
        std::cout << (ok ? 1 : 0) << ' ';
    }
    std::cout << "\n" << merc.nameId << ' ' << merc.cost << ' ' << mercResurrectOffered(merc) << "\n";
    for (uint32_t i : films.indexes) std::cout << i << ' ';
    std::cout << "\n";
}`;
function run(t, script) {
  if (spawnSync('g++', ['--version'], { stdio: 'ignore' }).status !== 0) { t.skip('g++ unavailable'); return null; }
  const dir = mkdtempSync(join(tmpdir(), 'd2-merc-'));
  try {
    writeFileSync(join(dir, 'f.cpp'), FIXTURE);
    const built = spawnSync('g++', ['-std=c++20', '-Wall', '-Wextra', '-Werror', '-I', resolve('src/core/realm'), join(dir, 'f.cpp'), '-o', join(dir, 'f')], { encoding: 'utf8' });
    assert.equal(built.status, 0, built.stderr);
    const r = spawnSync(join(dir, 'f'), [], { input: script.join('\n') + '\n', encoding: 'utf8' });
    const [acc, state, films] = r.stdout.split('\n');
    return { acc: acc.trim().split(' '), state: state.split(' ').map(Number), films: films.trim().split(' ').filter(Boolean).map(Number) };
  } finally { rmSync(dir, { recursive: true, force: true }); }
}

test('0x9B stores the name id and price, 0xFFFF clears the offer, a wrong size or opcode changes nothing', (t) => {
  const r = run(t, ['merc 9b 21 04 e8 03 00 00', 'merc 9b 21 04 e8 03', 'merc 9c 01 00 01 00 00 00']);
  if (!r) return;
  assert.deepEqual(r.acc, ['1', '0', '0']);
  assert.deepEqual(r.state, [0x421, 1000, 1]);
  const cleared = run(t, ['merc 9b 21 04 e8 03 00 00', 'merc 9b ff ff 00 00 00 00']);
  assert.deepEqual(cleared.state, [0xffff, 0, 0]);
});

test('0x61 queues the film index in arrival order; another size is refused', (t) => {
  const r = run(t, ['film 61 03', 'film 61 07', 'film 61', 'film 61 01 00']);
  if (!r) return;
  assert.deepEqual(r.acc, ['1', '1', '0', '0']);
  assert.deepEqual(r.films, [3, 7]);
});

test('the Resurrect row sits ahead of Hire, or ahead of Cancel, only for the mercenary NPCs', () => {
  const base = [{ stringId: 3381, action: 'talk' }, { stringId: 3397, action: 'hire' }, { stringId: 3400, action: 'cancel' }];
  const withRow = withResurrectRow(base, 198, 'x');
  assert.deepEqual(withRow.map((e) => e.action), ['talk', 'resurrect', 'hire', 'cancel']);
  assert.equal(withRow[1].stringId, S_RESURRECT);
  const noHire = withResurrectRow([base[0], base[2]], 198, 'x');
  assert.deepEqual(noHire.map((e) => e.action), ['talk', 'resurrect', 'cancel']);
  assert.equal(withResurrectRow(base, 148, 'x'), base);     // Charsi is not a mercenary NPC
  assert.equal(withResurrectRow(base, 198, null), base);    // no 0x9B offer
  assert.equal(withResurrectRow(withRow, 198, 'x'), withRow);
  assert.equal(resurrectLabel('Raise %s: %d gold', 'Rogue', 1000), 'Raise Rogue: 1000 gold');
});

test('choosing Resurrect sends 0x62 with the NPC guid; choosing Hire sends no 0x62 even with a dead mercenary', () => {
  const open = () => {
    const sent = [];
    const npc = createRealmNpc({ send: (b) => sent.push([...b]), classOf: () => 198, resurrectLabel: () => 'Resurrect' });
    const p = new Uint8Array(40); p[0] = 0x27; p[1] = 1; p[2] = 0x34; p[3] = 0x12; npc.handle(p);
    return { npc, sent, before: sent.length, entries: npc.view().entries };
  };
  let o = open();
  assert.ok(o.entries.findIndex((e) => e.action === 'resurrect') < o.entries.findIndex((e) => e.action === 'hire'));
  o.npc.choose(o.entries.find((e) => e.action === 'hire'));
  assert.ok(o.sent.slice(o.before).every((b) => b[0] !== 0x62));
  o = open();
  o.npc.choose(o.entries.find((e) => e.action === 'resurrect'));
  assert.deepEqual(o.sent.slice(o.before).find((b) => b[0] === 0x62), [0x62, 0x34, 0x12, 0, 0]);
});
