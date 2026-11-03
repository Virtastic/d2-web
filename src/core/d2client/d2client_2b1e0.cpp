// 1.13c D2Client+0x2B1E0 (ECX = p): per-entity refresh. p is a 32-bit entity address; fields are read at their retail offsets
// (kind +0, id +0xC, flags +0xC4, +0xC8). Callees through the injected dispatcher.
#include <cstdint>
#include <cstring>
#include "d2client_callee.h"
static inline uint32_t rd32_2b1e0(uint32_t a) { uint32_t x; std::memcpy(&x, (const void*)(uintptr_t)a, 4); return x; }
static inline void wr32_2b1e0(uint32_t a, uint32_t x) { std::memcpy((void*)(uintptr_t)a, &x, 4); }
// 1.13c D2Client+0x2B1E0
// cur = [0x11BBFC]; tabA / tabB = 0x10A608 / 0x109A08 (module addresses; stride 0x200 per kind). A null p faults in retail at the kind switch; it stops there.
extern "C" void d2c_2b1e0(const D2cSvc& v, uint32_t p, uint32_t cur, uint32_t tabA, uint32_t tabB) {
    if (cur == p || !p || !((rd32_2b1e0(p + 0xc4) >> 23) & 1)) {
        d2c_call(v, 0xcf16, 0, 0, 0, 0, 0, p, 0);              // Ordinal_11093(p, 0)
        d2c_call(v, 0xc7fb0, 0, 0, 0, 0, 0, p);
        if (!p) return;
        const uint32_t id = rd32_2b1e0(p + 0xc), kind = rd32_2b1e0(p), fl = (rd32_2b1e0(p + 0xc4) >> 21) & 1;
        d2c_call(v, 0x6c130, p, 0, 0, 0, 0);
        switch (kind) {
        case 0: d2c_call(v, 0x1c150, 0, 0, 0, 0, 0, p); break;
        case 1: d2c_call(v, 0x4f4a0, p, 0, 0, 0, 0); break;
        case 2: d2c_call(v, 0x25b40, p, 0, 0, 0, 0); break;
        case 3: d2c_call(v, 0xb1af0, p, 0, 0, 0, 0); break;
        case 4: d2c_call(v, 0x82ea0, 0, 0, 0, 0, 0); break;
        default: break;
        }
        const uint32_t tab = (fl ? tabB : tabA) + kind * 0x200;
        const uint32_t r = d2c_call(v, 0xa4e20, id & 0x7f, id, tab, 0, 0, kind);
        if (r) d2c_call(v, 0xabc20, 0, 0, 0, 0, 0, r);
    } else {
        if ((rd32_2b1e0(p + 0xc8) >> 5) & 1) {
            uint8_t m[12] = { 'K' };
            const uint32_t k = rd32_2b1e0(p), id = rd32_2b1e0(p + 0xc);
            std::memcpy(m + 1, &k, 4); std::memcpy(m + 5, &id, 4);
            d2c_call(v, 0x143e0, 0, 0, 0, 9, 0, (uint32_t)(uintptr_t)m);   // EBX = 9, stack word 0 = the packet
            wr32_2b1e0(p + 0xc4, rd32_2b1e0(p + 0xc4) & 0xff7fffffu);
            wr32_2b1e0(p + 0xc8, rd32_2b1e0(p + 0xc8) & 0xffffffdfu);
        }
        if ((rd32_2b1e0(p + 0xc4) >> 21) & 1) {
            const uint32_t kind = rd32_2b1e0(p), id = rd32_2b1e0(p + 0xc);
            d2c_call(v, 0xa6ef0, 0, tabB + kind * 0x200, id & 0x7f, 0, 0, id, kind);
        }
    }
}
