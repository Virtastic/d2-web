// src/core/d2client/sub_66eb0.cpp -- 1.13c D2Client.dll wrapper ctor.
#include <cstdint>
#include <emscripten.h>

struct D2Client66EB0Env {
    uint32_t vtMid, vtBase;                                                    // PTR_FUN_6fb8508c, PTR_LAB_6fb85048 (host-owned tags)
    uint32_t (*alloc)(uint32_t size, uint32_t srcName, int32_t line, uint32_t zero);   // Fog alloc thunk 0xBE66 (Ordinal_401)
    uint32_t (*ctor66da0)(uint32_t edx, uint32_t obj, uint32_t a1, uint32_t a2, uint32_t a3);   // D2Client+0x66DA0, EDX = a4
    uint32_t srcName;                                                          // DAT_6fb804f0
};

// 1.13c D2Client+0x66EB0 -- stdcall ret 0x14 (wrapper, a1, a2, a3, a4): vtable stores (base, then the final one), a 0x36-byte block
// from the Fog allocator (source name, line -1, 0), 0x66DA0 on it with EDX = a4 and the three stack words, the result (or 0) at wrapper+8.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_66eb0(uint32_t* w, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, const D2Client66EB0Env* env) {
    w[0] = env->vtBase;
    w[0] = env->vtMid;
    uint32_t o = env->alloc(0x36, env->srcName, -1, 0);
    w[2] = o ? env->ctor66da0(a4, o, a1, a2, a3) : 0;
    return (uint32_t)(uintptr_t)w;
}
