// 1.13c D2Game+0xE4F60, transcribed from the 1.13c disassembly: the Act 1 quest node test "has the player collected what the 0xF5 event needs".
// Convention: ECX = node (this), EDX = event type, EAX = caller residue (only its upper 24 bits matter), stack: player unit, quest flags qs, unused word (ret 0xC). Returns AL = 1 when
// the event type is 0xF5 and either node byte +0xC is 0 or group 0x12 bit 2 is set... in detail: each of the five items 0xE660 codes 0x20316671, 0x20796571, 0x20726871, 0x20726271 found
// in the player's pack must have its group 0x12 bit (5, 3, 6, 4) set, 0x20326671 needs bit 7 set or 0xE2960 (EAX = node +4) true, and the count of the first four found must be 4 with bit 5
// clear to fail. EAX upper bits are the last callee result (or EAX on entry for a non-0xF5 event).
#pragma once
#include <cstdint>

namespace re4f60 {
template <class E> uint32_t Do(E& e, uint32_t eax, uint32_t th, uint32_t ty, uint32_t unit, uint32_t qs, uint32_t a3)
{
    (void)a3;
    if (ty != 0xF5) return eax & ~0xFFu;
    auto q = [&](uint32_t b) { return e.h(0xA568, 0, 0, 0, 0, 0, qs, 0x12, b); };
    auto it = [&](uint32_t code) { return e.h(0xE660, unit, 0, 0, e.rd32(th + 4), 0, code); };
    auto ok = [&](uint32_t v) { return (v & ~0xFFu) | 1; };
    uint32_t esi = 0;
    if (e.rd8(th + 0xC) >= 1) {
        eax = q(2);
        if (!eax) return ok(eax);
    }
    static const uint32_t kCode[4] = { 0x20316671, 0x20796571, 0x20726871, 0x20726271 };
    static const uint32_t kBit[4] = { 5, 3, 6, 4 };
    for (int i = 0; i < 4; i++) {
        eax = it(kCode[i]);
        if (eax) {
            esi++;
            eax = q(kBit[i]);
            if (!eax) return ok(eax);
        }
    }
    eax = it(0x20326671);
    if (eax) {
        eax = q(7);
        if (!eax) {
            eax = e.h(0xE2960, e.rd32(th + 4));
            if (eax & 0xFF) return ok(eax);
        }
    }
    if (esi != 4) return eax & ~0xFFu;
    eax = q(5);
    if (eax) return eax & ~0xFFu;
    return ok(eax);
}
}
