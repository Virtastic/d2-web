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

// GAME/Clients.cpp CLIENTS_CheckFlag: whether one bit of the client's own save flags
// (D2ClientStrc::tSaveFlags -- the word CLIENTS_GetFlags/SetFlags/ToggleFlag in
// d2moo/compat/game_net_stubs.cpp read and write) is set. This was a constant 0, so every caller
// that gates on it (the hardcore resurrect refusal in D2GAME_PACKETCALLBACK_Rcv0x41_Resurrect,
// PlrModes.cpp's disconnect-instead-of-town-portal branches, PartyScreen and item-naming hardcore
// checks) always took the softcore path no matter what the client's flags actually held.
int32_t CLIENTS_CheckFlag(D2ClientStrc* pClient, uint16_t nFlag) {
    if (!pClient) return 0;
    return (pClient->tSaveFlags.nPackedValue & nFlag) != 0;
}

// GAME/Clients.cpp (not vendored: it is the client/packet layer)
int32_t CLIENTS_GetUnitX(D2UnitStrc* u) {
    switch (u->dwUnitType) { case UNIT_PLAYER: case UNIT_MONSTER: case UNIT_MISSILE: return u->pDynamicPath ? u->pDynamicPath->tGameCoords.wPosX : 0;
    case UNIT_OBJECT: case UNIT_ITEM: case UNIT_TILE: return u->pStaticPath ? u->pStaticPath->tGameCoords.nX : 0; default: return 0; }
}
int32_t CLIENTS_GetUnitY(D2UnitStrc* u) {
    switch (u->dwUnitType) { case UNIT_PLAYER: case UNIT_MONSTER: case UNIT_MISSILE: return u->pDynamicPath ? u->pDynamicPath->tGameCoords.wPosY : 0;
    case UNIT_OBJECT: case UNIT_ITEM: case UNIT_TILE: return u->pStaticPath ? u->pStaticPath->tGameCoords.nY : 0; default: return 0; }
}
