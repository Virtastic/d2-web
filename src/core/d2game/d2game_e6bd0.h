// 1.13c D2Game+0xE6BD0, transcribed from the 1.13c disassembly: the Act 1 quest node event handler that reacts to event types 0xFE, 0xF5 and 0x108.
// Convention: ECX = node (this), EDX = event record (+8 pointer whose +4 is the event type, NULL for -1); plain ret. Node +0x18 is the data block. Each reaction clears node +0x14,
// sets node +0xB (5 for 0xFE with data byte +8; 2 for 0xF5 with byte +9 and then 4 with byte +0xA; 3 for 0x108 with byte +0xB), runs 0xDFCC0 (EAX = 0, EBX = handler 0xE5F00,
// EDI = node +4, stack 0) and clears the data byte (0xFE also sets data byte +0 = 1). EAX at exit is the last 0xDFCC0 result when one ran, else the event type.
#pragma once
#include <cstdint>

namespace re6bd0 {
template <class E> uint32_t Do(E& e, uint32_t th, uint32_t ev)
{
    const uint32_t p8 = e.rd32(ev + 8);
    const uint32_t ty = p8 ? e.rd32(p8 + 4) : 0xFFFFFFFFu;
    const uint32_t d = e.rd32(th + 0x18);
    auto fire = [&](uint32_t st) {
        e.wr8(th + 0x14, 0);
        e.wr8(th + 0xB, st);
        const uint32_t r = e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0xE5F00, 0, 0);
        e.j(0xDFCC0, e.rd32(th + 4));
        return r;
    };
    const uint32_t t = ty & 0xFFFF;
    if (t == 0xFE) {
        if (e.rd8(d + 8) != 1) return ty;
        const uint32_t r = fire(5);
        e.wr8(d + 8, 0);
        e.wr8(d, 1);
        return r;
    }
    if (t == 0xF5) {
        uint32_t r = ty;
        if (e.rd8(d + 9) == 1) { r = fire(2); e.wr8(d + 9, 0); }
        if (e.rd8(d + 0xA) != 1) return r;
        r = fire(4);
        e.wr8(d + 0xA, 0);
        return r;
    }
    if (t == 0x108 && e.rd8(d + 0xB) == 1) {
        const uint32_t r = fire(3);
        e.wr8(d + 0xB, 0);
        return r;
    }
    return ty;
}
}
