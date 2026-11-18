// src/core/d2win/d2win_ec70.cpp -- 1.13c D2Win+0xEC70: the type-10 widget constructor.
// fastcall(ecx, edx) + two stack words (ret 8). Nodes keep the retail 0x44-byte layout with 32-bit links (+0x3c next).
#include <cstdint>

// alloc: the Fog allocator (thunk 0x7678: ecx = size, edx = source file, stack = line, 0); head: the list head DAT 0x214A0;
// handler: the D2Win+0xEBE0 widget handler as the module addresses it.
// 1.13c D2Win+0xEC70
extern "C" uint32_t* d2win_sub_ec70(uint32_t a1, uint32_t a2, uint32_t s0, uint32_t s1,
                                    uint32_t* (*alloc)(uint32_t size, uint32_t line), uint32_t* head, uint32_t handler) {
    uint32_t* n = alloc(0x44, 0x1a);
    for (int i = 0; i < 0x11; ++i) n[i] = 0;
    n[0] = 10;
    n[2] |= 1u << 0;                 // gdwBitMasks[0]
    n[2] |= 1u << 2;                 // gdwBitMasks[2]
    n[5] = s0; n[1] = 0; n[3] = a1; n[4] = a2; n[6] = s1; n[7] = handler;
    if (!*head) { *head = (uint32_t)(uintptr_t)n; return n; }
    uint32_t* p = (uint32_t*)(uintptr_t)*head;
    while (p[0xf]) p = (uint32_t*)(uintptr_t)p[0xf];
    p[0xf] = (uint32_t)(uintptr_t)n;
    return n;
}
