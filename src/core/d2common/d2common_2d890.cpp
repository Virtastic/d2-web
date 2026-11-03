// 1.13c D2Common+0x2D890: place a DRLG level from its levels.txt row (regs: EBX = DRLG, EDI = level; void). The row is the
// level's id (+0x1D0). Its size and origin come from the row column for the DRLG difficulty (byte +0x450). The origin is
// then offset by the parent level's box (parent id at row+0x2C, found on the DRLG list or made by 0x2D9B0).
// The retail loop at the end walks the list with no effect and is not ported.
#include <cstdint>

struct D2Common2D890Env {
    uint32_t levelsBase;                                                        // levels.txt rows (0x9C each)
    uint32_t levelCount;                                                        // the count in the data tables (+0xC5C)
    uint32_t (*node2D9B0)(uint32_t drlg, int32_t id);                           // 0x2D9B0: create a level (ECX = DRLG, EDX = id)
};

extern "C" void D2Common_2D890_SetLevelPosition(uint32_t drlg, uint32_t level, const D2Common2D890Env* e)
{
    auto W = [](uint32_t a) -> uint32_t& { return *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(a)); };
    const int32_t id = static_cast<int32_t>(W(level + 0x1D0));
    const bool inRange = id >= 0 && static_cast<uint32_t>(id) < e->levelCount;
    const uint32_t row = inRange ? e->levelsBase + static_cast<uint32_t>(id) * 0x9C : 0;
    const uint32_t diff = *reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(drlg + 0x450));
    const uint32_t parent = row ? W(row + 0x2C) : 0;
    W(level + 0x24) = row ? W(row + 0x0C + diff * 4) : 0;
    W(level + 0x28) = row ? W(row + 0x18 + diff * 4) : 0;
    uint32_t ox = 0, oy = 0;
    if (parent) {
        uint32_t node = W(drlg + 0x47C);
        while (node && static_cast<int32_t>(W(node + 0x1D0)) != static_cast<int32_t>(parent)) node = W(node + 0x1AC);
        if (!node) node = e->node2D9B0(drlg, static_cast<int32_t>(parent));
        ox = W(node + 0x1C);
        oy = W(node + 0x20);
    }
    W(level + 0x1C) = (row ? W(row + 0x24) : 0) + ox;
    W(level + 0x20) = (row ? W(row + 0x28) : 0) + oy;
}
