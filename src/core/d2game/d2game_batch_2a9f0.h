// The 1.13c object placement helpers of d2game_batch_2a9f0.cpp that a caller (or an FN probe) reaches directly.
#pragma once
#include <cstdint>
#include <D2Collision.h>
#include <D2Dungeon.h>
#include <GAME/Game.h>
#include <OBJECTS/Objects.h>

namespace d2game_wave3_1 {
// Register-only callees are explicit typed hooks. No game/global state is
// recreated here: the caller supplies the same live state used by the engine.
struct Calls {
    void (*queuePacket)(D2ClientStrc*, const void*, uint32_t); // 0x8A3E0
    void (*savePlayer)(D2GameStrc*, D2UnitStrc*, const char*, int32_t); // 0x54320
    void (*flushClient)(D2ClientStrc*, D2GameStrc*); // 0x2D790
    void (*leaveClient)(uint32_t, int32_t); // 0x2DC70
    void (*spawnAt)(D2GameStrc*, D2ActiveRoomStrc*, D2CoordStrc*, void*); // 0x12230
    void (*initializeFire)(D2ObjInitFnStrc*); // 0x2ECB0
};
// D2Common #10303 (thunk 0xAE38): the collision test the placement helpers call; a parameter so the FN probe can record it.
using Collide = uint16_t (*)(D2ActiveRoomStrc*, int32_t, int32_t, uint32_t, uint32_t, uint16_t);
uint16_t engineCollide(D2ActiveRoomStrc* room, int32_t x, int32_t y, uint32_t w, uint32_t h, uint16_t mask);
int32_t placement(D2ActiveRoomStrc* room, const D2DrlgCoordsStrc& r, const D2CoordStrc& p, int32_t sx, int32_t sy,
                  Collide collide = engineCollide);
int32_t widePlacement(D2ActiveRoomStrc* room, const D2DrlgCoordsStrc& r, const D2CoordStrc& p, int32_t sx, int32_t sy,
                      Collide collide = engineCollide);
int32_t populateSmallGrid(D2GameStrc* game, D2ActiveRoomStrc* room, const D2DrlgCoordsStrc& r, const Calls& calls,
                          Collide collide = engineCollide);
// 0x2DE30 / 0x2E160: reached by the FN probes.
void saveAndFlush(D2GameStrc* game, const Calls& calls);
void leaveSelectedClient(D2GameStrc* game, D2ClientStrc* client, const Calls& calls);
}
