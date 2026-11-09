// 1.13c D2Client+0x84990 -- item packet "create or replace the owner's cursor item". EAX = packet (cdecl, no stack arguments): byte +2 length,
// dword +4 item id, payload from +8. 0xCA90 decodes the payload into a 5-dword stack block: +4 word, +6 word, +8 byte, +0xC dword, +0x10 byte.
// The client item hash (D2Client+0x10AE08, 128 buckets, chain +0xE4, id +0xC, type +0) is walked for the id (a hit whose type is not 4 is fatal).
// A hit: when the owner's inventory (player +0x60) holds that very item at 0xC202, it is cleared (0xC7D8, 0xC202, 0x162D0); 0xCA3C(block +0xC, 1)
// == 0 keeps the item, otherwise it is unhashed (0xA6EF0) and a fresh one made. A miss makes a fresh item (0x835C0, EBX = block dword +4, stack:
// packet, block, dword at block +6, 0; null is fatal). Then 0x81AA0, 0xA5A60 (only when either word is nonzero), 0x82820 (EAX = 5, the two words,
// EDI = the 0xA5A60 result) and 0xCA24 (item, block byte +0x10; the stack word's upper bytes are stale in retail).
#include "cl_env.h"
extern "C" void d2c_84990(uint32_t pkt, const cl::Env& e) {
    uint32_t buf[5] = {};
    e.H(0xca90, pkt + 8, e.R8(pkt + 2) - 8, 0, (uint32_t)(uintptr_t)buf);
    const uint32_t id = e.R(pkt + 4);
    uint32_t node = 0;
    for (uint32_t n = e.R(e.gl + (id & 0x7f) * 4); n; n = e.R(n + 0xe4))
        if (e.R(n + 0xc) == id) { if (e.R(n) != 4) return; node = n; break; }   // fatal in retail
    uint32_t it = node;
    bool make = true;
    if (node) {
        const uint32_t inv = e.R(e.self + 0x60);
        const uint32_t p = e.H(0xc202, inv);
        if (p && p == node) {
            e.H(0xc7d8, inv, 0);
            const uint32_t r = e.H(0xc202, inv);
            e.H(0x162d0, 0, 0, 0, 0, 0, r);
        }
        if (!e.H(0xca3c, buf[3], 1)) make = false;
        else e.H(0xa6ef0, id, 4, 0, 0, 0, 0, 0xB2000000u | 0x10AE08u, id & 0x7f);
    }
    if (make) {
        it = e.H(0x835c0, pkt, (uint32_t)(uintptr_t)buf, (buf[1] >> 16) | ((buf[2] & 0xffff) << 16), 0, 0, 0, 0, 0, buf[1], 0);
        if (!it) return;                                       // fatal in retail
    }
    e.H(0x81aa0, 0, 0, 0, 0, 0, 0, 0, 0, 0, it);
    const uint32_t bp = buf[1] & 0xffff, bx = buf[1] >> 16;
    uint32_t edi = 0;
    if (bp || bx) edi = e.H(0xa5a60, 0, 0, 0, 0, 0, bx, bp);
    e.H(0x82820, bp, bx, 0, 0, 0, 5);
    e.Edj(0x82820, edi);
    e.H(0xca24, it, buf[4] & 0xff);
}
