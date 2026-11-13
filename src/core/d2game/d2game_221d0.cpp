// 1.13c D2Game+0x221D0: ECX = unit, EDX = a word handed on to 0x21BE0, one stack word `flag` (ret 4). Monster stat scale: for a type 1 unit with a
// record at +0x80, m = min(rec byte 0x6D, 2) and f = 1 when rec byte 0x6A or dword 0x74 is set; the level is #A42A(unit, 0xC, 0) clamped to 1 and
// to level-table count - 1 (dt+0xB74; rows dt+0xB70, 0x78 each; ends silently when it falls outside the table). b = row dword [3 * (f + 6) + m];
// two #21FB0 results (EAX = m + 0x1C / 0x1F with flag, m + 0x10 / 0x13 without) are scaled value * b / 100 (truncated toward zero) << 8 and set as stats
// 0x3E and 0x3F with #A4D2(unit, stat, value, 0); then 0x21BE0 (ECX = unit, EDX = the word; flag) ends it.
#include "d2game_seam4e.h"
static inline int32_t f1Div100(int32_t v)
{
    const int32_t hi = (int32_t)(((int64_t)0x51eb851f * v) >> 32), q = hi >> 5;
    return q + (int32_t)((uint32_t)q >> 31);
}
extern "C" void d2g_221d0(uint32_t unit, uint32_t word, uint32_t flag, uint32_t dt, const D2Seam* e)
{
    if (!unit || d2rd32(unit) != 1) return;
    const uint32_t rec = d2rd32(unit + 0x80);
    if (!rec) return;
    const uint32_t mm = d2rd8(rec + 0x6D);
    const uint32_t m = mm == 0 ? 0 : (mm < 2 ? mm : 2);
    const uint32_t f = (d2rd8(rec + 0x6A) || d2rd32(rec + 0x74)) ? 1 : 0;
    int32_t lvl = (int32_t)e->call(0xA42A, 0, 0, 0, 0, 0, unit, 0xc, 0);
    const int32_t cnt = (int32_t)d2rd32(dt + 0xB74);
    if (lvl <= 1) lvl = 1;
    if (lvl >= cnt - 1) lvl = cnt - 1;
    if (lvl < 0 || lvl >= cnt) return;
    const uint32_t row = d2rd32(dt + 0xB70) + (uint32_t)lvl * 0x78;
    if (!row) return;
    const int32_t b = (int32_t)d2rd32(row + 4 * (m + 3 * (f + 6)));
    const uint32_t i1 = flag ? m + 0x1C : m + 0x10, i2 = flag ? m + 0x1F : m + 0x13;
    const int32_t v1 = f1Div100((int32_t)e->call(0x21FB0, i1) * b);
    const int32_t v2 = f1Div100((int32_t)e->call(0x21FB0, i2) * b);
    e->call(0xA4D2, 0, 0, 0, 0, 0, unit, 0x3e, (uint32_t)v1 << 8, 0);
    e->call(0xA4D2, 0, 0, 0, 0, 0, unit, 0x3f, (uint32_t)v2 << 8, 0);
    e->call(0x21BE0, 0, unit, word, 0, 0, flag);
}
