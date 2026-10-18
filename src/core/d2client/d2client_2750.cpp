// src/core/d2client/d2client_2750.cpp -- 1.13c D2Client.dll+0x02750 reimplemented from
// build-wasm/ghidra-d2client/decomp/0x02750.c (alpha lane). Base 0x6FAB0000.
// Caller 0x2320 passes one chunk from the object's +0x40 list; the callee 0x2700 drains that same
// chunk's USGAMEDATA array. 0x2700 is an unrecovered row, so it is a host seam AFTER the retail
// argument (the d2client_batch_1de0.cpp pattern).
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

struct D2ClientBatch2750Env {
    void (*f_2700)(void* self);   // 0x2700 drains this chunk's USGAMEDATA array
};

// 1.13c D2Client+0x02750 (122 B): release one chunk. If the intrusive-list head at self+0x10 is set,
// unlink it (the pair at +0x10/+0x14: a non-negative predecessor word is relative to the node, a
// negative one is ~target), then clear both words; finally run the 0x2700 body on self.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_2750(void* self, const D2ClientBatch2750Env* env) {
    uint8_t* s = (uint8_t*)self;
    uint32_t node = *(uint32_t*)(s + 0x10);
    if (node != 0) {
        uint32_t enc = *(const uint32_t*)(s + 0x14);
        uint32_t slot;
        if ((int32_t)enc < 0)
            slot = ~enc;
        else
            slot = (uint32_t)(uintptr_t)(s + 0x10) +
                   (enc - *(const uint32_t*)(uintptr_t)(node + 4));
        *(uint32_t*)(uintptr_t)slot = node;
        *(uint32_t*)(uintptr_t)(node + 4) = enc;
        *(uint32_t*)(s + 0x10) = 0;
        *(uint32_t*)(s + 0x14) = 0;
    }
    env->f_2700(self);
}
