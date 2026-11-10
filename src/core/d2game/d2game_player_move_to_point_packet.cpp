// 0x0F, a player starts moving to a point: 16 bytes, header, unit type (u8), unit id (u32), move kind (u8),
// destination x, y (u16 each), one zero-initialised byte, current x, y (u16 each).
#include <cstdint>
#include <cstring>
#include <GAME/SCmd.h>

extern "C" uint32_t g_d2webModePackets; // counts mode packets for the skill probe

void __fastcall D2GAME_PACKETS_SendPacket0x0F_6FC3CA00(D2ClientStrc* pClient, char nHeader, uint8_t nUnitType, int32_t nUnitGUID, char nFlags, int16_t nDestPosX, int16_t nDestPosY, char a8, int16_t nCurrentPosX, int16_t nCurrentPosY)
{
    ++g_d2webModePackets;
    uint8_t packet[16] = {};
    packet[0] = (uint8_t)nHeader;
    packet[1] = nUnitType;
    std::memcpy(packet + 2, &nUnitGUID, 4);
    packet[6] = (uint8_t)nFlags;
    std::memcpy(packet + 7, &nDestPosX, 2);
    std::memcpy(packet + 9, &nDestPosY, 2);
    packet[11] = (uint8_t)a8;
    std::memcpy(packet + 12, &nCurrentPosX, 2);
    std::memcpy(packet + 14, &nCurrentPosY, 2);
    D2GAME_PACKETS_SendPacket_6FC3C710(pClient, packet, sizeof packet);
}
