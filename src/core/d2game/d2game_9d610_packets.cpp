// 1.13c D2Game+0x9D610 -- the two packets the act synchronisation composes for a client: 0x03 (load act)
// and 0x53 (the act's day/night state). The caller, LEVEL_SynchronizeDayNightCycleWithClient, reads the
// values; these write the wire bytes.
#include <cstdint>
#include <cstring>
#include <GAME/SCmd.h>

// 0x03, 12 bytes: header, act, map seed (u32), the act's town level (u16), object seed (u32).
void __fastcall D2GAME_PACKETS_SendPacket0x03_6FC3C810(D2ClientStrc* pClient, char nHeader, char nAct, int32_t nInitSeed, int32_t nObjectSeed, int16_t nTownId)
{ // 1.13c D2Game+0x9D610
    uint8_t packet[12] = {};
    packet[0] = (uint8_t)nHeader;
    packet[1] = (uint8_t)nAct;
    std::memcpy(packet + 2, &nInitSeed, 4);
    std::memcpy(packet + 6, &nTownId, 2);
    std::memcpy(packet + 8, &nObjectSeed, 4);
    D2GAME_PACKETS_SendPacket_6FC3C710(pClient, packet, sizeof packet);
}

// 0x53, 10 bytes: header, cycle index (u32), ticks into the cycle (u32), eclipse flag (u8).
void __fastcall D2GAME_SendPacket0x53_6FC3DF50(D2ClientStrc* pClient, D2GSPacketSrv53* pPacket)
{
    uint8_t packet[10] = {};
    packet[0] = pPacket->nHeader;
    std::memcpy(packet + 1, &pPacket->unk0x01, 4);
    std::memcpy(packet + 5, &pPacket->unk0x05, 4);
    packet[9] = pPacket->unk0x09;
    D2GAME_PACKETS_SendPacket_6FC3C710(pClient, packet, sizeof packet);
}
