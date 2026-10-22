// src/core/d2client/sub_66eb0.cpp -- 1.13c D2Client.dll wrapper ctor (alpha lane, no vector).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Client66EB0Env {
    uint8_t* (*alloc_0x36)();                       // Ordinal_401(0x36, ...) -> 0x36-byte block or null
    void (*ctor_66da0)(uint8_t* obj, uint8_t* wrapper); // D2Client+0x66DA0
    void (*set_vtable)(uint8_t* wrapper);           // vtable PTR_FUN_6fb8508c (host-owned)
};

// 1.13c D2Client+0x66EB0 -- wrapper ctor: vtable, alloc 0x36 bytes, run 0x66DA0 on it, store at wrapper+8
extern "C" EMSCRIPTEN_KEEPALIVE uint8_t* d2_client_66eb0(uint8_t* wrapper, const D2Client66EB0Env* env) {
    env->set_vtable(wrapper);
    uint8_t* o = env->alloc_0x36();
    if (o) env->ctor_66da0(o, wrapper);
    uint64_t p = (uint64_t)(uintptr_t)o;
    memcpy(wrapper + 8, &p, sizeof p);
    return wrapper;
}
