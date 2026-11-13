// 1.13c D2Game+0xE6CF0, transcribed from the 1.13c disassembly: the Act 1 quest node event handler that reacts to event types 0xFD and 0xFF.
// Convention: ECX = node (this), EDX = event record (+8 pointer whose +4 is the event type, NULL for -1); plain ret. Node +0x18 is the data block: type 0xFD with data byte +1 == 1
// clears node +0x14, sets node +0xB = 2 and runs 0xDFCC0 twice (EAX = 0, EBX = a handler address 0xE6130 then 0xE46F0, EDI = node +4, stack 0), clearing data byte +1 between them;
// type 0xFF with data byte +7 set clears it and sets data byte +6 = 1. EAX at exit is the second 0xDFCC0 result on the first path, else the event type word (or -1).
#pragma once
#include <cstdint>

namespace re6cf0 {
template <class E> uint32_t Do(E& e, uint32_t th, uint32_t ev)
{
    const uint32_t p8 = e.rd32(ev + 8);
    const uint32_t ty = p8 ? e.rd32(p8 + 4) : 0xFFFFFFFFu;
    const uint32_t d = e.rd32(th + 0x18);
    if ((ty & 0xFFFF) == 0xFD) {
        if (e.rd8(d + 1) == 1) {
            e.wr8(th + 0x14, 0);
            e.wr8(th + 0xB, 2);
            e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0xE6130, 0, 0);
            e.j(0xDFCC0, e.rd32(th + 4));
            e.wr8(d + 1, 0);
            const uint32_t r = e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0xE46F0, 0, 0);
            e.j(0xDFCC0, e.rd32(th + 4));
            return r;
        }
    } else if ((ty & 0xFFFF) == 0xFF) {
        if (e.rd8(d + 7)) { e.wr8(d + 7, 0); e.wr8(d + 6, 1); }
    }
    return ty;
}
}
