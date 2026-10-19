// src/core/d2client/crt_free_lc_time_acf5.cpp -- 1.13c D2Client+0x0ACF5 = MSVC 2003 Release CRT
// ___free_lc_time (build-wasm/ghidra-d2client/decomp/0xACF5.c). It frees the 43 pointer slots (indices
// 0x00-0x2A, byte offsets 0x00-0xA8) of the __lconv-style time block in retail's slot order and does
// nothing for a NULL block. The callee is the recovered _free, D2Client+0x04891, which ignores NULL.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"

extern "C" void d2_client_4891(uint32_t p, D2cHost host);   // _free, d2client_free_4891.cpp

// 1.13c D2Client+0x0ACF5. cdecl; [esp+4] = block. The decompile's call order: 1-6, 0, 8-0xD, 7, 0xE-0x2A.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_acf5(uint32_t block, D2cHost host) {
    if (block == 0) return;
    static const uint8_t kOrder[43] = {
        1, 2, 3, 4, 5, 6, 0, 8, 9, 0xA, 0xB, 0xC, 0xD, 7,
        0xE, 0xF, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B,
        0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A,
    };
    const uint32_t* slots = (const uint32_t*)(uintptr_t)block;
    for (uint32_t i = 0; i < 43; ++i) d2_client_4891(slots[kOrder[i]], host);
}
