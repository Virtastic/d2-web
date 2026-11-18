// src/core/d2win/d2win_sub_fd70.cpp -- 1.13c D2Win+0xFD70: the type-6 (text box) widget constructor.
// fastcall(ecx, edx) + nine stack words (ret 0x24). Nodes keep the retail 0x274-byte layout with 32-bit links (+0x3c next).
#include <cstdint>

// alloc: the Fog allocator (thunk 0x7678); head: the widget list head DAT 0x214A0; text: thunk 0x7780 (the initial text,
// copied by D2Lang Unicode::strcpy); h: the module's handlers 0xFA80, 0xF4E0, 0xF410, 0xF350.
// 1.13c D2Win+0xFD70
extern "C" uint32_t* d2win_sub_FD70(uint32_t a1, uint32_t a2, const uint32_t s[9],
                                    uint32_t* (*alloc)(uint32_t size, uint32_t line), uint32_t* head,
                                    const uint16_t* (*text)(), const uint32_t h[4]) {
    uint32_t* n = alloc(0x274, 0x24);
    for (int i = 0; i < 0x9d; ++i) n[i] = 0;
    n[0] = 6;
    n[2] |= 1u << 0;                 // gdwBitMasks[0]
    n[2] |= 1u << 2;                 // gdwBitMasks[2]
    n[0xc] = s[4] ? h[3] : 0;
    n[1] = s[2]; n[0x10] = s[6]; n[3] = a1; n[4] = a2; n[0xd] = s[3]; n[5] = s[0]; n[6] = s[1];
    n[7] = h[0]; n[9] = h[1]; n[10] = h[2];
    n[0x14] = s[4]; n[0x17] = s[5];
    if (s[7]) {
        const uint16_t* src = text(); uint16_t* dst = (uint16_t*)(n + 0x19);
        while ((*dst++ = *src++) != 0) {}
    }
    const uint32_t cells = (uint32_t)(((int32_t)s[1] + 0xff) >> 8) * (uint32_t)(((int32_t)s[0] + 0xff) >> 8);   // sar, imul
    n[0x15] = cells;
    n[0x16] = (n[0x10] & 0x20) ? cells * 2 : 0;
    n[0x9a] = s[8];
    if (!*head) { *head = (uint32_t)(uintptr_t)n; return n; }
    uint32_t* p = (uint32_t*)(uintptr_t)*head;
    while (p[0xf]) p = (uint32_t*)(uintptr_t)p[0xf];
    p[0xf] = (uint32_t)(uintptr_t)n;
    return n;
}
