#pragma once
// 1.13c D2Game+0x9D980 (LEVEL_RemoveClient), the first loop: before a client leaves a room the game tells it to
// drop every unit the room holds, in the room's unit order, except missiles (the 1.13c build skips them; 1.10f sent
// them too). The 0x0A it sends for a unit is D2Game+0xCC5F80. The local game's room change (sub_6FC33020) takes
// this loop, so a hero changing act, or walking out of range of a room, no longer keeps that room's units.
#include <D2Dungeon.h>
#include <GAME/Clients.h>
#include <UNIT/SUnitMsg.h>

inline int D2GAME_SendRoomUnitRemovals(D2ActiveRoomStrc* pRoom, D2ClientStrc* pClient)
{
    int sent = 0;
    for (D2UnitStrc* pUnit = pRoom ? pRoom->pUnitFirst : nullptr; pUnit; pUnit = pUnit->pRoomNext)
    {
        if (pUnit->dwUnitType == UNIT_MISSILE) continue;
        D2GAME_PACKETS_SendPacket0x0A_RemoveObject_6FCC5F80(pUnit, pClient);
        ++sent;
    }
    return sent;
}
