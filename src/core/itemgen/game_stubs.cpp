// What the vendored D2Game code links against that no vendored subsystem provides yet: clients,
// players, the D2Common path/environment pieces. Each goes away when its subsystem arrives.
#include <D2Common.h>
#include <D2DataTbls.h>
#include <D2StatList.h>
#include <D2Environment.h>
#include <Units/Units.h>
#include <Path/Path.h>
#include <GAME/Game.h>
#include <GAME/Clients.h>

int32_t CLIENTS_CheckFlag(D2ClientStrc*, uint16_t) { return 0; }

// GAME/Clients.cpp (not vendored: it is the client/packet layer)
int32_t CLIENTS_GetUnitX(D2UnitStrc* u) {
    switch (u->dwUnitType) { case UNIT_PLAYER: case UNIT_MONSTER: case UNIT_MISSILE: return u->pDynamicPath ? u->pDynamicPath->tGameCoords.wPosX : 0;
    case UNIT_OBJECT: case UNIT_ITEM: case UNIT_TILE: return u->pStaticPath ? u->pStaticPath->tGameCoords.nX : 0; default: return 0; }
}
int32_t CLIENTS_GetUnitY(D2UnitStrc* u) {
    switch (u->dwUnitType) { case UNIT_PLAYER: case UNIT_MONSTER: case UNIT_MISSILE: return u->pDynamicPath ? u->pDynamicPath->tGameCoords.wPosY : 0;
    case UNIT_OBJECT: case UNIT_ITEM: case UNIT_TILE: return u->pStaticPath ? u->pStaticPath->tGameCoords.nY : 0; default: return 0; }
}
