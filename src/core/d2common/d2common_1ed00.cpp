// 1.13c D2Common+0x1ED00 and +0x1EDF0 (retail-layout, 32-bit image): walk the item chain of one inventory grid for the first
// item whose class matches the link-table index of an item code. Stdcall(inv, code, startItem), ret 0xC. The two routines are
// identical except for the grid they read: 0x1ED00 uses grid 2, 0x1EDF0 grid 0; both fetch it with 0x1E1B0 and the constant
// size pair {13,1}. The code-to-class index comes from Fog ordinal 10213 (table pointer in D2Common global 0x9FF6C, a
// third argument of 0), and the items.txt row base global 0x9FB98 must give a non-null row address for that index.
#include <cstdint>
#include "inv_place.h"
struct D2CommonFindEnv {
    int32_t (*link)(uint32_t tbl, uint32_t code);   // Fog 10213 stand-in
    uint32_t linkTbl;                               // the value of global 0x9FF6C
    uint32_t itemRows;                              // the value of global 0x9FB98
    const InvEnv* inv;
};
namespace {
inline uint8_t* P(uint32_t a) { return (uint8_t*)(uintptr_t)a; }
inline uint32_t W(uint32_t a, uint32_t o) { return *(const uint32_t*)(P(a) + o); }
// Next item of the chain: the node's type must be 4 (item) with a usable data pointer, whose +0x70 holds the next node.
bool Next(uint32_t& p)
{
    if (W(p, 0) != 4) return false;
    uint32_t d = W(p, 0x14);
    if (d == 0 || d == 0xFFFFFFA4u) return false;
    p = W(d, 0x70);
    return true;
}
uint32_t Find(const D2CommonFindEnv& e, uint32_t inv, uint32_t code, uint32_t start, int32_t grid)
{
    if (!inv || W(inv, 0) != 0x1020304) return 0;
    int32_t idx = e.link(e.linkTbl, code);
    if (idx < 0) return 0;
    if ((uint32_t)idx * 0x1A8u + e.itemRows == 0) return 0;
    static const uint8_t kInfo[2] = { 13, 1 };
    uint32_t g = D2Common_1E1B0_GetGrid(*e.inv, inv, grid, (uint32_t)(uintptr_t)kInfo);
    if (!g) return 0;
    uint32_t p = W(g, 0);
    if (start) {
        if (!p) return 0;
        while (p != start) {
            if (!Next(p)) return 0;
            if (!p) return 0;
        }
    }
    while (p) {
        if ((int32_t)W(p, 4) == idx) {
            uint32_t t = W(p, 0), x = W(p, 0x2C), v;
            if (t == 2 || (t > 3 && t < 6)) v = W(x, 0xC);
            else {
                if (!x) return p;
                v = *(const uint16_t*)(P(x) + 2);
            }
            if (v < 0xB) return p;
        }
        if (!Next(p)) return 0;
    }
    return 0;
}
}
// 1.13c D2Common+0x1ED00
extern "C" uint32_t D2Common_1ED00_FindItemRaw(const D2CommonFindEnv* e, uint32_t inv, uint32_t code, uint32_t start) { return Find(*e, inv, code, start, 2); }
// 1.13c D2Common+0x1EDF0
extern "C" uint32_t D2Common_1EDF0_FindItemRaw(const D2CommonFindEnv* e, uint32_t inv, uint32_t code, uint32_t start) { return Find(*e, inv, code, start, 0); }
