// 1.13c D2Game+0x8D670 -- 14-byte unit packet to a client.
// ESI = client, BL = unit type, EDI = unit id; six stack arguments (ret 0x18): a1 byte -> pkt[0], a2 word -> pkt[6],
// a3 word -> pkt[8], a4 word -> pkt[0xA], a5 byte -> pkt[0xC], a6 byte -> pkt[0xD]. pkt[1] = type, pkt[2..5] = id.
// Fatal 0x996 (no client) / 0x341 (the unit is not in the client's game); the packet goes out through 0x8A3E0 (EAX = client).
#include "d2game_8d6_common.h"
struct D2Game8D670Env {
    uint32_t (*getUnit)(uint32_t game, uint32_t type, uint32_t id);                  // 0xE03A0 (EAX type, ECX game, EDX id)
    void     (*send)(uint32_t client, const void* packet, uint32_t length);          // 0x8A3E0 (client in EAX)
};
// 1.13c D2Game+0x8D670
void d2game_port_8d670(uint32_t client, uint8_t type, uint32_t id, uint8_t a1, uint16_t a2, uint16_t a3, uint16_t a4,
                       uint8_t a5, uint8_t a6, const D2Game8D670Env* env)
{
    if (!client) d2g8::fatal(0x996);
    if (!env->getUnit(d2g8::r32(client, 0x1A8), type, id)) d2g8::fatal(0x341);
    uint8_t pkt[14];
    std::memset(pkt, 0, sizeof pkt);
    pkt[0] = a1; pkt[1] = type; std::memcpy(pkt + 2, &id, 4);
    std::memcpy(pkt + 6, &a2, 2); std::memcpy(pkt + 8, &a3, 2); std::memcpy(pkt + 10, &a4, 2);
    pkt[12] = a5; pkt[13] = a6;
    env->send(client, pkt, 14);
}
