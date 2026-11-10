// 1.13c D2Game+0x8D710 -- 0x59, a player unit assigned to a client: 26 bytes, header, unit id (u32),
// class (u8), name (16 bytes, NUL padded, always terminated), x (u16), y (u16). The assigned player's
// roster line (0x75) follows at once, as the receiving client's own player holds it.
#include <cstdint>
#include <cstring>
#include <GAME/Clients.h>
#include <GAME/SCmd.h>
#include <UNIT/SUnit.h>

void __fastcall sub_6FC3D1F0(D2ClientStrc* pClient, int32_t nUnitGUID, uint8_t nClassId, const char* szName, int16_t nX, int16_t nY)
{ // 1.13c D2Game+0x8D710
    uint8_t packet[26] = {};
    packet[0] = 0x59;
    std::memcpy(packet + 1, &nUnitGUID, 4);
    packet[5] = nClassId;
    if (szName) for (int i = 0; i < 15 && szName[i]; ++i) packet[6 + i] = (uint8_t)szName[i];
    std::memcpy(packet + 22, &nX, 2);
    std::memcpy(packet + 24, &nY, 2);
    D2GAME_PACKETS_SendPacket_6FC3C710(pClient, packet, sizeof packet);
    // The receiver's own player (retail re-resolves the cached unit by its id first), then the assigned
    // unit by id in the game's players; a unit the game does not hold gets no roster line. Retail has
    // no check for a receiver with no player: every caller assigns to a client that has one.
    D2UnitStrc* pReceiver = CLIENTS_GetPlayerFromClient(pClient, 0);
    if (!pReceiver || !pClient->pGame) return;
    if (D2UnitStrc* pAssigned = SUNIT_GetServerUnit(pClient->pGame, UNIT_PLAYER, nUnitGUID))
        SCMD_Send0x75_PartyRosterUpdate(pReceiver, pAssigned);
}
