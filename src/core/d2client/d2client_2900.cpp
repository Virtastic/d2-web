// src/core/d2client/d2client_2900.cpp -- 1.13c D2Client.dll 0x02900 (size 93), reimplemented from
// build-wasm/ghidra-d2client/decomp/0x2900.c. Caller 0x02670 passes self+0x38.
//
// Initialises a three-dword debug-STL block at param: first the whole block to zero, then the pair
// [1] = address of itself and [2] = ~that address, with [0] = 0x10. The decompile stores the same
// pair twice (the compiler re-emitted it after the [0] = 0x10 store); the final state is identical
// and no read between the stores exists, so the duplicate is folded here.
// ponytail: retail keeps the self-link in a 32-bit word; the uintptr_t hop assumes the wasm64 heap
// stays below 4 GiB (true for the browser build's default memory). Widen if that stops holding.
#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x02900
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_2900(int32_t* param_1) {
    int32_t* self = param_1 + 1;
    param_1[0] = 0;
    self[0] = 0;
    param_1[2] = 0;
    self[0] = (int32_t)(intptr_t)self;
    param_1[2] = (int32_t)~(uint32_t)(uintptr_t)self;
    param_1[0] = 0x10;
    self[0] = (int32_t)(intptr_t)self;
    param_1[2] = (int32_t)~(uint32_t)(uintptr_t)self;
}
