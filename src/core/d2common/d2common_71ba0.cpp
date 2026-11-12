// 1.13c D2Common+0x71BA0 -- field compiler (ecx = formula text, edx = record, stack word 0 = field offset): nothing for a null record. The text is compiled through Fog 10254 into a 0x400-byte stack buffer with the callbacks at image offsets 0x71b40, 0x71b30 and 0x71aa0; a positive length is appended to the growing code block (cursor triple at image offsets 0x9ff70, 0x9ff74, 0x9ff78) through the worker 0x5DD80 and the block offset it answers is stored in the field. A null text or a non-positive length stores -1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2common {
uint32_t f71ba0(GE& e, uint32_t ea, uint32_t s, uint32_t rec, uint32_t off)
{
    if (!rec) return ea;
    if (s) {
        uint8_t buf[0x400];
        const uint32_t n = e.h(0x9234, 0, 0, 0, 0, 0, s, e.stk(buf), 0x400, 0xB2071b40u);
        const uint32_t w[2] = { 0xB2071b30u, 0xB2071aa0u };
        e.x(0x9234, w, 2);
        if ((int32_t)n > 0) {
            const uint32_t r = e.h(0x5dd80, 0xB209ff78u, 0xB209ff70u, 0, 0xB209ff74u, 0, e.stk(buf), n);
            e.wr32(rec + off, r);
            return r;
        }
    }
    e.wr32(rec + off, 0xffffffffu);
    return off;
}
}
