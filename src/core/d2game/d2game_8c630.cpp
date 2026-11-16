// 1.13c D2Game+0x8C630 (1.10f D2GAME_PACKETS_SendPacket0x8C_6FC3F8B0): eleven-byte packet {0x8C, player id dword (ECX),
// player id dword (EDX), relation word (stack)} sent to the unit's (EAX) client. The client is the player data's +0x9C
// only when the unit is a non-null player; any other unit sends to a NULL client, which the send drops (no assert).
#include <cstdint>
#include <cstring>
#include <D2BasicTypes.h>
#include <Units/Units.h>
#include <Units/Player.h>
#include <GAME/SCmd.h>

void __fastcall D2GAME_PACKETS_SendPacket0x8C_6FC3F8B0(D2UnitStrc* pUnit, DWORD dwPlayerId1, DWORD dwPlayerId2, WORD wRelationState)
{
    uint8_t b[11] = { 0x8C };
    const uint32_t a = dwPlayerId1, c = dwPlayerId2;
    const uint16_t f = wRelationState;
    std::memcpy(b + 1, &a, 4);
    std::memcpy(b + 5, &c, 4);
    std::memcpy(b + 9, &f, 2);
    D2ClientStrc* pClient = nullptr;
    if (pUnit && pUnit->dwUnitType == UNIT_PLAYER) pClient = UNITS_GetPlayerData(pUnit)->pClient;
    D2GAME_PACKETS_SendPacket_6FC3C710(pClient, b, sizeof b);
}
