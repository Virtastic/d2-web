// 1.13c D2Game+0x9E8E0 -- ECX, EDX, two stack words (ret 8): a skill id and one more word. No result.
// Skill record (sgptDataTables +0xB98, 0x23C each, +0xBA0 count) for 0 < id < count; its short at +0x80 must be 0 <= v < the +0xC4 count; 0x1AE0(EAX v) answers a 0x3C-byte record
// whose short at +0x36 must satisfy 0 <= w < 0xBF; the function-pointer table at D2Game+0x1074A8 (0xBF entries) slot w, when non-zero, is called with ECX, EDX as given and the
// stack words id, a1 (callee pops them).
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
void f9e8e0(GE& e, uint32_t ecx, uint32_t edx, uint32_t id, uint32_t a1, uint32_t dt, uint32_t tb)
{
    if ((int32_t)id <= 0) return;
    const uint32_t sk = e.skillRec(dt, (int32_t)id);
    if (!sk) return;
    const int16_t v = (int16_t)(e.rd32(sk + 0x80) & 0xFFFF);
    if (v < 0 || (int32_t)v >= (int32_t)e.rd32(dt + 0xC4)) return;
    const uint32_t rec = e.h(0x1AE0, (uint32_t)(int32_t)v);
    if (!rec) return;
    const int16_t w = (int16_t)(e.rd32(rec + 0x36) & 0xFFFF);
    if (w < 0 || w >= 0xBF) return;
    const uint32_t fn = e.rd32(tb + 4u * (uint32_t)w);
    if (!fn) return;
    e.h(fn & 0xFFFFFFu, 0, ecx, edx, 0, 0, id, a1);
}
}
