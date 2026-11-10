// 1.13c D2Game+0x8D120 -- 0x75, one player's line of the party roster as another player's client
// holds it: 13 bytes, header, the described unit's id (u32), its party id (u16, 0xFFFF outside a
// party), its level (u16), the relation flags the receiver keeps for it (u16, 0 for itself or with
// no roster entry) and the party state between the two (u16, 1 for itself).
// The receiver's player arrives in EBX and the described unit in EAX in 1.13c.
#include <cstdint>
#include <D2PacketDef.h>
#include <D2StatList.h>
#include <GAME/SCmd.h>
#include <PLAYER/PlayerList.h>
#include <UNIT/SUnit.h>

void __fastcall SCMD_Send0x75_PartyRosterUpdate(D2UnitStrc* pLocalPlayer, D2UnitStrc* pOtherPlayer)
{ // 1.13c D2Game+0x8D120
    D2ClientStrc* pClient = SUNIT_GetClientFromPlayer(pLocalPlayer, __FILE__, __LINE__);
    D2GSPacketSrv75 packet = {};
    packet.nHeader = 0x75u;
    packet.nUnitGUID = pOtherPlayer ? pOtherPlayer->dwUnitId : (uint32_t)-1;
    packet.nPartyId = SUNIT_GetPartyId(pOtherPlayer);
    packet.nLevel = STATLIST_UnitGetStatValue(pOtherPlayer, STAT_LEVEL, 0);
    packet.nFlags = PLAYERLIST_GetFlags(pLocalPlayer, pOtherPlayer);
    packet.unk0x0B = sub_6FCBA9D0(pLocalPlayer, pOtherPlayer);
    D2GAME_PACKETS_SendPacket_6FC3C710(pClient, &packet, sizeof packet);
}
