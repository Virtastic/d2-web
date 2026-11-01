// 1.13c D2Common+0x1F720 (retail-layout, 32-bit image): take an item out of one inventory grid.
// fastcall(ecx unused, edx = item), stack (grid, rectangular), ret 8. Item data (+0x14) holds the grid links at +0x6C / +0x70 and
// the node position byte at +0x68; the grid is {first, last, width byte at +8, cells at +0xC}. A rectangular removal clears
// the item's w*h cell block (size from items.txt row +0x10F / +0x110 via 0x719A0, else the low byte of the grid address).
#include <cstdint>
struct D2Common1F720Env { uint32_t (*itemRec)(uint32_t classId); };
namespace {
inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
inline uint32_t& W(uint32_t a) { return *reinterpret_cast<uint32_t*>(P(a)); }
// A linked neighbour is only touched when it is an item unit with item data.
inline uint32_t Data(uint32_t unit) { return unit && W(unit) == 4 && W(unit + 0x14) && W(unit + 0x14) != 0xFFFFFFA4u ? W(unit + 0x14) : 0; }
}
extern "C" void D2Common_1F720_UnlinkGridRaw(const D2Common1F720Env* e, uint32_t item, uint32_t grid, int32_t rectangular)
{
    const uint32_t d = Data(item);
    if (!d) return;
    const uint32_t prev = W(d + 0x6C), next = W(d + 0x70);
    if (!prev) { if (W(grid) == item) W(grid) = next; }
    else if (const uint32_t pd = Data(prev)) W(pd + 0x70) = next;
    if (!next) { if (W(grid + 4) == item) W(grid + 4) = prev; }
    else if (const uint32_t nd = Data(next)) W(nd + 0x6C) = prev;
    if (W(grid + 0xC)) {
        const uint32_t x = W(W(item + 0x2C) + 0xC), y = W(W(item + 0x2C) + 0x10), stride = P(grid)[8];
        uint32_t* cells = reinterpret_cast<uint32_t*>(P(W(grid + 0xC)));
        if (!rectangular) {
            cells[stride * y + x] = 0;
        } else {
            uint32_t rows = grid & 0xFF, cols = grid & 0xFF;
            if (const uint32_t rec = e->itemRec(W(item + 4))) { rows = P(rec)[0x110]; cols = P(rec)[0x10F]; }
            for (uint32_t r = y; static_cast<int32_t>(r) < static_cast<int32_t>(y + rows); ++r)
                for (uint32_t c = 0; c < cols; ++c) cells[stride * r + x + c] = 0;
        }
    }
    W(d + 0x70) = 0; W(d + 0x6C) = 0; P(d)[0x68] = 0;
}
