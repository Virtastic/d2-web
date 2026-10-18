// src/core/d2client/d2client_batch_74af.cpp -- one UNMAPPED 1.13c D2Client.dll CRT leaf reimplemented
// from build-wasm/ghidra-d2client/decomp/0x74AF.c (alpha lane). Image base 0x6FAB0000; every global is
// named at its retail RVA. The seam shape (Env struct after the retail arguments, module state in a
// pinned file static, `_pin` export) is the one already used for __getptd in d2client_batch_908f.cpp
// and for the TLS path of __CRT_INIT in crt_init_4475.cpp.
#include <cstdint>
#include <emscripten.h>

// +0x0DB774 DAT_6fb8b774: the _tiddata TLS slot; -1 means it was never assigned.
static uint32_t g_ptd_tls_index = 0x0DB774u;

struct D2ClientFreeptdEnv {
    void*   (*tls_get_value)(uint32_t index);              // IAT 0x0FAA64 TlsGetValue
    int32_t (*tls_set_value)(uint32_t index, void* value); // IAT 0x0FAA68 TlsSetValue
    void    (*freefls)(void* ptd);                         // 0x7368 __freefls@4 (still UNMAPPED)
};

// 1.13c D2Client+0x074AF (47 B): free the calling thread's _tiddata. No-op when the slot is -1. A null
// argument means "this thread's block": it is fetched with TlsGetValue(slot). __freefls releases the
// block's owned buffers and the block itself, then the slot is cleared with TlsSetValue(slot, NULL).
// Both TLS calls act on the slot index, not on the block, so the block's address is the only thing the
// argument pins.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_74af(void* ptd, const D2ClientFreeptdEnv* env) {
    if (g_ptd_tls_index == 0xFFFFFFFFu) return;
    if (ptd == nullptr) ptd = env->tls_get_value(g_ptd_tls_index);
    env->freefls(ptd);
    env->tls_set_value(g_ptd_tls_index, nullptr);
}

extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_freeptd_pin(uint32_t tls_index) {
    g_ptd_tls_index = tls_index;
}
