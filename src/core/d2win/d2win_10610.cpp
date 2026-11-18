// src/core/d2win/d2win_10610.cpp -- 1.13c D2Win.dll+0x10610: the type-2 control constructor.
// stdcall, seven stack words (ret 0x1c); no register input (retail clears ESI itself). Nodes keep the retail 0x54-byte
// layout with 32-bit links (+0x3c next).
#include <cstdint>

// alloc: the Fog allocator (thunk 0x7678: size, line); head: the control list head DAT 0x214A0;
// h: the module's handlers 0x104B0 (+0x1c) and 0x10190 (+0x24).
// 1.13c D2Win+0x10610
extern "C" uint32_t* d2win_10610(const uint32_t a[7], uint32_t* (*alloc)(uint32_t size, uint32_t line),
                                 uint32_t* head, const uint32_t h[2]) {
    uint32_t* n = alloc(0x54, 0x3b);
    for (int i = 0; i < 0x15; ++i) n[i] = 0;
    n[0] = 2;
    n[2] |= 1u << 0;                 // gdwBitMasks[0]
    n[2] |= 1u << 2;                 // gdwBitMasks[2]
    n[1] = a[4]; n[3] = a[0]; n[4] = a[1]; n[5] = a[2]; n[6] = a[3];
    n[7] = h[0]; n[9] = h[1];
    n[0x11] = 5;
    n[0x13] = a[5]; n[0x14] = a[6];
    if (!*head) { *head = (uint32_t)(uintptr_t)n; return n; }
    uint32_t* p = (uint32_t*)(uintptr_t)*head;
    while (p[0xf]) p = (uint32_t*)(uintptr_t)p[0xf];
    p[0xf] = (uint32_t)(uintptr_t)n;
    return n;
}
