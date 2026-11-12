// Level-list tick of a DRLG (EAX = pDrlg) and its room-side wrapper. The list hangs at pDrlg+0x47C (next at +0x1AC), the pool at
// pDrlg+0x478. Per level: a nonzero +0x0C or a zero +0x10 skips it; a nonzero delay at +0x1D4 is decremented; otherwise the
// can-discard test (0x2DDB0, level on the stack) decides: zero rearms the delay to 10, nonzero frees the level (0x2DAF0, level in ESI,
// pool and 1 on the stack). The next pointer is read after the callee returns. EAX on return is whatever the last step left:
// the +0x0C word, the zero +0x10 word, the decremented delay, the zero test result or the free's result; pDrlg for an empty list.
// The wrapper (stdcall, one pointer) loads EAX from [p+0x48] and runs the tick.
#include <cstdint>
#include <cstring>

struct D2Common2e2c0Env {
    uint32_t (*canDiscard)(void* ctx, uint32_t level);                 // 0x2DDB0
    uint32_t (*freeLevel)(void* ctx, uint32_t level, uint32_t pool);   // 0x2DAF0 (bAlloc = 1)
    void* ctx;
};

namespace {
inline uint32_t rd(uint32_t a) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }
inline void wr(uint32_t a, uint32_t v) { std::memcpy(reinterpret_cast<void*>(static_cast<uintptr_t>(a)), &v, 4); }
}

// 1.13c D2Common+0x2E2C0
extern "C" uint32_t d2common_2e2c0(uint32_t pDrlg, const D2Common2e2c0Env* e) {
    uint32_t eax = pDrlg;
    const uint32_t pool = rd(pDrlg + 0x478);
    for (uint32_t n = rd(pDrlg + 0x47C); n; n = rd(n + 0x1AC)) {
        eax = rd(n + 0x0C);
        if (eax) continue;
        eax = rd(n + 0x10);
        if (!eax) continue;
        eax = rd(n + 0x1D4);
        if (eax) { wr(n + 0x1D4, --eax); continue; }
        eax = e->canDiscard(e->ctx, n);
        if (eax) eax = e->freeLevel(e->ctx, n, pool);
        else wr(n + 0x1D4, 10);
    }
    return eax;
}

// 1.13c D2Common+0x3C800
extern "C" uint32_t d2common_3c800(uint32_t p, const D2Common2e2c0Env* e) {
    return d2common_2e2c0(rd(p + 0x48), e);
}
