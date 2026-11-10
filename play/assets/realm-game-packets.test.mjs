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
import { createRealmGameUi } from './realm-game-ui.mjs';
import { createRealmUnitFx, MARK_ROW } from './realm-unit-fx.mjs';

const FIXTURE = String.raw`
#include "d2gs_client_game_packets.h"
#include "d2client_afa90.h"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
using namespace d2::realm;
struct Unit { uint32_t guid, classId; bool npc; uint32_t type, flagEx; };
static std::vector<Unit> g_units;
static const Unit* unit(uint32_t guid, uint32_t type = 1) { for (auto& u : g_units) if (u.guid == guid && u.type == type) return &u; return nullptr; }
static bool isNpc(uint32_t classId) { for (auto& u : g_units) if (u.classId == classId) return u.npc; return false; }
int main() {
    GameInfo info; NpcAlertState alerts; PetState pets;
    d2client_afa90::State ui; d2client_afa90::Env env; unsigned starts = 0;
    std::string line;
    while (std::getline(std::cin, line)) {
        std::istringstream in(line);
        std::string cmd; in >> cmd;
        if (cmd == "unit") { Unit u{}; int npc = 0; uint32_t v = 0; in >> u.guid >> u.classId >> npc; u.npc = npc != 0; u.type = 1; if (in >> v) u.type = v; if (in >> v) u.flagEx = v; g_units.push_back(u); }
        else if (cmd == "hero") { int present = 0, busy = 0; in >> present >> env.playerType >> busy; env.playerPresent = present != 0; env.playerBusy = busy != 0; }
        else if (cmd == "pinned") { int v = 0; in >> v; env.miniPanelPinned = v != 0; }
        else if (cmd == "open") { unsigned slot = 0; in >> slot; if (slot < 38) ui.panel[slot] = 1; }
        else if (cmd == "gone") {
            uint32_t guid = 0, type = 1, v = 0; in >> guid; if (in >> v) type = v;
            for (size_t i = 0; i < g_units.size(); ++i) if (g_units[i].guid == guid && g_units[i].type == type) { g_units.erase(g_units.begin() + static_cast<long>(i)); break; }
            npcAlertUnitRemoved(alerts, guid, type); if (type == 1) petUnitRemoved(pets, guid);
        } else if (cmd == "active") { in >> alerts.hasActiveNpc >> alerts.activeNpc; }
        else if (cmd == "pkt") {
            std::vector<uint8_t> p; unsigned b = 0;
            while (in >> std::hex >> b) p.push_back(static_cast<uint8_t>(b));
            const int size = static_cast<int>(p.size());
            if (p.empty()) return 2;
            const uint8_t* d = p.data();
            switch (d[0]) {
                case 0x01:   // as the engine applies it: the game facts, then the UI start state, which also drops the open NPC
                    if (gameInfoApply(info, d, size)) { d2client_afa90::sub_AFA90(ui, d, size, env); alerts.hasActiveNpc = alerts.activeNpc = 0; ++starts; }
                    break;
                case 0x8a: { const Unit* u = size == 6 ? unit(gamePktU32(d + 2), d[1]) : nullptr; npcAlertApply(alerts, d, size, u != nullptr, u ? u->classId : 0, u ? u->flagEx : 0); break; }
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
    { int n = 0; for (auto& a : alerts.alerts) if (a.type == 1) std::printf("%s%u", n++ ? "," : "", a.guid); }
    std::printf("],\"marks\":[");
    { int n = 0; for (auto& a : alerts.alerts) if (a.type != 1) std::printf("%s{\"type\":%u,\"guid\":%u}", n++ ? "," : "", a.type, a.guid); }
    std::printf("],\"activeNpc\":%s,\"ui\":", alerts.hasActiveNpc ? "true" : "false");
    if (!starts) std::printf("null");
    else { std::printf("{\"starts\":%u,\"panels\":[", starts); for (int i = 0; i < d2client_afa90::kPanelCount; ++i) std::printf("%s%u", i ? "," : "", ui.panel[i]); std::printf("]}"); }
    std::printf(",\"pets\":[");
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
    const built = spawnSync('g++', ['-std=c++20', '-Wall', '-Wextra', '-Werror', '-I', resolve('src/core/realm'), '-I', resolve('src/core/d2client'), source, resolve('src/core/d2client/d2client_afa90.cpp'), '-o', join(dir, 'fixture')], { encoding: 'utf8' });
    assert.equal(built.status, 0, built.stderr || built.stdout);
    binary = join(dir, 'fixture');
  }
  const run = spawnSync(binary, [], { input: script.join('\n') + '\n', encoding: 'utf8' });
  assert.equal(run.status, 0, run.stderr || run.stdout);
  const state = JSON.parse(run.stdout);
  // The protocol the shell reads the client state through (assets/realm-client.mjs), over the fixture's state.
  let active = null;
  return { state, get active() { return active; },
    d2gsClientGameInfo: () => state.info, d2gsClientNpcAlerts: () => state.alerts, d2gsClientUnitMarks: () => state.marks, d2gsClientGameUi: () => state.ui, d2gsClientSetActiveNpc: (guid) => { active = guid; },
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

// ---- 0x8A on a unit that is not a monster: the receiving player's own hero ------------------------
const HERO = 1, OTHER = 2;
const PLAYERS = [`unit ${HERO} 1 0 0`, `unit ${OTHER} 4 0 0`];
// Overlay.txt with the mark's row in place: eight frames at one frame a tick.
const OVERLAY = ['Overlay\tFilename\tFrames\tPreDraw\tTrans\tAnimRate', ...Array.from({ length: MARK_ROW }, () => ''), 'mark\tmark_cel\t8\t0\t3\t16'].join('\r\n');
const unitFx = () => createRealmUnitFx(() => new TextEncoder().encode(OVERLAY), () => {});

test('unit mark: the server marking the player\'s own hero is kept, apart from the NPC marks', (t) => {
  const protocol = client(t, [...TOWN, ...PLAYERS, wantsToTalk(HERO, 0), wantsToTalk(10)]);
  if (!protocol) return;
  const game = realmGameReads({ protocol });
  assert.deepEqual(game.marks, [{ type: 0, guid: HERO }]);
  assert.deepEqual(game.alerts, [10]);
});

test('unit mark: a player the client does not hold, a second packet for the same hero and a unit that takes no overlay add nothing', (t) => {
  const protocol = client(t, [...PLAYERS, 'unit 3 2 0 0 262144', wantsToTalk(HERO, 0), wantsToTalk(HERO, 0), wantsToTalk(99, 0), wantsToTalk(3, 0),
    wantsToTalk(HERO, 1), wantsToTalk(HERO), pkt(0x8a, 0, u32(OTHER), 0)]);
  if (!protocol) return;
  assert.deepEqual(protocol.state.marks, [{ type: 0, guid: HERO }]);
  assert.deepEqual(protocol.state.alerts, []);
});

test('unit mark: a monster and a player sharing a guid are marked apart, and the talk reply leaves the player\'s mark', (t) => {
  const script = [...TOWN, 'unit 10 0 0 0', wantsToTalk(10), wantsToTalk(10, 0)];
  const before = client(t, script);
  if (!before) return;
  assert.deepEqual([before.state.alerts, before.state.marks], [[10], [{ type: 0, guid: 10 }]]);
  const after = client(t, [...script, interactReply(1, 12)]);
  assert.deepEqual([after.state.alerts, after.state.marks], [[], [{ type: 0, guid: 10 }]]);
});

test('unit mark: a player who leaves takes the mark along; the monster of the same guid leaving does not', (t) => {
  const script = [...TOWN, 'unit 10 0 0 0', wantsToTalk(10, 0)];
  assert.deepEqual(client(t, [...script, 'gone 10'])?.state.marks, [{ type: 0, guid: 10 }]);
  assert.deepEqual(client(t, [...script, 'gone 10 0'])?.state.marks, []);
});

test('unit mark: the hero and another marked player draw the looping mark, which wraps instead of ending', (t) => {
  const protocol = client(t, [...PLAYERS, wantsToTalk(HERO, 0), wantsToTalk(OTHER, 0)]);
  if (!protocol) return;
  const fx = unitFx(), marks = realmGameReads({ protocol }).marks;
  const frame = (tick, m = marks) => { fx.step([], tick); const s = { entities: [{ type: 0, id: OTHER }, { type: 1, id: OTHER }, { type: 0, id: 7 }] }; return { hero: fx.apply(s, HERO, m), s }; };
  let f = frame(100);
  assert.deepEqual(f.hero, [{ kind: 'overlay', overlay: MARK_ROW, file: 'mark_cel', frames: 8, rate: 256, trans: 3, predraw: false, age: 1 }]);
  assert.deepEqual(f.s.entities.map((e) => e.stateOverlays?.map((o) => o.file)), [['mark_cel'], undefined, undefined]);
  assert.equal(frame(106).hero[0].age, 7);
  assert.equal(frame(107).hero[0].age, 0);   // the eighth tick wraps
  assert.equal(frame(116).hero[0].age, 1);
  // The mark gone from the engine's list is gone from the frame; without marks nothing is drawn at all.
  f = frame(117, [{ type: 0, guid: OTHER }]);
  assert.deepEqual([f.hero, f.s.entities[0].stateOverlays.length], [[], 1]);
  assert.deepEqual(frame(118, []).hero, []);
  assert.deepEqual(fx.apply({ entities: [] }, HERO), []);
});

// ---- 0x01: the UI start state ---------------------------------------------------------------------
const GAME_START = pkt(0x01, 0x00, 0x04, 0x00, 0x10, 0x00, 0x01, 0x00);
const openSlots = (ui) => ui.panels.flatMap((v, i) => (v ? [i] : []));

test('game start: every open panel closes and the game view, the mini panel and the help button come back', (t) => {
  assert.equal(client(t, [])?.state.ui, null);
  const protocol = client(t, ['open 1', 'open 2', 'open 9', 'open 21', 'open 25', 'open 37', GAME_START]);
  if (!protocol) return;
  assert.equal(protocol.state.ui.panels.length, 38);
  assert.deepEqual(openSlots(protocol.state.ui), [0x00, 0x15, 0x22]);
  assert.equal(protocol.state.ui.starts, 1);
});

test('game start: a stored mini panel choice skips its toggle, and a dying or non-player unit reopens nothing', (t) => {
  assert.deepEqual(openSlots(client(t, ['pinned 1', GAME_START]).state.ui), [0x00, 0x22]);
  assert.deepEqual(openSlots(client(t, ['hero 1 0 0', GAME_START]).state.ui), [0x00, 0x15, 0x22]);
  assert.deepEqual(openSlots(client(t, ['hero 1 0 1', 'open 4', GAME_START]).state.ui), []);
  assert.deepEqual(openSlots(client(t, ['hero 1 1 0', GAME_START]).state.ui), []);
});

test('game start: the open NPC is forgotten, so the server can mark it again; a malformed packet changes nothing', (t) => {
  const protocol = client(t, [...TOWN, 'active 1 12', wantsToTalk(12), GAME_START, wantsToTalk(12)]);
  if (!protocol) return;
  assert.deepEqual([protocol.state.activeNpc, protocol.state.alerts], [false, [12]]);
  const bad = client(t, ['open 1', 'active 1 12', pkt(0x01, 0x02, 0x00)]);
  assert.deepEqual([bad.state.ui, bad.state.activeNpc], [null, true]);
});

test('game start: the page closes its panels once per game start, and only those the client closed', (t) => {
  const first = client(t, [GAME_START]), second = client(t, [GAME_START, GAME_START]);
  if (!first) return;
  const ui = createRealmGameUi(), closed = [];
  const close = (names) => closed.push(names);
  assert.deepEqual(ui.observe(realmGameReads({ protocol: client(t, []) }).ui, close), []);   // nothing before the packet
  const all = ['inventory', 'character', 'skillTree', 'chat', 'gameMenu', 'automap', 'quests', 'waypoint', 'party', 'messageLog', 'stash', 'cube', 'help', 'hireling'];
  assert.deepEqual(ui.observe(realmGameReads({ protocol: first }).ui, close).sort(), [...all].sort());
  assert.deepEqual(ui.observe(realmGameReads({ protocol: first }).ui, close), []);   // the same start again: panels the player opened since stay
  assert.equal(ui.observe(realmGameReads({ protocol: second }).ui, close).length, all.length);   // the next game
  assert.equal(closed.length, 2);
  // A flag the client left set keeps its panel open.
  const kept = { starts: 9, panels: second.state.ui.panels.map((v, i) => (i === 1 ? 1 : v)) };
  assert.equal(ui.observe(kept, close).includes('inventory'), false);
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
