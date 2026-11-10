// 1.13c D2Game+0xCBF50: the 5-byte position-packet validator (packets with x at +1, y at +3). Disassembled convention:
// EAX = packet, ESI = unit, stack = (game, length, &outX, &outY), ret 0x10. Both out-words are cleared first; length != 5 -> 3;
// Ordinal_10920 (thunk 0xA4C0, 1 stack word = unit) null -> 2. 0xCB690 (EAX = unit, EBX = 0x32, stack = x, y; ret 8) checks the
// target: a non-zero result is returned as is, after (game+0xA8 - pd+0x168 > 0x19, unit non-null, unit type 0, a second Ordinal_10920
// whose +0x9C client is non-null) 0x8BF80 is told (AL = 0x15 over the upper bytes of the 0x1100 result, CL = 0, EDX = unit id,
// stack = client, 0x1100(unit), 0x1130(unit), 1; ret 0x10); zero stamps pd+0x168 = game+0xA8, writes x to *outX and y to *outY,
// returns 0.
#include <cstdint>
#include <cstring>
#include "d2game_cbf50.h"

static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
static inline uint16_t rd16(uint32_t a, uint32_t o) { uint16_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 2); return v; }
static inline void wr32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(a + o), &v, 4); }
static inline void wr16(uint32_t a, uint16_t v) { std::memcpy((void*)(uintptr_t)a, &v, 2); }

uint32_t d2game_cbf50_port(uint32_t pkt, uint32_t unit, uint32_t game, uint32_t len, uint32_t outX, uint32_t outY,
                           const D2GameCBF50Env* e) {
    wr16(outX, 0);
    wr16(outY, 0);
    if (len != 5) return 3;
    const uint32_t pd = e->o10920(unit);
    if (!pd) return 2;
    const uint16_t y = rd16(pkt, 3), x = rd16(pkt, 1);
    const uint32_t r = e->f_cb690(unit, 0x32, x, y);
    const uint32_t frame = rd32(game, 0xa8);
    if (r == 0) {
        wr32(pd, 0x168, frame);
        wr16(outX, x);
        wr16(outY, y);
        return 0;
    }
    if ((int32_t)(frame - rd32(pd, 0x168)) > 0x19 && unit && rd32(unit, 0) == 0) {
        const uint32_t client = rd32(e->o10920(unit), 0x9c);
        if (client) {
            const uint32_t id = rd32(unit, 0xc);
            const uint32_t b = e->f_1130(unit);
            const uint32_t a = e->f_1100(unit);
            e->f_8bf80((a & 0xffffff00u) | 0x15, id, client, a, b, 1);
        }
    }
    return r;
}
