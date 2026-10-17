// 1.13c D2Game+0x6F930: the missile-from-request creator (D2MOO MISSILES_CreateMissileFromParams),
// written from the decompile + disassembly as a rule over an environment E (src/core/fn/hookenv.h). Every D2Common ordinal and every
// not-yet-mapped D2Game callee is one E::h(rva, eax, ecx, edx, ebx, esi, stack...) call; fields go through E::rd*/wr32 on retail-layout
// (32-bit) addresses. Register-convention: ecx = pGame, edx = request; returns the missile unit (0 on any refusal).
// ponytail: the 1.10f->1.13c deltas vs the vendored MISSILES_CreateMissileFromParams are listed in the spec; production still runs the vendored one.
#pragma once
#include <cstdint>

namespace m6f930 {
template <class E> static inline uint32_t S(E& e, uint32_t rva, uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0)
{ return e.h(rva, 0, 0, 0, 0, 0, s0, s1, s2, s3); }
static inline int32_t Abs(uint32_t v) { int32_t d = (int32_t)v; return d < 0 ? (int32_t)(0u - (uint32_t)d) : d; }

// bm = &gdwBitMasks (byte +0xC = CANSLOW, +0x10 = CANDESTROY); ct = the 8-byte-stride collision table at D2Game+0x10D92C.
template <class E> uint32_t Create(E& e, uint32_t G, uint32_t R, uint32_t bm, uint32_t ct)
{
    const uint32_t FILE = 0xB20FBD64;   // "..\Source\D2Game\MISSILES\Missiles.cpp", recorded as a module word
    auto q = [&](int i) { return e.rd32(R + i * 4); };
    const uint32_t own = q(1), flags = q(0);
    if (!own) return 0;
    const int32_t mid = (int32_t)q(4);
    if (mid < 0) return 0;
    if (mid >= (int32_t)e.rd32(e.rd32(e.rd32(e.dtPtr)) + 0xB6C)) return 0;
    const uint32_t ot = e.rd32(own);
    if (ot != 0 && ot != 1) return 0;
    const uint32_t rec = e.h(0x1250, mid);
    if (!rec) return 0;
    uint32_t x, y;
    if (flags & 1) { x = q(5); y = q(6); }
    else {
        const uint32_t org = q(2);
        if (!org) return 0;
        x = e.h(0x1100, 0, org); y = e.h(0x1130, 0, org);
    }
    const uint32_t room0 = S(e, 0xA478, own);
    const uint32_t room = e.h(0x1280, room0, 0, 0, x);
    if (!room) return 0;
    uint32_t tx = x, ty = y;
    if (flags & 2) { tx = x + q(7); ty = y + q(8); }
    else if (flags & 0x20) { tx = q(7); ty = q(8); }
    int32_t vel;
    if (!(flags & 4)) {
        const int32_t p = (int32_t)((uint32_t)e.rd8(rec + 0x9B) * q(12));
        vel = (int32_t)(((uint32_t)(((p + ((p >> 31) & 7)) >> 3)) + e.rd8(rec + 0x9A)) << 8);
    } else { vel = (int32_t)q(10); if (!(flags & 0x10)) vel = (int32_t)((uint32_t)vel << 8); }
    if (e.rd8(rec + 4) & e.rd8(bm + 0xC)) {
        if (S(e, 0xA68E, own, 0x57)) {
            const uint32_t sl = S(e, 0xA5D4, own, 0x57);
            if (sl) vel = (int32_t)((uint32_t)S(e, 0xA712, sl, 0xA1, 0) * (uint32_t)vel) / 100;
        }
    }
    if (vel) vel = (int32_t)((uint32_t)vel * 0x4B) / 100;
    uint32_t tgt = q(3);
    if (vel) {
        if (!tgt) { if (tx == x && ty == y) { tx++; ty++; } }
        else if (tgt != own) {
            const uint32_t a = e.h(0x1100, 0, own), b = e.h(0x1100, 0, tgt);
            if (a == b) {
                const uint32_t c = e.h(0x1130, 0, tgt), d = e.h(0x1130, 0, own);
                if (c == d) { tx++; ty++; tgt = 0; }
            }
        }
        uint32_t cx = tx, cy = ty;
        if (tgt) { cx = e.h(0x1100, 0, tgt); cy = e.h(0x1130, 0, tgt); }
        if (Abs(cx - x) >= 100) return 0;
        if (Abs(cy - y) >= 100) return 0;
    }
    const uint32_t M = S(e, 0xA6B8, e.rd32(G + 0x1C), 3);
    if (!M) return 0;
    e.wr32(M, 3); e.wr32(M + 4, (uint32_t)mid); e.wr32(M + 8, e.rd32(G + 0x1C)); e.wr32(M + 0x80, G);
    uint32_t f = e.rd32(M + 0xC8);
    f = e.rd32(G + 0x70) ? (f | 0x2000000u) : (f & 0xFDFFFFFFu);
    e.wr32(M + 0xC8, f | 0x4000000u);
    const uint32_t act = S(e, 0xA56E, S(e, 0xA556, room)) & 0xFF;
    e.wr32(M + 0x18, (e.rd32(M + 0x18) & ~0xFFu) | act);
    e.wr32(M + 0x1C, e.rd32(G + 0xBC + act * 4));
    e.wr32(M + 0x74, 0);
    e.h(0xE0110, 0, 0, 0, 0, G + 0xD0);
    e.wr32(M + 0xC4, e.rd32(M + 0xC4) | 0x10);
    e.wr32(M + 0xD0, 0xB);
    int32_t cnt = (int32_t)e.rd32(G + 0x9C) + 1;
    if (cnt == -1) cnt = 1;
    e.wr32(G + 0x9C, (uint32_t)cnt);
    e.wr32(M + 0x10, e.rd8(rec + 0x183));
    e.h(0x6F8A0, M, 0, 0, 0, 0, (uint32_t)cnt);
    e.h(0xE0F40, room, y, M, 0, 0, x, G, 1);
    int32_t life;
    if (flags & 0x8000) life = (int32_t)q(0x13);
    else {
        life = (int32_t)((uint32_t)e.rd16s(rec + 0x98) * q(12) + (uint32_t)e.rd16s(rec + 0x96));
        if ((flags & 8) && e.rd8(rec + 0x180))
            life += (int32_t)((uint32_t)(e.rd8(rec + 0x182) - e.rd8(rec + 0x181)) * q(13));
    }
    if (flags & 0x200) { life -= (int32_t)q(0x10); e.wr32(M + 0x44, q(0x10) << 8); }
    S(e, 0xAD42, M, (uint32_t)life);
    S(e, 0xAC7C, M, (uint32_t)life);
    const uint32_t actf = (flags & 0x800) ? q(0x11) : e.rd8(rec + 0x136);
    S(e, 0xB120, M, (uint32_t)life - actf);
    const uint32_t P = e.rd32(M + 0x2C);
    if (!P) return 0;
    S(e, 0xACAC, P, 0, FILE, 0x167);
    if (tgt) S(e, 0xA5FE, P, tgt); else S(e, 0xAB2C, P, tx, ty);
    S(e, 0xA682, P, e.rd8(rec + 0x184) ? 0x40 : 0);
    S(e, 0xAB08, P, e.rd32(ct + e.rd8(rec + 0x183) * 8));
    if (e.rd8(rec + 4) & e.rd8(bm + 0x10)) e.wr32(M + 0xC4, e.rd32(M + 0xC4) | 4);
    if (vel) { S(e, 0xACAC, P, (uint32_t)vel, FILE, 0x183); S(e, 0xAAF0, P, M, 0); }
    S(e, 0xB0F0, M, own);
    S(e, 0xB12C, P, (uint32_t)e.rd16s(rec + 0x9E));
    S(e, 0xB0DE, P, e.rd8(rec + 0x9C) << 8);
    if (flags & 0x400) {
        uint32_t d = S(e, 0xAB80, M, tx, ty);
        if (!d) d = 1;
        uint32_t cur = 0;
        if ((uint32_t)vel << 4) cur = (d << 16) / ((uint32_t)vel << 4);
        S(e, 0xAC7C, M, cur);
    }
    S(e, 0xADAE, M, 0, 0, 0);
    if (q(0x15)) e.h(e.handlerRva(q(0x15)), 0, M, q(0x16));
    S(e, 0xB126, M, q(0xB));
    S(e, 0xB11A, M, q(0xC));
    e.h(0x6F620, own, 0, 0, 0, 0, q(2));
    S(e, 0xAC76, M, own);
    e.wr32(M + 0xC4, e.rd32(M + 0xC4) & ~8u);
    e.h(0x6F660, own, 0, 0, 0, 0, M);
    if (flags & 0x1000) S(e, 0xA472, M, 0x13, q(0x12), 0);
    if (flags & 0x10000) { const uint32_t v = S(e, 0xB108, M); S(e, 0xB0F6, M, v | 2); }
    e.h(0x21880, M, 0, own, 0, 0, G);
    S(e, 0xA472, M, 0x147, e.rd32(rec + 0x19C), 0);
    return M;
}
}  // namespace m6f930
