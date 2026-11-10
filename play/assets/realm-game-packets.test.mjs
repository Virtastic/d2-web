// A joined game's game flags, NPC speech marks and mercenary: server packets go through the client's
// packet state (src/core/realm/d2gs_client_game_packets.h, compiled natively here), and what the shell's
// view returns from it is asserted. The fixture below resolves units from a small table, as the engine
// does from its monster list, and prints the state in the shapes realm-client.mjs hands the view.
import test from 'node:test';
import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { mkdtempSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join, resolve } from 'node:path';
import { realmGameReads, realmViewSnapshot } from './realm-client-view.mjs';
import { hirelingMercFrom } from './hireling-panel.mjs';

const FIXTURE = String.raw`
#include "d2gs_client_game_packets.h"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
using namespace d2::realm;
struct Unit { uint32_t guid, classId; bool npc; };
static std::vector<Unit> g_units;
static const Unit* unit(uint32_t guid) { for (auto& u : g_units) if (u.guid == guid) return &u; return nullptr; }
static bool isNpc(uint32_t classId) { for (auto& u : g_units) if (u.classId == classId) return u.npc; return false; }
int main() {
    GameInfo info; NpcAlertState alerts; PetState pets;
    std::string line;
    while (std::getline(std::cin, line)) {
        std::istringstream in(line);
        std::string cmd; in >> cmd;
        if (cmd == "unit") { Unit u{}; int npc = 0; in >> u.guid >> u.classId >> npc; u.npc = npc != 0; g_units.push_back(u); }
        else if (cmd == "gone") {
            uint32_t guid = 0; in >> guid;
            for (size_t i = 0; i < g_units.size(); ++i) if (g_units[i].guid == guid) { g_units.erase(g_units.begin() + static_cast<long>(i)); break; }
            npcAlertUnitRemoved(alerts, guid); petUnitRemoved(pets, guid);
        } else if (cmd == "active") { in >> alerts.hasActiveNpc >> alerts.activeNpc; }
        else if (cmd == "pkt") {
            std::vector<uint8_t> p; unsigned b = 0;
            while (in >> std::hex >> b) p.push_back(static_cast<uint8_t>(b));
            const int size = static_cast<int>(p.size());
            if (p.empty()) return 2;
            const uint8_t* d = p.data();
            switch (d[0]) {
                case 0x01: gameInfoApply(info, d, size); break;
                case 0x8a: { const Unit* u = size == 6 && d[1] == 1 ? unit(gamePktU32(d + 2)) : nullptr; npcAlertApply(alerts, d, size, u != nullptr, u ? u->classId : 0); break; }
                case 0x28: npcInteractReplyApply(alerts, d, size, size == 103 && unit(gamePktU32(d + 2)) != nullptr, isNpc); break;
                case 0x81: petAssignApply(pets, d, size); break;
                case 0x7f: petLifeApply(pets, d, size, size == 10 && unit(gamePktU32(d + 4)) != nullptr); break;
                default: petStatApply(pets, d, size, size >= 7 && unit(gamePktU32(d + 2)) != nullptr); break;
            }
        }
    }
    std::printf("{\"info\":");
    if (!info.received) std::printf("null");
    else std::printf("{\"difficulty\":%u,\"gameFlags\":%u,\"expansion\":%s,\"ladder\":%s,\"flags\":%u}", info.difficulty, info.gameFlags,
        info.expansion ? "true" : "false", info.ladder ? "true" : "false", gameInfoSaveFlags(info));
    std::printf(",\"alerts\":[");
    for (size_t i = 0; i < alerts.alerts.size(); ++i) std::printf("%s%u", i ? "," : "", alerts.alerts[i].guid);
    std::printf("],\"pets\":[");
    for (size_t i = 0; i < pets.pets.size(); ++i) {
        const Pet& p = pets.pets[i];
        std::printf("%s{\"classId\":%u,\"kind\":%u,\"guid\":%u,\"owner\":%u,\"area\":%u,\"lifePct\":%u,\"seed\":%u,\"nameId\":%u,\"unitPresent\":%s}", i ? "," : "",
            p.classId, p.kind, p.guid, p.owner, p.area, p.lifePct, p.seed, p.nameId, unit(p.guid) ? "true" : "false");
    }
    std::printf("],\"stats\":[");
    for (size_t i = 0; i < pets.stats.size(); ++i) std::printf("%s[%u,%d,%d]", i ? "," : "", pets.stats[i].guid, pets.stats[i].stat, pets.stats[i].value);
    std::printf("]}\n");
    return 0;
}
`;

let dir = null, binary = null, unavailable = false;
function client(t, script) {
  if (unavailable) { t.skip('g++ unavailable for the native fixture'); return null; }
  if (!binary) {
    if (spawnSync('g++', ['--version'], { stdio: 'ignore' }).status !== 0) { unavailable = true; t.skip('g++ unavailable for the native fixture'); return null; }
    dir = mkdtempSync(join(tmpdir(), 'd2-game-packets-'));
    const source = join(dir, 'fixture.cpp');
    writeFileSync(source, FIXTURE);
    const built = spawnSync('g++', ['-std=c++20', '-Wall', '-Wextra', '-Werror', '-I', resolve('src/core/realm'), source, '-o', join(dir, 'fixture')], { encoding: 'utf8' });
    assert.equal(built.status, 0, built.stderr || built.stdout);
    binary = join(dir, 'fixture');
  }
  const run = spawnSync(binary, [], { input: script.join('\n') + '\n', encoding: 'utf8' });
  assert.equal(run.status, 0, run.stderr || run.stdout);
  const state = JSON.parse(run.stdout);
  // The protocol the shell reads the client state through (assets/realm-client.mjs), over the fixture's state.
  let active = null;
  return { state, get active() { return active; },
    d2gsClientGameInfo: () => state.info, d2gsClientNpcAlerts: () => state.alerts, d2gsClientSetActiveNpc: (guid) => { active = guid; },
    d2gsClientPets: () => state.pets,
    d2gsClientPetStat: (guid, stat) => state.stats.find((s) => s[0] === guid && s[1] === stat)?.[2] ?? null };
}
test.after(() => { if (dir) rmSync(dir, { recursive: true, force: true }); });

const hex = (bytes) => bytes.map((b) => b.toString(16).padStart(2, '0')).join(' ');
const u16 = (v) => [v & 0xff, (v >>> 8) & 0xff];
const u32 = (v) => [v & 0xff, (v >>> 8) & 0xff, (v >>> 16) & 0xff, (v >>> 24) & 0xff];
const pkt = (...bytes) => 'pkt ' + hex(bytes.flat());
const wantsToTalk = (guid, type = 1) => pkt(0x8a, type, u32(guid));
const interactReply = (kind, guid) => pkt(0x28, kind, u32(guid), 0, new Array(96).fill(0));
const actor = () => null;
const pending = (protocol, monsters) => realmViewSnapshot({ base: null, monsters, actor, frame: 0, alerts: realmGameReads({ protocol }).alerts })
  .entities.filter((e) => e.speechPending === true).map((e) => e.id);

test('game flags: the view has no client flags until the server sends its game flags packet', (t) => {
  const protocol = client(t, []);
  if (!protocol) return;
  assert.equal('clientFlags' in realmGameReads({ protocol }).reads, false);
});

test('game flags: the packet a recorded server sent on join reads as an expansion softcore game', (t) => {
  // The first eight bytes a recorded 1.13c server sent the joining client: normal difficulty, expansion, not ladder.
  const protocol = client(t, [pkt(0x01, 0x00, 0x04, 0x00, 0x10, 0x00, 0x01, 0x00)]);
  if (!protocol) return;
  assert.deepEqual(protocol.state.info, { difficulty: 0, gameFlags: 0x00100004, expansion: true, ladder: false, flags: 0x20 });
  const { reads } = realmGameReads({ protocol });
  assert.equal(reads.clientFlags & 0x20, 0x20);   // the area name block draws EXPANSION
  assert.equal(reads.clientFlags & 0x04, 0);      // the death screen is the softcore one
});

test('game flags: a classic hardcore ladder nightmare game reads as one, and a short packet is ignored', (t) => {
  const protocol = client(t, [pkt(0x01, 0x01, u32(0x00000804), 0x00, 0x01), pkt(0x01, 0x02, 0x00)]);
  if (!protocol) return;
  assert.deepEqual(protocol.state.info, { difficulty: 1, gameFlags: 0x804, expansion: false, ladder: true, flags: 0x44 });
  assert.equal(realmGameReads({ protocol }).reads.clientFlags, 0x44);
});

const TOWN = ['unit 10 154 1', 'unit 11 534 1', 'unit 12 148 1', 'unit 13 63 0'];
const MONSTERS = [10, 11, 12, 13].map((guid) => ({ guid, classId: 0, mode: 1, x: 0, y: 0, direction: 0 }));

test('speech mark: the NPC the server names is marked; an absent unit, the speaking-only class and the open NPC are not', (t) => {
  const protocol = client(t, [...TOWN, 'active 1 12', wantsToTalk(10), wantsToTalk(11), wantsToTalk(12), wantsToTalk(99), wantsToTalk(10)]);
  if (!protocol) return;
  assert.deepEqual(pending(protocol, MONSTERS), [10]);
});

test('speech mark: the view tells the client which NPC is open before it reads the marks', (t) => {
  const protocol = client(t, TOWN);
  if (!protocol) return;
  realmGameReads({ protocol, npcGuid: 12 });
  assert.equal(protocol.active, 12);
  realmGameReads({ protocol });
  assert.equal(protocol.active, null);
});

test('speech mark: a talk reply for a unit that is there clears the mark from every NPC, and only from NPCs', (t) => {
  const marked = [...TOWN, wantsToTalk(10), wantsToTalk(12), wantsToTalk(13)];
  const before = client(t, marked);
  if (!before) return;
  assert.deepEqual(pending(before, MONSTERS), [10, 12, 13]);
  assert.deepEqual(pending(client(t, [...marked, interactReply(1, 12)]), MONSTERS), [13]);
  // A reply that only carries quest bytes (kind 6), or names a unit the client does not have, clears nothing.
  assert.deepEqual(pending(client(t, [...marked, interactReply(6, 12), interactReply(1, 99)]), MONSTERS), [10, 12, 13]);
});

test('speech mark: a unit that leaves takes its mark with it', (t) => {
  const protocol = client(t, [...TOWN, wantsToTalk(10), wantsToTalk(12), 'gone 10']);
  if (!protocol) return;
  assert.deepEqual(pending(protocol, MONSTERS), [12]);
});

const petAssign = (kind, classId, owner, guid, seed, nameId) => pkt(0x81, kind, u16(classId), u32(owner), u32(guid), u32(seed), u32(nameId));
const petLife = (guid, pct, area) => pkt(0x7f, 0, u16(pct), u32(guid), u16(area));
const mercStat = (op, stat, guid, value) => pkt(op, stat, u32(guid), op === 0xa0 ? u32(value) : op === 0x9f || op === 0xa2 ? u16(value) : [value]);
const HIRED = ['unit 50 271 0', petAssign(7, 271, 1, 50, 0x1234abcd, 1021),
  mercStat(0xa0, 7, 50, 300 << 8), mercStat(0x9e, 12, 50, 15), mercStat(0x9f, 0, 50, 290), mercStat(0xa1, 12, 50, 1), mercStat(0xa2, 0, 50, 10),
  mercStat(0x9e, 39, 50, 25), petLife(50, 50, 1)];
const world = { wornBy: (guid) => (guid === 50 ? [
  { bodyLoc: 4, invFile: 'invspr', invWidth: 2, invHeight: 4, twoHanded: true }, { bodyLoc: 3, invFile: 'invqlt', invWidth: 2, invHeight: 3, twoHanded: false }] : []) };
const tables = { inventory: { components: (name) => (name === 'Hireling2' ? Array.from({ length: 60 }, (_, i) => i) : null) } };
const mercOf = (protocol, monsters, ownGuid = 1) => {
  const { reads } = realmGameReads({ protocol, world, tables, ownGuid, monsters, string: (i) => 'name ' + i });
  return { reads, merc: hirelingMercFrom(reads.hirelingSource, reads.hirelingRoster, -40) };
};

test('hireling: the assigned mercenary, its stats, life share and worn items reach the panel', (t) => {
  const protocol = client(t, HIRED);
  if (!protocol) return;
  const { reads, merc } = mercOf(protocol, [{ guid: 50, mode: 1 }]);
  assert.deepEqual(reads.hirelingRoster, [{ guid: 50, classId: 271, nameId: 1021, name: 'name 1021', seed: 0x1234abcd, flags: 0, hirelingId: -1,
    level: 16, life: 150 << 8, maxLife: 300 << 8, lifePct: 50, unitPresent: true }]);
  assert.equal(merc.name, 'name 1021');
  assert.equal(merc.dead, false);
  assert.deepEqual(merc.life, { pct: 50, max: 300 });
  assert.equal(merc.stats[12], 16);    // set to 15, then one added
  assert.equal(merc.stats[0], 300);    // set to 290, then ten added
  assert.equal(merc.stats[39], 25);
  assert.equal(merc.stats[41], null);  // never sent
  assert.equal(merc.resistPenalty, -40);
  assert.deepEqual(merc.items.map((i) => [i.bodyLoc, i.invFile]), [[4, 'invspr'], [3, 'invqlt']]);
  assert.equal(merc.twoHanded, true);
  assert.deepEqual(merc.slots[4], { left: 0, right: 1, top: 2, bottom: 3, w: 4, h: 5 });
  assert.deepEqual(merc.slots[1], { left: 18, right: 19, top: 20, bottom: 21, w: 22, h: 23 });
});

test('hireling: another player\'s mercenary, a pet that is not a hireling, and a mercenary whose unit is gone draw no panel', (t) => {
  const theirs = client(t, HIRED);
  if (!theirs) return;
  assert.equal(mercOf(theirs, [{ guid: 50, mode: 1 }], 2).merc, null);
  assert.equal(mercOf(client(t, ['unit 60 419 0', petAssign(4, 419, 1, 60, 0, 0)]), []).merc, null);
  const gone = mercOf(client(t, [...HIRED, 'gone 50']), []);
  assert.equal(gone.reads.hirelingRoster[0].unitPresent, false);
  assert.equal(gone.merc, null);
});

test('hireling: a new mercenary replaces the listed one, a stat for a unit that is not there is dropped, and a dead one reads dead', (t) => {
  const protocol = client(t, [...HIRED, mercStat(0x9e, 12, 51, 40), 'unit 51 338 0', petAssign(7, 338, 1, 51, 7, 1040), mercStat(0x9e, 12, 51, 30)]);
  if (!protocol) return;
  assert.deepEqual(protocol.state.pets.map((p) => [p.guid, p.kind, p.lifePct]), [[51, 7, 100]]);
  const { merc } = mercOf(protocol, [{ guid: 51, mode: 12 }]);
  assert.equal(merc.name, 'name 1040');
  assert.equal(merc.stats[12], 30);
  assert.equal(merc.dead, true);
  assert.deepEqual(merc.life, { pct: 100, max: 0 });
});
