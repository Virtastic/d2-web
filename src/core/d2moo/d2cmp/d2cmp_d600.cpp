// src/core/d2cmp/d2cmp_d600.cpp -- 1.13c D2CMP sub_D600, transcribed from the disassembly.
#include <cstdint>

struct D2CmpD600Env {
    void (*hook)(uint32_t a, uint32_t b, uint32_t c);   // DAT_6fe87f04, called with ECX=[n20], EDX=[n20+8], stack=[n20+4]; 0 = none
    uint32_t pool;                                      // DAT_6fe77240, passed in ECX to the free
    void (*freeNode)(uint32_t pool, uint32_t node);     // 0x16D60 on &DAT_6fe772b0
    void (*fatal)(uint32_t code);                       // Ordinal_10265 / Ordinal_10024 / _exit(-1)
};

static inline uint32_t& W(uint32_t n, uint32_t o) { return *(uint32_t*)((uintptr_t)n + o); }

// ECX = cache node. Unlinks the node from its doubly linked list (+0x28 prev, +0x2C next) and frees it.
// 1.13c D2CMP+0x0D600
void d2cmp_d600(uint32_t node, const D2CmpD600Env* e) {
    if (e->hook) {
        uint32_t n20 = W(node, 0x20);
        if (!n20) { e->fatal(0xb3); return; }
        e->hook(W(n20, 0), W(n20, 8), W(n20, 4));
    }
    if (!node) { e->fatal(0x141); return; }
    uint32_t prev = W(node, 0x28), next = W(node, 0x2c);
    if (prev) W(prev, 0x2c) = next;
    if (next) W(next, 0x28) = prev;
    e->freeNode(e->pool, node);
}
