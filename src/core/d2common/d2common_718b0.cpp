// 1.13c D2Common+0x718B0 -- field compiler (ecx = text, edx = record, stack word 0 = field offset): nothing for a null record. A text starting with a minus sign or a digit stores its decimal value; otherwise the three link tables at image offsets 0xA0A74, 0xA0B10 and 0x9FF98 are asked in turn through Fog 10217 (a missing table is skipped) and the first index found is stored; a null, empty or unknown text stores 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2common_a0a74;
__attribute__((weak)) uint32_t g_d2common_a0b10;
__attribute__((weak)) uint32_t g_d2common_9ff98;
namespace d2common {
uint32_t f718b0(GE& e, uint32_t ea, uint32_t s, uint32_t rec, uint32_t off)
{
    if (!rec) return ea;
    uint32_t v = ea;
    if (s) {
        const uint32_t c = e.rd8(s);
        v = (ea & 0xffffff00u) | c;
        if (c) {
            if (c == '-' || (c >= '0' && c <= '9')) {
                v = e.h(0x1ebb, 0, 0, 0, 0, 0, s);
                e.wr32(rec + off, v);
                return v;
            }
            const uint32_t tb[3] = { g_d2common_a0a74, g_d2common_a0b10, g_d2common_9ff98 };
            for (uint32_t t : tb) {
                v = t;
                if (!t) continue;
                v = e.h(0x923a, 0, 0, 0, 0, 0, t, s, 0);
                if ((int32_t)v >= 0) { e.wr32(rec + off, v); return v; }
            }
        }
    }
    e.wr32(rec + off, 0);
    return v;
}
}
