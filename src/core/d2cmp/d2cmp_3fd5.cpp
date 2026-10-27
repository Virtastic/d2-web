// 1.13c D2CMP+0x03FD5 (47 B, caller 0x2AA2): MSVC 2003 CRT `__freeptd`. Decomp: ghidra-d2cmp/decomp/0x3FD5.c.
// Prior art: src/core/realm/bnclient_freeptd_340d.cpp (identical body, other module's key/slots).
#include <cstdint>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

struct D2cmpFreeptdEnv {
    int32_t tls_index;                        // +0x32188 DAT_6fe32188 (-1 = no key)
    void*   (*get)(uint32_t index);           // +0x347B4 FlsGetValue/TlsGetValue
    void    (*set)(uint32_t index, void* v);  // +0x347B8 FlsSetValue/TlsSetValue
    void    (*freefls)(void* ptd);            // 0x3E8E __freefls@4
};

extern "C" EMSCRIPTEN_KEEPALIVE void d2cmp_3fd5(void* ptd, const D2cmpFreeptdEnv* env) {
    if (env->tls_index == -1) return;
    if (ptd == nullptr) ptd = env->get((uint32_t)env->tls_index);
    env->freefls(ptd);
    env->set((uint32_t)env->tls_index, nullptr);
}
