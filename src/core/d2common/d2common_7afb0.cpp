// 1.13c D2Common+0x7AFB0 -- A* neighbour evaluation. EBX = current node; stack: unused word, search context, packed new point
// (X low16, Y high16), packed target. Node (0x38 bytes): +0 point, +4 f (i16), +6 h (i16), +8 g (i16), +0xC best parent,
// +0x10..+0x2C eight child slots. Context: nodes from ctx+0x404 (stride 0x38), used count at ctx+0x2FC4 (cap 200).
// The lookups, the distance estimate, the hash insert (ESI = node, EDI = ctx) and the resort (EAX, EDX = node) are seams.
#include <cstdint>
#include <cstring>

struct D2Common7AFB0Env {
    uint32_t (*find1)(uint32_t ctx, uint32_t point);
    uint32_t (*find2)(uint32_t ctx, uint32_t point);
    int32_t (*dist)(uint32_t point, uint32_t target);
    void (*hashIn)(uint32_t ediCtx, uint32_t esiNode);
    void (*resort)(uint32_t eaxCtx, uint32_t edxNode);
};

namespace {
inline uint32_t* P32(uint32_t a) { return reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(a)); }
inline int16_t* P16(uint32_t a) { return reinterpret_cast<int16_t*>(static_cast<uintptr_t>(a)); }
// first empty child slot of cur; returns false when all eight are used
bool link(uint32_t cur, uint32_t node, uint32_t* last) {
    for (uint32_t o = 0x10; o <= 0x2C; o += 4) {
        uint32_t* s = P32(cur + o);
        if (*s == 0) { *s = node; if (last) *last = 0; return true; }
    }
    if (last) *last = *P32(cur + 0x2C);
    return false;
}
}  // namespace

extern "C" uint32_t d2_port_d2common_7afb0(uint32_t cur, uint32_t ctx, uint32_t tNew, uint32_t tTarget, const D2Common7AFB0Env* E) {
    const int16_t* c = P16(cur);
    int16_t step = 3;
    if (c[0] == (int16_t)tNew || c[1] == (int16_t)(tNew >> 16)) step = 2;
    const int16_t g = (int16_t)(c[4] + step);
    uint32_t n = E->find1(ctx, tNew);
    if (n) {
        link(cur, n, nullptr);
        int16_t* h = P16(n);
        if (g < h[4]) { h[4] = g; *P32(n + 0xC) = cur; h[2] = (int16_t)(h[3] + g); }
        return 1;
    }
    n = E->find2(ctx, tNew);
    if (n) {
        uint32_t eax;
        link(cur, n, &eax);
        int16_t* h = P16(n);
        if (g < h[4]) {
            h[2] = (int16_t)(h[3] + g);
            *P32(n + 0xC) = cur;
            h[4] = g;
            E->resort(eax, n);
        }
        return 1;
    }
    const uint32_t cnt = *P32(ctx + 0x2FC4);
    if (cnt == 200) return 0;
    const uint32_t nn = ctx + 0x404 + cnt * 0x38;
    *P32(ctx + 0x2FC4) = cnt + 1;
    memset(P32(nn), 0, 0x38);
    *P32(nn + 0xC) = cur;
    P16(nn)[4] = g;
    const int16_t hd = (int16_t)E->dist(tNew, tTarget);
    P16(nn)[3] = hd;
    *P32(nn) = tNew;
    P16(nn)[2] = (int16_t)(hd + g);
    E->hashIn(ctx, nn);
    link(cur, nn, nullptr);
    return 1;
}
