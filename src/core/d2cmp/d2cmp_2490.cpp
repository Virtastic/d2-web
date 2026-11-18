// 1.13c D2CMP+0x2490 (EAX = array {cap, count, data, granule}, EDI = new count): resize. Growing past
// cap reallocates to the new count rounded up to the granule (+0xc, else 0x2570); new entries are
// constructed (0x2520), dropped ones destroyed (0x1490).
#include "d2cmp_guest.h"
using namespace d2cmpg;
extern "C" uint32_t d2cmp_2570(uint8_t* m, uint32_t n, uint32_t arr);
extern "C" void d2cmp_25b0(uint8_t* m, uint32_t cap, uint32_t arr, const D2CmpGuestEnv* e);
extern "C" uint32_t d2cmp_2520(uint8_t* m, uint32_t p);
// 1.13c D2CMP+0x2490
extern "C" void d2cmp_2490(uint8_t* m, uint32_t arr, uint32_t n, const D2CmpGuestEnv* e) {
    uint32_t count = rd(m, arr + 4);
    if (n > count) {
        if (n > rd(m, arr)) {
            uint32_t g = rd(m, arr + 0xc);
            if (g == 0) g = d2cmp_2570(m, n, arr);
            uint32_t r = n % g;
            d2cmp_25b0(m, r ? n + (g - r) : n, arr, e);
        }
        for (uint32_t i = rd(m, arr + 4); i < n; ++i) d2cmp_2520(m, rd(m, arr + 8) + i * 0xc);
    } else if (n < count) {
        for (uint32_t i = n; i < rd(m, arr + 4); ++i) e->f1490(m, rd(m, arr + 8) + i * 0xc);
    }
    wr(m, arr + 4, n);
}
