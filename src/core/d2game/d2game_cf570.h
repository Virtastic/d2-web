// 1.13c D2Game client packet handlers 0x33 (sell, +0xCF570), 0x32 (buy, +0xCF220) and 0x35 (repair, +0xCF0A0), transcribed
// from the 1.13c disassembly. Convention: ECX = game, EDX = unit, stack packet, size (ret 8). A size other than 0x11 returns 3
// before the packet is read. The play-path bodies are PlrMsg.cpp's Rcv0x33 / Rcv0x32 / Rcv0x35.
#pragma once
#include <cstdint>

namespace rcf570 {
// 0x33: EAX = NPC id (+1); stack game, unit, item id (+5), mode word (+9, zero-extended); ret 0x10. Its result is returned.
// The cost dword (+0xD) is read into a dead stack slot and never passed.
template <class E> uint32_t Sell(E& e, uint32_t game, uint32_t unit, uint32_t pkt, uint32_t size)
{
    if (size != 0x11) return 3;
    return e.h(0xBF990, e.rd32(pkt + 1), 0, 0, 0, 0, game, unit, e.rd32(pkt + 5), e.rd32(pkt + 9) & 0xFFFF);
}

// 0x32: EAX = game, EBX = NPC id (+1), ESI = unit; stack item id (+5), (d & 0x7FFFFFFF) >> 16, d & 0x7FFFFFFF, cost (+0xD),
// d & 0x80000000 where d is the dword at +9; ret 0x14. Its result is returned.
template <class E> uint32_t Buy(E& e, uint32_t game, uint32_t unit, uint32_t pkt, uint32_t size)
{
    if (size != 0x11) return 3;
    const uint32_t d = e.rd32(pkt + 9), lo = d & 0x7FFFFFFFu;
    const uint32_t w4[1] = { d & 0x80000000u };
    const uint32_t r = e.h(0xBF200, game, 0, 0, e.rd32(pkt + 1), unit, e.rd32(pkt + 5), lo >> 16, lo, e.rd32(pkt + 0xD), w4[0]);
    e.x(0xBF200, w4, 1);
    return r;
}

// 0x35: EAX = NPC id (+1), ECX = unit, EDX = game; stack item id (+5), flags (+0xD); ret 8. Returns 0.
template <class E> uint32_t Repair(E& e, uint32_t game, uint32_t unit, uint32_t pkt, uint32_t size)
{
    if (size != 0x11) return 3;
    e.h(0xDDE60, e.rd32(pkt + 1), unit, game, 0, 0, e.rd32(pkt + 5), e.rd32(pkt + 0xD));
    return 0;
}
}
