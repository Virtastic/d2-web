// src/core/d2win/d2win_ec70.cpp -- 1.13c D2Win+0xEC70, transcribed from the cached Ghidra decompile.
// NAMED only: stateful (appends to the list at DAT_6f9014a0), and two inputs (EBX, the return address)
// are caller state the decompiler could not name, so no stateless probe/vector.
#include <cstdint>
#include <cstdlib>

struct d2win_node { uint32_t f[0x11]; };   // 0x44 bytes, dword fields as in retail
static d2win_node* g_head;                 // DAT_6f9014a0

// 1.13c D2Win+0xEC70
extern "C" d2win_node* d2win_sub_ec70(uint32_t a1, uint32_t a2, uint32_t ebx, uint32_t retaddr) {
    d2win_node* n = (d2win_node*)calloc(1, sizeof *n);   // Ordinal_10042(0x1a,0) allocation, zeroed
    n->f[0] = 10;
    n->f[2] = 1u << 0 | 1u << 2;   // gdwBitMasks[0] | gdwBitMasks[2]
    n->f[3] = a1; n->f[4] = a2; n->f[5] = ebx; n->f[6] = retaddr;
    n->f[7] = 0x6f8eebe0;          // callback LAB_6f8eebe0
    if (!g_head) { g_head = n; return n; }
    d2win_node* p = g_head;
    while (p->f[0xf]) p = (d2win_node*)(uintptr_t)p->f[0xf];   // tail link at +0x3c
    p->f[0xf] = (uint32_t)(uintptr_t)n;
    return n;
}
