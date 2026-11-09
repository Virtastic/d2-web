// 1.13c D2Client+0xBB470 -- fastcall(ECX = unit). Seam form of sub_BB470 (d2client_batch_ba8f0.cpp): callees go through D2cHost,
// the way the FN probe logs them. tbl = *sgptDataTables.
// Row = *(tbl+0xB64) + unit[1] * 0x1A4, unit[1] in [0, *(tbl+0xB6C)); no row: 0xB1720(esi unit). Row word +0x34 > 0:
// ordinal 11018 (thunk 0xC322), c = unit[0x15], then +0x30, then +0x40 (0 when a link is null); a != 0 && a != c: 0x88A70(ebx unit);
// ordinal 10637 (thunk 0xC448) with (unit, c). Tail 0xB92B0(ecx unit).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "client_host.h"
namespace {
inline uint32_t rd(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
inline uint32_t go(D2cHost h, uint32_t rva, uint32_t ecx, uint32_t ebx, uint32_t esi, uint32_t s0 = 0, uint32_t s1 = 0) {
    D2cCall c = {}; c.rva = rva; c.ecx = ecx; c.ebx = ebx; c.esi = esi; c.s[0] = s0; c.s[1] = s1;
    return h(&c);
}
}
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2c_bb470(D2cHost h, uint32_t unit, const uint8_t* tbl) {
    const uint32_t t = (uint32_t)(uintptr_t)tbl;
    if (unit) {
        const int32_t i = (int32_t)rd(unit + 4);
        if (i >= 0 && i < (int32_t)rd(t + 0xb6c)) {
            const uint32_t row = (uint32_t)i * 0x1a4 + rd(t + 0xb64);
            if (row) {
                if ((int16_t)rd(row + 0x34) > 0) {
                    const uint32_t a = go(h, 0xc322, 0, 0, 0, unit);
                    const uint32_t p = rd(unit + 0x54), q = p ? rd(p + 0x30) : 0;
                    const uint32_t c = q ? rd(q + 0x40) : 0;
                    if (a != 0 && a != c) go(h, 0x88a70, 0, unit, 0);
                    go(h, 0xc448, 0, 0, 0, unit, c);
                }
                return go(h, 0xb92b0, unit, 0, 0);
            }
        }
    }
    return go(h, 0xb1720, 0, 0, unit);
}
