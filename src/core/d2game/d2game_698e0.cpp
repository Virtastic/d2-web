// 1.13c D2Game+0x698E0 -- does the unit's inventory hold an item whose data matches a filter.
// EAX = unit (the inventory is unit+0x60, fatal when null), EDI = the filter word (ESI is only saved), ; plain ret. Walks the inventory with
// #A3DC(inv) / #A4CC(item); an item matches when #A430(#A46C(item), edi) is non-zero.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game698E0Env {
    void     (*fatal)(uint32_t line);
    uint32_t (*first)(uint32_t inv);
    uint32_t (*data)(uint32_t item);
    uint32_t (*test)(uint32_t data, uint32_t edi);
    uint32_t (*next)(uint32_t item);
};

// 1.13c D2Game+0x698E0
uint32_t D2Game_698E0(uint32_t unit, uint32_t edi, const D2Game698E0Env& e)
{
    uint32_t inv; std::memcpy(&inv, (const void*)(uintptr_t)(unit + 0x60), 4);
    if (!inv) { e.fatal(0x6d3); return 0; }
    for (uint32_t it = e.first(inv); it; it = e.next(it))
        if (e.test(e.data(it), edi)) return 1;
    return 0;
}
