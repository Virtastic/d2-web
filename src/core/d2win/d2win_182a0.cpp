// src/core/d2win/d2win_182a0.cpp -- 1.13c D2Win.dll+0x182A0, dispatch a key message to the focus, then down the control chain
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

struct D2Win182A0Env {
    uint32_t* focus;                      // 0x6F9014B0
    uint32_t* modal;                      // 0x6F9014B8
    uint32_t* head;                       // 0x6F9014A0
    int32_t* depth;                       // 0x6F9014A8
    int32_t (*focusKey)(uint32_t key);    // D2Win+0x14EC0
    int32_t (*call)(uint32_t cb, uint32_t* msg);   // control callback at +0x30
    void (*passOn)(uint32_t* msg);        // Storm #511
};

static bool in_chain(uint32_t from, uint32_t c) {
    for (uint32_t p = from; p; p = rd32(ptr(p), 0x3C)) if (p == c) return true;
    return false;
}

// 1.13c D2Win+0x182A0 (stdcall msg; key = msg[2], pushed whole). A focused control outside an open modal chain
// is skipped; otherwise a non-zero D2Win+0x14EC0(key) ends the dispatch. Then every control from
// the modal (or the chain head) with a callback at +0x30 gets the message with msg[0] = itself,
// the nesting depth held up across the call, until one returns non-zero. Storm #511 always last.
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_182A0(uint32_t* msg, const D2Win182A0Env* env) {
    bool handled = false;
    if (*env->focus && !(*env->modal && !in_chain(*env->modal, *env->focus)))
        handled = env->focusKey(msg[2]) != 0;   // retail pushes the whole key dword (the callee narrows it)
    if (!handled) {
        for (uint32_t c = *env->modal ? *env->modal : *env->head; c; c = rd32(ptr(c), 0x3C)) {
            uint32_t cb = rd32(ptr(c), 0x30);
            if (!cb) continue;
            ++*env->depth;
            msg[0] = c;
            int32_t r = env->call(cb, msg);
            --*env->depth;
            if (r) break;
        }
    }
    env->passOn(msg);
}
