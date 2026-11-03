// 1.13c D2Client+0x53320 (EAX = unit, kept in ESI): resets the unit's motion block (unit+0x54, then +0x30) and tail-calls 0x532C0
// with EAX = #10331(local unit). Callees through the injected dispatcher.
#include <cstdint>
#include <cstring>
#include "d2client_callee.h"
extern "C" void d2c_532c0(const D2cSvc& v, uint32_t unit, uint32_t* d3478, uint32_t cc2ec, uint32_t cbbfc);
static inline uint32_t rd32_53320(uint32_t a) { uint32_t x; std::memcpy(&x, (const void*)(uintptr_t)a, 4); return x; }
static inline void wr32_53320(uint32_t a, uint32_t x) { std::memcpy((void*)(uintptr_t)a, &x, 4); }
// 1.13c D2Client+0x53320
extern "C" void d2c_53320(const D2cSvc& v, uint32_t unit, uint32_t* d3478, uint32_t cc2ec, uint32_t cbbfc) {
    const uint32_t t = d2c_call(v, 0xc1f0, 0, 0, 0, 0, 0, unit, 3, 0x271);       // #10331(unit, 3, 0x271)
    const uint32_t o = d2c_call(v, 0xc334, 0, 0, 0, 0, 0, t);                    // #10754
    if (!o) return;
    d2c_call(v, 0xc7b00, unit, 0, 0, 0, 0);                                      // FUN_6fb77b00 (EAX = unit)
    if (unit) {
        const uint32_t path = rd32_53320(unit + 0x54), in = path ? rd32_53320(path + 0x30) : 0;
        if (in) {
            wr32_53320(in + 4, 0); wr32_53320(in + 8, 0); wr32_53320(in + 0xc, 0x96000);
            wr32_53320(in + 0x10, 0); wr32_53320(in + 0x14, 0); wr32_53320(in + 0x18, 0xfffff000u);
            wr32_53320(in + 0x28, 0); wr32_53320(in + 0x2c, 0); wr32_53320(in + 0x30, 5);
            wr32_53320(in, rd32_53320(in) & 0xffffffdfu);
        }
    }
    d2c_call(v, 0xc394, 0, 0, 0, 0, 0, o, 1);                                    // #10019(o, 1)
    const uint32_t e = cbbfc ? d2c_call(v, 0xc1f0, 0, 0, 0, 0, 0, cbbfc) : 0;    // #10331(local unit), 1 stack word at this site
    d2c_532c0(v, e, d3478, cc2ec, cbbfc);
}
