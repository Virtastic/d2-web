// 1.13c D2Common+0x3A650 (#10343): stdcall(pStatList), ret 4. Empty a stat list's own array (+0x24: entries {key, value},
// s16 count at +0x28, s16 capacity at +0x2A), announcing each removal. While the count is positive: binary-search the array
// for the first entry's key; on the hit remove the entry (0x381A0, below), announce it with the negated value (0x39F70) and,
// for a list with flag bit 31 set at +0x10 and no owner kind at +8, notify 0x38550. 32-bit retail layout through raw reads.
#include "d2common_3a650.h"
#include <cstring>

namespace {
uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(uintptr_t(a)), 4); return v; }
int32_t rd16s(uint32_t a) { int16_t v; std::memcpy(&v, reinterpret_cast<const void*>(uintptr_t(a)), 2); return v; }
void wr32(uint32_t a, uint32_t v) { std::memcpy(reinterpret_cast<void*>(uintptr_t(a)), &v, 4); }
void wr16(uint32_t a, int32_t v) { const int16_t s = int16_t(v); std::memcpy(reinterpret_cast<void*>(uintptr_t(a)), &s, 2); }

// 1.13c D2Common+0x381A0 (ECX = pool, EDX = entry, ESI = the array header {arr, s16 count, s16 capacity}): drop one entry,
// shrinking the array by four when more than eight slots are spare (Fog 10047, StatsEx.cpp line 0x110).
void removeEntry(uint32_t pool, uint32_t e, uint32_t list, const D2Common3A650Env* env)
{
    const int32_t count = rd16s(list + 4);
    const int32_t idx = static_cast<int32_t>(e - rd32(list)) >> 3;
    if (idx >= count) return;
    if (idx < count - 1)
        std::memmove(reinterpret_cast<void*>(uintptr_t(e)), reinterpret_cast<const void*>(uintptr_t(e + 8)), size_t((count - idx) * 8 - 8));
    wr16(list + 4, rd16s(list + 4) - 1);
    if (rd16s(list + 6) - rd16s(list + 4) > 8) {
        const int32_t cap = int16_t(rd16s(list + 6) - 4);
        wr16(list + 6, cap);
        wr32(list, env->realloc(pool, rd32(list), uint32_t(cap << 3), 0x110));
    }
}
}

// 1.13c D2Common+0x3A650
extern "C" uint32_t D2Common_3A650_ClearStats(uint32_t st, uint32_t eax, const D2Common3A650Env* e)
{
    if (!st) return eax;
    while (rd16s(st + 0x28) > 0) {
        int32_t hi = rd16s(st + 0x28), lo = 0;
        while (lo < hi) {
            const int32_t mid = (hi - lo) / 2 + lo;
            const uint32_t arr = rd32(st + 0x24);
            const int32_t k = static_cast<int32_t>(rd32(arr + mid * 8)), want = static_cast<int32_t>(rd32(arr));
            eax = uint32_t(mid);   // a miss (or a zero value) leaves EAX = the probe index; retail then spins on the same entry
            if (k < want) { lo = mid + 1; continue; }
            if (k > want) { hi = mid; continue; }
            const uint32_t ent = arr + uint32_t(mid) * 8;
            const int32_t value = static_cast<int32_t>(rd32(ent + 4));
            if (mid >= 0 && ent && -value != 0) {
                const uint32_t key = uint32_t(want);
                removeEntry(rd32(st), ent, st + 0x24, e);
                e->announce(st, key, -value);
                eax = rd32(st + 0x10);                              // 0x3A6D9: EAX = flags; non-negative ends the step
                if (static_cast<int32_t>(eax) < 0) {
                    eax = rd32(st + 8);                             // 0x3A6E0: EAX = owner kind; non-zero ends the step
                    if (!eax) eax = e->notify(key, st);
                }
            }
            break;
        }
    }
    return eax;
}
