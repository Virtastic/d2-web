// probe for Fog 0x163C0 (sub_163C0, build a CONTEXT and walk the current thread): __fastcall, ECX = param (r[0]), plain ret.
// blob lg = r[14] (the hook log). Port: fog_sub_163c0 (src/core/fog/fog_sub_163c0.cpp). The recorded hook is the stack walker 15F50
// (stdcall, 3 words: param, thread, &CONTEXT): the call record, then the 50 CONTEXT dwords that matter (flags, Ebp, Eip, Esp) by hookblk.
// The walker is the probe's own seam: it logs the call and the block, the same as the capture's hook and hookblk records.
#include <emscripten.h>
#include <cstdint>
#include "hookenv.h"

extern "C" void fog_sub_163c0(void* param_1);

namespace {
HookEnv* H;
}

// strong definition over the weak stub in src/core/compat_link_weak.cpp: only the probe calls 15F50 on this path
extern "C" void fog_sub_15f50(void* param, uint32_t thread, void* ctx) {
    if (!H) return;
    H->h(0x15F50, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)param, thread, (uint32_t)(uintptr_t)ctx);
    H->blk(0x15F50, (const uint32_t*)ctx, 50);
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_fog_163c0(uint32_t* r) {
    static const uint32_t sc[2] = { 0, 0 };
    HookEnv e((uint32_t*)(uintptr_t)r[14], sc);
    H = &e;
    fog_sub_163c0((void*)(uintptr_t)r[0]);
    H = nullptr;
    r[4] = 0;
}
