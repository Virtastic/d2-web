// 1.13c D2Game+0x9EC80, +0x9FA30, +0x78550, +0xA1790, +0x9F850: the callees of sub_A19A0, written from the
// decompile as rules over an environment E. Every D2Common ordinal and every
// not-yet-mapped D2Game callee is one E::h(rva, regs, stack) call; field reads/writes go through E::rd*/wr32 on
// retail-layout (32-bit) addresses. A probe's E logs and replays those calls (src/core/fn/hookenv.h).
// ponytail: no production E yet (the D2Common ordinals of 1.13c are not named); wire when sub_A19A0 is ported.
#pragma once
#include <cstdint>

namespace a19a0 {
template <class E> static inline uint32_t S(E& e, uint32_t rva, uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0)
{ return e.h(rva, 0, 0, 0, 0, 0, s0, s1, s2, s3); }

// 0x9EC80: ebx = pUnit. Does the unit have the mana (or a charge) for its used skill.
template <class E> uint32_t HasManaForUsedSkill(E& e, uint32_t u)
{
    if (!u || e.rd32(u) != 0) return 1;
    const uint32_t sk = S(e, 0xAABA, u);
    if (!sk) return 0;
    if (S(e, 0xA4BA, sk) != 0xFFFFFFFFu) return (int32_t)e.rd32(sk + 0x38) > 0;
    const uint32_t rec = e.h(0xA928, 0, sk);
    if (!rec) return 0;
    int32_t lvl = (int32_t)S(e, 0xA400, u, sk, 1) - 1;
    if (lvl < 0) lvl = 0;
    const uint32_t cost = ((uint32_t)(e.rd16s(rec + 0x18C) * lvl) + (uint32_t)e.rd16s(rec + 0x18A)) << (e.rd8(rec + 0x188) & 31);
    if (e.rd16s(rec + 0x2E) == 0x74 && S(e, 0xAE56, u)) return 1;
    return (int32_t)S(e, 0xA42A, u, 8, 0) >= (int32_t)cost;
}

// 0x9FA30: eax = pUnit, ebx = pX, ecx = this (passed through), stack = pY. Target coordinates of the unit's
// target (or of the unit itself); 1 when both are non-zero.
template <class E> uint32_t GetTargetCoords(E& e, uint32_t u, uint32_t pX, uint32_t pY, uint32_t self)
{
    e.h(0xE07A0, u, self);
    const uint32_t t = S(e, 0xA598, e.rd32(u + 0x2C));
    uint32_t y;
    if (t == u || !t) {
        const uint32_t d = e.rd32(u + 0x2C);
        e.wr32(pX, S(e, 0xAAC6, d));
        y = S(e, 0xAAD8, d);
    } else {
        auto kind = [&] { const uint32_t k = e.rd32(t); return k == 2 || (k > 3 && k <= 5); };
        uint32_t d = e.rd32(t + 0x2C);
        e.wr32(pX, kind() ? e.rd32(d + 0xC) : d ? S(e, 0xA454, d) : 0);
        d = e.rd32(t + 0x2C);
        y = kind() ? e.rd32(d + 0x10) : d ? S(e, 0xA3B8, d) : 0;
    }
    e.wr32(pY, y);
    return e.rd32(pX) != 0 && e.rd32(pY) != 0;
}

// 0x78550: ecx = pGame (must be non-zero; retail logs and _exit(-1) otherwise), edx = unit-ish, stack
// (a, idx, b, c, noCheck). Dispatch through the 8-byte-stride handler table at D2Game+0xFA3F8 (tbl).
template <class E> void Dispatch(E& e, uint32_t game, uint32_t edx, uint32_t a, uint32_t idx, uint32_t b, uint32_t c, uint32_t noCheck)
{
    if (!game) return;   // retail: log 0x804 and _exit(-1)
    if (!e.h(0x78340, edx, 0, 0, 0, idx)) return;
    if (!noCheck && !e.h(0x77FA0, 0, 0, 0, 0, edx, idx, 1, a)) return;
    const uint32_t fn = e.rd32(e.tbl + idx * 8);
    if (e.badCodePtr(fn)) return;   // retail: log 0x811 and _exit(-1)
    if (a) S(e, 0xA496, edx, a);
    e.h(e.handlerRva(fn), 0, game, edx, 0, 0, idx, b, c);
}

// 0xA1790: ebx = pUnit (player only), stack = arg. Checks the two hand slots (4, 5) for a usable item.
template <class E> uint32_t HandItemUse(E& e, uint32_t u, uint32_t arg)
{
    if (!u || e.rd32(u) != 0) return 0;
    const uint32_t p = S(e, 0x372C0, u);
    const bool any = S(e, 0xA430, p, 0x1B) || S(e, 0xA430, p, 0x23);
    const uint32_t inv = e.rd32(u + 0x60);
    for (uint32_t slot = 4; slot <= 5; slot++) {
        const uint32_t it = S(e, 0xA7AE, inv, slot);
        if (it && (any || it == p) && (S(e, 0xA940, it) || S(e, 0xA9AC, it) || S(e, 0xA802, it, 0x7D, 0)))
            return S(e, 0xA16A0, arg, u);
    }
    return 0;
}

// 0x9F850: eax = skillId, ecx = pUnit, stack = (a, level). Pay the skill's mana (or charge) cost.
template <class E> uint32_t PaySkillCost(E& e, uint32_t skillId, uint32_t u, uint32_t a, uint32_t level)
{
    if (!u || e.rd32(u) != 0) return 1;
    const uint32_t sk = S(e, 0xAABA, u);
    if (sk && S(e, 0xA49C, sk, e.fileStr, 0x3BB) == skillId) {
        const uint32_t own = S(e, 0xA4BA, sk);
        if (own != 0xFFFFFFFFu) return e.h(0x9F4A0, a, own, u, 0, 0, sk);
    }
    const uint32_t dt = e.rd32(e.rd32(e.dtPtr));
    if ((int32_t)skillId < 0 || (int32_t)skillId >= (int32_t)e.rd32(dt + 0xBA0)) return 0;
    const uint32_t rec = e.rd32(dt + 0xB98) + skillId * 0x23C;
    const int32_t m = e.rd16s(rec + 0x18A), per = e.rd16s(rec + 0x18C);
    if (!m && !per) return 0;
    int32_t l = (int32_t)level - 1; if (l < 0) l = 0;
    int32_t cost = (int32_t)(((uint32_t)(per * l) + (uint32_t)m) << (e.rd8(rec + 0x188) & 31));
    const int32_t minc = e.rd16s(rec + 0x186) * 0x100;
    if (cost < minc) cost = minc;
    if (S(e, 0xA68E, u, 0x72)) return e.h(0xBE20, (uint32_t)cost);
    if ((int32_t)S(e, 0xA42A, u, 8, 0) < cost) return 0;
    S(e, 0xA4D2, u, 8, (uint32_t)-cost, 0);
    return 1;
}

// 0xA19A0: eax = pUnit, stack (game, id, level, a5, a6, a7), ret 0x18. The 1.13c D2GAME_SKILLS_Handler_6FD12BA0: run a skill's do-function
// (and its server missile), pay its cost (a5). e.tbl = D2Game+0x107338 (word table by srvstart, the srvdo table at +0x170), e.bmSlot = the
// gdwBitMasks pointer slot (D2Game+0xF8BE0); the flag tests are byte tests against mask bytes exactly as retail compiles them.
template <class E> uint32_t ExecuteSkill(E& e, uint32_t u, uint32_t game, uint32_t id, uint32_t lvl, uint32_t a5, uint32_t a6, uint32_t a7)
{
    if (!game || !u) return 0;
    const uint32_t dt = e.rd32(e.rd32(e.dtPtr));
    if ((int32_t)id < 0 || (int32_t)id >= (int32_t)e.rd32(dt + 0xBA0)) return 0;
    const uint32_t rec = e.rd32(dt + 0xB98) + id * 0x23C;
    if (!rec) return 0;
    const uint32_t bm = e.rd32(e.bmSlot);
    auto bit = [&](uint32_t mo, uint32_t ro) { return (e.rd8(bm + mo) & e.rd8(rec + ro)) != 0; };
    uint32_t living = 0;
    if (e.rd32(u) == 0 || e.rd32(u) == 1) {
        living = 1;
        const uint32_t sk = S(e, 0xAABA, u);
        if (sk && !(S(e, 0xA79C, sk) & 1) && !bit(0, 5)) {
            const uint32_t room = S(e, 0xA478, u);
            if (!room || S(e, 0xA808, room)) {
                S(e, 0xA496, u, 0);
                if (e.rd32(u) == 0) e.h(0x78550, 0, game, u, 0, 0, 0, 5, 0, 0);
                return 0;
            }
        }
        if (!a6 && !(sk && S(e, 0xA49C, sk, e.fileStr, 0xB47) == id && (int32_t)S(e, 0xA400, u, sk, 1) > 0)) {
            const uint32_t hs = e.h(0xA3F4, 0, u, id);
            if (!hs || (int32_t)S(e, 0xA400, u, hs, 1) <= 0) return 0;
        }
    }
    const int32_t sd = e.rd16s(rec + 0x2E);
    if (sd < 0 || sd >= 0xBF) return 0;
    if (living && bit(0x14, 8) && !e.h(0x9EC80, 0, 0, 0, u)) return 0;
    int32_t c = sd;
    if (a7 && a6 && e.rd16s(rec + 0x16A) > 1) c = e.rd16s(rec + 0x16A);
    uint32_t r14 = 0;
    const uint32_t fn = e.rd32(e.tbl + 0x170 + (uint32_t)c * 4);
    if (fn) r14 = e.h(e.handlerRva(fn), 0, game, u, 0, 0, id, lvl);
    const int32_t mi = e.rd16s(rec + 0x46);
    if (mi >= 0 && e.h(0x1250, (uint32_t)mi)) {
        e.wr32(u + 0xC4, e.rd32(u + 0xC4) | 0x40);
        uint32_t xy[2] = {0, 0}, dX = 0, dY = 0, tX = 0, tY = 0;
        if (a6 && a7) {
            GetTargetCoords(e, u, (uint32_t)(uintptr_t)&xy[0], (uint32_t)(uintptr_t)&xy[1], game);
            dX = xy[0]; dY = xy[1];
            if (dX && dY) {   // 1100/1130: the unit's own x / y
                auto pos = [&](uint32_t rva, uint32_t off) {
                    const uint32_t k = e.rd32(u), d = e.rd32(u + 0x2C);
                    return (k == 2 || (k > 3 && k <= 5)) ? e.rd32(d + off) : d ? S(e, rva, d) : 0;
                };
                const uint32_t x = dX, y = dY;
                dX = x - pos(0xA454, 0xC); dY = y - pos(0xA3B8, 0x10);
                tX = x + dX; tY = y + dY;
            }
        }
        if (bit(4, 4)) e.h(0xA1880, tX, 0, 0, 0, u, dX, dY, tY, 0);
        else e.h(0xA28F0, 0, game, (uint32_t)mi, 0, 0, dX, dY, tX, tY);
        r14 = 1;
    } else if (!r14) return 0;
    if (a5) {
        if (living) {
            if (!e.rd32(e.tbl + (uint32_t)e.rd16s(rec + 0x2C) * 4) || bit(0x14, 8)) e.h(0x9F850, id, u, 0, 0, 0, game, lvl);
            if (bit(0, 4)) e.h(0xA1790, 0, 0, 0, u, 0, game);
        }
        const int32_t dl = (int32_t)S(e, 0xAC9A, u, e.rd32(rec + 0x190), id, lvl);
        if (dl > 0 && e.rd32(u) == 0 && (e.rd8(rec + 0x10) != 0x12 || !(e.rd32(u + 0x38) & 0xFFFFFF00u))) e.h(0x9FF70, (uint32_t)dl, 0, 0, 0, 0, game);
    }
    return r14;
}
}
