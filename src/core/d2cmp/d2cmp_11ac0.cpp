// 1.13c D2CMP+0x11AC0 (#10006): one-time fixup of a loaded DC6-shaped cell file.
// stdcall (p, out, arena, _, expectType, _). `arena` is the block Ordinal 10042 returns in retail (the probe pins
// it; the allocator is not re-derived here). Void. The probe src/core/fn/d2cmp_11ac0.cpp drives this body.
#include <cstdint>
#include <unistd.h>

// Fatal paths (the message via Ordinal 10265/10024, then `push -1; call _exit`) are not reproduced: only the
// exit. Layout: [0] type, [1] flags, [4] directions, [5] frames, [6..] one offset per frame ([4]*[5] of them).
// 1.13c D2CMP+0x11AC0
extern "C" void d2cmp_11ac0(uint32_t* p, uint32_t* out, uint8_t* arena, int32_t expectType) {
    if (!p) return;
    uint32_t fl = p[1];
    if (!(fl & 1)) _exit(-1);
    if (p[0] != (uint32_t)(expectType == -1 ? 6 : expectType)) _exit(-1);
    if (fl & 2) return;
    uint32_t n = p[4] * p[5], bytes = n * 0x2c;
    p[1] = fl | 2;
    uint8_t* blk = arena;
    for (uint32_t i = 0; i < bytes / 4; i++) ((uint32_t*)blk)[i] = 0;
    for (uint32_t i = (bytes / 4) * 4; i < bytes; i++) blk[i] = 0;
    for (uint32_t i = 0; i < n; i++) {
        p[6 + i] += (uint32_t)(uintptr_t)p;                // relative offset -> absolute
        *(uint32_t*)(uintptr_t)(p[6 + i] + 0x18) = (uint32_t)(uintptr_t)(blk + i * 0x2c);
    }
    *out = (uint32_t)(uintptr_t)p;
}
