#include "GAME/SCmd.h"
#include <D2PacketDef.h>
#include <test_seam.h>

// 1.13c D2Game+0x8AEC0
// EAX = unit (or null), client on the stack. 0x47 {header, unit type (6 for null), 0, unit GUID (-1 for null), 0}, 11 bytes, queued to the client.
void __fastcall D2GAME_SendPacket0x47_6FC3EEE0(D2ClientStrc* pClient, D2UnitStrc* pUnit)
{
    D2GSPacketSrv47 packet47 = {};

    packet47.nHeader = 0x47;
    packet47.nUnitType = pUnit ? pUnit->dwUnitType : 6;
    packet47.nUnitGUID = pUnit ? pUnit->dwUnitId : -1;

    D2_CALL(D2GAME_PACKETS_SendPacket_6FC3C710, pClient, &packet47, sizeof(packet47));
}
