// src/core/d2client/d2client_batch_2b90.cpp -- 1.13c D2Client.dll+0x2B90 from
// build-wasm/ghidra-d2client/decomp/0x2B90.c. Base 0x6FAB0000.
#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x2B90 (78 B): initialise an intrusive-list head at self+4. Stores *src at self[0],
// zeroes the head, then publishes it as self-referential and empty: successor at self[1] points at
// the head itself and the encoded predecessor at self[2] is ~(address of the head). Caller 0x2B40.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_2b90(void* self, const uint32_t* src) {
    uint32_t* obj = (uint32_t*)self;
    uint32_t head = (uint32_t)(uintptr_t)self + 4;
    obj[1] = 0;
    obj[2] = 0;
    uint32_t value = *src;
    obj[1] = head;
    obj[0] = value;
    obj[2] = ~head;
}
