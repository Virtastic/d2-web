// src/core/d2client/d2client_batch_2c40.cpp -- 1.13c D2Client.dll+0x02C40 scalar deleting destructor,
// reimplemented from build-wasm/ghidra-d2client/decomp/0x2C40.c (alpha lane). Unrecovered callees keep
// their retail argument order and sit as seam pointers in the env struct AFTER the retail arguments:
// 0x3F240 is the destructor body, and 0xBE6C is the Storm ordinal-403 (SMemFree) import thunk, whose
// signature is SMemFree(ptr, file, line, flags).
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

struct D2ClientBatch2c40Env {
    void (*sub_3F240)(void* self);   // 0x3F240  destructor body of the object
    void (*smem_free)(void* location, const char* logfile, int32_t logline, uint32_t flags); // 0xBE6C Ordinal_403
};

// 1.13c D2Client+0x02C40 (40 B): scalar deleting destructor (vtable slot). Runs the 0x3F240 destructor
// body, then, when flags bit 0 is set and this is non-NULL, releases the object with
// SMemFree(this, "delete", -1, 0); returns this (the decompile clears no state itself -- the 0x3F240
// body rewrites the vtable and drops the owned child at this+8).
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_client_2c40(void* self, uint8_t flags, const D2ClientBatch2c40Env* env) {
    env->sub_3F240(self);
    if ((flags & 1) != 0 && self != nullptr)
        env->smem_free(self, "delete", -1, 0);
    return self;
}
