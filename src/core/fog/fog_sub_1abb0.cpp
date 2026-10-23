// 1.13c Fog+0x1ABB0: bitmap search for a word with a clear bit. EAX=start bit, ECX=uint32 bitmap; returns bit index or -1.
#include <cstdint>

static int fog_lowest_clear(uint32_t w) {  // 1.13c Fog+0x1AB20
    int i = 0;
    if (~w != 0) while (((~w >> i) & 1) == 0) ++i;
    return i;
}

int fog_sub_1abb0(uint32_t start, const uint32_t* p) {
    int idx = (int32_t)start >> 5, base = 0;
    for (;;) {
        if (idx != 0) {
            // words below the start word (up to 8 per chunk), highest first
            int top = idx > 8 ? 8 : idx;
            for (int k = top - 1; k >= 0; --k)
                if (p[k] != 0xffffffffu) return fog_lowest_clear(p[k]) + 32 * k + base;
        }
        if (idx < 9) {
            uint32_t sh = start & 0x1f;
            if (sh && p[idx] != (uint32_t)((1ull << sh) - 1)) return fog_lowest_clear(p[idx]) + idx * 32 + base;
            return -1;
        }
        idx -= 8; p += 8; base += 0x100;
    }
}
