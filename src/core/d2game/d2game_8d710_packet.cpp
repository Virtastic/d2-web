// 1.13c D2Game+0x8D710 -- 0x59, a player unit assigned to a client: 26 bytes, header, unit id (u32),
// class (u8), name (16 bytes, NUL padded, always terminated), x (u16), y (u16).
#include <cstdint>
#include <cstring>
#include <GAME/SCmd.h>

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
}
