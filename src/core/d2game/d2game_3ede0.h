// 1.13c D2Game+0x3EDE0 (UNITS_RunTriggerHooks, the missile hit dispatcher): edi = missile unit, stack = (pGame, target unit, force),
// ret 0xC. Written from the 1.13c disassembly as a rule over an environment E (HookEnv shape; E::g(rva) a module dword, E::fn(addr)
// the RVA of a module code address read from a dispatch table). rec = the missile row of unit class (+4) (null for a null unit or an
// out-of-range class; retail then reads through it). o = 0xE03A0 (eax = unit +0x94, ecx = game, edx = unit +0x98) when unit +0xC8 bit 10
// is set. A = 2, B = 3; rec byte +4 & 2 -> A = 0 (gdwBitMasks[1]). With a target: rec +0x188 set and import 0xA68E(target, 0x56) -> 1;
// import 0xB27C(unit, target) -> 1; import 0xB0F0(unit, target); 0x38FB0 (eax = target, ecx = unit +0x10 or 0) zero -> 1; with o:
// 0xE0A60 (ecx = o, eax = target, stack game) zero and rec +0x187 clear -> 1; then rec byte +4 & 4 (gdwBitMasks[2]) and (import
// 0xA42A(o, 0xA6, 0) or import 0xA802(o, 0x9C, 0)) -> B = 0x38F40(esi = unit). rec +0x186 -> A |= 1. A target with rec +0x18B: a =
// unit ? import 0xA42A(unit, 0x13, 0) : 0; 0xDDE90 (eax = o, stack target, a, 1) zero -> 0x3E590 (ecx = unit, stack target), 0x37030
// (ebx = target, stack game, 0, unit, 0), and with rec +0x18C and k = rec.s16[+0xE] in (0, g(0x10DB64)) the table 0x10DA48[k] (ecx = game,
// edx = unit, stack target) with bit 4 -> 1; else 2. No target: force and rec +0x18C both clear skip to the tail. B & 2: 0x3E590,
// 0x37030 as above, k in range -> A = table 0x10DA48[k](...); A & 4 -> 1. A target with A & 2: 0x3A4E0 (ebx = unit, stack target, &buf),
// k = rec.s16[+0x10] in (0, g(0x10DBE4)) -> table 0x10DB68[k] (ecx = game, edx = unit, stack target, &buf); 0x3AFF0 (eax = game, ecx =
// &buf, esi = target, ebx = unit). Tail: (A & 1 or force) and B & 1: u = import 0xA478(unit); u -> import 0xB258(u, 0x1100(ecx = unit),
// 0x1130(ecx = unit), import 0xA71E(unit, 0x40)); result 2. Otherwise 1.
#pragma once
#include <cstdint>

namespace d2g3ede0 {
template <class E> uint32_t RunTriggerHooks(E& e, uint32_t dt, uint32_t unit, uint32_t game, uint32_t target, uint32_t force)
{
    uint32_t rec = 0, o = 0;
    if (unit) {
        const int32_t cls = (int32_t)e.rd32(unit + 4);
        if (cls >= 0 && cls < (int32_t)e.rd32(dt + 0xB6C)) rec = (uint32_t)cls * 0x1A4u + e.rd32(dt + 0xB64);
        if ((e.rd32(unit + 0xC8) >> 10) & 1) o = e.h(0xE03A0, e.rd32(unit + 0x94), game, e.rd32(unit + 0x98));
    }
    uint32_t A = 2, B = 3;
    if (e.rd8(rec + 4) & 2) A = 0;
    if (target) {
        if (e.rd8(rec + 0x188) && e.h(0xA68E, 0, 0, 0, 0, 0, target, 0x56)) return 1;
        if (e.h(0xB27C, 0, 0, 0, 0, 0, unit, target)) return 1;
        e.h(0xB0F0, 0, 0, 0, 0, 0, unit, target);
        if (!e.h(0x38FB0, target, unit ? e.rd32(unit + 0x10) : 0)) return 1;
        if (o && !e.h(0xE0A60, target, o, 0, 0, 0, game) && !e.rd8(rec + 0x187)) return 1;
        if (o && (e.rd8(rec + 4) & 4)) {
            if (e.h(0xA42A, 0, 0, 0, 0, 0, o, 0xA6, 0) || e.h(0xA802, 0, 0, 0, 0, 0, o, 0x9C, 0)) B = e.h(0x38F40, 0, 0, 0, 0, unit);
        }
    }
    if (e.rd8(rec + 0x186)) A |= 1;
    bool tail = false;
    if (target) {
        if (e.rd8(rec + 0x18B)) {
            const uint32_t a = unit ? e.h(0xA42A, 0, 0, 0, 0, 0, unit, 0x13, 0) : 0;
            if (!e.h(0xDDE90, o, 0, 0, 0, 0, target, a, 1)) {
                e.h(0x3E590, 0, unit, 0, 0, 0, target);
                e.h(0x37030, 0, 0, 0, target, 0, game, 0, unit, 0);
                if (e.rd8(rec + 0x18C)) {
                    const int32_t k = e.rd16s(rec + 0xE);
                    if (k > 0 && k < (int32_t)e.g(0x10DB64)) {
                        if (e.h(e.fn(e.g(0x10DA48 + 4u * (uint32_t)k)), 0, game, unit, 0, 0, target) & 4) return 1;
                    }
                }
                return 2;
            }
        }
    } else if (!force && !e.rd8(rec + 0x18C)) tail = true;
    if (!tail) {
        if (B & 2) {
            e.h(0x3E590, 0, unit, 0, 0, 0, target);
            e.h(0x37030, 0, 0, 0, target, 0, game, 0, unit, 0);
            const int32_t k = e.rd16s(rec + 0xE);
            if (k > 0 && k < (int32_t)e.g(0x10DB64)) {
                A = e.h(e.fn(e.g(0x10DA48 + 4u * (uint32_t)k)), 0, game, unit, 0, 0, target);
                if (A & 4) return 1;
            }
        }
        if (target && (A & 2)) {
            uint32_t buf[28];
            e.h(0x3A4E0, 0, 0, 0, unit, 0, target, e.stk(buf));
            const int32_t k = e.rd16s(rec + 0x10);
            if (k > 0 && k < (int32_t)e.g(0x10DBE4)) e.h(e.fn(e.g(0x10DB68 + 4u * (uint32_t)k)), 0, game, unit, 0, 0, target, e.stk(buf));
            e.h(0x3AFF0, game, e.stk(buf), 0, unit, target);
        }
    }
    if (((A & 1) || force) && (B & 1)) {
        const uint32_t u = e.h(0xA478, 0, 0, 0, 0, 0, unit);
        if (u) {
            const uint32_t w = e.h(0xA71E, 0, 0, 0, 0, 0, unit, 0x40);
            const uint32_t y = e.h(0x1130, 0, unit);
            const uint32_t x = e.h(0x1100, 0, unit);
            e.h(0xB258, 0, 0, 0, 0, 0, u, x, y, w);
        }
        return 2;
    }
    return 1;
}
}
