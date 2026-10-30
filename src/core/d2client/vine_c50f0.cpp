// D2Client cltactivefunc 89 (States.txt vine_beast, state 150): per-tick vine beast handler.
//   1.13c D2Client+0xC50F0 (walker 0xA08E0 -> handler table 0xDEA48), spawn helper 0xC4410 -> 0xAFF10 -> 0xB30F0.
// Reading of the body (thunks resolved through PlugY's 1.13c column): the unit carries a hidden ticker skill, Vine Attack (294):
// 10630 GetSkillById, 10302 SetSkillBaseLevel (adds it, level 1), 10980/10243 skill param1 get/set (tick counter),
// 10649/10179 param2/param3 get (last spawn x/y), 10554/10748 param2/param3 set, 10080 distance(unit, x, y).
// Per tick: n = param1; param1 = n+1. fire = n >= cltcalc2 (row+0x118, 35 for Vine Attack) or dist(unit, p2, p3) >= cltcalc2.
// Mode 1 (neutral): missile row+0xF0; mode 2 (walk): missile row+0xEE + rand(2) on the unit's client seed (unit+0x20); other modes: nothing.
// On fire (missile id > 0): param1 = 0, param2/3 = x/y, then the missile create, which submits the missile's TravelSound.
// Not simulated: the missile objects and mode 1's cleanup of the previous missile (0x61420); only the sound submit is kept.
// ponytail: client unit seed {0x29A, unitId} and the one-frame mode lag are fitted to a retail recording trace (3 rand bits), not read from code.
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <unordered_map>
#include <emscripten.h>
#include "../d2moo/include/D2DataTbls.h"
#include "../d2moo/include/D2Dungeon.h"
#include "../d2moo/include/D2Skills.h"
#include "../d2moo/include/D2States.h"
#include "../d2moo/include/Units/Units.h"

namespace d2client_ambient { void submit(int32_t row); }

namespace {
constexpr int32_t kVineAttack = 294;
struct Ticker { int32_t p1 = 0, p2 = 0, p3 = 0; uint32_t seed[2]; uint32_t prevMode = 1; };
std::unordered_map<uint32_t, Ticker> T;
int16_t s16(const uint8_t* r, int o) { int16_t v; std::memcpy(&v, r + o, 2); return v; }
uint32_t u32(const uint8_t* r, int o) { uint32_t v; std::memcpy(&v, r + o, 4); return v; }
}

namespace d2client_vine {
void reset() { T.clear(); }
// 1.13c D2Client+0xC50F0. Called once per frame for each monster carrying the vine_beast state.
void unit(D2UnitStrc* u) {
    if (!u || u->dwUnitType != UNIT_MONSTER || !u->pDynamicPath || kVineAttack >= sgptDataTables->nSkillsTxtRecordCount) return;
    const uint8_t* row = reinterpret_cast<const uint8_t*>(sgptDataTables->pSkillsTxt) + uint32_t(kVineAttack) * 0x23c;
    auto ins = T.try_emplace(u->dwUnitId);
    Ticker& t = ins.first->second;
    if (ins.second) { t.seed[0] = 0x29a; t.seed[1] = u->dwUnitId; }
    const int32_t n = t.p1; t.p1 = n + 1;
    const int32_t x = u->pDynamicPath->tGameCoords.wPosX, y = u->pDynamicPath->tGameCoords.wPosY;
    const int32_t calc = SKILLS_EvaluateSkillFormula(u, u32(row, 0x118), kVineAttack, 1);
    const int32_t dx = std::abs(x - t.p2), dy = std::abs(y - t.p3);   // ponytail: Chebyshev stands in for #10080's distance
    const bool fire = n >= calc || (dx > dy ? dx : dy) >= calc;
    // The client sees the mode the server set one frame earlier (a unit is created neutral); fitted to a retail recording's trace.
    const uint32_t mode = t.prevMode; t.prevMode = u->dwAnimMode;
    int32_t v;
    if (mode == 1) v = s16(row, 0xf0);
    else if (mode == 2) {
        uint64_t s = uint64_t(t.seed[0]) * 0x6ac690c5ull + t.seed[1]; t.seed[0] = uint32_t(s); t.seed[1] = uint32_t(s >> 32);
        v = s16(row, 0xee) + int32_t(t.seed[0] & 1);
    } else return;
    if (!fire || v <= 0 || v >= sgptDataTables->nMissilesTxtRecordCount) return;
    t.p1 = 0; t.p2 = x; t.p3 = y;
    const int32_t snd = sgptDataTables->pMissilesTxt[v].wTravelSound;
    if (snd > 0) d2client_ambient::submit(snd);
}
// All monsters of the hero's neighbourhood with the state.
void tick(D2UnitStrc* hero) {
    D2ActiveRoomStrc* room = hero ? UNITS_GetRoom(hero) : nullptr;
    if (!room) return;
    D2ActiveRoomStrc** rl = nullptr; int32_t nr = 0;
    DUNGEON_GetAdjacentRoomsListFromRoom(room, &rl, &nr);
    for (int32_t i = 0; i < nr; ++i)
        for (D2UnitStrc* u = rl[i]->pUnitFirst; u; u = u->pRoomNext)
            if (u->dwUnitType == UNIT_MONSTER && STATES_CheckState(u, STATE_VINE_BEAST)) unit(u);
}
}
