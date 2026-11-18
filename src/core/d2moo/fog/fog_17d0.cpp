// 1.13c Fog+0x17D0 array resize(this = ESI, capacity on the stack, ret 4), guest 32-bit layout: +0 capacity, +4 count, +8 items (dwords).
// The three allocator thunks go through the FogEnv seam: Storm #405 realloc (0xCE7E), #401 alloc (0xCDC4), #403 free (0xCDCA). A failed
// realloc (0) falls back to alloc + copy of min(capacity, count) entries + free of the old block.
#include <cstdint>
#include "fog_env.h"
extern "C" void fog_17d0(const FogEnv* e, uint32_t self, uint32_t n) {
    constexpr uint32_t kTag = 0xB2000000u | 0x2FFE8u;   // the "\\PAX" source-file string (Fog+0x2FFE8)
    const uint32_t old = fe_r32(self + 8);
    fe_w32(self, n);
    uint32_t p = fe_call(e, 0xce7e, 0, 0, 0, 0, 0, old, n * 4, kTag, 0xfffffffeu, 0x10);
    fe_w32(self + 8, p);
    if (p) return;
    p = fe_call(e, 0xcdc4, 0, 0, 0, 0, 0, n * 4, kTag, 0xfffffffeu, 0);
    fe_w32(self + 8, p);
    if (!old) return;
    uint32_t m = fe_r32(self + 4);
    if (n <= m) m = n;
    for (uint32_t i = 0; i < m; i++) if (fe_r32(self + 8) + i * 4) fe_w32(fe_r32(self + 8) + i * 4, fe_r32(old + i * 4));
    fe_call(e, 0xcdca, 0, 0, 0, 0, 0, old, kTag, 0xfffffffeu, 0);
}
