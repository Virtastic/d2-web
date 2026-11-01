// Decompile-derived, 1.13c-only. Seams: base value (#10913, D2Game+0xA3A0), 64-bit divide (D2Game+0xF44B0), stat add (#10866, D2Game+0xA472).
#include <cstdint>

struct D2Game21ec0Env {
    int32_t (*base)(uint32_t unit);                                     // EBX = unit, no stack
    uint32_t (*alldiv)(uint32_t lo, uint32_t hi, uint32_t dlo, uint32_t dhi);   // __alldiv; only the low word is used
    void (*addStat)(uint32_t unit, uint32_t id, int32_t v, uint32_t sub);
};
static int32_t div100(int32_t x) { const int32_t h = (int32_t)(((int64_t)(int32_t)0x51eb851f * x) >> 32) >> 5; return h + (int32_t)((uint32_t)h >> 31); }   // retail's magic multiply: x / 100 truncated

// 1.13c D2Game+0x21EC0
// EBX = unit, EDI = percent, plain ret. r = base(unit); the bonus is r * pct / 100 (the side that exceeds the small-value bound is divided first,
// 32-bit wrapping products); stats 7 and 6 are each raised by r + bonus.
extern "C" void d2game_apply_percent_bonus_21ec0(uint32_t unit, int32_t pct, const D2Game21ec0Env* env)
{
    const int32_t r = env->base(unit);
    int32_t bonus;
    if (r > 0x100000) {
        if ((r & ~0xf) < 0x640) goto wide;
        bonus = (int32_t)((uint32_t)div100(r) * (uint32_t)pct);
    } else if (pct > 0x10000) {
        if ((pct & ~0xf) < 0x640) goto wide;
        bonus = (int32_t)((uint32_t)div100(pct) * (uint32_t)r);
    } else {
        bonus = div100((int32_t)((uint32_t)r * (uint32_t)pct));
    }
    goto done;
wide: {
        const int64_t p = (int64_t)r * pct;
        bonus = (int32_t)env->alldiv((uint32_t)p, (uint32_t)((uint64_t)p >> 32), 100, 0);
    }
done:
    const int32_t v = (int32_t)((uint32_t)r + (uint32_t)bonus);
    env->addStat(unit, 7, v, 0);
    env->addStat(unit, 6, v, 0);
}
