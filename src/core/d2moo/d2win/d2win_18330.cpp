// src/core/d2win/d2win_18330.cpp -- 1.13c D2Win.dll+0x18330, dispatch a message to the captured control
// Decompile-derived from the retail bytes (retail/D2/D2SE/CORES/1.13c/D2Win.dll, image base
// 0x6F8E0000) and the cached Ghidra decompile or, where Ghidra defines no function, the
// disassembly. Imported callees and D2Win state are seams on the Env struct, passed last, so the
// retail argument order is preserved. Prior art: D2MOO 1.10f reference/D2MOO/source/D2Win/
// (control type numbering, list item layout, control chain fields).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

struct D2Win18330Env {
    uint32_t* capture;                    // 0x6F9014B4
    uint32_t* modal;                      // 0x6F9014B8
    void (*dispatch)(uint32_t* msg);      // D2Win+0x182A0
    void (*deliver)(uint32_t* msg);       // D2Win+0xF6E0 (ECX)
    void (*passOn)(uint32_t* msg);        // Storm #511
};

// 1.13c D2Win+0x18330 (stdcall msg). No captured control, or one outside an open modal chain:
// fall back to 0x182A0. Otherwise msg[0] = captured control, 0xF6E0 delivers it, Storm #511.
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_18330(uint32_t* msg, const D2Win18330Env* env) {
    uint32_t c = *env->capture;
    bool inModal = true;
    if (c && *env->modal) {
        inModal = false;
        for (uint32_t p = *env->modal; p; p = rd32(ptr(p), 0x3C)) if (p == c) { inModal = true; break; }
    }
    if (!c || !inModal) { env->dispatch(msg); return; }
    msg[0] = c;
    env->deliver(msg);
    env->passOn(msg);
}
