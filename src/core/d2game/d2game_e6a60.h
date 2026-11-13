// 1.13c D2Game+0xE6A60, transcribed from the 1.13c disassembly: the Act 1 quest node item-pickup counter.
// Convention: ECX = node (this), EDX = event record (+0 game, +0xC player); plain ret. Node +0x18 is the data block (dword +0x10 the pickup counter, byte +0x15 a pending flag).
// 0xE660 (EAX = player, EBX = game, stack code): code 0x2034336A bumps the counter, code 0x20343367 bumps it again and selects the second state. When the counter equals 1, node
// byte +9 and data byte +0x15 are set: the flag is cleared, node +0x14 := 0, node +0xB := 1 (3 with the second state) and 0xDFCC0 (EAX = 0, EBX = 0xE5F00, EDI = node +4, stack 0) runs.
// EAX at exit: the 0xDFCC0 result, node byte +9, the data block's counter + 1 (second state), or 0.
#pragma once
#include <cstdint>

namespace re6a60 {
template <class E> uint32_t Do(E& e, uint32_t th, uint32_t ev)
{
    const uint32_t game = e.rd32(ev), pl = e.rd32(ev + 0xC);
    const uint32_t d = e.rd32(th + 0x18);
    if (e.h(0xE660, pl, 0, 0, game, 0, 0x2034336A)) e.wr32(d + 0x10, e.rd32(d + 0x10) + 1);
    uint32_t cl = 0, eax;
    if (e.h(0xE660, pl, 0, 0, game, 0, 0x20343367)) {
        eax = e.rd32(d + 0x10) + 1;
        cl = 1;
        e.wr32(d + 0x10, eax);
    } else {
        eax = 0;
    }
    if (e.rd32(d + 0x10) != 1) return eax;
    const uint32_t b9 = e.rd8(th + 9);
    eax = (eax & ~0xFFu) | b9;
    if (!b9) return eax;
    if (!e.rd8(d + 0x15)) return eax;
    e.wr8(d + 0x15, 0);
    e.wr8(th + 0x14, 0);
    e.wr8(th + 0xB, cl ? 3 : 1);
    const uint32_t r = e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0xE5F00, 0, 0);
    e.j(0xDFCC0, e.rd32(th + 4));
    return r;
}
}
