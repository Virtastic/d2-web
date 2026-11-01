// 1.13c D2Game+0x8D710 -- 0x59 string packet to a client, then a 0x8D120 refresh of the named unit.
// EAX = client, ECX (CL) -> pkt[5], EDI = unit id -> pkt[1]; stack (ret 0xC): string, word arg2 -> pkt[0x16], word arg3 -> pkt[0x18].
// 26-byte packet (zeroed first, the string copied to pkt+6 through #? 0xA280 with length 0x7FFFFFFF), sent via 0x8A3E0.
// Then the client's current unit (EBX): client+0x3D4 bit 0 set -> client+0x174 as is, else client+0x174 re-resolved through
// 0xE03A0 (type +0x16C, id +0x170) and cleared when it no longer resolves. Fatal 0xFB (no client) / 0x2D3 (no game).
// The id is looked up in the game's 128-bucket unit table (game+0x1120, chain link +0xE4, id +0xC); a hit calls 0x8D120(EAX = node, EBX = current unit).
#include "d2game_8d6_common.h"
struct D2Game8D710Env {
    void     (*strCopy)(uint32_t dest, const char* src, uint32_t len);                // 0xA280
    void     (*send)(uint32_t client, const void* packet, uint32_t length);           // 0x8A3E0
    uint32_t (*getUnit)(uint32_t game, uint32_t type, uint32_t id);                   // 0xE03A0
    void     (*sub8D120)(uint32_t node, uint32_t unit);                               // 0x8D120
};
// 1.13c D2Game+0x8D710
void d2game_port_8d710(uint32_t client, uint8_t cl, uint32_t id, const char* str, uint16_t arg2, uint16_t arg3, const D2Game8D710Env* env)
{
    uint8_t pkt[0x1A];
    std::memset(pkt, 0, sizeof pkt);
    pkt[0] = 0x59; std::memcpy(pkt + 1, &id, 4); pkt[5] = cl;
    env->strCopy((uint32_t)(uintptr_t)(pkt + 6), str, 0x7FFFFFFF);
    std::memcpy(pkt + 0x16, &arg2, 2); std::memcpy(pkt + 0x18, &arg3, 2);
    env->send(client, pkt, 0x1A);
    if (!client) d2g8::fatal(0xFB);
    uint32_t cur;
    if (d2g8::r8(client, 0x3D4) & 1) cur = d2g8::r32(client, 0x174);
    else if (d2g8::r32(client, 0x174) == 0) cur = 0;
    else {
        cur = env->getUnit(d2g8::r32(client, 0x1A8), d2g8::r32(client, 0x16C), d2g8::r32(client, 0x170));
        if (!cur) d2g8::w32(client, 0x174, 0);
    }
    const uint32_t game = d2g8::r32(client, 0x1A8);
    if (!game) d2g8::fatal(0x2D3);
    uint32_t node = d2g8::r32(game, 0x1120 + (id & 0x7F) * 4);
    while (node && d2g8::r32(node, 0xC) != id) node = d2g8::r32(node, 0xE4);
    if (node) env->sub8D120(node, cur);
}
