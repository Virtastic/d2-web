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
#include <cstdint>
#include <vector>
#include <algorithm>
#include <D2Collision.h>
#include <D2Monsters.h>
#include <Path/Path.h>
#include <Path/PathMisc.h>
#include <Units/Units.h>

namespace d2client_ambient { void submit(int32_t row); }

namespace d2client_blizzard {
namespace {
constexpr int32_t kBlizzard = 59;        // Skills.txt id
constexpr int32_t kSoundLoop = 2393, kSoundImpact = 2394;   // Missiles.txt blizzardcenter / blizzard1-4 TravelSound, HitSound
constexpr int32_t kCastDelay = 8;        // frames from the cast to the center's creation (fitted to a retail recording of skill 59)
constexpr int32_t kCenterRange = 100, kFrequency = 4, kShardLife = 24, kRadius = 7;   // Skills.txt calc1 (par1), calc2 (par3)
constexpr uint32_t kFirstId = 0xDC;      // the client's unit id counter at the first cast (the first center's id in a retail recording of skill 59)
constexpr int32_t kIceBreakLife = 30;    // Missiles.txt icebreaklarge Range
struct Obj { uint32_t id; bool center; int32_t life, x, y; bool ice = false; };
struct Cast { int64_t due; int32_t type; uint32_t id; int32_t tx, ty; };
std::vector<Obj> objs;
std::vector<Cast> casts;
int64_t frame = 0;
uint32_t nextId = kFirstId;
int32_t shatters = 0;   // monsters shattered this frame, before the walk ran
D2ActiveRoomStrc* room = nullptr;

void spawn(bool center, int32_t x = 0, int32_t y = 0) { objs.push_back({nextId++, center, center ? kCenterRange : kShardLife, x, y}); }

// 1.13c D2Client+0x1380 on the unit seed (D2Common's SEED_RollLimitedRandomNumber)
uint32_t roll(uint64_t& s, uint32_t n) {
    s = (s & 0xFFFFFFFFull) * 0x6AC690C5ull + (s >> 32);
    return (n & (n - 1)) ? static_cast<uint32_t>(s) % n : static_cast<uint32_t>(s) & (n - 1);
}

// 1.13c D2Client+0xB8DF0 past the frequency gate: false when the rolled cell is blocked for missiles.
bool shardCell(const Obj& c, int32_t elapsed) {
    uint64_t s = (0x29Aull << 32) | static_cast<uint32_t>(elapsed + c.x);
    const int32_t dx = (kRadius - 1) - static_cast<int32_t>(roll(s, 2 * kRadius - 2));
    const int32_t dy = (kRadius - 1) - static_cast<int32_t>(roll(s, 2 * kRadius - 2));
    return !room || !COLLISION_CheckMask(room, c.x + dx, c.y + dy, COLLIDE_MISSILE_BARRIER);
}
}

void reset() { objs.clear(); casts.clear(); frame = 0; nextId = kFirstId; room = nullptr; shatters = 0; }

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
            if (it->center) {
                const int32_t elapsed = kCenterRange - it->life;
                --it->life;
                const bool ended = it->life < 1;
                if (elapsed % kFrequency == 0 && shardCell(*it, elapsed)) { spawn(false); d2client_ambient::submit(kSoundLoop); }
                if (ended) objs.erase(std::find_if(objs.begin(), objs.end(), [id](const Obj& o) { return o.id == id; }));
            } else if (--it->life < 1) {
                const bool ice = it->ice;
                objs.erase(it);
                if (ice) { ++nextId; continue; }   // icebreaklargemelt
                d2client_ambient::submit(kSoundImpact);
                ++nextId;                          // blizzardexplode
            }
        }
    }
    for (; shatters > 0; --shatters) { objs.push_back({nextId, false, kIceBreakLife, 0, 0, true}); nextId += 2; }
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
        spawn(true, x, y); d2client_ambient::submit(kSoundLoop);
    }
}
}
