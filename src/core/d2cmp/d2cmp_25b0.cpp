// 1.13c D2CMP+0x25B0 (EAX = new capacity, ESI = array {cap, count, data}): destroy entries past the new
// capacity (0x1490), store cap, Storm-realloc data (flags 0x10, in place only). When that fails,
// allocate fresh and move the surviving min(count, cap) entries (0x2670 then 0x1490), free the old block.
#include "d2cmp_guest.h"
using namespace d2cmpg;
extern "C" uint32_t d2cmp_2670(uint8_t* m, uint32_t dst, uint32_t src);
// 1.13c D2CMP+0x25B0
extern "C" void d2cmp_25b0(uint8_t* m, uint32_t cap, uint32_t arr, const D2CmpGuestEnv* e) {
    uint32_t old = rd(m, arr + 8);
    for (uint32_t i = cap; i < rd(m, arr + 4); ++i) e->f1490(m, old + i * 0xc);
    wr(m, arr, cap);
    uint32_t d = e->realloc(m, old, cap * 0xc, kTagTsList, -2, 0x10);
    wr(m, arr + 8, d);
    if (d != 0) return;
    d = e->alloc(m, cap * 0xc, kTagTsList, -2, 0);
    wr(m, arr + 8, d);
    if (old == 0) return;
    uint32_t n = rd(m, arr + 4);
    if (cap < n) n = cap;
    for (uint32_t i = 0; i < n; ++i) {
        d2cmp_2670(m, rd(m, arr + 8) + i * 0xc, old + i * 0xc);
        e->f1490(m, old + i * 0xc);
    }
    e->free(m, old, kTagTsList, -2, 0);
}
