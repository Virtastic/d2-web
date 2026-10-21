// 1.13c D2Gfx+0xBE31 _GetRangeOfTrysToCheck (MSVC 2003 CRT EH). FuncInfo is a 32-bit-pointer retail struct, read by offset.
#include <cstdint>
#include <cstring>
static uint32_t rd(uintptr_t a) { uint32_t v; memcpy(&v, (void*)a, 4); return v; }
// Returns the TryBlockMap entry address (map + 20*start), or 0 where retail calls _inconsistency (-> terminate).
extern "C" uintptr_t d2gfx_getrange_be31(uintptr_t fi, int32_t state, int32_t catchLevel, uint32_t* pStart, uint32_t* pEnd) {
    uint32_t n = rd(fi + 0xC);
    uintptr_t map = rd(fi + 0x10);
    uint32_t i = n, hi = n, end = n;
    while (state >= 0) {
        uint32_t cur = hi;
        if (i == 0xffffffffu) return 0;
        i--;
        uintptr_t e = map + 20 * (uintptr_t)i;
        if (i == 0xffffffffu || ((int32_t)rd(e + 4) < catchLevel && catchLevel <= (int32_t)rd(e + 8))) {
            state--; hi = i; end = cur;
        } else hi = cur;
    }
    i++;
    *pStart = i; *pEnd = end;
    if (n < end || end < i) return 0;
    return map + 20 * (uintptr_t)i;
}
