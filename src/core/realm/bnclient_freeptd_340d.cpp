// src/core/realm/bnclient_freeptd_340d.cpp -- one UNMAPPED 1.13c BNClient.dll row reimplemented from
// build-wasm/ghidra-bnclient/decomp/0x340D.c. Base 0x6FF20000; every global is named at its retail RVA.
// MSVC 2003 (VC7.1) Release CRT `__freeptd(_tiddata*)`: the per-thread data destructor the thread exit
// path calls. Prior art: the byte-identical 47-byte body D2MCPClient+0x01A96 (d2_mcp_freeptd,
// src/core/realm/mcp_crt_batch_b56.cpp), which has the same shape with that module's key and slots.
#include <cstdint>
#include <emscripten.h>

struct BnFreeptdEnv {
    int32_t  tls_index;                        // +0x3D1B0 DAT_6ff3d1b0, the FlsAlloc/TlsAlloc index
    void*    (*get)(uint32_t index);           // +0x3E4D4 DAT_6ff3e4d4 (FlsGetValue/TlsGetValue)
    void     (*set)(uint32_t index, void* v);  // +0x3E4D8 DAT_6ff3e4d8 (FlsSetValue/TlsSetValue)
    void     (*freefls)(void* ptd);            // 0x32C6 __freefls@4, the FLS destructor
};

// 1.13c BNClient+0x0340D (47 B, callers 0x1E59 __endthreadex, 0x1FDD __CRT_INIT@12): with a live key,
// fetch the calling thread's _tiddata when none was passed, run the freefls destructor on it, and set
// the key's slot back to null.
// TODO(spec): freefls (0x32C6 __freefls@4) body not read; it stays a seam like the two resolved
// Fls*/Tls* slots, which mtinit installs.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_340d(void* ptd, const BnFreeptdEnv* env) {
    if (env->tls_index == -1) return;
    if (ptd == nullptr) ptd = env->get((uint32_t)env->tls_index);
    env->freefls(ptd);
    env->set((uint32_t)env->tls_index, nullptr);
}
