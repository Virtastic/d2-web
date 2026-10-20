// D2Game 1.13c batch 5D5C0..5E660 (A5Q4 quest object and its helpers). Not compiled against a test runner yet.
// Rule over an environment E (same shape as src/core/fn/hookenv.h plus wr8, mod): every callee is one E::h(rva, ...)
// call, memory goes through E::rd32/rd8/wr32/wr8 on the retail layout, mod(rva) is the address of a DLL data word.
// Import thunks: A2E6 Fog#10045 alloc, A2C2 Fog#10024 fatal log, A2DA D2Common#10265 string, 2A3C _exit,
// A4C0 #10920, A568 #10174, A496 #11103, A4A2 #11090, A478 #10331, A556 #10826, A68E #10494, A44E #10885,
// A550 #10408, A63A #11016, A51A #10346, A520 #10688.
// UNVERIFIED seams (callee argument order/registers not proven against the thunks): every h() argument list below,
// and callees 33EB0, 337D0, E660, 75E10, 48410, 79180, DFCC0, E2240, 1EC40, 8EA80, 5DD00.
// The EBX/EDI inputs of 5E000 are taken as (game=param_1 of 5E0E0, id=the slot id) from the 5E0E0 caller: unverified.
#include <cstdint>

namespace b5d5c0 {
typedef uint32_t u32;

template <class E> static void Fatal(E& e) { u32 s = e.h(0xA2DA, 0x2d3); e.h(0xA2C2, 0, s); e.h(0x2A3C, 0xFFFFFFFFu); }

// find a unit/record in the hash at base+off by id (chain link at +0xE4 for 0x1520 tables, node id at +0xC)
template <class E> static u32 Find(E& e, u32 base, u32 off, u32 id)
{
    for (u32 n = e.rd32(base + off + (id & 0x7f) * 4); n; n = e.rd32(n + 0xE4)) if (e.rd32(n + 0xC) == id) return n;
    return 0;
}
// first entry of the type list at game+0x10F4 with type t (link at +0xF4)
template <class E> static u32 FindType(E& e, u32 game, u32 t)
{
    u32 g = e.rd32(game + 0x10F4);
    if (!g) return 0;
    for (u32 n = e.rd32(e.rd32(g)); n; n = e.rd32(n + 0xF4)) if (e.rd32(n) == t) return n;
    return 0;
}

// 1.13c D2Game+0x5D5C0 (ECX=quest record)
template <class E> void A5Q4Init(E& e, u32 q)
{
    for (u32 i = 0; i < 15; i++) e.wr32(q + 0xA0 + i * 4, 0);
    e.wr8(q + 0xC, 0);
    e.wr32(q + 0xA0, 0x77A60); e.wr32(q + 0xC8, 0x77A20); e.wr32(q + 0xAC, 0x7AD50); e.wr32(q + 0xC0, 0x7B690);
    e.wr32(q + 0xA8, 0x7D520); e.wr32(q + 0xCC, 0x7B7A0); e.wr32(q + 0xD4, 0x7AE20); e.wr32(q + 0xDC, e.mod(0x12C488));
    e.wr8(q + 0xA, 1);
    u32 p = e.h(0xA2E6, 0, 0x30d, 0);   // "..\Source\D2Game/Quests/a5q4.cpp", line 0x30d, flag 0
    e.wr32(q + 0x18, p);
    e.wr32(q + 0xE0, 0x26);
    e.wr32(q + 0xE8, 0x768C0);
    e.wr8(q + 0xD, 4);
    e.wr32(q + 0xEC, 0x76840);
    e.wr32(q + 0xF0, 0x7AFB0);
    e.wr32(q + 0x10, 0x23);
    for (u32 i = 0; i < 0x24; i++) e.wr32(p + i * 4, 0);
    e.wr8(p + 0x80, 0); e.wr8(p + 0x81, 0);
}

// 1.13c D2Game+0x5DA70 (ECX=quest record, EDX=message)
template <class E> void A5Q4Msg(E& e, u32 q, u32 msg)
{
    u32 blk[4] = { 0xC, 0, 0, 0 };
    const u32 E0 = e.rd32(q + 0xE0);
    u32 st = e.rd32(msg + 0xC); (void)st;
    u32 unit = e.rd32(q + 4);
    u32 t = e.h(0xA4C0);
    u32 s = e.rd32(t + 0x10 + e.rd8(unit + 0x6d) * 4), s5 = s;
    e.h(0xA568, s, E0, 0);
    u32 id = E0;
    e.h(0xA568, s, id, 0xd);
    u32 tgt = e.rd32(msg + 8);
    int32_t type = tgt ? (int32_t)e.rd32(tgt + 4) : -1;
    u32 d = e.rd32(q + 0x18);
    const u32 m5 = e.rd32(msg + 0x14);
    if (type == 0x20f) {
        if (e.rd8(q + 0xC) == 6) { e.h(0xA496, tgt, 0); e.h(0x75E10, e.rd32(msg), e.stk(blk)); }
        return;
    }
    bool done = false;
    if (type == 0x201) {
        if (e.rd8(q + 0xC) == 4) {
            if (e.h(0xE660, 0x20656369)) return;
            if (e.rd32(d + 0xD8)) return;
            e.h(0x33EB0, q, m5, 0x201);
            return;
        }
        if ((e.h(0xA568, s, E0, 0) || e.h(0xA568, s, E0, 0xF)) && e.rd8(d + 0x102) == 0 &&
            e.h(0xA568, s, 0x25, 8) && !e.h(0xA568, s, E0, 7) && !e.h(0xE660, 0x20327274) && !e.h(0xA568, s, 0x25, 8))
            e.h(0x33EB0, q, m5, 0x201);
    }
    (void)done;
    if (e.h(0xA568, s, E0, 1) == 0) {
        if (e.h(0x337D0, q) == 1) { e.h(0x33EB0, q, m5, id); return; }
        if (s5 == 1) return;
        if (e.rd8(q + 0xC) > 4 && type == 0) return;
        if (e.rd8(q + 9) == 0) return;
        u32 v = e.rd32(e.mod(0x12C468) + e.rd8(q + 0xC) * 4);
        if (v == 0xFFFFFFFFu || v > 9) return;
        e.h(0x33EB0, q, m5, id);
        return;
    }
    if (id == 0x200 || id == 0x201) {
        if (e.h(0xA568, s, 0x25, id == 0x200 ? 9 : 8)) return;
    }
    e.h(0x33EB0, q, m5, id);
}

// 1.13c D2Game+0x5DDF0 (ECX=game, EDX=message)
template <class E> u32 A5Q4Pickup(E& e, u32 game, u32 msg)
{
    if (!msg || e.rd32(msg + 4) != 0x20f) return 0;
    u32 p = (e.rd32(msg) == 1 && e.rd32(msg + 0x14)) ? e.rd32(e.rd32(msg + 0x14) + 0x30) : 0;
    for (u32 n = e.rd32(p); n; n = e.rd32(n + 8))
        if (e.rd32(n)) { e.h(0x48410, game, msg, 0x79180); return 1; }
    u32 blk[4] = { 0, 0, 0, 0 };
    e.h(0xA496, msg, 0);
    e.h(0x75E10, game, e.stk(blk));
    if (u32 n = FindType(e, game, 0x21)) e.wr8(e.rd32(n + 0x18) + 0xE3, 1);
    return 1;
}

// 1.13c D2Game+0x5DEF0 (EAX=game)
template <class E> void A5Q4Tick(E& e, u32 game)
{
    u32 n = FindType(e, game, 0x21);
    if (!n) return;
    u32 g = e.rd32(n + 4);
    if (!g) { Fatal(e); return; }
    u32 id = e.rd32(e.rd32(n + 0x18) + 0x9C);
    for (u32 c = e.rd32(g + 0x1320 + (id & 0x7f) * 4); c; c = e.rd32(c + 0xE4))
        if (e.rd32(c + 0xC) == id) { e.h(0x5DD00, n); return; }
}

// 1.13c D2Game+0x5E000 (EBX=game, EDI=unit id)
template <class E> u32 A5Q4FreeSlot(E& e, u32 game, u32 id)
{
    if (!game) { Fatal(e); return 0; }
    u32 p = 0;
    for (u32 c = e.rd32(game + 0x1320 + (id & 0x7f) * 4); c; c = e.rd32(c + 0xE4)) if (e.rd32(c + 0xC) == id) { p = c; break; }
    if (!p) return 0;
    if (e.h(0xA68E, p, 0x36)) e.h(0xE2240, p, 0);
    u32 r = 0;
    if (e.h(0xA478, p)) {
        u32 blk[4] = { 0xC, p, 0, 0 };
        e.h(0xA496, p, 0);
        e.h(0x75E10, game, e.stk(blk));
        r = e.h(0xA478, p, 1);
        e.h(0xA44E, r);
        e.h(0xA550, p);
        r = e.h(0xA63A, p);
    }
    return (r & 0xFFFFFF00u) | 1;
}

// 1.13c D2Game+0x5E0E0 (game, unit, quest data)
template <class E> void A5Q4Reset(E& e, u32 game, u32 unit, u32 q)
{
    for (u32 i = 0; i < 3; i++) {
        if (e.rd8(q + 0x2C + i)) {
            if (((uint8_t)A5Q4FreeSlot(e, game, e.rd32(q + 0x14 + 4 * i))) == 0) e.h(0x1EC40, i + 0x21C);
            else { e.wr32(q + 0x40, e.rd32(q + 0x40) - 1); e.wr32(q + 0x44, e.rd32(q + 0x44) - 1); }
            e.wr8(q + 0x2C + i, 0);
        }
    }
    e.wr8(q + 0x48, 0);
    for (u32 i = 0; i < 3; i++) {
        e.wr8(q + 0x3C + i, 0);
        u32 g = e.rd32(unit + 4);
        if (!g) { Fatal(e); return; }
        if (u32 n = Find(e, g, 0x1520, e.rd32(q + 0x14 + 4 * i))) e.h(0xA4A2, n, 0);
        e.wr32(q + 0x30 + 4 * i, 0);
    }
    e.wr32(q + 0x4C, 0);
    if (!game) { Fatal(e); return; }
    if (u32 n = Find(e, game, 0x1520, e.rd32(q + 0x50))) e.h(0xA4A2, n, 0);
}

// 1.13c D2Game+0x5E200 (ECX=game)
template <class E> void A5Q4Exit(E& e, u32 game)
{
    u32 u = e.h(0xA478, 0);
    if (!u || e.h(0xA556, u) != 0x78) return;
    u32 n = FindType(e, game, 0x23);
    if (!n) return;
    u32 q = e.rd32(n + 0x18);
    if (e.rd8(n + 9) && e.rd32(q + 0xC) == 0 && e.rd8(q) == 0 && e.rd8(q + 0x11)) {
        e.wr32(q + 0x54, 0);
        e.h(0xDFCC0, 0, 0);
        if (e.rd32(q + 0x54) == 0) {
            e.wr8(e.rd32(n + 0x18) + 0x11, 0); e.wr8(e.rd32(n + 0x18) + 0x10, 0);
            A5Q4Reset(e, game, n, e.rd32(n + 0x18));
        }
    }
}

// 1.13c D2Game+0x5E560 (game, quest data)
template <class E> u32 A5Q4Arm(E& e, u32 game, u32 q)
{
    u32 ids[3];
    for (u32 i = 0; i < 3; i++) {
        u32 g = e.rd32(game + 4);
        if (!g) { Fatal(e); return 0; }
        ids[i] = Find(e, g, 0x1520, e.rd32(q + 0x14 + 4 * i));
        if (!ids[i]) return 0;
        e.h(0xA51A, e.h(0xA478, ids[i], 0));
    }
    for (u32 i = 0; i < 3; i++) {
        e.h(0xA4A2, ids[i], 3);
        e.h(0xA550, ids[i]);
        u32 g = e.rd32(game + 4);
        e.h(0x8EA80, g, ids[i], 7, e.rd32(g + 0xA8) + 0x14, 0, 0, 0);
        e.wr32(q + 0x30 + 4 * i, 3);
    }
    e.wr32(q + 0x40, 0); e.wr32(q + 0x44, 0);
    return 1;
}

// 1.13c D2Game+0x5E660 (ECX=game, EDX=unit)
template <class E> u32 A5Q4Release(E& e, u32 game, u32 unit)
{
    u32 d = e.rd32(unit + 0x18);
    bool bad = false;
    if (e.rd32(d + 0xC) || e.rd8(d + 0x11) == 0) { e.wr8(d + 4, 0); return 1; }
    for (u32 i = 0; i < 3; i++) {
        if (!e.rd8(d + 0x3C + i)) continue;
        u32 g = e.rd32(unit + 4);
        if (!g) { Fatal(e); return 0; }
        if (u32 n = Find(e, g, 0x1520, e.rd32(d + 0x14 + 4 * i))) {
            e.h(0xA4A2, n, 1);
            u32 rec = e.h(0xA520, e.rd32(n + 4));
            e.h(0x8EA80, game, n, 1, (e.rd32(rec + 0xDC) >> 8) + e.rd32(game + 0xA8), 0, 0, 0);
            e.wr8(d + 0x3C + i, 0);
            e.wr32(d + 0x30 + 4 * i, 2);
        } else bad = true;
    }
    if (bad) return 0;
    e.wr8(d + 4, 0);
    return 1;
}
}  // namespace b5d5c0
