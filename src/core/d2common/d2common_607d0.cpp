// 1.13c D2Common+0x607D0 -- field compiler (ecx = formula text, edx = record, stack word 0 = field offset): nothing for a null record. The text is compiled through Fog 10254 into a 0x400-byte stack buffer with the three callbacks at image offsets 0x5dcd0, 0x5dcc0 and 0x5e520; a positive length is appended to the growing code block (cursor triple at image offsets 0x9ff24, 0x9ff28, 0x9ff2c) through the worker 0x5DD80 and the block offset it answers is stored in the field. A null text or a non-positive length stores -1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2common {
uint32_t f607d0(GE& e, uint32_t ea, uint32_t s, uint32_t rec, uint32_t off)
{
    if (!rec) return ea;
    if (s) {
        uint8_t buf[0x400];
        const uint32_t n = e.h(0x9234, 0, 0, 0, 0, 0, s, e.stk(buf), 0x400, 0xB205dcd0u);
        const uint32_t w[2] = { 0xB205dcc0u, 0xB205e520u };
        e.x(0x9234, w, 2);
        if ((int32_t)n > 0) {
            const uint32_t r = e.h(0x5dd80, 0xB209ff2cu, 0xB209ff24u, 0, 0xB209ff28u, 0, e.stk(buf), n);
            e.wr32(rec + off, r);
            return r;
        }
    }
    e.wr32(rec + off, 0xffffffffu);
    return off;
}
}
