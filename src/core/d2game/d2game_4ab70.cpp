// 1.13c D2Game+0x4AB70 -- 0x0B, the unit a client plays: 6 bytes, header, unit type (u8), unit id (u32).
// With no unit the type is 6 and the id is -1.
#include <cstdint>
#include <cstring>
#include <GAME/SCmd.h>
#include <Units/Units.h>

void __fastcall D2GAME_SendPacket0x0B_ClientUnit(D2ClientStrc* pClient, D2UnitStrc* pUnit)
{ // 1.13c D2Game+0x4AB70
    uint8_t packet[6] = {};
    packet[0] = 0x0B;
    packet[1] = pUnit ? (uint8_t)pUnit->dwUnitType : 6;
    const uint32_t guid = pUnit ? (uint32_t)pUnit->dwUnitId : 0xFFFFFFFFu;
    std::memcpy(packet + 2, &guid, 4);
    D2GAME_PACKETS_SendPacket_6FC3C710(pClient, packet, sizeof packet);
}
