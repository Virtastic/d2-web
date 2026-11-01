// 1.13c D2Game+0x8C570: seven-byte packet {0x8D, unit id dword (-1 if none), party word} sent to the recipient's client.
// EAX = unit, ESI = recipient unit. The party word is 0xFFFF unless the unit is non-null, has a game (+0x80) and *unit == 0,
// in which case it is the 0xBBEB0 lookup (that routine reads the game pointer from EDI). The client is [10920(recipient)+0x9C]
// when the recipient is a player (*recipient == 0), otherwise NULL.
#include <cstdint>
#include <cstring>
struct D2Game8C570Env {
    uint16_t (*partyId)(const void* unit);          // 0xBBEB0
    const void* (*playerData)(const void* unit);    // Fog ordinal 10920 (0xA4C0)
    void (*send)(const void* client, const void* buf, uint32_t len);   // 0x8A3E0
};

void D2Game_8C570_PacketSend8D(const D2Game8C570Env* e, const uint8_t* unit, const uint8_t* recipient)
{
    uint8_t b[7] = { 0x8D };
    uint32_t id = 0xFFFFFFFFu;
    if (unit) std::memcpy(&id, unit + 0xC, 4);
    std::memcpy(b + 1, &id, 4);
    uint16_t party = 0xFFFF;
    if (unit) {
        const uint8_t* game; std::memcpy(&game, unit + 0x80, sizeof game);
        uint32_t type; std::memcpy(&type, unit, 4);
        if (game && type == 0) party = e->partyId(unit);
    }
    std::memcpy(b + 5, &party, 2);
    const uint8_t* client = nullptr;
    if (recipient) {
        uint32_t type; std::memcpy(&type, recipient, 4);
        if (type == 0) { const uint8_t* pd = (const uint8_t*)e->playerData(recipient); std::memcpy(&client, pd + 0x9C, sizeof client); }
    }
    e->send(client, b, 7);
}
