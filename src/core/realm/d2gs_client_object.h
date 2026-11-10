#pragma once
#include <cstdint>

struct D2UnitStrc;
struct D2ActiveRoomStrc;

// The client's unit for an object of a joined game: made by the object-assign packet, changed by the
// object-state packet, advanced once a client tick. The unit carries the object's mode, animation clock,
// selectable flag and walk-collision footprint, so the hero's route, reach and the light pass read an
// object the way they read one in a local game.

// The units live in the client's object table (128 chains keyed by the low 7 bits of the id, the unit's list
// link), and each placed unit is on its room's unit list: one source for lookup, reach, collision and light.

// A new unit in `mode` with no room yet, entered in the object table; nullptr for an unknown class or a mode
// past the table. A shrine class takes the shrine record its interact byte names and its idle overlays.
D2UnitStrc* d2gs_client_object_create(uint32_t guid, uint32_t classId, uint32_t mode, uint8_t interact);
// Puts the unit at (x, y) of an active room and writes its footprint when its mode has one.
void d2gs_client_object_place(D2UnitStrc* unit, D2ActiveRoomStrc* room, int x, int y);
// The room is going away: the unit keeps its position and forgets the room. `roomAlive` is false once the
// rooms are already freed, so their unit lists are not touched.
void d2gs_client_object_unplace(D2UnitStrc* unit, bool roomAlive = true);
// The object-state packet's mode change. `selectable` is the packet's byte 7; only a door reads it.
void d2gs_client_object_set_mode(D2UnitStrc* unit, uint32_t mode, uint8_t selectable);
// One client tick of the object's animation.
void d2gs_client_object_step(D2UnitStrc* unit);
void d2gs_client_object_destroy(D2UnitStrc* unit);

// The object table. `next` walks every unit: pass nullptr for the first, the last returned for the one after.
D2UnitStrc* d2gs_client_object_find(uint32_t guid);
D2UnitStrc* d2gs_client_object_next(const D2UnitStrc* previous);

// The overlays a shrine shows while idle (Overlay table rows; -1 for none). The count written, at most 3.
// `ages`, when given, takes for each the frames since that overlay was put on (its loop's clock).
int d2gs_client_object_overlays(const D2UnitStrc* unit, int32_t out[3], int32_t ages[3] = nullptr);

// The unit as the scene reads it, one record of kClientObjectViewBytes: the 44-byte object presentation a
// local game's objects are read in {id, class, token (the class's art name, up to 3 letters), mode, x, y,
// 8.8 cel clock, 8.8 cel count, component mask (every layer), u16 8.8 step, u8 facing (the class's), u8 can
// be picked, u8 a portal's destination level, u8 a locked chest, u16 stands in a revealed room}, then seven
// int32: the overlay count, three Overlay rows (-1 unused) and the frames each has looped.
constexpr int kClientObjectViewBytes = 72;
void d2gs_client_object_view(const D2UnitStrc* unit, uint8_t out[kClientObjectViewBytes]);

// What the client does when the server reports a player activating an object.
enum D2gsClientObjectEffect : uint32_t {
    kObjectEffectNone = 0,
    kObjectEffectReverseName = 1,   // the player's name is reversed in place
    kObjectEffectFire = 2,          // a ring of fire missiles around the object
    kObjectEffectExploding = 3,     // six bomb missiles around the object
    kObjectEffectPoison = 4,        // six poison missiles around the object
};
struct D2gsClientObjectEvent {
    uint32_t objectGuid, playerGuid;
    uint32_t shrine;    // the shrine kind the effect and sound were chosen by
    uint32_t sound;     // Sounds table row played at the player; 0 for none
    uint32_t effect;    // D2gsClientObjectEffect
};
// One missile of a fire, exploding or poison shrine, as the client's missile maker is asked for it: the
// Missiles table row, the request flags, the origin (the object's cell), the target and the missile level.
// A fire missile's target is a step from its start (flags 3: start at x, y, target added to it); the others'
// is a cell (flags 0x520: the object is the origin unit, the life is the flight time, the missile is thrown).
struct D2gsClientObjectMissile { int32_t missile; uint32_t flags; int32_t x, y, targetX, targetY, level; };
// The missiles an activation effect makes, in creation order; the count written (16 for fire, 6 for the
// others, 0 for no missile effect or cap too small). `playerLevel` is the activating player's level stat.
int d2gs_client_object_effect_missiles(const D2UnitStrc* object, uint32_t effect, int playerLevel,
    D2gsClientObjectMissile* out, int cap);

// The name-reversing shrine's effect: the (at most 15 character) name turned around in place.
void d2gs_client_object_reverse_name(char name[16]);

// A server packet aimed at an object: the object-state packet (0x0E) and the unit-action packet (0x4D).
// `player` resolves a player id to its client unit. Returns 1 for a mode change, 2 for an activation (the
// event is written), 0 when nothing applies, -1 for a malformed packet.
int d2gs_client_object_packet(const uint8_t* packet, int length, D2UnitStrc* (*player)(uint32_t guid),
    D2gsClientObjectEvent* event);
