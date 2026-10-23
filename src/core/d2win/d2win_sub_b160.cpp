// src/core/d2win/d2win_sub_b160.cpp -- 1.13c D2Win scrollbar (type 5) constructor, from the cached decompile.
#include <cstdint>
#include <emscripten.h>

struct D2WinB160Env {
    void* (*alloc)(int32_t size, int32_t line);  // Ordinal_10042(0x1c,0)
    const uint32_t* bitMasks;                    // gdwBitMasks
    int32_t cb_af40, cb_ad60, cb_ab50;           // 0xAF40, 0xAD60, 0xAB50 callbacks
    int32_t* listHead;                           // DAT_6f9014a0 (widget list head, 32-bit handle)
    int32_t (*h)(const void* p);                 // pointer -> handle
    int32_t* (*p)(int32_t handle);               // handle -> pointer
};

// 1.13c D2Win+0x0B160
// a=ECX, b=EDX, c,d stack, ebx=EBX at entry, stk=slot at the return address (as the comments reads it).
// Allocs 0x1c bytes (as retail) but clears 0x18 dwords, sets type 5, appends to the widget list tail (link at +0x3c).
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t* d2win_sub_B160(int32_t a, int32_t b, int32_t c, int32_t d, int32_t ebx, int32_t stk, D2WinB160Env* e) {
    int32_t* w = (int32_t*)e->alloc(0x1c, 0);
    for (int i = 0; i < 0x18; i++) w[i] = 0;
    w[0] = 5;
    w[2] = e->bitMasks[0] | e->bitMasks[2];
    w[1] = c; w[6] = stk; w[5] = ebx; w[3] = a; w[4] = b;
    w[7] = e->cb_af40; w[9] = e->cb_ad60; w[10] = e->cb_ab50;
    w[0xb] = 0; w[0xc] = 0; w[0xd] = d;
    w[0xf] = w[0x10] = w[0x11] = w[0x12] = w[0x13] = w[0x14] = w[0x15] = w[0x17] = 0;
    if (!*e->listHead) { *e->listHead = e->h(w); return w; }
    int32_t* t = e->p(*e->listHead);
    while (t[0xf]) t = e->p(t[0xf]);
    t[0xf] = e->h(w);
    return w;
}
