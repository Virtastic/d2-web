// 1.13c D2Game+0x8CFB0: seven-byte packet {0x8D, unit id dword (-1 if none), party word} handed to the 0xDFC10 broadcast with the
// callback 0x6FCAC5F0 (EBX) and the caller's stack argument (EDI). EAX = unit. The party word is the 0xBBEB0 lookup when the
// unit is non-null, has a game (+0x80) and *unit == 0, else 0xFFFF.
#include <cstdint>
#include <cstring>
void D2Game_8CFB0_PartyBroadcast(void (*broadcast)(const void* packet, uint32_t edi), uint16_t (*partyId)(const void* unit),
                                 const uint8_t* unit, uint32_t edi)
{
    uint8_t b[8] = { 0x8D };
    uint32_t id = 0xFFFFFFFFu;
    if (unit) std::memcpy(&id, unit + 0xC, 4);
    std::memcpy(b + 1, &id, 4);
    uint16_t party = 0xFFFF;
    if (unit) {
        const uint8_t* game; std::memcpy(&game, unit + 0x80, sizeof game);
        uint32_t type; std::memcpy(&type, unit, 4);
        if (game && type == 0) party = partyId(unit);
    }
    std::memcpy(b + 5, &party, 2);
    broadcast(b, edi);
}
