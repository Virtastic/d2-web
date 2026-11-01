// 1.13c D2Client+0x1C940: EDX = key; scan the 41 sixteen-byte entries at D2Client+0xF8190 for dword[0] == key; hit -> dword[0x11B7E8 + 4*b] = a, returns a
// (a, b = the entry bytes at -6 and -5 relative to the entry, as retail reads them); miss -> returns the table end constant 0x6FBA8420
#include "cl17.h"
extern "C" uint32_t d2c_1c940(uint32_t key, uint32_t g1, uint32_t g2, const cl::Env& e) {
    for (uint32_t i = 0; i < 0x29; i++) {
        if (e.R(g1 + 0x10 + 16 * i) != key) continue;
        uint32_t val = e.R8(g1 + 0xa + 16 * i);      // retail: [i*16 + 0x6fba818a]
        uint32_t slot = e.R8(g1 + 0xb + 16 * i);     // retail: [i*16 + 0x6fba818b]
        e.W(g2 + slot * 4, val);
        return val;
    }
    return 0x6fba8420;
}
