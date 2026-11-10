// AI target-scan callbacks 0x7BC70 / 0x7BD00 / 0x7BD50 with the retail register and stack arguments.
// 0x7BC70, 0x7BD50 (fastcall + ret 8): EDX = scanning unit, stack = candidate, accumulator
// {+0 best, +4 count, +8 value, +0xC max distance}; the data-table count is *(*(sgptDataTables)+0xA80).
// 0x7BD00 (fastcall): ECX = unit. Callee seams carry the registers retail passes (EAX/EBX/stack).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

namespace {
inline uint32_t rd(const uint8_t* p, uint32_t o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; }
inline void wr(uint8_t* p, uint32_t o, uint32_t v) { std::memcpy(p + o, &v, 4); }
}

#include "d2game_7bc70.h"

// 1.13c D2Game+0x7BC70
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_game_7bc70(const uint8_t* unit, const uint8_t* cand, uint8_t* acc,
                                                       const D2g7bc70Env* e) {
    if (unit == cand || !cand || rd(cand, 0) != 1) return 0;
    if (!e->a6f4(unit, cand)) return 0;
    if (e->a514(cand) == 2) return 0;
    if (e->a688(cand) != 0) return 0;
    const int32_t m = e->c1850(e->c18a0(cand));
    if (m != 0xeb && m != 0xee) return 0;
    if (e->c1900(unit, cand) > (int32_t)rd(acc, 0xc)) return 0;
    const int32_t v = e->a718(cand);
    if (v > 0x3c) return 0;
    if ((int32_t)rd(acc, 8) < 1) wr(acc, 8, 0x7fffffffu);
    wr(acc, 4, rd(acc, 4) + 1);
    if (v < (int32_t)rd(acc, 8)) wr(acc, 0, (uint32_t)(uintptr_t)cand);
    return 0;
}

// 1.13c D2Game+0x7BD00
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_game_7bd00(const uint8_t* unit, const D2g7bc70Env* e) {
    if (unit && rd(unit, 0) == 1 && e->a688(unit) == 0) {
        const int32_t k = e->c18a0(unit);
        if (k >= 0 && k < e->tableCount && k == 0x76) return 1;
    }
    return 0;
}

// 1.13c D2Game+0x7BD50
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_game_7bd50(const uint8_t* unit, const uint8_t* cand, uint8_t* acc,
                                                       const D2g7bc70Env* e) {
    if (unit == cand || !cand || rd(cand, 0) != 1) return 0;
    const int32_t k = e->c18a0(cand);
    if (k < 0 || k >= e->tableCount || (k != 0x13 && k != 0x3a)) return 0;
    if (e->a514(cand) != 0) return 0;
    if (!((rd(cand, 0xc4) >> 1) & 1)) return 0;
    if (e->ad60(cand) != 0) return 0;
    if (rd(cand, 0x10) != 0xc) return 0;
    if (e->ad4e(cand) != 0) return 0;
    const int32_t d = e->c1900(unit, cand);
    if (d > (int32_t)rd(acc, 0xc)) return 0;
    if (e->c22a80(cand, 0xc)) return 0;
    wr(acc, 0, (uint32_t)(uintptr_t)cand);
    wr(acc, 4, rd(acc, 4) + 1);
    wr(acc, 8, (uint32_t)d);
    return 0;
}
