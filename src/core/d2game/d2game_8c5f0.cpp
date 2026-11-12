// 1.13c D2Game+0x8C5F0 (1.10f sub_6FC3F930): the living-player walk callback of the party-word broadcast (0x8CFB0).
// It sends the seven-byte 0x8D packet to the walked unit's client. Retail takes the client from the player data
// (+0x9C) only when the unit is a player; any other unit gets a NULL client, which the send drops. There is no
// assert here, unlike 1.10f's SUNIT_GetClientFromPlayer.
#include <cstdint>
#include <D2BasicTypes.h>
#include <Units/Units.h>
#include <Units/Player.h>
#include <GAME/SCmd.h>

void __fastcall sub_6FC3F930(D2GameStrc* pGame, D2UnitStrc* pPlayer, void* packet)
{
    (void)pGame;
    D2ClientStrc* pClient = nullptr;
    if (pPlayer && pPlayer->dwUnitType == UNIT_PLAYER)
    {
        pClient = UNITS_GetPlayerData(pPlayer)->pClient;
    }
    D2GAME_PACKETS_SendPacket_6FC3C710(pClient, packet, 7);
}

// The same rule over the seam environment, for the FN probe -- ECX unused, EDX = unit, one stack argument. For a
// player unit (non-null, type 0) the D2Common import 0xA4C0(unit) gives the player data and its +0x9C word goes in
// EAX; otherwise EAX = 0. The worker 0x8A3E0 then runs with (argument, 0x7) and its result is returned.
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f8c5f0(GE& e, uint32_t, uint32_t unit, uint32_t a)
{
    uint32_t v = 0;
    if (unit && !e.rd32(unit)) v = e.rd32(e.h(0xA4C0, 0, 0, 0, 0, 0, unit) + 0x9c);
    return e.h(0x8A3E0, v, 0, 0, 0, 0, a, 0x7);
}
}
