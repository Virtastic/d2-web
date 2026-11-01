// 1.13c D2Game+0x8E2A0 -- 0x7F stat packet to a client, plus the 0x90 packet.
// EAX = client, EDI = unit (fatal 0x72A if null); no stack argument. EBX is saved and restored by retail.
#include "d2game_8d6_common.h"
struct D2Game8E2A0Env {
    int32_t  (*sub2A718)(uint32_t unit);                                                // D2Common stdcall thunk 0xA718
    uint32_t (*sub2A478)(uint32_t unit);                                                // 0xA478
    uint16_t (*sub2A556)(uint32_t x);                                                   // 0xA556 (word result)
    uint32_t (*getPlayer)(uint32_t client, uint32_t sel);                               // 0x4AD10 (ESI = client, stack 0)
    uint32_t (*sub8E110)(uint32_t player, uint32_t unit, uint32_t edx, uint32_t arg);   // 0x8E110
    int32_t  (*sub2AB86)(uint32_t player, uint32_t unit);                               // 0xAB86 stdcall 2
    uint32_t (*sub21100)(uint32_t unit);                                                // 0x21100 (ECX = unit)
    uint32_t (*sub21130)(uint32_t unit);                                                // 0x21130 (ECX = unit)
    void     (*sendPacket)(uint32_t client, const void* packet, uint32_t length);       // 0x8A3E0
};
// 1.13c D2Game+0x8E2A0
void d2game_port_8e2a0(uint32_t client, uint32_t unit, const D2Game8E2A0Env* env)
{
    if (!unit) d2g8::fatal(0x72A);
    const int32_t v = env->sub2A718(unit);
    uint8_t pkt[10] = {0};
    pkt[0] = 0x7F;
    if (d2g8::r32(unit, 0) == 0) pkt[1] = 1;
    const uint32_t id = d2g8::r32(unit, 0xC);
    std::memcpy(pkt + 4, &id, 4);
    uint16_t w = v < 0xFFFF ? (uint16_t)v : (uint16_t)0xFFFF;
    std::memcpy(pkt + 2, &w, 2);
    const uint16_t lvl = env->sub2A556(env->sub2A478(unit));
    std::memcpy(pkt + 8, &lvl, 2);
    const uint32_t player = env->getPlayer(client, 0);
    if (env->sub8E110(player, unit, (uint32_t)v, lvl)) env->sendPacket(client, pkt, 10);
    if (!pkt[1]) return;
    const int32_t r = env->sub2AB86(env->getPlayer(client, 0), unit);
    if (r <= 0x32) return;
    uint8_t p2[13] = {0};
    p2[0] = 0x90;
    std::memcpy(p2 + 1, &id, 4);
    uint32_t a = env->sub21100(unit); std::memcpy(p2 + 5, &a, 4);
    a = env->sub21130(unit); std::memcpy(p2 + 9, &a, 4);
    env->sendPacket(client, p2, 13);
}
