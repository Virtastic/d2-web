// 1.13c D2Game+0xF4260: the percent-scaled roll. Retail: EAX = hi, EDI = lo,
// ECX = unit (plain ret). r = 0x11D0(unit+0x20 seed, hi - lo + 0x100) + lo; the record 0xA520(unit id, -1 for
// no unit) gives the percent byte at +0x19C; the result is (pct * r) / 100 in 32 bits. lo > hi and a zero
// percent are the fatal paths (lines 0x14 and 0x1B).
#include <cstdint>
#include <cstring>
#include "../prng.h"

extern "C" uint32_t D2Game_SEED_RollLimitedRandomNumber(d2::Seed* pSeed, int32_t nLimit);

extern "C" uint32_t d2game_f4260_impl(uint32_t hi, uint32_t lo, uint32_t unit,
                                      uint32_t (*record)(uint32_t id), void (*fatal)(int32_t line))
{
    if (lo > hi) { fatal(0x14); return 0; }
    const uint32_t r = D2Game_SEED_RollLimitedRandomNumber(
                           (d2::Seed*)(uintptr_t)(unit + 0x20), (int32_t)(hi - lo + 0x100)) + lo;
    uint32_t id = 0xFFFFFFFFu;
    if (unit) std::memcpy(&id, (const void*)(uintptr_t)(unit + 4), 4);
    const uint32_t pct = *(const uint8_t*)(uintptr_t)(record(id) + 0x19C);
    const uint32_t q = (pct * r) / 100u;
    if (pct == 0) fatal(0x1B);
    return q;
}
