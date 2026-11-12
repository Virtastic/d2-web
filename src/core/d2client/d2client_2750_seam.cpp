// 1.13c D2Client+0x2750: release one chunk, stdcall(obj). When the link at obj+0x10 (previous link) is set, the chunk is unlinked:
// the next link is ~next for a negative next word at +0x14, else (obj+0x10) + (next - previous->next); next link->previous :=
// previous, previous->next := next, both own words cleared. Then 0x2700 (EAX = obj) drains the chunk; its EAX is returned.
#include "d2c_seam.h"
extern "C" uint32_t d2c_2750(const D2cSeam* s, uint8_t* obj) {
    auto rd = [](uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; };
    auto wr = [](uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); };
    const uint32_t link = (uint32_t)(uintptr_t)obj + 0x10, prev = rd(link);
    if (prev) {
        const uint32_t next = rd(link + 4);
        const uint32_t nl = (int32_t)next < 0 ? ~next : link + (next - rd(prev + 4));
        wr(nl, prev);
        wr(prev + 4, rd(link + 4));
        wr(link, 0); wr(link + 4, 0);
    }
    return (uint32_t)(*s)(0x2700, (uintptr_t)obj);
}
