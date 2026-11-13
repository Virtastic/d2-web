// 1.13c D2Game+0xBCC60: ESI = unit, one stack word = threshold (ret 4). Moves stat 0xE (then 0xF) so that the sum of both stats covers the
// threshold; returns 0 when stat14 + stat15 < threshold. Transcribed from the disassembly (the decompile misses the stack word).
// Callees (stdcall thunks): A42A get(unit, stat, 0), A45A max14(unit), A4A8 max15(unit), A472 set(unit, stat, 0, 0), A4D2 add(unit, stat, delta, 0).
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_bcc60(uint32_t unit, int32_t thr, const D2Seam* e)
{
    auto get = [&](uint32_t st) { return (int32_t)e->call(0xA42A, 0, 0, 0, 0, 0, unit, st, 0); };
    auto set0 = [&](uint32_t st) { e->call(0xA472, 0, 0, 0, 0, 0, unit, st, 0, 0); };
    auto add = [&](uint32_t st, int32_t d) { e->call(0xA4D2, 0, 0, 0, 0, 0, unit, st, (uint32_t)d, 0); };
    int32_t a = get(0xE);
    int32_t sum = get(0xF) + a;
    if (sum < thr) return 0;
    if (thr <= a) {
        if (!unit) return 1;
        int32_t v = get(0xE) - thr;
        if (v < 0 || (d2rd32(unit) == 0 && v > (int32_t)e->call(0xA45A, 0, 0, 0, 0, 0, unit))) set0(0xE);
        else add(0xE, -thr);
        return 1;
    }
    if (unit) {
        int32_t v = get(0xE) - a;
        if (v < 0 || (d2rd32(unit) == 0 && v > (int32_t)e->call(0xA45A, 0, 0, 0, 0, 0, unit))) set0(0xE);
        else add(0xE, -a);
    }
    int32_t rest = a - thr;
    if (!unit) return 1;
    int32_t v = get(0xF) + rest;
    if (v < 0 || (d2rd32(unit) == 0 && v > (int32_t)e->call(0xA4A8, 0, 0, 0, 0, 0, unit))) set0(0xF);
    else add(0xF, rest);
    return 1;
}
