// src/core/realm/bnclient_batch_5872.cpp -- one UNMAPPED 1.13c BNClient.dll alpha-lane row (Stage A:
// implementation + marker only, no spec, no vector), transcribed from
// build-wasm/ghidra-bnclient/decomp/0x5872.c (image base 0x6FF20000).
//
// Row: __local_unwind2 0x5872, MSVC 2003 (VC7.1) Release SEH scope-table walker. Same rule as D2Gfx
// 0x3922 (src/core/d2gfx/d2gfx_crt_misc.cpp); the 0x5906 __NLG_Notify thunk is a seam. The retail
// fs:[0] ExceptionList bracket leaves nothing observable, as in the sibling.
#include <cstdint>
#include <emscripten.h>

struct BnLocalUnwind2Env {
    void (*notify)(void);   // 0x05906 __NLG_Notify-style thunk
};

struct BnSehEntry {
    int32_t to_state;       // +0x0
    int32_t filter;         // +0x4
    uintptr_t handler;      // +0x8
};

// 1.13c BNClient+0x05872 (104 B, callers 0x2692/0x30C3/0x790D): __local_unwind2(frame, target).
// frame+8 is the 0xC-byte-entry scope table, frame+0xC the current trylevel. Until the trylevel is -1
// or equals target: publish the entry's to_state as the new trylevel and, when the entry has no filter,
// notify and run its handler.
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_local_unwind2(uint32_t frame, int32_t target,
                                           const BnLocalUnwind2Env* env) {
    int32_t* const f = (int32_t*)(uintptr_t)frame;
    for (;;) {
        const int32_t idx = f[3];
        if (idx == -1 || idx == target) break;
        BnSehEntry* const e = (BnSehEntry*)(uintptr_t)(uint32_t)f[2] + idx;
        f[3] = e->to_state;
        if (e->filter == 0) {
            env->notify();
            ((void (*)(void))e->handler)();
        }
    }
}
