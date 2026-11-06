// Tile record fill: reset a caller-supplied 0x30-byte record for a tile handle and, when a room is given,
// its screen and room-relative coords; the flag word starts from the packed tile flags' layer bits.
// Retail: ESI = record, EAX = room (may be 0), EDX = x, ECX = y, stack flags, handle (ret 8). The
// handle-flags import is a seam.
#include <cstdint>
#include <cstring>

struct D2TileFillSeam {
    uint32_t (*pfnHandleFlags)(uint32_t hTile);
};

namespace {
uint32_t R32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
void W32(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }
void W8(uint32_t a, uint8_t v) { memcpy((void*)(uintptr_t)a, &v, 1); }
}

// 1.13c D2Common+0x68DF0
extern "C" void D2Common_68DF0_FillTileRec(const D2TileFillSeam* s, uint32_t rec, uint32_t pRoom, int32_t x, int32_t y,
                                           uint32_t f, uint32_t hTile)
{
    W32(rec + 0x18, hTile);
    W32(rec + 0x1c, 0); W32(rec + 0x14, 0); W32(rec + 0x24, 0);
    W8(rec + 0x29, 0xff); W8(rec + 0x2a, 0xff); W8(rec + 0x28, 0xff);
    if (pRoom) {
        W32(rec + 8, (uint32_t)(x - (int32_t)R32(pRoom + 0x34)));
        W32(rec + 0, (uint32_t)((x - (y + 1)) * 0x50));
        W32(rec + 0xc, (uint32_t)(y - (int32_t)R32(pRoom + 0x38)));
        W32(rec + 4, (uint32_t)((((y + 1 + x) * 0x50) >> 1) + 0x28));
    }
    uint32_t r = ((f >> 18 & 3) + 1) << 14;
    if ((int8_t)f < 0) r |= 1;
    if (f & 0x10000000) r |= 0x102;
    if (f & 0x20000) r |= 0x40;
    if (f & 0x10000) r |= 0x80;
    if (f & 8) r |= 4;
    r = (int32_t)f < 0 ? r | 8 : r & ~8u;
    if (f & 0x4000000) r |= 0x20c;
    if (f & 0x20000000) r |= 0x800;
    if (f & 4) r |= 0x2000;
    W32(rec + 0x14, r);
    const uint32_t h = s->pfnHandleFlags(hTile);
    if (h & 1) r |= 4;
    if (h & 4) r |= 0x800;
    W32(rec + 0x14, r);
}
