// 1.13c D2Common+0x528B0: remove a skill from a unit's skill list, retail record layout, written from the
// 1.13c disassembly as a rule over E (HookEnv: rd32/rd16s/wr32 on 32-bit addresses, h(rva, eax, ecx, edx,
// ebx, esi, s0..) for each callee). EBX = unit, stack = (skill id, bDecrement), ret 8.
// Layout: unit +0x08 memory pool, +0xA8 skill list; list +0x04 first node, +0x08 left, +0x0C right,
// +0x10 used skill; node +0x00 skills.txt row (word 0 = skill id), +0x04 next, +0x28 level, +0x34 owner.
// A node matches when its row's signed skill-id word equals the id and its owner is -1.
// Nothing happens without a unit, a list or a first node. Else: the skill's passive state
// 0x4F0E0(id), when nonzero, is cleared through 0x34510(unit, state, 0); a matching left skill runs
// 0x4ED60(unit, 0, -1), a matching right skill 0x4ECC0(unit, 0, -1), a matching used skill is cleared.
// The first matching node is then unlinked (from its predecessor or the list head) and handed to the
// pool free (Fog import thunk 0x9228: ECX = pool, EDX = node, stack = file, line 0x4AD, 0) -- unless
// bDecrement is set and its level, decremented in place, stays above 0. Every path past the list
// checks ends in the tail call 0x52480(unit, id).
#pragma once
#include <cstdint>

namespace d2c528b0 {
constexpr uint32_t kFile = 0xB2000000u | 0x93118u;   // the source-file string (D2Common .rdata) the free call is handed, in the capture log form 0xB2000000|rva
template <class E> void RemoveSkill(E& e, uint32_t u, int32_t id, int32_t bDec)
{
    if (!u) return;
    const uint32_t l = e.rd32(u + 0xA8);
    if (!l) return;
    const uint32_t first = e.rd32(l + 4);
    if (!first) return;
    if (const uint32_t st = e.h(0x4F0E0, 0, 0, 0, 0, 0, (uint32_t)id)) e.h(0x34510, 0, 0, 0, 0, 0, u, st, 0);
    auto match = [&](uint32_t n) { return E::rd16s(e.rd32(n)) == id && e.rd32(n + 0x34) == 0xFFFFFFFFu; };
    uint32_t n = e.rd32(l + 8);
    if (n && match(n)) e.h(0x4ED60, 0, 0, 0, 0, 0, u, 0, 0xFFFFFFFFu);
    n = e.rd32(l + 0xC);
    if (n && match(n)) e.h(0x4ECC0, 0, 0, 0, 0, 0, u, 0, 0xFFFFFFFFu);
    n = e.rd32(l + 0x10);
    if (n && match(n)) e.wr32(l + 0x10, 0);
    uint32_t prev = 0;
    for (n = first; n; prev = n, n = e.rd32(n + 4)) {
        if (!match(n)) continue;
        if (bDec) {
            const uint32_t lv = e.rd32(n + 0x28) - 1;
            e.wr32(n + 0x28, lv);
            if ((int32_t)lv > 0) break;
        }
        e.wr32(prev ? prev + 4 : l + 4, e.rd32(n + 4));
        e.h(0x9228, 0, e.rd32(u + 8), n, 0, 0, kFile, 0x4AD, 0);
        break;
    }
    e.h(0x52480, 0, 0, 0, 0, 0, u, (uint32_t)id);
}
}
