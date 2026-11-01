// 1.13c D2Game+0x8E170 -- 0x5B player-info packet to a client.
// ESI = unit (may be null), EBX = info record (+0x45E word, +0x460 dword, +0x464 string), one stack argument = client (ret 4).
// Retail passes no argument to #10920 (0xA4C0): it reads the unit from ESI, and EBX is live across the call.
#include "d2game_8d6_common.h"
struct D2Game8E170Env {
    uint32_t (*getName)(uint32_t unit, uint32_t info);                                  // #10920, 0xA4C0 (ESI = unit, EBX = info)
    int32_t  (*ordinal10973)(uint32_t unit, uint32_t stat, uint32_t layer);             // 0xA42A
    uint16_t (*sub9BEB0)(uint32_t unit);                                                // 0x9BEB0 (EAX = unit)
    void     (*copyString)(void* dest, uint32_t src, uint32_t limit);                   // 0xA280 (dest, src, 0x7FFFFFFF)
    void     (*sendPacket)(uint32_t client, const void* packet, uint32_t length);       // 0x8A3E0 (client in EAX)
    void     (*sub96920)(uint32_t unit, uint32_t client);                               // 0x96920 (EAX = unit)
};
// 1.13c D2Game+0x8E170
void d2game_port_8e170(uint32_t unit, uint32_t info, uint32_t client, const D2Game8E170Env* env)
{
    uint8_t pkt[0x122];
    const uint32_t name = env->getName(unit, info);
    std::memset(pkt, 0, sizeof pkt);
    pkt[0] = 0x5B;
    const uint32_t id = unit ? d2g8::r32(unit, 0xC) : 0xFFFFFFFFu;
    std::memcpy(pkt + 3, &id, 4);
    pkt[7] = unit ? (uint8_t)d2g8::r32(unit, 4) : 0xFF;
    uint16_t w = (uint16_t)env->ordinal10973(unit, 0xC, 0);
    std::memcpy(pkt + 0x18, &w, 2);
    if (unit && d2g8::r32(unit, 0x80) && d2g8::r32(unit, 0) == 0) w = env->sub9BEB0(unit); else w = 0xFFFF;
    std::memcpy(pkt + 0x1A, &w, 2);
    w = d2g8::r16(info, 0x45E);
    std::memcpy(pkt + 0x20, &w, 2);
    env->copyString(pkt + 8, name, 0x7FFFFFFF);
    const uint32_t v = d2g8::r32(info, 0x460);
    std::memcpy(pkt + 0x22, &v, 4);
    uint32_t l1 = (uint32_t)std::strlen((const char*)pkt + 0x22);
    uint8_t* dst = pkt + 0x23 + l1;
    const uint8_t* src = d2g8::P(info + 0x464);
    uint32_t l2 = 0;
    for (;; ++l2) { dst[l2] = src[l2]; if (!src[l2]) break; }
    const uint16_t len = (uint16_t)(l2 + l1 + 0x24);
    std::memcpy(pkt + 1, &len, 2);
    env->sendPacket(client, pkt, len);
    if (unit && d2g8::r32(unit, 0) == 0) env->sub96920(unit, client);
}
