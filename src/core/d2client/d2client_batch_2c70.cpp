// src/core/d2client/d2client_batch_2c70.cpp -- 1.13c D2Client.dll RVA 0x2C70 from
// build-wasm/ghidra-d2client/decomp/0x2C70.c. Base 0x6FAB0000. The unrecovered destructor body
// 0x38770 and Storm ordinal 403 (SMemFree, reached through thunk 0x0BE6C) are host seams AFTER the
// retail arguments, which keep their order (the d2client_batch_1de0.cpp pattern).
#include <cstdint>
#include <emscripten.h>

struct D2Client2c70Env {
    void (*sub_38770)(void* self);                                            // 0x38770 destructor body
    void (*smem_free)(void* location, const char* logfile, int32_t logline,
                      uint32_t flags);                                        // Storm ordinal 403
};

// 1.13c D2Client+0x02C70 (41 B): scalar deleting destructor (vtable slot). Runs the destructor body
// 0x38770(self), then when flags bit 0 is set and self is non-null frees it with
// SMemFree(self, "delete", -1, 0); returns self in EAX.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_client_2c70(void* self, uint8_t flags,
                                          const D2Client2c70Env* env) {
    env->sub_38770(self);
    if ((flags & 1) != 0 && self != nullptr)
        env->smem_free(self, "delete", -1, 0);
    return self;
}
