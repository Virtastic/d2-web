// D2Client client-unit seeds. Every client unit creator (player 1.13c D2Client+0xA7630, monster +0xA788B,
// item +0x833F0/+0x835C0, generic +0xA7400: object, missile, tile) finds the client room at the unit's
// coordinates (+0xA5A60) and, when there is one, rolls that room's seed (room+0x6C/+0x70) once; the roll is
// the unit's init seed (+0x28) and its seed (+0x20) is SEED_InitLowSeed of it. Coordinates (0,0) skip the
// room lookup and the roll. The client room's seed starts as DUNGEON_AllocRoom's (the DRLG gives client and
// server the same low seed per room).
// The local player never rolls: the server creates it unplaced (the SUNIT_AllocUnitData call at D2Game+0x55666: type 0,
// x=y=0, no room) and sends its own 0x59 at once (SUNITMSG_FirstFn, D2Game+0x7B0A0), so it carries (0,0);
// the room-add paths skip the client's own player (LEVEL_AddClient, the room-change client loop).
// Ours: the client room seeds live beside the server rooms; a server unit created into a room stands for the
// unit-add packet the client would build it from; client missiles roll where we emit them (vine_c50f0.cpp).
// ponytail: units that reach a room only after creation (a dropped item, a unit moving into a room before the
// client sees it) are not counted where the client would count them; a live player is taken as the local
// one (the sessions are single-player), a corpse is a unit-add.
#include <cstdint>
#include <unordered_map>
#include "../d2moo/include/D2Dungeon.h"
#include "../d2moo/include/Units/Units.h"
#include "../d2moo/include/Units/Player.h"

namespace {
struct Seed { uint32_t lo, hi; };
std::unordered_map<const D2ActiveRoomStrc*, Seed> g_room;
std::unordered_map<uint64_t, uint32_t> g_unit;
uint64_t key(uint32_t type, uint32_t id) { return uint64_t(type) << 32 | id; }
}

namespace d2client_unitseed {
// DUNGEON_AllocRoom's low seed for a new room (its seed is {low, 666}); a reused room address starts over.
void room(const D2ActiveRoomStrc* r, uint32_t lowSeed) { if (r) g_room[r] = { lowSeed, 666 }; }
// A client unit created in room r: the room seed's next roll, or 0 when the client has no room there.
uint32_t roll(const D2ActiveRoomStrc* r) {
    auto it = r ? g_room.find(r) : g_room.end();
    if (it == g_room.end()) return 0;
    const uint64_t s = uint64_t(it->second.lo) * 0x6ac690c5ull + it->second.hi;
    it->second = { uint32_t(s), uint32_t(s >> 32) };
    return uint32_t(s);
}
// A server unit created into a room: the client creates it from its unit-add packet. Missiles are the
// client's own; the local player arrives at (0,0) and rolls nothing.
void created(D2UnitStrc* u) {
    if (!u || u->dwUnitType == UNIT_MISSILE) return;
    if (u->dwUnitType == UNIT_PLAYER && u->dwAnimMode != PLRMODE_DEAD && u->dwAnimMode != PLRMODE_DEATH) return;
    g_unit[key(u->dwUnitType, u->dwUnitId)] = roll(UNITS_GetRoom(u));
}
// The client unit's init seed (+0x28); its seed is {init, 666}.
uint32_t init(uint32_t type, uint32_t id) { auto it = g_unit.find(key(type, id)); return it == g_unit.end() ? 0 : it->second; }
}
void d2client_unitseed_room(const D2ActiveRoomStrc* r, uint32_t lowSeed) { d2client_unitseed::room(r, lowSeed); }
void d2client_unitseed_created(D2UnitStrc* u) { d2client_unitseed::created(u); }
