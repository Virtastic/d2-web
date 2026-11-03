// 1.13c D2Game+0xBD9B0, transcribed from the 1.13c disassembly. Convention: EAX = esi
// (a record pointer or 0), ECX = item, one stack word p (ret 4). Callees (Ordinals reached by jmp thunks): 0xABF2 / 0xA9AC / 0xA940 /
// 0xAA06 / 0xAA00 / 0xA4C0 (1 word), 0xA472 (item, event, value, 0), 0xA8A4 (item, 0x100, 0xBA, file string); 0x8B010 (EAX = item, stack
// [rec+0x9c or 0, 1, event, value]); 0xD2380 (EAX = p, ECX = esi, stack item); 0xD5690 (p).
#pragma once
#include <cstdint>

namespace rbd9b0 {
template <class E> void Do(E& e, uint32_t es, uint32_t esTag, uint32_t item, uint32_t p, uint32_t rec)
{
    auto notify = [&](uint32_t ev, uint32_t v) {
        e.h(0xA472, 0, 0, 0, 0, 0, item, ev, v, 0);
        if (!es) return;
        uint32_t x = 0;
        if (e.rd32(es) == 0) { e.h(0xA4C0, 0, 0, 0, 0, 0, esTag); x = e.rd32(rec + 0x9c); }   // the hook returns the record blob
        e.h(0x8B010, item, 0, 0, 0, 0, x, 1, ev, v);
    };
    if (!e.h(0xABF2, 0, 0, 0, 0, 0, item)) return;
    if (e.h(0xA9AC, 0, 0, 0, 0, 0, item) && e.h(0xA940, 0, 0, 0, 0, 0, item))
        notify(0x46, e.h(0xAA06, 0, 0, 0, 0, 0, item));
    e.h(0xD2380, p, esTag, 0, 0, 0, item);
    if (e.h(0xA8A4, 0, 0, 0, 0, 0, item, 0x100, 0xBA, 0xB20FB3A4)) { e.h(0xD5690, 0, 0, 0, 0, 0, p); return; }
    const int32_t cost = (int32_t)e.h(0xAA00, 0, 0, 0, 0, 0, item);
    if (cost > 0) notify(0x48, (uint32_t)cost);
}
}
