// 1.13c D2Client+0x84bb0 -- item packet "create and flag". EAX = packet (cdecl, no stack arguments): byte +2 length, payload from +8, decoded
// by 0xCA90 into a 5-dword stack block (+4 word, +6 word, +8 byte, +0xC dword, +0x10 byte). 0x835C0 makes the item (EBX = block dword +4, stack:
// packet, block, dword at block +6, 0; null is fatal), then 0x81AA0. An item whose +0x10 is 3 goes through 0xC1A8(item, 0x2000, 0x388, file);
// a nonzero result runs 0xC43C(item, 5) and 0x827C0 (EAX = item).
#include "cl_env.h"
extern "C" void d2c_84bb0(uint32_t pkt, const cl::Env& e) {
    uint32_t buf[5] = {};
    e.H(0xca90, pkt + 8, e.R8(pkt + 2) - 8, 0, (uint32_t)(uintptr_t)buf);
    const uint32_t it = e.H(0x835c0, pkt, (uint32_t)(uintptr_t)buf, (buf[1] >> 16) | ((buf[2] & 0xffff) << 16), 0, 0, 0, 0, 0, buf[1], 0);
    if (!it) return;                                           // fatal in retail
    e.H(0x81aa0, 0, 0, 0, 0, 0, 0, 0, 0, 0, it);
    if (e.R(it + 0x10) != 3) return;
    if (!e.H(0xc1a8, it, 0x2000, 0x388, 0xB2000000u | 0xD4964u)) return;
    e.H(0xc43c, it, 5);
    e.H(0x827c0, 0, 0, 0, 0, 0, it);
}
