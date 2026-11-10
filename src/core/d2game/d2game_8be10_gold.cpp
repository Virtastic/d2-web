// The gold update the per-client vitals pass sends when the player's gold differs from what the client was last told.
#include <cstdint>

#include <D2StatList.h>
#include <GAME/Clients.h>
#include <GAME/SCmd.h>

// 1.13c D2Game+0x8BE10
void __fastcall D2GAME_PACKETS_SendPacket0x19_6FC3D3D0(D2ClientStrc* pClient, int32_t nValue, int32_t nPreviousGold)
{
    // A gain of 1..254 goes as a one-byte delta (0x19); anything else as the absolute gold stat.
    const int32_t nDiff = nValue - nPreviousGold;
    if (nDiff > 0 && nDiff < 0xFF) {
        uint8_t packet[2] = { 0x19, (uint8_t)nDiff };
        D2GAME_PACKETS_SendPacket_6FC3C710(pClient, packet, sizeof(packet));
        return;
    }
    D2GAME_PACKETS_SendPacket0x1D_E_F_6FC3D480(pClient, STAT_GOLD, (uint32_t)nValue);
}
