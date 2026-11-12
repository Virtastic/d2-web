// The session's client blood: 1.13c D2Client+0x4DC70 cases 6 and 0x13 call D2Client+0x4C500 (spawnTrail,
// d2client_batch_4b420.cpp) on the struck client monster.
//   case 6, the get-hit order (0x69/0x6A, selector 6): life byte b = packet & 0x7F, plus one when > 1. When b is
//     nonzero and equals the client unit's stat 6 >> 8 nothing bleeds; otherwise stat 6 takes b << 8 and 4C500 runs
//     with EAX 0 and a stack count of 1. The unit is in mode 3 (get hit) by then.
//   case 0x13, the life update (0x0C message 19): stat 6 takes b << 8 when b is nonzero, then 4C500 runs with EAX 1
//     and a stack count of 1.
// 4C500 rolls the client unit's own seed (+0x20/+0x24) and spawns each drop through 0xB0450 (source the unit, the
// free cell as target, skill 0, level 1). Callees as the spec names them: #10910 stat 0x8C, #10777 path velocity,
// #10663 the unit's direction, #10331 its room, #10867/#10750 path x/y, #10482 the collision test (mask 1).
// Ours: the server unit stands for the client unit (single player); the orders queue as the server sends them and
// apply at the client frame's tail, in send order. ponytail: the client's stat 6 is not seeded from the unit-add
// packet, so a unit's first get-hit with a nonzero life always runs 4C500.
#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <D2Collision.h>
#include <D2DataTbls.h>
#include <D2StatList.h>
#include <Path/Path.h>
#include <Units/Units.h>
#include "../d2moo/include/D2Game/UNIT/SUnit.h"

namespace d2client_unitseed { uint32_t* unit_seed(const D2GameStrc* game, uint32_t type, uint32_t id); }
namespace d2client_clmissile { void spawn(D2UnitStrc* source, int32_t missileId, int32_t level, int32_t tx, int32_t ty); }
namespace d2client_batch_4b420 {
struct Spawn { void* ctx; uint32_t (*count)(void*); uint32_t (*rate)(void*, uint32_t*); int32_t (*playerLike)(void*); uint32_t (*ord10663)(void*);
 uint32_t (*room)(void*); int32_t (*pathX)(void*, int32_t); int32_t (*pathY)(void*, int32_t); int32_t (*blocked)(void*, uint32_t, int32_t, int32_t); void (*place)(void*, uint32_t); };
uint32_t spawnTrail(const Spawn&, uint32_t*, int32_t, int32_t, int32_t, int32_t, const int32_t[8], const int32_t[8]);
}

static_assert(offsetof(D2MonStatsTxt, wMonStatsEx) == 0x18 && offsetof(D2MonStats2Txt, nBleed) == 0x11D, "MonStats rows");

namespace {
// D2Client+0xD5FFC (x) and +0xD5FDC (y): the cell next to a unit in each of the eight directions.
constexpr int32_t kDx[8] = {0, -1, -2, -1, 0, 1, 2, 1}, kDy[8] = {2, 1, 0, -1, -2, -1, 0, 1};

struct Ctx { D2UnitStrc* u; int32_t stackCount; D2ActiveRoomStrc* room; int32_t x, y; };
uint32_t cbCount(void* p) {
    Ctx* c = static_cast<Ctx*>(p);
    const int32_t n = STATLIST_UnitGetStatValue(c->u, 0x8C, 0);
    return uint32_t(n ? n : c->stackCount);
}
uint32_t cbRate(void* p, uint32_t* ret) {
    const D2UnitStrc* u = static_cast<Ctx*>(p)->u;
    const int32_t cls = int32_t(u->dwClassId);
    const D2DataTablesStrc& t = *sgptDataTables;
    if (cls < 0 || cls >= t.nMonStatsTxtRecordCount) { *ret = uint32_t(cls); return 0; }
    const int32_t row = int16_t(t.pMonStatsTxt[cls].wMonStatsEx);
    if (row < 0 || row >= t.nMonStats2TxtRecordCount) { *ret = uint32_t(row); return 0; }
    *ret = 1;
    return t.pMonStats2Txt[row].nBleed;
}
int32_t cbMoving(void* p) { D2DynamicPathStrc* path = static_cast<Ctx*>(p)->u->pDynamicPath; return path ? PATH_GetVelocity(path) : 0; }
uint32_t cbDirection(void* p) { D2DynamicPathStrc* path = static_cast<Ctx*>(p)->u->pDynamicPath; return path ? PATH_GetDirection(path) : 0; }
uint32_t cbRoom(void* p) { Ctx* c = static_cast<Ctx*>(p); c->room = UNITS_GetRoom(c->u); return 0; }
int32_t cbX(void* p, int32_t) { return PATH_GetXPosition(static_cast<Ctx*>(p)->u->pDynamicPath); }
int32_t cbY(void* p, int32_t) { return PATH_GetYPosition(static_cast<Ctx*>(p)->u->pDynamicPath); }
int32_t cbBlocked(void* p, uint32_t, int32_t x, int32_t y) {
    Ctx* c = static_cast<Ctx*>(p);
    c->x = x; c->y = y;
    return COLLISION_CheckMask(c->room, x, y, 1) != 0;
}
void cbPlace(void* p, uint32_t missileId) { Ctx* c = static_cast<Ctx*>(p); d2client_clmissile::spawn(c->u, int32_t(missileId), 1, c->x, c->y); }

void bleed(D2UnitStrc* u, int32_t gate, uint32_t mode, int32_t count) {
    uint32_t* seed = d2client_unitseed::unit_seed(u->pGame, u->dwUnitType, u->dwUnitId);
    if (!seed) return;   // the client never created the unit
    Ctx c = { u, count, nullptr, 0, 0 };
    const d2client_batch_4b420::Spawn s = { &c, cbCount, cbRate, cbMoving, cbDirection, cbRoom, cbX, cbY, cbBlocked, cbPlace };
    d2client_batch_4b420::spawnTrail(s, seed, int32_t(u->dwUnitType), int32_t(mode), u->pDynamicPath != nullptr, gate, kDx, kDy);
}

struct Event { D2GameStrc* game; uint32_t type, id; int32_t kind; uint32_t life; };
std::vector<Event> g_events;
std::unordered_map<uint64_t, uint32_t> g_life;   // the client unit's stat 6 >> 8
uint64_t key(uint32_t t, uint32_t id) { return uint64_t(t) << 32 | id; }
}

// The server sent the client unit a get-hit order (kind 6, life = the packet's life byte) or a life update (kind 0x13).
extern "C" void d2_client_monster_hit_event(D2UnitStrc* unit, int32_t kind, uint32_t life) {
    if (unit && unit->pGame) g_events.push_back({ unit->pGame, unit->dwUnitType, unit->dwUnitId, kind, life });
}
// 1.13c D2Client+0x4DC70 cases 6 and 0x13 (their life and blood part), at the client frame's tail.
extern "C" void d2_client_monster_hit_flush() {
    std::vector<Event> q;
    q.swap(g_events);
    for (const Event& e : q) {
        D2UnitStrc* u = SUNIT_GetServerUnit(e.game, int32_t(e.type), int32_t(e.id));
        if (!u) continue;
        uint32_t v = e.life & 0x7F;
        if (v > 1) v += 1;
        if (e.kind == 6) {
            bool changed = true;
            if (v) {
                auto it = g_life.find(key(e.type, e.id));
                if (it != g_life.end() && it->second == v) changed = false;
                else g_life[key(e.type, e.id)] = v;
            }
            if (!changed) continue;
            bleed(u, 0, 3, 1);   // the order has put the client unit in get hit
        } else {
            if (v) g_life[key(e.type, e.id)] = v;
            bleed(u, 1, u->dwAnimMode, 1);
        }
    }
}
void d2_client_monster_hit_reset() { g_events.clear(); g_life.clear(); }
