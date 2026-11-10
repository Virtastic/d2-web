// 0x10, a player starts moving to a unit: 16 bytes, header, unit type (u8), unit id (u32), move kind (u8),
// target type (u8), target id (u32), current x, y (u16 each).
#include <cstdint>
#include <cstring>
#include <GAME/SCmd.h>

extern "C" uint32_t g_d2webModePackets; // counts mode packets for the skill probe

void __fastcall D2GAME_PACKETS_SendPacket0x10_6FC3C9A0(D2ClientStrc* pClient, char nHeader, char nUnitType, int32_t nUnitGUID, char nFlags, char nTargetType, int32_t nTargetGUID, int16_t nCurrentPosX, int16_t nCurrentPosY)
{
    ++g_d2webModePackets;
    uint8_t packet[16] = {};
    packet[0] = (uint8_t)nHeader;
    packet[1] = (uint8_t)nUnitType;
    std::memcpy(packet + 2, &nUnitGUID, 4);
    packet[6] = (uint8_t)nFlags;
    packet[7] = (uint8_t)nTargetType;
    std::memcpy(packet + 8, &nTargetGUID, 4);
    std::memcpy(packet + 12, &nCurrentPosX, 2);
    std::memcpy(packet + 14, &nCurrentPosY, 2);
    D2GAME_PACKETS_SendPacket_6FC3C710(pClient, packet, sizeof packet);
}
