// 1.13c D2Win+0x18370 (141 B): entry of the control message table; the twin of the key dispatcher for another message.
// stdcall msg. A focused control outside an open modal chain is skipped; otherwise the focus handler 0x15450 runs
// (ECX = the focus, EDX = msg[2], no stack words) and a non-zero answer ends the dispatch. Then every control from the
// modal (or the chain head; next at +0x3C) with a callback at +0x2C gets the message with msg[0] = itself, the nesting
// depth held up across the call, until one answers non-zero. Storm #511 always runs last.
#include <cstdint>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

struct D2Win18370Env {
    uint32_t* focus;                                    // 0x214B0
    uint32_t* modal;                                    // 0x214B8
    uint32_t* head;                                     // 0x214A0
    int32_t* depth;                                     // 0x214A8
    int32_t (*focusMsg)(uint32_t focus, uint32_t word); // D2Win+0x15450
    int32_t (*call)(uint32_t cb, uint32_t* msg);        // control callback at +0x2C
    void (*passOn)(uint32_t* msg);                      // Storm #511
};

extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_18370(uint32_t* msg, const D2Win18370Env* env) {
    const uint32_t focus = *env->focus, word = msg[2];
    bool handled = false;
    if (focus) {
        bool reach = true;
        if (*env->modal) {
            reach = false;
            for (uint32_t p = *env->modal; p; p = rd32(ptr(p), 0x3C)) if (p == focus) { reach = true; break; }
        }
        if (reach) handled = env->focusMsg(focus, word) != 0;
    }
    if (!handled) {
        for (uint32_t c = *env->modal ? *env->modal : *env->head; c; c = rd32(ptr(c), 0x3C)) {
            const uint32_t cb = rd32(ptr(c), 0x2C);
            if (!cb) continue;
            ++*env->depth;
            msg[0] = c;
            const int32_t r = env->call(cb, msg);
            --*env->depth;
            if (r) break;
        }
    }
    env->passOn(msg);
}
