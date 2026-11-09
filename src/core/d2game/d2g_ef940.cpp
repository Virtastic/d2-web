// 1.13c D2Game+0xEF940 -- place a spawn group from a footprint list. Stdcall with EAX = list, ECX = class, EBX = room, six stack
// words (w1, x, y, owner record, start index, flags), ret 0x18. List: +0 count, +8 byte (bit 0 = bump the start index per record),
// records of {dx, dy, kind} at +0xC, stride 0xC. Flags bit 0 forces kind 4. Kinds: 0 spawn (0xEF650, ECX = start index, EDX = room);
// 1 / 2 0xEF8B0 (EAX = start index, ECX = room, EDX = w1; the second stack flag 0 / 1) keeping the result as the group handle;
// 3 0xEF7D0 on that handle; 4 spawn and fail the whole call on a miss; 5 spawn owned by the owner record (stamps the new
// monster's +0x2C / +0x30 / +0x28); kind > 5 skipped. Returns 1 if anything spawned.
// 0xEF650 stack: cls, w1, x', y', -1, flags. 0xEF8B0 stack: x', y', -1, flag, 0. 0xEF7D0 stack: handle, x', y', start, room, -1, 0.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_ef940(uint32_t list, uint32_t cls, uint32_t room, uint32_t w1, uint32_t x, uint32_t y, uint32_t owner,
                              uint32_t start, uint32_t flags, const D2Seam* e)
{
    auto spawn = [&](uint32_t px, uint32_t py, uint32_t fl) {
        const uint32_t a[6] = {cls, w1, px, py, 0xffffffffu, fl};
        const uint32_t r = e->call(0xEF650, 0, start, room, 0, 0, a[0], a[1], a[2], a[3], a[4], a[5]);
        e->blk(0xEF650, a, 6);
        return r;
    };
    const bool bump = (d2rd8(list + 8) & 1) != 0, force = (flags & 1) != 0;
    const int32_t count = (int32_t)d2rd32(list);
    uint32_t result = 0, handle = 0;
    uint32_t rec = list;
    for (int32_t i = 0; i < count; ++i) {
        rec += 0xC;
        const uint32_t px = d2rd32(rec) + x, py = d2rd32(rec + 4) + y, kind = force ? 4 : d2rd32(rec + 8);
        if (kind <= 5) {
            uint32_t r = 0;
            bool took = false;
            switch (kind) {
            case 0: r = spawn(px, py, flags); took = r != 0; break;
            case 1: case 2:
                r = handle = e->call(0xEF8B0, start, room, w1, 0, 0, px, py, 0xffffffffu, kind == 2, 0);
                took = r != 0; break;
            case 3:
                if (handle) {
                    const uint32_t a[7] = {handle, px, py, start, room, 0xffffffffu, 0};
                    r = e->call(0xEF7D0, 1, 0, 0, 0, 0, a[0], a[1], a[2], a[3], a[4], a[5], a[6]);
                    e->blk(0xEF7D0, a, 7);
                    took = r != 0;
                }
                break;
            case 4:
                result = 1;
                if (!spawn(px, py, flags)) return 0;
                break;
            case 5:
                if (owner) {
                    r = spawn(px, py, 0);
                    if (r) {
                        const uint32_t oid = d2rd32(owner + 0xC), otype = d2rd32(owner);
                        if (d2rd32(r) == 1) {
                            const uint32_t od = d2rd32(r + 0x14), md = od ? d2rd32(od + 0x28) : 0;
                            if (md) { d2wr32(md + 0x2C, oid); d2wr32(md + 0x30, otype); d2wr32(md + 0x28, cls); }
                        }
                        result = 1;
                    }
                }
                break;
            }
            if (took) result = 1;
        }
        if (bump) ++start;
    }
    return result;
}
