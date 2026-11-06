// D2Client client missiles of Blizzard (Skills.txt id 59): the blizzardcenter do-function (Missiles.txt pCltDoFunc 13) 1.13c
// D2Client+0xBBC10 and the blizzard1-4 shards it spawns through 0xB8DF0.
// The client runs its own center whatever the server did. The center (Range 100) spawns a shard on every 4th frame of its
// life (elapsed % Skills.txt frequency), a shard lives 24 frames (CltParam 1/2); the shard's TravelSound (2393) plays at its
// creation, its HitSound (2394) when its life ends. The center's TravelSound (2393) plays at creation.
// Client units sit in a 128-slot hash by id, and each frame walks the slots in order, newest-first within a slot: that order
// is why a shard ends on its 23rd or 24th frame after birth. Ported as a sound stream (no gfx).
// The id counter is shared by every client missile: a shard's end (pCltHitFunc 19) spawns one blizzardexplode (one id), and a
// monster shattering (state 107 on) spawns icebreaklarge + icebreaksmoke (two ids) after the walk, the large one spawning
// icebreaklargemelt (one id) when its Range 30 ends (pCltHitFunc 31). None of them plays a sound here.
// 0xB8DF0 places a shard: it reseeds the center (unit+0x20: low = elapsed + the center's x, high 0x29A, D2Common #10614),
// rolls dx then dy on it (each (calc1 - 1) - roll(2 * calc1 - 2), Skills.txt calc1 = radius 7) and skips the spawn when the
// cell holds collision mask 4 (#10482): no unit, no id, no sound. The center sits on its target (0xA1340: the target
// unit while it is alive (a corpse reads as (0, 0)), else the point, else two subtiles off the owner by its direction). The row roll (0xBBC10, blizzard1-4) picks no sound.
// Drawing: every Blizzard sprite a player sees is one of these client units; the server's blizzard1 shards are not ClientSend
// and never reach a retail client. Each frame 0xBBC10 first rolls the shard row on the center's seed, CltSubMissile1 +
// roll(CltSubMissile2 - CltSubMissile1 + 1) (blizzard1-4: icestormfallvar01/02 and Blizzard twice), then 0xB8DF0 reseeds and places.
// A client unit's creation (0xB30F0 through the generic creator) rolls the client room at its cell for its init seed ({init, 666});
// the shard's request flag 0x2000 then rolls its facing, roll(64) on its own seed. 0xB2E00 sets its fall, height CltParam1 (120)
// falling CltParam2 (5) a frame, 24 frames. A shard's end (pCltHitFunc 19, 0xB1080) rolls the impact row on the shard's seed,
// CltHitSubMissile1 + roll(CltHitSubMissile2 - CltHitSubMissile1 + 1) (blizzardexplode1-3), created on the shard's cell for
// its Range (6). AnimSpeed 16 (<< 4) steps one frame a tick; the shards loop their AnimLen, the impacts do not.
// The unit draw places a missile by its gfx x/y offsets (0x6AA30); the fall height is not a screen offset there.
#include <cstdint>
#include <vector>
#include <emscripten.h>
#include <D2DataTbls.h>
#include <algorithm>
#include <D2Collision.h>
#include <D2Monsters.h>
#include <Path/Path.h>
#include <Path/PathMisc.h>
#include <Units/Units.h>

namespace d2client_ambient { void submit(int32_t row); }
namespace d2client_unitseed { uint32_t roll(const D2ActiveRoomStrc* r); }

namespace d2client_blizzard {
namespace {
constexpr int32_t kBlizzard = 59;        // Skills.txt id
constexpr int32_t kSoundLoop = 2393, kSoundImpact = 2394;   // Missiles.txt blizzardcenter / blizzard1-4 TravelSound, HitSound
constexpr int32_t kCastDelay = 8;        // frames from the cast to the center's creation (fitted to a retail recording of skill 59)
constexpr int32_t kCenterRange = 100, kFrequency = 4, kShardLife = 24, kRadius = 7;   // Skills.txt calc1 (par1), calc2 (par3)
constexpr uint32_t kFirstId = 0xDC;      // the client's unit id counter at the first cast (the first center's id in a retail recording of skill 59)
constexpr int32_t kIceBreakLife = 30;    // Missiles.txt icebreaklarge Range
constexpr int32_t kCenterRow = 158;      // Missiles.txt blizzardcenter
enum Kind : uint8_t { kShard, kCenter, kIce, kExplode };
struct Obj { uint32_t id; Kind kind; int32_t life, x, y; int32_t row = 0, age = 0; uint32_t dir = 0; uint64_t seed = 0; };
struct Cast { int64_t due; int32_t type; uint32_t id; int32_t tx, ty; };
std::vector<Obj> objs;
std::vector<Cast> casts;
int64_t frame = 0;
uint32_t nextId = kFirstId;
int32_t shatters = 0;   // monsters shattered this frame, before the walk ran
D2ActiveRoomStrc* room = nullptr;

const D2MissilesTxt* txt(int32_t row) { return row >= 0 && row < static_cast<int32_t>(sgptDataTables->nMissilesTxtRecordCount) ? &sgptDataTables->pMissilesTxt[row] : nullptr; }
// a client unit's init seed: the client room at its cell rolls once (0 when the client has no room there); its seed is {init, 666}
uint64_t created(int32_t x, int32_t y) {
    D2ActiveRoomStrc* r = room ? COLLISION_GetRoomBySubTileCoordinates(room, x, y) : nullptr;
    return (666ull << 32) | d2client_unitseed::roll(r);
}

// 1.13c D2Client+0x1380 on the unit seed (D2Common's SEED_RollLimitedRandomNumber)
uint32_t roll(uint64_t& s, uint32_t n) {
    s = (s & 0xFFFFFFFFull) * 0x6AC690C5ull + (s >> 32);
    return (n & (n - 1)) ? static_cast<uint32_t>(s) % n : static_cast<uint32_t>(s) & (n - 1);
}

// 1.13c D2Client+0xB8DF0 past the frequency gate: false when the rolled cell is blocked for missiles.
bool shardCell(Obj& c, int32_t elapsed, int32_t& x, int32_t& y) {
    c.seed = (0x29Aull << 32) | static_cast<uint32_t>(elapsed + c.x);
    const int32_t dx = (kRadius - 1) - static_cast<int32_t>(roll(c.seed, 2 * kRadius - 2));
    const int32_t dy = (kRadius - 1) - static_cast<int32_t>(roll(c.seed, 2 * kRadius - 2));
    x = c.x + dx; y = c.y + dy;
    return !room || !COLLISION_CheckMask(room, x, y, COLLIDE_MISSILE_BARRIER);
}
// row + roll(span) for a Missiles.txt sub-missile pair (0xBBC10's CltSubMissile1/2, 0xB1080's CltHitSubMissile1/2)
int32_t pick(uint64_t& seed, int32_t lo, int32_t hi) { return hi > lo ? lo + static_cast<int32_t>(roll(seed, static_cast<uint32_t>(hi - lo + 1))) : lo; }
}

struct Rec { uint32_t id, classId; int32_t x, y; uint32_t direction, framePrecise, z, trans; };
static_assert(sizeof(Rec) == 32, "the browser reads 32-byte records");
std::vector<Rec> recs;

void reset() { recs.clear(); objs.clear(); casts.clear(); frame = 0; nextId = kFirstId; room = nullptr; shatters = 0; }

// the server set state 107 (shatter) on a monster: its client icebreak missiles take their ids after this frame's walk
void shatter() { ++shatters; }

void cast(int32_t skillId, D2UnitStrc* target, int32_t tx, int32_t ty) {
    if (skillId == kBlizzard) casts.push_back({frame + kCastDelay, target ? static_cast<int32_t>(target->dwUnitType) : -1, target ? target->dwUnitId : 0u, tx, ty});
}

void tick(D2UnitStrc* owner, D2UnitStrc* (*find)(int32_t type, uint32_t id)) {
    room = owner ? UNITS_GetRoom(owner) : nullptr;
    ++frame;
    for (uint32_t b = 0; b < 128; ++b) {
        std::vector<uint32_t> ids;
        for (const Obj& o : objs) if ((o.id & 127u) == b) ids.push_back(o.id);
        std::sort(ids.rbegin(), ids.rend());
        for (uint32_t id : ids) {
            auto it = std::find_if(objs.begin(), objs.end(), [id](const Obj& o) { return o.id == id; });
            if (it == objs.end()) continue;
            if (it->kind == kCenter) {
                const int32_t elapsed = kCenterRange - it->life;
                --it->life;
                const bool ended = it->life < 1;
                const D2MissilesTxt* ct = txt(kCenterRow);
                const int32_t row = ct ? pick(it->seed, static_cast<int16_t>(ct->wCltSubMissile[0]), static_cast<int16_t>(ct->wCltSubMissile[1])) : 0;
                int32_t sx = 0, sy = 0;
                if (elapsed % kFrequency == 0 && shardCell(*it, elapsed, sx, sy)) {
                    Obj s{nextId++, kShard, kShardLife, sx, sy, row};
                    s.seed = created(sx, sy);
                    s.dir = roll(s.seed, 64);
                    objs.push_back(s);
                    d2client_ambient::submit(kSoundLoop);
                }
                if (ended) objs.erase(std::find_if(objs.begin(), objs.end(), [id](const Obj& o) { return o.id == id; }));
            } else if (--it->life < 1) {
                const Obj o = *it;
                objs.erase(it);
                if (o.kind == kIce) { ++nextId; continue; }   // icebreaklargemelt
                if (o.kind == kExplode) continue;
                d2client_ambient::submit(kSoundImpact);
                const D2MissilesTxt* st = txt(o.row);
                Obj e{nextId++, kExplode, 0, o.x, o.y};      // blizzardexplode1-3
                uint64_t seed = o.seed;
                e.row = st ? pick(seed, static_cast<int16_t>(st->wCltHitSubMissile[0]), static_cast<int16_t>(st->wCltHitSubMissile[1])) : 0;
                const D2MissilesTxt* et = txt(e.row);
                e.life = et ? et->wRange : 0;
                e.seed = created(o.x, o.y);
                if (e.life > 0) objs.push_back(e);
            }
        }
    }
    for (; shatters > 0; --shatters) { objs.push_back({nextId, kIce, kIceBreakLife, 0, 0}); nextId += 2; }
    for (size_t i = 0; i < casts.size();) {
        if (casts[i].due != frame) { ++i; continue; }
        const Cast c = casts[i];
        casts.erase(casts.begin() + i);
        // 1.13c D2Client+0xA1340: the path's target unit while it is selectable (0x2A0B0; a dying or dead monster
        // is not), else the target point, else 0xA0B30 two subtiles off the owner by its direction
        D2UnitStrc* target = c.type >= 0 && find ? find(c.type, c.id) : nullptr;
        // a dying or dead monster's client unit reads as (0, 0): the client cast turned the owner toward that, and the zero
        // coordinate sends 0xA1340 to the fallback
        const bool corpse = target && target->dwUnitType == UNIT_MONSTER && (target->dwAnimMode == MONMODE_DEATH || target->dwAnimMode == MONMODE_DEAD);
        if (corpse) target = nullptr;
        static constexpr int8_t kDx[8] = {0, -1, -2, -1, 0, 1, 2, 1}, kDy[8] = {2, 1, 0, -1, -2, -1, 0, 1};
        const int32_t d = !owner ? 0 : corpse ? PATH_ComputeDirection(UNITS_GetXPosition(owner), UNITS_GetYPosition(owner), 0, 0) >> 3
                        : owner->pDynamicPath ? PATH_GetDirection(owner->pDynamicPath) >> 3 : 0;
        const bool point = c.type < 0;
        const int32_t x = target ? UNITS_GetXPosition(target) : point ? c.tx : owner ? UNITS_GetXPosition(owner) + kDx[d & 7] : 0;
        const int32_t y = target ? UNITS_GetYPosition(target) : point ? c.ty : owner ? UNITS_GetYPosition(owner) + kDy[d & 7] : 0;
        Obj o{nextId++, kCenter, kCenterRange, x, y, kCenterRow};
        o.seed = created(x, y);
        objs.push_back(o);
        d2client_ambient::submit(kSoundLoop);
    }
    // what the client draws this frame: the shards and impacts (the center's CelFile is null)
    recs.clear();
    for (Obj& o : objs) {
        if (o.kind != kShard && o.kind != kExplode) continue;
        const D2MissilesTxt* t = txt(o.row);
        const int32_t len = t && t->nAnimLen > 0 ? t->nAnimLen : 1;
        const int32_t f = !t || !t->nLoopAnim ? std::min(o.age, len - 1) : o.age % len;
        const int32_t z = o.kind == kShard ? std::max(0, static_cast<int32_t>(t ? t->dwCltParam[0] : 0) - o.age * std::max(1, static_cast<int32_t>(t ? t->dwCltParam[1] : 1))) : 0;
        recs.push_back({o.id, static_cast<uint32_t>(o.row), o.x, o.y, o.dir, static_cast<uint32_t>(f) << 8, static_cast<uint32_t>(z), t ? t->nTrans : 0u});
        ++o.age;
    }
}
}

// The client's Blizzard sprites of the current frame, 32 bytes each: id, Missiles.txt row, subtile x, y, 64-slot facing,
// frame (8.8), fall height (0xB2E00's z >> 11) and nTrans.
extern "C" EMSCRIPTEN_KEEPALIVE const void* d2_client_blizzard_missiles() { return d2client_blizzard::recs.data(); }
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_blizzard_missile_count() { return static_cast<uint32_t>(d2client_blizzard::recs.size()); }
