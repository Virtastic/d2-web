// 1.13c D2Common+0x1E1B0 / 0x208E0 / 0x20A30 / 0x20D20 / 0x81FB0: inventory grid fetch, free-position search, spot reservation on retail's 32-bit images.
#include "inv_place.h"
#include <cstring>
uint32_t D2Common_20520_Place(uint32_t, uint8_t, uint32_t, uint32_t, uint32_t, uint8_t);
uint32_t D2Common_20560_Place(uint32_t, uint8_t, uint32_t, uint32_t, uint32_t, uint8_t);
uint32_t D2Common_205A0_Place(uint32_t, uint8_t, uint32_t, uint32_t, uint32_t, uint8_t);
uint32_t D2Common_205E0_Place(uint32_t, uint8_t, uint32_t, uint32_t, uint32_t, uint8_t);
namespace {
inline uint8_t* P(uint32_t a) { return (uint8_t*)(uintptr_t)a; }
inline uint32_t& W(uint8_t* b, int o) { return *(uint32_t*)(b + o); }
// 0x81FB0: inventory.txt row's 24 bytes at +0x10 (retail aborts on a bad index; here that is a failure). Bound is > (retail-exact; vector 81fb0 covers rec==count).
bool LayoutInfo(const InvEnv& e, int32_t rec, uint8_t* out24)
{
    if (!e.inv || (uint32_t)rec > e.invCount) return false;
    memcpy(out24, e.inv + (uint32_t)rec * 0xF0 + 0x10, 24);
    return true;
}
// 0x231A0 + 0x719A0: items.txt width/height bytes (+0x10F/+0x110) of the item's class; left untouched when no record.
void Dims(const InvEnv& e, uint32_t item, uint8_t* w, uint8_t* h)
{
    uint8_t* u = P(item);
    if (!u || W(u, 0) != 4) return;
    uint32_t c = W(u, 4);
    if (c >= e.itemCount || !e.items) return;
    const uint8_t* r = e.items + c * 0x1A8;
    *w = r[0x10F]; *h = r[0x110];
}
int32_t OwnerRecord(uint32_t owner)   // 0x2FB60(owner, 0, 1); the player class table (type 0) is not modelled
{
    uint8_t* o = P(owner);
    if (!o) return -1;
    uint32_t t = W(o, 0);
    if (t == 1) return 5;
    if (t == 2) return W(o, 4) == 0x152 ? 10 : W(o, 4) == 0x153 ? 11 : -1;
    return -1;
}
}
// 1.13c D2Common+0x81FB0: stdcall(rec, sub, out24); row = rec + sub*16.
bool D2Common_81FB0_GetLayout(const InvEnv& e, int32_t rec, int32_t sub, uint8_t* out24) { return LayoutInfo(e, rec + sub * 16, out24); }
// EAX index, EBX inventory, stack info (2 bytes: width, height) or null.
uint32_t D2Common_1E1B0_GetGrid(const InvEnv& e, uint32_t inv, int32_t index, uint32_t info)
{
    uint8_t* v = P(inv);
    auto zero = [](uint8_t* b, uint32_t n) { memset(b, 0, n); };
    if (index < (int32_t)W(v, 0x18)) {
        uint8_t* g = P(W(v, 0x14)) + (uint32_t)index * 0x10;
        uint32_t cells = W(g, 0xC);
        if (!info) return cells ? (uint32_t)(uintptr_t)g : 0;
        const uint8_t* i = P(info);
        if (!cells) {
            g[8] = i[0]; g[9] = i[1];
            uint32_t a = e.alloc((uint32_t)g[8] * g[9] * 4, e.ctx); W(g, 0xC) = a;
            zero(P(a), (uint32_t)g[8] * g[9] * 4);
        }
        if (g[8] != i[0] || g[9] != i[1]) return 0;
        return (uint32_t)(uintptr_t)g;
    }
    if (!info) return 0;
    const uint8_t* i = P(info);
    uint32_t n = W(v, 0x18);
    uint32_t a = e.realloc(W(v, 0x14), (uint32_t)index * 0x10 + 0x10, e.ctx); W(v, 0x14) = a;
    zero(P(a) + n * 0x10, ((uint32_t)index - n + 1) * 0x10);
    W(v, 0x18) = (uint32_t)index + 1;
    uint8_t* g = P(a) + (uint32_t)index * 0x10;
    g[8] = i[0]; g[9] = i[1];
    uint32_t c = e.alloc((uint32_t)g[8] * g[9] * 4, e.ctx); W(g, 0xC) = c;
    zero(P(c), (uint32_t)g[8] * g[9] * 4);
    return (uint32_t)(uintptr_t)g;
}
// stdcall (inv, item, record id, &x, &y, page).
uint32_t D2Common_208E0_GetFreePosition(const InvEnv& e, uint32_t inv, uint32_t item, int32_t rec, uint32_t pX, uint32_t pY, uint8_t page)
{
    uint8_t* v = P(inv); uint8_t* u = P(item);
    if (!v || W(v, 0) != 0x01020304 || !u || W(u, 0) != 4 || !W(u, 0x14) || W(u, 0x14) == 0xFFFFFFA4) return 0;
    uint8_t w = (uint8_t)inv, h = (uint8_t)item;   // retail's out bytes alias its argument slots
    Dims(e, item, &w, &h);
    if (!w || !h) return 0;
    uint8_t info[24];
    if (!LayoutInfo(e, rec, info)) return 0;
    uint32_t grid = D2Common_1E1B0_GetGrid(e, inv, (int32_t)page + 2, (uint32_t)(uintptr_t)info);
    if (!grid) return 0;
    if (h == 1) return D2Common_205E0_Place(inv, h, grid, pX, pY, w);
    if (h == 3 && w < 3) return D2Common_205A0_Place(inv, h, grid, pX, pY, w);
    if (h == 2 && w == 2) return D2Common_20560_Place(inv, h, grid, pX, pY, w);
    return D2Common_20520_Place(inv, h, grid, pX, pY, w);
}
// EAX inventory, stack (owner, item): finds a spot on page 0 and marks its cells taken (-1).
uint32_t D2Common_20A30_ReserveSpot(const InvEnv& e, uint32_t inv, uint32_t owner, uint32_t item)
{
    uint8_t* v = P(inv);
    if (!v || W(v, 0) != 0x01020304) return 0;
    int32_t rec = OwnerRecord(owner);
    uint8_t info[24];
    if (!LayoutInfo(e, rec, info)) return 0;
    uint32_t grid = D2Common_1E1B0_GetGrid(e, inv, 2, (uint32_t)(uintptr_t)info);
    if (!grid) return 0;
    int32_t x = 0, y = 0;
    if (!D2Common_208E0_GetFreePosition(e, inv, item, rec, (uint32_t)(uintptr_t)&x, (uint32_t)(uintptr_t)&y, 0)) return 0;
    uint8_t w = (uint8_t)owner, h = (uint8_t)item;
    Dims(e, item, &w, &h);
    uint8_t* g = P(grid);
    if (!w || !h || x < 0 || w + x > g[8] || y < 0 || h + y > g[9]) return 0;
    for (int32_t r = y; r < y + h; ++r)
        for (int32_t c = x; c < x + w; ++c)
            W(P(W(g, 0xC)), ((uint32_t)g[8] * r + c) * 4) = 0xFFFFFFFF;
    return 1;
}
// 1.13c D2Common+0x20D20: EAX = unit, EDI = target inventory, stack owner; reserves every storage-page item (itemdata+0x45 == 2) of unit's inventory.
uint32_t D2Common_20D20_ReserveAll(const InvEnv& e, uint32_t unit, uint32_t inv, uint32_t owner)
{
    uint8_t* u = P(unit);
    uint32_t si = u ? W(u, 0x60) : 0;
    if (!si || W(P(si), 0) != 0x01020304) return 1;
    for (uint32_t it = W(P(si), 0xC); it;) {
        uint8_t* p = P(it);
        if (W(p, 0) != 4) return 1;
        uint32_t d = W(p, 0x14), next = (d == 0 || d == (uint32_t)-0x5c) ? 0 : W(P(d), 0x64);
        if (d && *(P(d) + 0x45) == 2 && !D2Common_20A30_ReserveSpot(e, inv, owner, it)) return 0;
        it = next;
    }
    return 1;
}
// 1.13c D2Common+0x20B50: stdcall(inv, flag, index, rec); can the item in the inventory's slot `index` (grid 0, 13 entries) be placed in grid page 2? (x/y outputs are discarded)
uint32_t D2Common_20B50_CanPlaceSlot(const InvEnv& e, uint32_t inv, uint32_t flag, int32_t index, int32_t rec)
{
    uint8_t* v = P(inv);
    if (!v || W(v, 0) != 0x01020304 || index < 0 || index >= 13) return 0;
    static const uint8_t info[2] = {13, 1};   // global 0x9D378, constant in the image
    uint32_t grid = D2Common_1E1B0_GetGrid(e, inv, 0, (uint32_t)(uintptr_t)info);
    if (!grid) return 0;
    uint32_t item = W(P(W(P(grid), 0xC)), (uint32_t)index * 4);
    if (!item || !flag) return 0;
    int32_t x = 0, y = 0;
    return D2Common_208E0_GetFreePosition(e, inv, item, rec, (uint32_t)(uintptr_t)&x, (uint32_t)(uintptr_t)&y, 0) ? 1 : 0;
}
