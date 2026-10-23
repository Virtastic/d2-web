// src/core/d2win/d2win_sub_fd70.cpp -- 1.13c D2Win+0x0FD70 from the cached Ghidra decompile.
// Alpha lane: implementation + marker only. Widget = int32 array (0x9d dwords), handles are 32-bit.
#include <cstdint>
#include <emscripten.h>

struct D2WinFD70Env {
    void* (*alloc)(int32_t size, int32_t line);   // Ordinal_10042(0x24,0)
    const uint16_t* (*text)(void);                // Ordinal_10003()
    const uint32_t* bitMasks;                     // gdwBitMasks
    int32_t* (*p)(int32_t handle);
    int32_t (*h)(const void* ptr);
    int32_t* listHead;                            // DAT_6f9014a0 (handle)
    int32_t cbDraw, cbA, cbB, cbFlags;            // 0x6F8EF350 (if p5), 0x6F8EFA80, 0xF4E0, 0xF410
};

// 1.13c D2Win+0x0FD70
// Creates a type-6 widget (zeroed, 0x274 bytes), fills it from the args (p1,p2 -> [3],[4]; p3 -> [1];
// p4 -> [0xd]; p5 -> [0x14] and selects [0xc]; p6 -> [0x17]; p7 -> [0x10]; wd,ht -> [5],[6]; p9 -> [0x9a]),
// optionally copies the Ordinal_10003 text to +0x19*4 when p8, derives [0x15] = ceil(ht/256)*ceil(wd/256)
// ([0x16] = double that if [0x10]&0x20), and appends it to the widget list (via [0xf] links).
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t* d2win_sub_FD70(int32_t p1, int32_t p2, int32_t p3, int32_t p4, int32_t p5, int32_t p6,
                        int32_t p7, int32_t p8, int32_t p9, int32_t wd, int32_t ht, const D2WinFD70Env* e) {
    int32_t* w = (int32_t*)e->alloc(0x24, 0);
    for (int i = 0; i < 0x9d; i++) w[i] = 0;
    w[0] = 6;
    w[2] |= e->bitMasks[0];
    w[2] |= e->bitMasks[2];
    w[0xc] = p5 ? e->cbDraw : 0;
    w[1] = p3; w[0x10] = p7; w[3] = p1; w[4] = p2; w[0xd] = p4; w[5] = wd; w[6] = ht;
    w[7] = e->cbA; w[9] = e->cbB; w[10] = e->cbFlags;
    w[0x14] = p5; w[0x17] = p6;
    if (p8) { uint16_t* d = (uint16_t*)(w + 0x19); const uint16_t* s = e->text(); while ((*d++ = *s++)) {} }
    int32_t n = ((ht + 0xff) >> 8) * ((wd + 0xff) >> 8);
    w[0x15] = n;
    w[0x16] = (w[0x10] & 0x20) ? n * 2 : 0;
    w[0x9a] = p9; w[0x9b] = 0; w[0x9c] = 0;
    int32_t* t = e->listHead && *e->listHead ? e->p(*e->listHead) : nullptr;
    if (t) { while (t[0xf]) t = e->p(t[0xf]); t[0xf] = e->h(w); }
    else *e->listHead = e->h(w);
    return w;
}
