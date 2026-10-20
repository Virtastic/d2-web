// src/core/d2gfx/d2gfx_freeptd.cpp -- 1.13c D2Gfx CRT __freeptd, reimplemented from the cached decompile
// (ghidra-d2gfx/decomp/0x2AB6.c). Alpha batch lane (OBJECTIVE v8.3): implementation + marker only, no spec,
// no vector. Prior art: the byte-identical 47-byte body D2MCPClient+0x01A96 (d2_mcp_freeptd,
// src/core/realm/mcp_crt_batch_b56.cpp) and BNClient+0x0340D (src/core/realm/bnclient_freeptd_340d.cpp).
#include <cstdint>
#include <emscripten.h>

// Fls/Tls seam: 0x2AE5 __mtinit resolves 0x90F54/0x90F58 from kernel32 and seeds 0x10170.
struct D2GfxFreeptdEnv {
    int32_t  tls_index;                        // +0x10170 DAT_6fa90170, the FlsAlloc/TlsAlloc index
    void*    (*get)(uint32_t index);           // +0x90F54 DAT_6fa90f54 (FlsGetValue/TlsGetValue)
    void     (*set)(uint32_t index, void* v);  // +0x90F58 DAT_6fa90f58 (FlsSetValue/TlsSetValue)
    void     (*freefls)(void* ptd);            // 0x296F __freefls@4, the FLS destructor
};

// 1.13c D2Gfx+0x02AB6 (47 B, caller 0x1686 __CRT_INIT@12): with a live key, fetch the calling thread's
// _tiddata when none was passed, run the freefls destructor on it, and set the key's slot back to null.
// TODO(spec): freefls (0x296F __freefls@4) body not read; it stays a seam like the two resolved Fls*/Tls*
// slots, which mtinit installs.
// 1.13c D2Gfx+0x02AB6
void d2gfx_freeptd(void* ptd, const D2GfxFreeptdEnv* env) {
    if (env->tls_index == -1) return;
    if (ptd == nullptr) ptd = env->get((uint32_t)env->tls_index);
    env->freefls(ptd);
    env->set((uint32_t)env->tls_index, nullptr);
}
