// 1.13c D2Common+0x1A130 (retail-layout, 32-bit image): where in a level the party should be placed (a level's reference point).
// An id outside the table leaves the row null and the flag read at +0x90 faults (not captured). EAX = pX (out), ECX = drlg, EDX = level id, stack (mode, pY), ret 8; the finders get the outs as stack words (pY, pX). Both outs start at -1. The level is found in the DRLG's list (+0x47C, next +0x1AC, id +0x1D0)
// or made by 0x2D9B0; 0x2E360 runs once when its +0x10 is still zero. A levels.txt row (0x9C bytes, flag +0x90) picks the route: with the flag, 0x19FA0 (mode 13) or
// 0x19B60 (any other mode) answers; without it the finders 0x19FA0, 0x19AD0, 0x2D600 (given the level's middle, less 2) and 0x19A50 are tried in turn. A finder's record
// fills any out still -1 with the centre of its box (+0x34 / +0x38 origin, +0x3C / +0x40 size, halved toward zero). 0x1A080 shapes the answer.
#include <cstdint>
struct D2Common1A130Env {
    uint32_t levelsBase; uint32_t levelCount;                         // levels.txt rows (global 0xA0B38) and the count in sgptDataTables +0xC5C
    uint32_t (*lookup)(uint32_t drlg, uint32_t id);                   // 0x2D9B0
    void (*setup)(uint32_t level);                                    // 0x2E360
    uint32_t (*find19FA0)(uint32_t level, uint32_t pY, uint32_t pX);
    uint32_t (*find19B60)(uint32_t level, uint32_t mode, uint32_t pY, uint32_t pX);
    uint32_t (*find19AD0)(uint32_t level);                            // EAX = 0, EDI = level
    uint32_t (*find2D600)(uint32_t level, uint32_t esi, uint32_t edi);
    uint32_t (*find19A50)(uint32_t level);
    uint32_t (*shape)(uint32_t eax);                                  // 0x1A080
};
extern "C" uint32_t D2Common_1A130_LevelAnchorRaw(const D2Common1A130Env* e, uint32_t pX, uint32_t drlg, int32_t id, int32_t mode, uint32_t pY)
{
    auto P = [](uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); };
    auto W = [&](uint32_t a) -> int32_t& { return *reinterpret_cast<int32_t*>(P(a)); };
    const uint32_t row = (id >= 0 && static_cast<uint32_t>(id) < e->levelCount) ? e->levelsBase + static_cast<uint32_t>(id) * 0x9C : 0;
    W(pX) = -1; W(pY) = -1;
    uint32_t level = static_cast<uint32_t>(W(drlg + 0x47C));
    while (level && W(level + 0x1D0) != id) level = static_cast<uint32_t>(W(level + 0x1AC));
    if (!level) level = e->lookup(drlg, id);
    if (!W(level + 0x10)) e->setup(level);
    if (W(row + 0x90)) {
        const uint32_t r = mode == 0xD ? e->find19FA0(level, pY, pX) : e->find19B60(level, static_cast<uint32_t>(mode), pY, pX);
        return e->shape(r);
    }
    uint32_t box = e->find19FA0(level, pY, pX);
    if (!box) {
        box = e->find19AD0(level);
        if (!box) {
            const int32_t edi = W(level + 0x24) / 2 + W(level + 0x1C) - 2, esi = W(level + 0x28) / 2 + W(level + 0x20) - 2;
            box = e->find2D600(level, static_cast<uint32_t>(esi), static_cast<uint32_t>(edi));
            if (!box) { box = e->find19A50(level); if (!box) return 0; }
        }
    }
    if (W(pX) == -1 || W(pY) == -1) {
        W(pY) = W(box + 0x3C) / 2 + W(box + 0x34);   // the stack out (arg 2) gets the box's horizontal centre
        W(pX) = W(box + 0x40) / 2 + W(box + 0x38);   // the register out gets the vertical centre
    }
    return e->shape(box);
}
