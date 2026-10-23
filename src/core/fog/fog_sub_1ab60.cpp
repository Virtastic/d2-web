// 1.13c Fog+0x1AB60: pool free-list node `n` (EAX) is moved to the front of list `l` (EDX)
// when its next node's size (+8) is >= l->+0x1C. Nodes are 32-bit-link records: +0xC next, +0x10 prev; list +0x28 head, +0x2C tail.
#include <cstdint>
static inline uint32_t& W(uint8_t* b, uint32_t o) { return *reinterpret_cast<uint32_t*>(b + o); }
static inline uint8_t* P(uint32_t v) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(v)); }
extern "C" void fog_sub_1ab60(uint8_t* n, uint8_t* l) {
    uint32_t nx = W(n, 0xC);
    if (nx == 0 || W(l, 0x1C) > W(P(nx), 8)) return;
    W(P(nx), 0x10) = W(n, 0x10);
    if (W(n, 0x10) == 0) W(l, 0x2C) = W(n, 0xC); else W(P(W(n, 0x10)), 0xC) = W(n, 0xC);
    W(n, 0x10) = W(l, 0x28);
    W(P(W(l, 0x28)), 0xC) = (uint32_t)(uintptr_t)n;
    W(l, 0x28) = (uint32_t)(uintptr_t)n;
    W(n, 0xC) = 0;
}
