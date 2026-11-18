// src/core/d2client/d2client_82fe0.cpp -- 1.13c D2Client+0x82FE0, stdcall(unit), ret 4.
#include "d2client_82fe0.h"
#include <cstring>
#include <emscripten.h>

namespace {
uint32_t rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
constexpr uint32_t kFile = 0xB2000000u | 0xD4964u;   // "..\Source\D2Client\UNIT\Item.cpp"
}

// 1.13c D2Client+0x82FE0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_82fe0(const D2c82fe0Env* e, uint32_t u) {
    if (!u) return;
    if (rd32(u) != 0 && !((rd32(u + 0xC4) >> 9) & 1)) return;
    const uint32_t inv = rd32(u + 0x60);
    if (!inv) return;
    uint8_t blk[0x1C];
    uint32_t changed = 0;
    e->f81b90(u, blk);
    // pass 1: equipped nodes whose item no longer qualifies are flagged off
    for (uint32_t n = e->c4f0(inv); n; n = e->c568(n)) {
        const uint32_t it = e->c526(n);
        if (it && e->c994(n) == 1 && e->ca30(it, u) && e->c51a(it, 0) == 0 && e->f81c30(u, it)) {
            e->ca42(it, 0x4000, 0);
            e->f81ff0(u, it, 0);
        }
    }
    // pass 2: body slots that fail 0x81C30 are flagged on and re-applied
    for (uint32_t s = 0; s < 0xB; s++) {
        const uint32_t it = e->c262(inv, s);
        if (!it) continue;
        if (e->c1a8(it, 0x100, 0x203, kFile) && !e->c51a(it, 0)) continue;
        if (e->c1a8(it, 0x4000, 0x208, kFile) && !e->c51a(it, 0)) continue;
        if (e->f81c30(u, it)) continue;
        e->ca42(it, 0x4000, 1);
        const uint32_t r = (uint8_t)e->c9f4(u, it);
        e->f82610(u, r);
        changed = 1;
    }
    // pass 3: re-seat items that now meet their requirements, repeated while anything moved (at most 101 sweeps)
    uint32_t guard = 100, moved;
    do {
        moved = 0;
        for (uint32_t s = 0; s < 0xB; s++) {
            const uint32_t it = e->c262(inv, s);
            if (!it) continue;
            if (e->c1a8(it, 0x100, 0x226, kFile)) continue;
            if (!e->c1a8(it, 0x4000, 0x22A, kFile) && e->c51a(it, 0)) continue;
            if (!e->c898(it, u, 0, 0, 0, 0)) continue;
            if (e->c928(it)) {
                const uint32_t inv2 = rd32(u + 0x60);
                uint32_t o = e->c262(inv2, 4);
                if (o == it) o = e->c262(inv2, 5);
                if (!o) continue;
                if (!e->c1cc(o, e->c928(it))) continue;
            }
            e->ca42(it, 0x4000, 0);
            const uint32_t r = (uint8_t)e->c9f4(u, it);
            const uint32_t inv3 = rd32(u + 0x60);
            const uint32_t mode = rd32(it + 0x10);
            if (inv3) {
                const uint8_t c = (uint8_t)e->ca54(it);
                if (c != 0xB && c != 0xC) {
                    e->ca2a(inv3, it);
                    e->f69a90(r, u);
                    if (mode == 1 && e->c51a(it, 0) == 0) e->f81ff0(u, it, 1);
                    e->ca18(inv3, it);
                }
            }
            moved = 1;
            changed = 1;
        }
    } while (moved && guard-- != 0);
    // pass 4: kind-5 items in a body slot that pass neither check are dropped
    for (uint32_t s = 0; s < 0xB; s++) {
        const uint32_t it = e->c262(inv, s);
        if (!it || e->c19c(it) != 5) continue;
        if (e->c1a8(it, 0x100, 0x247, kFile) || e->c1a8(it, 0x4000, 0x248, kFile)) continue;
        if (rd32(u) == 0) e->ca0c(rd32(u + 0x60));
        e->c1a8(it, 0x100, 0x96, kFile);
        e->c544(u, it);
        e->f81ff0(u, it, 0);
    }
    e->f82410(u, blk);
    if (!changed) return;
    if (rd32(u) != 0) { e->f4dc70(7, u, 0); return; }
    if (!(rd32(u + 0xC4) & 0x10000)) {
        const uint32_t m = rd32(u + 0x10);
        if (m != 0 && m != 0x11) e->f19830(7, u, 0, 1);
    }
    e->f6a220(u, 0);
}
