// 1.13c D2Game+0x8D010 (1.10f sub_6FC3F840): the eleven-byte 0x8C relation packet {0x8C, player id (EAX), player id
// (ECX), relation word (DX)} broadcast through SUNIT_IterateLivingPlayers (0xDFC10) with the 0x8C690 callback.
#include <cstdint>
#include <cstring>
#include <D2BasicTypes.h>
#include <Units/Units.h>
#include <Units/Player.h>
#include <GAME/SCmd.h>
#include <UNIT/SUnit.h>

// 1.13c D2Game+0x8C690 (1.10f sub_6FC3F880): the walk callback. It sends the packet to the walked unit's client: the
// player data's +0x9C for a player, else a NULL client, which the send drops (no assert, unlike 1.10f).
void __fastcall sub_6FC3F880(D2GameStrc* pGame, D2UnitStrc* pPlayer, void* packet)
{
    (void)pGame;
    D2ClientStrc* pClient = nullptr;
    if (pPlayer && pPlayer->dwUnitType == UNIT_PLAYER) pClient = UNITS_GetPlayerData(pPlayer)->pClient;
    D2GAME_PACKETS_SendPacket_6FC3C710(pClient, packet, 11);
}

void __fastcall sub_6FC3F840(D2GameStrc* pGame, int32_t dwPlayerId1, int32_t dwPlayerId2, int16_t wRelationState)
{
    uint8_t b[12] = { 0x8C };
    std::memcpy(b + 1, &dwPlayerId1, 4);
    std::memcpy(b + 5, &dwPlayerId2, 4);
    std::memcpy(b + 9, &wRelationState, 2);
    SUNIT_IterateLivingPlayers(pGame, sub_6FC3F880, b);
}
