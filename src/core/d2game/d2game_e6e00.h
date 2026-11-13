// 1.13c D2Game+0xE6E00, transcribed from the 1.13c disassembly: the Act 1 quest node "what state is this player in" query.
// Convention: ECX = node (this), EDX = player unit, stack: unused word, quest flags qs, out byte pointer (ret 0xC); always returns with AL = 1. Writes one byte to out:
// 0 when group 0xF bit 0 is clear; 0xB / 0xD from group 0x11 bits 0 and 0xD; 2 when the global item 0xE660 code 0x20626262 is found or the registered callback (0xE5080, run by
// 0x9C160 with ESI = node +4, EBX = 0 for a placed player whose 0x9BEB0 id is not -1) sets data byte +4; 8 / 9 or 1 when node byte +9 is set and node byte +0xC is <= 3 (data
// byte +2 == 1 and data dword +8 == 0 gives 8 + (game byte +0x6A != 3)); 0xC then 1 when the player's 0xA56E(0xA556(0xA478(unit))) low byte is 2 and data dword +8 is set.
// EAX upper 24 bits are whatever the last callee left (the 0xE660, 0x9BEB0, 0x9C160, 0xA478, 0xA56E results, the data dword +8, or the out pointer).
#pragma once
#include <cstdint>

namespace re6e00 {
template <class E> uint32_t Do(E& e, uint32_t th, uint32_t unit, uint32_t a1, uint32_t qs, uint32_t out)
{
    (void)a1;
    auto q = [&](uint32_t g, uint32_t b) { return e.h(0xA568, 0, 0, 0, 0, 0, qs, g, b); };
    if (q(0xF, 0) == 0) {
        e.wr8(out, 0);
        return (out & ~0xFFu) | 1;
    }
    if (q(0x11, 0) != 0) {
        const uint32_t v = q(0x11, 0xD);
        e.wr8(out, (uint8_t)(0xB + (v ? 2 : 0)));
        return ((0u - v) & ~0xFFu) | 1;
    }
    e.wr8(out, 0);
    uint32_t eax = e.h(0xE660, unit, 0, 0, e.rd32(th + 4), 0, 0x20626262);
    if (eax) {
        e.wr8(out, 2);
        return (eax & ~0xFFu) | 1;
    }
    const uint32_t d = e.rd32(th + 0x18);
    if (unit && e.rd32(unit + 0x80) && e.rd32(unit) == 0) {
        eax = e.h(0x9BEB0, unit);
        e.j(0x9BEB0, e.rd32(unit + 0x80));
        if ((eax & 0xFFFF) != 0xFFFF) {
            e.wr8(d + 4, 0);
            eax = e.h(0x9C160, 0, 0, 0, 0, e.rd32(th + 4), 0xB2000000u | 0xE5080);
            if (e.rd8(d + 4)) {
                e.wr8(out, 2);
                return (out & ~0xFFu) | 1;
            }
        }
    }
    if (e.rd8(th + 9)) {
        if (e.rd8(th + 0xC) <= 3) {
            e.wr8(out, 1);
            if (e.rd8(d + 2) == 1 && e.rd32(d + 8) == 0) {
                const uint32_t ne = e.rd8(e.rd32(th + 4) + 0x6A) != 3;
                e.wr8(out, (uint8_t)(8 + ne));
            }
        } else {
            e.wr8(out, 0xC);
            eax = e.h(0xA478, 0, 0, 0, 0, 0, unit);
            if (eax) {
                eax = e.h(0xA556, 0, 0, 0, 0, 0, eax);
                eax = e.h(0xA56E, 0, 0, 0, 0, 0, eax);
                if ((eax & 0xFF) == 2) {
                    eax = e.rd32(d + 8);
                    if (eax) e.wr8(out, 1);
                }
            }
        }
    }
    return (eax & ~0xFFu) | 1;
}
}
