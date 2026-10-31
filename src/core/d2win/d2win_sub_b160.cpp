// src/core/d2win/d2win_sub_b160.cpp -- 1.13c D2Win+0xB160: the type-5 (scrollbar) widget constructor.
// fastcall(ecx, edx) + four stack words (ret 0x10). Nodes keep the retail 0x60-byte layout with 32-bit links (+0x3c next).
#include <cstdint>

// alloc: the Fog allocator (thunk 0x7678); head: the widget list head DAT 0x214A0; h: the module's handlers 0xAF40, 0xAD60, 0xAB50.
// 1.13c D2Win+0xB160
extern "C" uint32_t* d2win_sub_B160(uint32_t a1, uint32_t a2, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3,
                                    uint32_t* (*alloc)(uint32_t size, uint32_t line), uint32_t* head, const uint32_t h[3]) {
    uint32_t* n = alloc(0x60, 0x1c);
    for (int i = 0; i < 0x18; ++i) n[i] = 0;
    n[0] = 5;
    n[2] |= 1u << 0;                 // gdwBitMasks[0]
    n[2] |= 1u << 2;                 // gdwBitMasks[2]
    n[1] = s2; n[6] = s1; n[5] = s0; n[3] = a1; n[4] = a2;
    n[7] = h[0]; n[9] = h[1]; n[10] = h[2]; n[0xd] = s3;
    if (!*head) { *head = (uint32_t)(uintptr_t)n; return n; }
    uint32_t* p = (uint32_t*)(uintptr_t)*head;
    while (p[0xf]) p = (uint32_t*)(uintptr_t)p[0xf];
    p[0xf] = (uint32_t)(uintptr_t)n;
    return n;
}
