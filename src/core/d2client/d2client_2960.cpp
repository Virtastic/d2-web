// src/core/d2client/d2client_2960.cpp -- 1.13c D2Client.dll+0x02960 reimplemented from
// build-wasm/ghidra-d2client/decomp/0x2960.c (alpha lane). Base 0x6FAB0000.
// Header of the TSExplicitList<USGAMEDATA>: {capacity +0x0, size +0x4, data +0x8, flag +0xC},
// elements 0xC bytes. The callees are unrecovered rows, so they are host seams AFTER the retail
// arguments (the d2client_batch_1de0.cpp pattern): 0x29F0/0x2EC0 construct/destroy one element
// through 0x28A0/0x2580 and 0x2A80 reallocates. The capacity policy 0x2A40 is already recovered as
// D2Client_ClampPow2 (pure_leaves_b3.cpp): EAX = requested count, out = this.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

uint32_t D2Client_ClampPow2(uint32_t n, uint8_t* out); // 0x2A40

struct D2ClientBatch2960Env {
    void (*construct)(void* elem);      // 0x29F0 -> 0x28A0 ctor of one USGAMEDATA
    void (*destroy)(void* elem);        // 0x2EC0 -> 0x2580 dtor of one USGAMEDATA
    void (*realloc_data)(void* self, uint32_t capacity); // 0x2A80
};

// Resize the list to new_size. Growing past +0x4 first makes room when capacity (+0x0) is short:
// 0x2A40 clamps the capacity unless flag +0xC is set, then 0x2A80 reallocates and drops any tail
// above the requested count. The new slots [size, new_size) are constructed. Shrinking destroys
// [new_size, size). Either way +0x4 becomes new_size.
// 1.13c D2Client+0x02960
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_2960(uint32_t* self, uint32_t new_size,
                                         const D2ClientBatch2960Env* env) {
    if (self[1] < new_size) {
        if (self[0] < new_size) {
            uint32_t capacity = new_size;
            if (self[3] == 0)
                capacity = D2Client_ClampPow2(new_size, (uint8_t*)self);
            env->realloc_data(self, capacity);
        }
        for (uint32_t i = self[1]; i < new_size; ++i)
            env->construct((void*)(uintptr_t)(self[2] + i * 0xC));
        self[1] = new_size;
        return;
    }
    if (new_size < self[1]) {
        for (uint32_t i = new_size; i < self[1]; ++i)
            env->destroy((void*)(uintptr_t)(self[2] + i * 0xC));
    }
    self[1] = new_size;
}
