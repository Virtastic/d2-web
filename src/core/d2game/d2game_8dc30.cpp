// 1.13c D2Game+0x8DC30 -- 21-byte unit packet to a client.
// ECX = unit id, EBX = client, DL = pkt[0]; four stack arguments (ret 0x10): a1 byte -> pkt[5], a2 byte -> pkt[0xA],
// a3 dword -> pkt[0xB], a4 byte -> pkt[0xF]. Walks the type-1 unit list at game+0x1320 (link +0xE4, id +0xC).
// Retail does NOT return when the id is not in the table: ESI = 0 falls into the type load and faults.
#include "d2game_8d6_common.h"
struct D2Game8DC30Env {
    void    (*sendPacket)(uint32_t client, const void* packet, uint32_t length); // 0x8A3E0 (client in EAX)
    uint8_t (*ordinal10471)(uint32_t path);                                      // 0xAB5C
    int32_t (*ordinal10867)(uint32_t path);                                      // 0xA454
    int32_t (*ordinal10750)(uint32_t path);                                      // 0xA3B8
    int32_t (*ordinal10973)(uint32_t unit, uint32_t stat, uint32_t layer);       // 0xA42A
    uint8_t (*ordinal11058)(uint32_t path);                                      // 0xAB4A
};
// 1.13c D2Game+0x8DC30
void d2game_port_8dc30(uint32_t id, uint32_t client, uint8_t dl, uint8_t a1, uint8_t a2, uint32_t a3, uint8_t a4, const D2Game8DC30Env* env)
{
    if (!client) d2g8::fatal(0x996);
    const uint32_t game = d2g8::r32(client, 0x1A8);
    if (!game) d2g8::fatal(0x2D3);
    uint8_t pkt[21];
    std::memset(pkt, 0, sizeof pkt);
    pkt[0] = dl; std::memcpy(pkt + 1, &id, 4); pkt[5] = a1; pkt[0xA] = a2; std::memcpy(pkt + 0xB, &a3, 4);
    uint32_t node = d2g8::r32(game, 0x1320 + (id & 0x7F) * 4);
    while (node && d2g8::r32(node, 0xC) != id) node = d2g8::r32(node, 0xE4);
    if (!node) __builtin_trap();                        // retail: ESI = 0, `mov ecx,[esi]` faults
    const int32_t type = (int32_t)d2g8::r32(node, 0);
    const uint32_t path = d2g8::r32(node, 0x2C);
    const bool direct = type == 2 || (type > 3 && type <= 5);
    uint16_t w = (uint16_t)(direct ? d2g8::r32(path, 0xC) : path ? (uint32_t)env->ordinal10867(path) : 0);
    std::memcpy(pkt + 6, &w, 2);
    w = (uint16_t)(direct ? d2g8::r32(path, 0x10) : path ? (uint32_t)env->ordinal10750(path) : 0);
    std::memcpy(pkt + 8, &w, 2);
    pkt[0xF] = a4;
    uint8_t mode = env->ordinal10471(path);
    pkt[0x11] = mode;
    if (mode >= 5) {
        if (mode <= 6) pkt[0x11] = 2;
        else if (mode == 8) pkt[0x11] = 0xB;
    }
    int32_t v = env->ordinal10973(node, 0x43, 0);
    if (v < -0x8000) v = -0x8000;
    if (v > 0x7FFF) v = 0x7FFF;
    w = (uint16_t)v; std::memcpy(pkt + 0x12, &w, 2);
    pkt[0x14] = env->ordinal11058(path);
    env->sendPacket(client, pkt, 21);
}
