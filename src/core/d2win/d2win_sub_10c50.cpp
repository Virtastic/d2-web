// src/core/d2win/d2win_sub_10c50.cpp -- 1.13c D2Win+0x10C50, transcribed from the cached Ghidra decompile.
// Stateful (appends to the control list at DAT_6f9014a0); EBP/return-address inputs are caller state.
#include <cstdint>
#include <cstdlib>
#include <ctime>

struct d2win_node10 { uint32_t f[0x18]; };
static d2win_node10* g_head10;   // DAT_6f9014a0

// 1.13c D2Win+0x10C50
extern "C" d2win_node10* d2win_sub_10c50(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5,
                                         uint32_t a6, uint32_t a7, uint32_t a8, uint32_t ebp, uint32_t retaddr, uint32_t tick) {
    d2win_node10* n = (d2win_node10*)calloc(1, sizeof *n);
    n->f[0] = 2;
    n->f[2] = 1u << 0 | 1u << 2;     // gdwBitMasks[0] | gdwBitMasks[2]
    n->f[1] = a3; n->f[3] = ebp; n->f[5] = a1; n->f[4] = a2; n->f[6] = retaddr;
    n->f[7] = 0x6f8f0a50; n->f[9] = 0x6f8f0740;
    n->f[0xd] = a5; n->f[0x10] = a6; n->f[0x11] = a4;
    n->f[0x12] = tick;               // GetTickCount()
    if (a6) n->f[0xe] = 0x6f8f07c0;
    n->f[0x15] = a7 ? a7 : 5;
    n->f[0x16] = a8; n->f[0x17] = 1;
    if (!g_head10) { g_head10 = n; return n; }
    d2win_node10* p = g_head10;
    while (p->f[0xf]) p = (d2win_node10*)(uintptr_t)p->f[0xf];   // tail link +0x3c
    p->f[0xf] = (uint32_t)(uintptr_t)n;
    return n;
}
