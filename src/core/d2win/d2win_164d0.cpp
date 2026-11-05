// src/core/d2win/d2win_164d0.cpp -- 1.13c D2Win.dll+0x164D0: the type-8 (timer) control constructor.
// stdcall, two stack words (ret 8): the interval (+0x40) and the callback (+0x34). Nodes keep the retail 0x48-byte layout
// with 32-bit links (+0x3c next).
#include <cstdint>

// alloc: the Fog allocator (thunk 0x7678: size, line); head: the control list head DAT 0x214A0; tick: GetTickCount;
// h: the module's handlers 0x163E0 (+0x20) and 0x16310 (+0x30).
// 1.13c D2Win+0x164D0
extern "C" uint32_t* d2win_164d0(uint32_t interval, uint32_t callback,
                                 uint32_t* (*alloc)(uint32_t size, uint32_t line), uint32_t* head,
                                 uint32_t (*tick)(), const uint32_t h[2]) {
    uint32_t* n = alloc(0x48, 0x1c);
    for (int i = 0; i < 0x12; ++i) n[i] = 0;
    n[0] = 8;
    n[2] |= 1u << 0;                 // gdwBitMasks[0]
    n[2] |= 1u << 2;                 // gdwBitMasks[2]
    n[0x10] = interval;
    n[0x11] = tick();
    n[0xd] = callback;
    n[8] = h[0]; n[0xc] = h[1];
    n[0xf] = 0;
    if (!*head) { *head = (uint32_t)(uintptr_t)n; return n; }
    uint32_t* p = (uint32_t*)(uintptr_t)*head;
    while (p[0xf]) p = (uint32_t*)(uintptr_t)p[0xf];
    p[0xf] = (uint32_t)(uintptr_t)n;
    return n;
}
