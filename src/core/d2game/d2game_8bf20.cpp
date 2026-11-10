// 1.13c D2Game+0x8BF20 -- 0x08, a room the client leaves: 6 bytes, header, tile x (u16), tile y (u16),
// level (u8). The same layout as 0x07, which reveals a room.
#include <cstdint>
#include <cstring>
#include <GAME/SCmd.h>

void __fastcall D2GAME_PACKETS_SendPacket0x08_6FC3D160(D2ClientStrc* pClient, uint8_t nAreaId, uint16_t nTileX, uint16_t nTileY)
{ // 1.13c D2Game+0x8BF20
    uint8_t packet[6] = {};
    packet[0] = 0x08;
    std::memcpy(packet + 1, &nTileX, 2);
    std::memcpy(packet + 3, &nTileY, 2);
    packet[5] = nAreaId;
    D2GAME_PACKETS_SendPacket_6FC3C710(pClient, packet, sizeof packet);
}
