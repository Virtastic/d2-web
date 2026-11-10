#pragma once
#include <cstdint>

struct D2UnitStrc;
struct D2ActiveRoomStrc;

// The client's unit for an object of a joined game: made by the object-assign packet, changed by the
// object-state packet, advanced once a client tick. The unit carries the object's mode, animation clock,
// selectable flag and walk-collision footprint, so the hero's route, reach and the light pass read an
// object the way they read one in a local game.

// A new unit in `mode` with no room yet; nullptr for an unknown class or a mode past the table.
D2UnitStrc* d2gs_client_object_create(uint32_t guid, uint32_t classId, uint32_t mode, uint8_t interact);
// Puts the unit at (x, y) of an active room and writes its footprint when its mode has one.
void d2gs_client_object_place(D2UnitStrc* unit, D2ActiveRoomStrc* room, int x, int y);
// The room is going away: the unit keeps its position and forgets the room.
void d2gs_client_object_unplace(D2UnitStrc* unit);
// The object-state packet's mode change. `selectable` is the packet's byte 7; only a door reads it.
void d2gs_client_object_set_mode(D2UnitStrc* unit, uint32_t mode, uint8_t selectable);
// One client tick of the object's animation.
void d2gs_client_object_step(D2UnitStrc* unit);
void d2gs_client_object_destroy(D2UnitStrc* unit);
