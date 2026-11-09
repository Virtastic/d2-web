// 1.13c D2Game+0xEDE90 -- run 0xEDDB0 `count` times for an owner and tag each unit found. EAX = count, EBX = class, ESI = owner;
// stack (ret 0x10): w0, w1, w2, w3. 0xEDDB0 takes EAX = w0, ECX = w1, EDX = owner, stack (class, w2, w3). A hit sets the result to 1;
// a hit that is a type-1 unit also gets +0x2C = owner+0xC, +0x30 = 1, +0x28 = class on its +0x14 record's +0x28 block; then 0x20CF0
// (EAX = owner, EDI = the hit, stack class).
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_ede90(uint32_t count, uint32_t cls, uint32_t owner, uint32_t w0, uint32_t w1, uint32_t w2, uint32_t w3, const D2Seam* e)
{
    if (!owner) return 0;
    const uint32_t id = d2rd32(owner + 0xc);
    uint32_t result = 0;
    for (int32_t n = (int32_t)count; n > 0; --n) {
        const uint32_t u = e->call(0xEDDB0, w0, w1, owner, 0, 0, cls, w2, w3);
        if (!u) continue;
        result = 1;
        if (d2rd32(u) == 1) {
            const uint32_t rec = d2rd32(u + 0x14);
            const uint32_t b = rec ? d2rd32(rec + 0x28) : 0;
            d2wr32(b + 0x2c, id);
            d2wr32(b + 0x30, 1);
            d2wr32(b + 0x28, cls);
        }
        e->call(0x20CF0, owner, 0, 0, 0, 0, cls);
        e->edi(0x20CF0, u);
    }
    return result;
}
