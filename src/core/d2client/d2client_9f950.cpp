// Panel toggle 1 <-> 2. Retail: EAX = argument record (or 0), plain ret. State word 0x10B214:
//  1 to 2: 0x11B834 = 0, 0x11B864 = 1, 0x11B854 = 5, 0x11B858 = 0; with an argument: 0x10B220 = arg[+0xC], EBX := 0x81E40(eax = arg, ecx = arg[+0xC], edx = 0,
//          ebx = 1, esi = arg, edi = 0), then 0x88A70 (four stack zeros, EBX = that result).
//  2 to 1: 0x10B220 = 0; with an argument 0x162D0 (eax = arg, ecx = 2, edx = 0, ebx = 1; EDX survives it); 0x88A70 with four stack words = that EDX, EBX = 1.
// any other state: nothing.
// 1.13c D2Client+0x9F950
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2cRow9f950Env {
    uint32_t* st;                                                        // D2Client+0x10B214 (0x18 bytes): +0 state, +0xC = 0x10B220
    uint32_t* gl;                                                        // D2Client+0x11B834 (0x38 bytes): +0, +0x20 (B854), +0x24 (B858), +0x30 (B864)
    uint32_t (*c81e40)(uintptr_t arg, uint32_t ecx, uint32_t ebx);       // 0x81E40
    uint32_t (*c162d0)(uintptr_t arg, uint32_t ecx, uint32_t ebx);       // 0x162D0, returns EDX after the call
    void (*c88a70)(uint32_t ebx, uint32_t a, uint32_t b, uint32_t c, uint32_t d); // 0x88A70 (stdcall, 4 words)
};

extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_9f950(uintptr_t arg, const D2cRow9f950Env* e) {
    uint32_t* st = e->st; uint32_t* gl = e->gl;
    if (st[0] == 1) {
        st[0] = 2; gl[0] = 0; gl[0x30 / 4] = 1; gl[0x20 / 4] = 5; gl[0x24 / 4] = 0;
        if (!arg) return;
        uint32_t c; std::memcpy(&c, (const void*)(arg + 0xC), 4);
        st[0xC / 4] = c;
        const uint32_t r = e->c81e40(arg, c, 1);
        e->c88a70(r, 0, 0, 0, 0);
    } else if (st[0] == 2) {
        st[0] = 1; st[0xC / 4] = 0;
        uint32_t edx = 0;
        if (arg) edx = e->c162d0(arg, 2, 1);
        e->c88a70(1, edx, edx, edx, edx);
    }
}
