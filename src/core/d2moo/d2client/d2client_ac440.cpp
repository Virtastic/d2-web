// 1.13c D2Client+0xAC440: packet handler, ECX = packet (word at +1, word at +3, byte at +5). The module dword at D2Client+0x11C3B8 (g) must be
// nonzero, else retail's fatal assert (0xBF92(0x58A), 0xBF80, 0x3664). v = 0xC1F0(local player at D2Client+0x11BBFC) when that is set, else 0. Then 0xC53E (stdcall 5)
// with g, the byte, both words and v. EAX is that callee's result.
#include "d2c_seam.h"
extern "C" uint32_t d2c_ac440(const D2cSeam* s, uint32_t g, uint32_t local, const uint8_t* pkt) {
    uint16_t w1, w3; std::memcpy(&w1, pkt + 1, 2); std::memcpy(&w3, pkt + 3, 2);
    if (!g) {   // never returns in retail (0x3664 is exit)
        const uint32_t rc = (uint32_t)(*s)(0xbf92, 0, 0, 0, 0, 0, 0x58a);
        (*s)(0xbf80, 0, 0, 0, 0, 0, (uint32_t)s->mod(0xd359f), rc);
        (*s)(0x3664, 0, 0, 0, 0, 0, 0xffffffffu);
        return 0;
    }
    const uint32_t v = local ? (uint32_t)(*s)(0xc1f0, 0, 0, 0, 0, 0, local) : 0;
    return (uint32_t)(*s)(0xc53e, 0, 0, 0, 0, 0, g, pkt[5], w1, w3, v);
}
