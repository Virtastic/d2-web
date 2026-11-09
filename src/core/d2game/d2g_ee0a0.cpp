// 1.13c D2Game+0xEE0A0 -- spawn a ring of units around an owner. EAX = mode (0..2, else 0), stdcall, six stack words, ret 0x18:
// cls, owner, s2, s3, count, s5. Position from the owner (types 2 and 4-5 read the path at +0x2C, else 0xA454 / 0xA3B8 on it),
// seed at owner+0x20/+0x24 advanced as lo * 0x6AC690C5 + hi (64-bit), start slot = new low % 6. Each pass offsets by the
// 8-byte entry at RVA 0xFC298 [(mode*6 + slot)], slot = (slot + 5) % 6, builds the 10-word request {cls, room, 0, s2, s3, 0, x, y, -1,
// word s5} and calls 0xEEF10 (ECX = request). A type-1 result gets its monster data stamped (+0x2C owner id, +0x30 = 1, +0x28 = cls),
// then 0x20CF0 (EAX = owner, stack cls, EDI = the result). Returns 1 if any call produced a unit. Callees through the seam: 0xA478
// (unit to room), 0xA454 / 0xA3B8 (path x / y).
// The offset table is the 36 dwords at RVA 0xFC298 (12 offset pairs, then the six pairs of the following data that mode 2 reads).
#include "d2game_seam4e.h"
static const uint32_t kOffs[36] = {
    0xffffffffu, 0xfffffffcu, 1, 4, 1, 0xfffffffdu, 0xffffffffu, 3, 0, 2, 0, 0xfffffffeu,
    0xfffffffdu, 0xffffffffu, 3, 1, 2, 0xffffffffu, 0xfffffffeu, 1, 1, 0, 0xffffffffu, 0,
    0x20647362u, 0x104, 0x20786174u, 0x104, 0x20756f76u, 0x104, 0x20776f74u, 0x105, 0x20786174u, 0x105, 0x206e6972u, 0x106};
extern "C" uint32_t d2g_ee0a0(uint32_t mode, uint32_t cls, uint32_t owner, uint32_t s2, uint32_t s3, uint32_t count, uint32_t s5, const D2Seam* e)
{
    if (!owner) return 0;
    if ((int32_t)mode < 0 || (int32_t)mode > 2) mode = 0;
    const uint32_t room = e->call(0xA478, 0, 0, 0, 0, 0, owner);
    const uint32_t id = d2rd32(owner + 0xC);
    const uint32_t type = d2rd32(owner), path = d2rd32(owner + 0x2C);
    const bool onPath = type == 2 || ((int32_t)type > 3 && (int32_t)type < 6);
    const uint32_t x = onPath ? d2rd32(path + 0xC) : path ? e->call(0xA454, 0, 0, 0, 0, 0, path) : 0;
    const uint32_t y = onPath ? d2rd32(path + 0x10) : path ? e->call(0xA3B8, 0, 0, 0, 0, 0, path) : 0;
    const uint64_t seed = (uint64_t)d2rd32(owner + 0x20) * 0x6ac690c5u + d2rd32(owner + 0x24);
    d2wr32(owner + 0x20, (uint32_t)seed);
    d2wr32(owner + 0x24, (uint32_t)(seed >> 32));
    uint32_t slot = (uint32_t)seed % 6, result = 0;
    for (int32_t n = (int32_t)count; n > 0; --n) {
        const uint32_t i = (mode * 6 + slot) * 2;
        slot = (slot + 5) % 6;
        uint32_t blk[10] = {cls, room, 0, s2, s3, 0, kOffs[i] + x, kOffs[i + 1] + y, 0xffffffffu, s5 & 0xffffu};
        const uint32_t obj = e->call(0xEEF10, 0, (uint32_t)(uintptr_t)blk);
        e->blk(0xEEF10, blk, 10);
        if (!obj) continue;
        result = 1;
        if (d2rd32(obj) == 1) {
            const uint32_t od = d2rd32(obj + 0x14), md = od ? d2rd32(od + 0x28) : 0;
            if (md) { d2wr32(md + 0x2C, id); d2wr32(md + 0x30, 1); d2wr32(md + 0x28, cls); }
        }
        e->call(0x20CF0, owner, 0, 0, 0, 0, cls);
        e->edi(0x20CF0, obj);
    }
    return result;
}
