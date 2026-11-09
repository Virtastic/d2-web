// 1.13c D2Client+0xA7000 (178 B): for each of the six type indices in the order table (D2Client+0xD412C), walk the unit slot table 0x10A608 and then, in a second
// pass, 0x109A08: for all 0x80 buckets of row (order[k] << 7), while the slot is nonzero, read the unit's +0xE4 next, call 0xA6D60 (stdcall unit), then store next in the
// slot (re-reading the order word, as retail does).
#include <cstdint>
#include <emscripten.h>

extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a7000(uint32_t* tab_a, uint32_t* tab_b, const uint32_t* order, void (*fa6d60)(uint32_t unit)) {
    for (int pass = 0; pass < 2; pass++) {
        uint32_t* tab = pass ? tab_b : tab_a;
        for (int k = 0; k < 6; k++)
            for (uint32_t i = 0; i < 0x80; i++) {
                uint32_t cur = tab[(order[k] << 7) + i];
                while (cur) {
                    uint32_t nx = *(const uint32_t*)(uintptr_t)(cur + 0xe4);
                    fa6d60(cur);
                    tab[(order[k] << 7) + i] = nx;
                    cur = nx;
                }
            }
    }
}
