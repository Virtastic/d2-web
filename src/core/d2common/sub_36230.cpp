// 1.13c D2Common+0x36230
#include <cstddef>
#include <cstdint>
// Retail's 32-bit path image, only the offsets this slice reads or writes.
struct Path32 { uint16_t pad00, x; uint16_t pad04, y; uint8_t pad08[0x10 - 8]; uint16_t tx, ty; uint8_t pad14[0x1C - 0x14]; uint32_t room; uint8_t pad20[0x24 - 0x20]; uint32_t moveCur, moveEnd; uint8_t pad2c[0x30 - 0x2C]; uint32_t owner, flags; uint8_t pad38[0x54 - 0x38]; uint16_t step; uint16_t pad56; uint32_t pLink; uint8_t pad5c[0xA0 - 0x5C]; };
static_assert(offsetof(Path32, x) == 2 && offsetof(Path32, y) == 6 && offsetof(Path32, tx) == 0x10 && offsetof(Path32, room) == 0x1C && offsetof(Path32, moveCur) == 0x24 && offsetof(Path32, moveEnd) == 0x28 && offsetof(Path32, flags) == 0x34 && offsetof(Path32, step) == 0x54 && offsetof(Path32, pLink) == 0x58, "36230 path offsets");
// The no-move slice: null path -> 0; flag 0x40000, a linked path (+0x58), or a pending in-range move into a live room is outside it.
// Otherwise step clears; an out-of-range/identical/zero move clears both counters; flag 0x20 drops; result 0.
uint32_t D2Common_36230_PathStep(Path32* p)
{
    if (!p) return 0;
    p->step = 0;
    if (p->x || p->y) {
        int32_t dx = (int32_t)p->tx - p->x, dy = (int32_t)p->ty - p->y;
        // dx/dy in [-100,100] with a live room: the per-class step callback runs (not ported).
        (void)dx; (void)dy;
    }
    p->moveCur = 0; p->moveEnd = 0; p->flags &= ~0x20u;
    return 0;
}
