// src/core/d2win/d2win_sub_10c50.cpp -- 1.13c D2Win+0x10C50: the type-2 widget constructor.
// fastcall(ecx, edx) + eight stack words (ret 0x20). Nodes keep the retail 0x60-byte layout with 32-bit links (+0x3c next).
#include <cstdint>

// alloc: the Fog allocator (thunk 0x7678); head: the widget list head DAT 0x214A0; tick: GetTickCount;
// h: the module's handlers 0x10A50, 0x10740, 0x107C0.
// 1.13c D2Win+0x10C50
extern "C" uint32_t* d2win_sub_10c50(uint32_t a1, uint32_t a2, const uint32_t s[8],
                                     uint32_t* (*alloc)(uint32_t size, uint32_t line), uint32_t* head, uint32_t (*tick)(),
                                     const uint32_t h[3]) {
    uint32_t* n = alloc(0x60, 0x1c);
    for (int i = 0; i < 0x18; ++i) n[i] = 0;
    n[0] = 2;
    n[2] |= 1u << 0;                 // gdwBitMasks[0]
    n[2] |= 1u << 2;                 // gdwBitMasks[2]
    n[1] = s[2]; n[3] = a1; n[5] = s[0]; n[4] = a2; n[6] = s[1];
    n[7] = h[0]; n[9] = h[1]; n[0xd] = s[4]; n[0x10] = s[5]; n[0x11] = s[3];
    n[0x12] = tick();
    if (s[5]) n[0xe] = h[2];
    n[0x15] = s[6] ? s[6] : 5;
    n[0x16] = s[7]; n[0x17] = 1;
    if (!*head) { *head = (uint32_t)(uintptr_t)n; return n; }
    uint32_t* p = (uint32_t*)(uintptr_t)*head;
    while (p[0xf]) p = (uint32_t*)(uintptr_t)p[0xf];
    p[0xf] = (uint32_t)(uintptr_t)n;
    return n;
}
