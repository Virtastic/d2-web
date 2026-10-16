// 1.13c D2Common+0x1E310 / 0x1F630 / 0x1F690 / 0x2F270 / 0x2F2C0 / 0x1E360 / 0x1F370 / 0x1F410 / 0x1F4E0 / 0x1F580 / 0x20520 / 0x20560 / 0x205A0 / 0x205E0 / (0x1E010 inlined): inventory grid/list helpers on retail's 32-bit images.
#include <cstdint>
namespace {
inline uint8_t* P(uint32_t a) { return (uint8_t*)(uintptr_t)a; }
inline uint32_t& W(uint8_t* b, int o) { return *(uint32_t*)(b + o); }
// item data node (+0x5c) of a type-4 unit with data, else null (the 0x1E790 test)
inline uint8_t* Node(uint32_t u) { uint8_t* p = P(u); return (p && W(p, 0) == 4 && W(p, 0x14)) ? P(W(p, 0x14)) + 0x5c : nullptr; }
}
// 0x1E310: write pItem into rows r0..r0+rows, columns c0..c0+cols of the grid's cell array (row stride = grid byte +8).
void D2Common_1E310_FillCells(uint8_t rows, int32_t r0, int32_t c0, uint32_t grid, uint8_t cols, uint32_t val)
{
    uint8_t* g = P(grid);
    for (int32_t r = r0; r < r0 + rows; ++r)
        for (int32_t c = c0; c < c0 + cols; ++c)
            W(P(W(g, 0xc)), ((uint32_t)g[8] * r + c) * 4) = val;
}
// 0x1F690: append pItem to inventory pInv's item list (+0xc first, +0x10 last; node +0 parent, +4 prev, +8 next).
void D2Common_1F690_AppendItem(uint32_t pItem, uint32_t pInv)
{
    uint8_t *n = Node(pItem), *inv = P(pInv);
    if (!n) return;
    if (!W(inv, 0x10)) W(inv, 0xc) = pItem;
    else if (uint8_t* l = Node(W(inv, 0x10))) W(l, 8) = pItem;
    W(n, 4) = W(inv, 0x10);
    W(inv, 0x10) = pItem;
    W(n, 0) = pInv;
}
// 0x1F630: when the item's parent (node +0) == pInv, append it to the grid's list (+0 first, +4 last; node +0x10 prev, +0x14 next).
void D2Common_1F630_AppendGridItem(uint32_t pItem, uint32_t grid, uint32_t pInv)
{
    uint8_t *n = Node(pItem), *g = P(grid);
    if (!n || W(n, 0) != pInv) return;
    if (!W(g, 4)) W(g, 0) = pItem;
    else if (uint8_t* l = Node(W(g, 4))) W(l, 0x14) = pItem;
    W(n, 0x10) = W(g, 4);
    W(g, 4) = pItem;
}
// 0x2F270 / 0x2F2C0: static path y (+0x10) / x (+0xc) for unit types 2, 4, 5 (others: retail logs and _exit(-1), not modelled).
void D2Common_2F270_SetY(uint32_t pUnit, int32_t y) { W(P(W(P(pUnit), 0x2c)), 0x10) = y; }
void D2Common_2F2C0_SetX(uint32_t pUnit, int32_t x) { W(P(W(P(pUnit), 0x2c)), 0xc) = x; }
// 0x1E360: 1 when every cell of rows y..y+h-1, columns x..x+w-1 is empty, else 0. No bounds check against the grid size
// (1.10f had one): out-of-range cells are read by row stride.
uint32_t D2Common_1E360_CellsFree(uint32_t grid, int32_t y, uint8_t w, int32_t x, uint8_t h)
{
    uint8_t* g = P(grid);
    for (int32_t r = y; r < y + h; ++r)
        for (int32_t c = x; c < x + w; ++c)
            if (W(P(W(g, 0xc)), ((uint32_t)g[8] * r + c) * 4)) return 0;
    return 1;
}
namespace {
// 0x1E010: placement weight (count of blocked neighbour cells, 0xFF when fully hugged); grid cells by row stride g[8], height g[9].
uint8_t Weight(uint8_t* g, int32_t y, uint8_t h, int32_t x, uint8_t w)
{
    uint8_t* c = P(W(g, 0xc));
    auto cell = [&](int32_t cx, int32_t cy) { return W(c, ((uint32_t)g[8] * cy + cx) * 4) != 0; };
    uint32_t r = 0;
    if (x > 0) { for (int i = 0; i < h; ++i) r += cell(x - 1, y + i); } else r = h;
    if (x + w < g[8]) { for (int i = 0; i < h; ++i) r += cell(x + w, y + i); } else r += h;
    if (y > 0) { for (int i = 0; i < w; ++i) r += cell(x + i, y - 1); } else r += w;
    if (y + h < g[9]) { for (int i = 0; i < w; ++i) r += cell(x + i, y + h); } else r += w;
    return (int32_t)r >= (w + h) * 2 ? 0xff : (uint8_t)r;
}
// Shared finder body. xmajor: outer loop over x (else y); descO/descI: outer/inner loop counts down; weighted: keep best 0x1E010 weight (else first fit).
// rows = a 0x1E360-fit test at (x,y) for item w x h, after the retail cell/bounds pre-checks.
uint32_t Find(uint32_t grid, uint32_t pX, uint32_t pY, uint8_t w, uint8_t h, bool xmajor, bool descO, bool descI, bool weighted)
{
    uint8_t* g = P(grid);
    int32_t nx = g[8], ny = g[9], no = xmajor ? nx : ny, ni = xmajor ? ny : nx;
    uint8_t best = 0;
    for (int32_t o = descO ? no - 1 : 0; descO ? o >= 0 : o < no; o += descO ? -1 : 1)
        for (int32_t i = descI ? ni - 1 : 0; descI ? i >= 0 : i < ni; i += descI ? -1 : 1) {
            int32_t x = xmajor ? o : i, y = xmajor ? i : o;
            if (W(P(W(g, 0xc)), ((uint32_t)g[8] * y + x) * 4)) continue;
            if (w + x > g[8] || h + y > g[9]) continue;
            if (!D2Common_1E360_CellsFree(grid, y, w, x, h)) continue;
            if (!weighted) { W(P(pX), 0) = x; W(P(pY), 0) = y; return 1; }
            uint8_t s = Weight(g, y, h, x, w);
            if (s > best) {
                W(P(pX), 0) = x; best = s; W(P(pY), 0) = y;
                if (s == 0xff) return 1;
            }
        }
    return weighted && best;
}
}
// BL = item height (register), stack grid, &outX, &outY, item width. Returns 1 and writes x,y on a hit.
// 0x1F370: x outer ascending, y inner ascending, first fit.
uint32_t D2Common_1F370_FindFirst(uint8_t h, uint32_t grid, uint32_t pX, uint32_t pY, uint8_t w) { return Find(grid, pX, pY, w, h, true, false, false, false); }
// 0x1F410: y outer, x inner, both ascending, best 0x1E010 weight.
uint32_t D2Common_1F410_FindBest(uint8_t h, uint32_t grid, uint32_t pX, uint32_t pY, uint8_t w) { return Find(grid, pX, pY, w, h, false, false, false, true); }
// 0x1F4E0: x outer descending, y inner ascending, first fit.
uint32_t D2Common_1F4E0_FindFirstRev(uint8_t h, uint32_t grid, uint32_t pX, uint32_t pY, uint8_t w) { return Find(grid, pX, pY, w, h, true, true, false, false); }
// 0x1F580: x outer descending, y inner descending, best weight.
uint32_t D2Common_1F580_FindBestRev(uint8_t h, uint32_t grid, uint32_t pX, uint32_t pY, uint8_t w) { return Find(grid, pX, pY, w, h, true, true, true, true); }
// 0x20520 / 0x20560 / 0x205A0 / 0x205E0: finder wrappers. edi=grid, esi=&outX, edx=&outY, ecx=width, stack inv, height (ret 8).
// inv null: nothing. inv+8 a non-null pointer whose first dword is 0: best-weight finder, else first-fit finder.
static uint32_t Wrap(uint32_t inv, uint8_t h, uint32_t grid, uint32_t pX, uint32_t pY, uint8_t w, bool rev)
{
    if (!inv) return 0;
    uint32_t q = W(P(inv), 8);
    bool best = q && !W(P(q), 0);
    if (rev) return best ? D2Common_1F580_FindBestRev(h, grid, pX, pY, w) : D2Common_1F4E0_FindFirstRev(h, grid, pX, pY, w);
    return best ? D2Common_1F410_FindBest(h, grid, pX, pY, w) : D2Common_1F370_FindFirst(h, grid, pX, pY, w);
}
uint32_t D2Common_20520_Place(uint32_t i, uint8_t h, uint32_t g, uint32_t x, uint32_t y, uint8_t w) { return Wrap(i, h, g, x, y, w, false); }
uint32_t D2Common_20560_Place(uint32_t i, uint8_t h, uint32_t g, uint32_t x, uint32_t y, uint8_t w) { return Wrap(i, h, g, x, y, w, false); }
uint32_t D2Common_205A0_Place(uint32_t i, uint8_t h, uint32_t g, uint32_t x, uint32_t y, uint8_t w) { return Wrap(i, h, g, x, y, w, false); }
uint32_t D2Common_205E0_Place(uint32_t i, uint8_t h, uint32_t g, uint32_t x, uint32_t y, uint8_t w) { return Wrap(i, h, g, x, y, w, true); }
