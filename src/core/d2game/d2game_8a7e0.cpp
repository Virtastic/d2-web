// 1.13c D2Game+0x8A7E0 (1.10f D2GAME_PACKETS_SendPacket0x8B_6FC3F810): six-byte packet {0x8B, unit id dword (EAX),
// party byte (CL)} sent to the client in the one stack argument through the 0x8A3E0 send.
#include <cstdint>
#include <cstring>
#include <D2BasicTypes.h>
#include <GAME/SCmd.h>

void __fastcall D2GAME_PACKETS_SendPacket0x8B_6FC3F810(D2ClientStrc* pClient, DWORD dwUnitId, BYTE bParty)
{
    uint8_t b[6] = { 0x8B };
    const uint32_t id = dwUnitId;
    std::memcpy(b + 1, &id, 4);
    b[5] = bParty;
    D2GAME_PACKETS_SendPacket_6FC3C710(pClient, b, sizeof b);
}
