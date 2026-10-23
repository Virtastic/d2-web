// 1.13c Fog+0x1A9F0: pool (eax) range lookup. Table of {value,key} u32 pairs sorted by key;
// returns value of the last entry whose key <= param, else 0. Retail wraps it in the pool critical section (+4), a no-op single-threaded.
#include <cstdint>
extern "C" uint32_t fog_sub_1a9f0_lookup(const uint32_t* tab, int32_t count, uint32_t key) {
    int32_t lo = 0, hi = count;
    while (lo < hi) {
        int32_t mid = lo + ((hi - lo) >> 1);
        if (tab[mid * 2 + 1] <= key) lo = mid + 1; else hi = mid;
    }
    lo--;
    return (lo < 0 || lo >= count) ? 0 : tab[lo * 2];
}
