// The session replay's client unit sounds: the D2Client unit-sound entries run on the units the session client holds, their
// 0x88A70 submits queued for the sound pump (sound_pump.cpp logs them in the A-capture shape after 0x88DD0, where retail's
// unit pass falls in the frame). Nothing is queued unless the A trace is armed.
//   0x24490 (a unit entering a mode, bUseGiven = 1): d2_client_unit_mode_sounds, the proved dispatch with its host.
//   0x237E0 object branch (each held object, each frame): the loop row of 0x224F0, the per-mode row of 0x22570.
#include <cstdint>
#include <cstring>
#include <map>
#include <set>
#include <string>
#include <vector>
#include "sound_unit_refs.h"
#include "d2client_5dfd0.h"
#include "../d2moo/include/Units/Units.h"
#include "../d2moo/include/Units/Monster.h"
#include "../d2moo/include/D2States.h"

extern "C" uint32_t d2_client_unit_mode_sounds(uint32_t type, uint32_t id, int32_t cls, uint32_t typeFlags, int32_t superUnique,
                                               uint32_t mode, uint32_t hit, uint32_t cvt, uint32_t flags, int32_t roll);
extern "C" uint32_t d2_client_unit_mode_sound_at(uint32_t i);

void d2client_session_unit_mode(const D2UnitStrc* u, uint32_t mode, int32_t roll, bool hero) {
    if (!u || u->dwUnitType > 1) return;
    uint32_t typeFlags = 0; int32_t su = -1;
    if (u->dwUnitType == 1 && u->pMonsterData) {
        typeFlags = u->pMonsterData->nTypeFlag;
        if (typeFlags & 0x02) su = (int16_t)u->pMonsterData->wBossHcIdx;
    }
    D2UnitStrc* m = const_cast<D2UnitStrc*>(u);
    const uint32_t flags = (hero ? 1u : 0u) | (STATES_CheckState(m, 1) ? 4u : 0u) | (STATES_CheckState(m, 0x92) ? 8u : 0u);
    const uint32_t n = d2_client_unit_mode_sounds(u->dwUnitType, u->dwUnitId, (int32_t)u->dwClassId, typeFlags, su, mode,
                                                  u->dwLastHitClass, 0, flags, roll);
    for (uint32_t i = 0; i < n; ++i) d2client_sound_unit_submit((int32_t)(d2_client_unit_mode_sound_at(i) & 0xFFFF));
}

namespace {
// D2Client+0xF6C58 arrays, words 8..11 (0x224F0): {class, loop row, its mode (8 = any), second row, its mode}.
struct Loop { uint16_t cls, row, mode, row2, mode2; };
const Loop kLoop[] = {
{12,2573,8,0,0},{29,2574,8,0,0},{37,2578,8,0,0},{38,2578,8,0,0},{39,2574,1,2574,2},{48,2578,8,0,0},{49,2578,8,0,0},{65,2599,8,0,0},{66,2599,8,0,0},{82,2541,8,0,0},{101,2578,8,0,0},{102,2578,8,0,0},{103,2583,8,0,0},{116,2611,0,0,0},{117,2578,8,0,0},{131,49,1,0,0},{149,45,2,0,0},{152,45,8,0,0},{160,2576,8,0,0},{161,2576,8,0,0},{162,2576,8,0,0},{192,2631,8,0,0},{212,2544,8,0,0},{239,2574,2,0,0},{245,2574,0,0,0},{252,2586,1,2586,2},{273,2578,8,0,0},{283,2578,8,0,0},{285,2578,8,0,0},{286,2578,8,0,0},{287,2578,8,0,0},{296,2578,8,0,0},{297,2578,8,0,0},{298,2557,2,0,0},{304,2631,8,0,0},{305,2631,8,0,0},{306,2631,8,0,0},{307,2532,8,0,0},{308,2532,8,0,0},{309,2532,8,0,0},{310,2532,8,0,0},{311,2532,8,0,0},{312,2532,8,0,0},{313,2532,8,0,0},{327,2578,8,0,0},{328,2578,8,0,0},{342,2562,2,0,0},{345,2574,8,0,0},{346,2574,8,0,0},{347,2574,8,0,0},{358,2575,8,0,0},{359,2575,8,0,0},{364,2572,0,0,0},{370,2578,8,0,0},{400,2574,8,0,0},{403,2574,8,0,0},{404,667,8,0,0},{408,2574,8,0,0},{409,2578,8,0,0},{411,2578,8,0,0},{412,2578,8,0,0},{434,2574,8,0,0},{435,2578,8,0,0},{436,2578,8,0,0},{437,2578,8,0,0},{438,2578,8,0,0},{439,2574,8,0,0},{451,2656,1,2656,0},{474,2555,4,0,0},{475,2555,4,0,0},{476,2555,4,0,0},{480,2578,8,0,0},{481,2578,8,0,0},{482,2578,8,0,0},{489,2578,8,0,0},{490,2578,8,0,0},{514,2578,8,0,0},{515,2578,8,0,0},{527,2578,8,0,0},{528,2578,8,0,0},{536,2578,8,0,0},{537,2578,8,0,0},{552,2578,8,0,0},{553,2574,8,0,0},{554,2578,8,0,0},{559,2578,8,0,0},{560,2576,8,0,0},{563,2661,1,2661,2},{564,2654,1,0,0}
};
const uint16_t k3D[8] = { 0, 413, 414, 415, 416, 417, 0, 0 };   // D2Client+0xF5998: class 0x3D's rows by mode
// Classes whose array is D2Client+0xF6988 (0x22570 reads it only when the previous mode is <= the new one).
bool tbl6988(uint32_t c) { for (uint32_t k : { 111, 113, 115, 130, 132, 137, 138, 322, 426, 493, 498, 513, 519 }) if (c == k) return true; return false; }
// 1.13c D2Client+0x224F0 (ESI = unit, EDI = mode)
int32_t loopRow(uint32_t cls, uint32_t m) {
    if (cls == 0x3D) return m < 8 ? k3D[m] : 0;
    for (const Loop& l : kLoop) if (l.cls == cls) {
        if (l.mode == m || l.mode == 8) return l.row;
        if (l.mode2 == m || l.mode2 == 8) return l.row2;
        return 0;
    }
    return 0;
}
// 1.13c D2Client+0x22570 (EDI = mode, stack = the previous mode)
int32_t modeRow(uint32_t cls, uint32_t m, uint32_t prev) {
    if (m > 7) return 0;
    if (tbl6988(cls) && prev > m) return 0;
    return d2client_object_slot_row(cls, m);
}
// Per held object: unit+0x70 (seen), +0x74 (mode last handled), and the loop rows its sound list (+0x78) still holds.
struct Obj { uint32_t mode; std::set<int32_t> loops; };
std::map<uint32_t, Obj> g_obj;
}

void d2client_session_objects(const D2SndUnit* u, uint32_t n) {
    std::map<uint32_t, Obj> next;
    for (uint32_t i = 0; i < n; ++i) {
        if (u[i].type != 2) continue;
        const uint32_t cls = u[i].cls, m = u[i].unit ? u[i].unit->dwAnimMode : 0;   // no unit: a client-only object, neutral
        auto it = g_obj.find(u[i].id);
        const bool seen = it != g_obj.end();
        Obj o = seen ? it->second : Obj{ 0, {} };
        if (seen && m == o.mode) { next[u[i].id] = o; continue; }
        const int32_t sel = loopRow(cls, m);
        if (!sel) o.loops.clear();                                            // 0x23070: the unit's sounds stop
        else if (!o.loops.count(sel)) { d2client_sound_unit_submit(sel); o.loops.insert(sel); }
        if (seen || cls == 0x3B) if (const int32_t r = modeRow(cls, m, o.mode)) d2client_sound_unit_submit(r);
        o.mode = m;
        next[u[i].id] = o;
    }
    g_obj.swap(next);
}
namespace { struct PlrPkt { const D2UnitStrc* u; uint32_t mode; }; std::vector<PlrPkt> g_plr; }
// A player mode packet 0x0D (GetHit, block, death: D2Game+0x82010) as the server sends it; the client reaches 0x24490 with the mode
// when the packet arrives, at the frame's tail (d2client_session_player_packets).
extern "C" void d2_client_player_mode_packet(D2UnitStrc* u, int32_t mode) { if (u && u->dwUnitType == 0) g_plr.push_back({ u, uint32_t(mode) }); }
void d2client_session_player_packets() {
    std::vector<PlrPkt> q; q.swap(g_plr);
    for (const PlrPkt& p : q) d2client_session_unit_mode(p.u, p.mode, -1, true);
}
// A unit event packet 0x2C {type, id, event} as the server sends it (D2Game+0xCC6080 -> 0x3F3F0); the client's handler
// (0xAD740 -> 0x23F20) runs under the next packet dispatch, d2client_session_event_packets.
namespace { struct EvPkt { uint32_t type, id, cls, event; }; std::vector<EvPkt> g_ev; }
extern "C" void d2_client_unit_event_packet(const D2UnitStrc* u, int32_t event) {
    if (u && d2client_sound_armed()) g_ev.push_back({ uint32_t(u->dwUnitType), uint32_t(u->dwUnitId), uint32_t(u->dwClassId), uint32_t(event) & 0xFF });
}
struct D2c0x9cce0Env { uint8_t* (*f9cad0)(uint32_t eax); int32_t (*thunk_c7e4)(uint32_t arg); int32_t (*f5ee70)(int32_t v); };
extern "C" int32_t d2c_port_9cce0(uint32_t eax, int32_t kind, uint8_t* player, uint32_t g11c3b8, uint32_t g1197f8, const D2c0x9cce0Env& e);
extern "C" int32_t d2_client_5ee70(const uint8_t* recs, int32_t count, int32_t id, uint32_t (*rand)(uint32_t));
extern "C" uint32_t d2_session_client_rand(uint32_t n);
uint8_t* d2client_9cad0(uint32_t cls);
void d2client_9cad0_reset();
namespace {
uint32_t g_evCycle;
uint8_t g_evPlayer[0x28];
uint32_t evRand(uint32_t n) {   // 1.13c D2Client+0x89640 on the copy's seed
    if (!n) return 0;
    uint32_t s[2]; memcpy(s, g_evPlayer + 0x20, 8);
    const uint64_t v = uint64_t(s[0]) * 0x6ac690c5ull + s[1];
    s[0] = uint32_t(v); s[1] = uint32_t(v >> 32); memcpy(g_evPlayer + 0x20, s, 8);
    return (n & (n - 1)) ? uint32_t(v) % n : ((n - 1) & uint32_t(v));
}
}
void d2client_session_event_packets(uint32_t* heroSeed, uint32_t cycle, uint32_t tick) {
    std::vector<EvPkt> q; q.swap(g_ev);
    for (const EvPkt& p : q) {
        if (p.event != 0x12 || p.type != 1) continue;   // 0x23F20 case 0x12 (an NPC greeting); its other events are not wired here
        // 0x23F20 case 0x12: 0x88A70(player, row = 0x9CCE0(kind 1) on the NPC, offset 0, 1, 0)
        // 0x9CCE0 steps the player's seed (+0x20) itself and 0x5EE70 draws from it through 0x89640: one copy carries both,
        // written back to the hero seed after.
        memset(g_evPlayer, 0, sizeof g_evPlayer);
        memcpy(g_evPlayer + 0x20, heroSeed, 8);
        g_evCycle = cycle;
        D2c0x9cce0Env e{};
        e.f9cad0 = [](uint32_t cls) { return d2client_9cad0(cls); };
        e.thunk_c7e4 = [](uint32_t) { return int32_t(g_evCycle); };   // Ordinal 10396: the act environment's cycle index
        e.f5ee70 = [](int32_t v) {
            if (!d2client_sounds::records()) d2client_sounds::load();
            return d2_client_5ee70(d2client_sounds::records(), d2client_sounds::recordCount(), v, evRand); };
        const int32_t row = d2c_port_9cce0(p.cls, 1, g_evPlayer, 0, tick, e);
        memcpy(heroSeed, g_evPlayer + 0x20, 8);
        d2client_sound_unit_submit_now(row, 1);
    }
}
void d2client_session_unit_sound_reset() { g_obj.clear(); g_plr.clear(); g_ev.clear(); d2client_9cad0_reset(); }
