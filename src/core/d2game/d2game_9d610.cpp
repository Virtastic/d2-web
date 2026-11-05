// Send a unit's slot state to a client: ESI = client, stack unit; ret 4 (a null client is fatal). The client's +0x1AC byte m picks
// the unit's handle slot (+0xBC + m * 4). A 12-byte packet {3, m, unit+0x7C dword, the handle's 16-bit code, unit+0x80 dword} goes
// out, then the handle is split into three results (out-pointers: the caller's argument slot, then two locals) and a 10-byte packet
// {0x53, first dword, second dword, low byte of the third} follows. Both sends take EAX = client, stack bytes, length. A rule over E
// (E::h = the callee call, E::send = the packet send, E::stk = a local's address, E::rd32/rd8 = memory).
#ifndef D2GAME_RULE_9D610
#define D2GAME_RULE_9D610
#include <cstdint>
#include <cstring>

namespace g9d610 {
template <class E> void Send(E& e, uint32_t client, uint32_t unit)
{
    const uint32_t m = e.rd8(client + 0x1AC), slot = unit + 0xBC + m * 4;
    const uint32_t code = e.h(0xAF1C, 0, 0, 0, 0, 0, e.rd32(slot));
    uint8_t p1[12] = { 3, (uint8_t)m };
    const uint32_t a = e.rd32(unit + 0x7C), b = e.rd32(unit + 0x80);
    const uint16_t c16 = (uint16_t)code;
    memcpy(p1 + 2, &a, 4); memcpy(p1 + 6, &c16, 2); memcpy(p1 + 8, &b, 4);
    e.send(client, p1, 12);
    uint32_t o1 = unit, o2 = 0, o3 = 0;   // o1 reuses the argument slot
    e.h(0xAEFE, 0, 0, 0, 0, 0, e.rd32(slot), e.stk(&o1), e.stk(&o2), e.stk(&o3));
    uint8_t p2[10] = { 0x53 };
    memcpy(p2 + 1, &o1, 4); memcpy(p2 + 5, &o2, 4); p2[9] = (uint8_t)o3;
    e.send(client, p2, 10);
}
}
#endif
