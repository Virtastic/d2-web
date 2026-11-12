// 1.13c D2Common+0x52E70 -- field compiler (ecx = text, edx = record): clears the kind byte at +1 and the index word at +2, then asks the link tables at image offsets 0xA09B0 (kind 2), 0xA0954 (kind 1) and 0xA0978 (kind 0, exact match) in turn through Fog 10217 and stores the first kind and index found. Nothing for a null record; a null or empty text leaves the cleared fields.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2common_a09b0;
__attribute__((weak)) uint32_t g_d2common_a0954;
__attribute__((weak)) uint32_t g_d2common_a0978;
namespace d2common {
static uint32_t rd16(GE& e, uint32_t a) { return e.rd8(a) | (e.rd8(a + 1) << 8); }
static void wr16(GE& e, uint32_t a, uint32_t v) { e.wr8(a, v & 0xff); e.wr8(a + 1, (v >> 8) & 0xff); }
uint32_t f52e70(GE& e, uint32_t ea, uint32_t s, uint32_t rec)
{
    if (!rec) return ea;
    e.wr8(rec + 1, 0);
    wr16(e, rec + 2, 0);
    if (!s || !e.rd8(s)) return ea;
    uint32_t v = e.h(0x923a, 0, 0, 0, 0, 0, g_d2common_a09b0, s, 0);
    if ((int32_t)v >= 0) { e.wr8(rec + 1, 2); wr16(e, rec + 2, v); return v; }
    v = e.h(0x923a, 0, 0, 0, 0, 0, g_d2common_a0954, s, 0);
    if ((int32_t)v >= 0) { e.wr8(rec + 1, 1); wr16(e, rec + 2, v); return v; }
    v = e.h(0x923a, 0, 0, 0, 0, 0, g_d2common_a0978, s, 1);
    if ((int32_t)v >= 0) { e.wr8(rec + 1, 0); wr16(e, rec + 2, v); }
    return v;
}
}
