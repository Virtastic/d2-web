// Tile sound emitter: unless the tile (if any) is already marked, find the sound row for (room owner list,
// packed tile id), place it at the tile's sub-cell inside the room box and queue it on the room's emitter
// list; then mark the tile. Retail: ECX = room, EAX = x, stack tile, id, y, fallback type (ret 0x10). The
// row table (0x1c-byte rows: value, kind, offset x, offset y) and the callees are seams.
#include <cstdint>
#include <cstring>

struct D2TileSoundSeam {
    const uint8_t* pRows;
    int32_t (*pfnFindRow)(uint32_t bWall, uint32_t nList, uint32_t nId);            // EBX = bWall
    uint32_t (*pfnMapValue)(uint32_t nValue);                                         // EAX = value
    int32_t (*pfnRoll)(uint32_t nMax, uint32_t pSeed);                                // EAX = max, ECX = seed
    void (*pfnQueue)(uint32_t pPool, uint32_t pRoom, uint32_t nKind, uint32_t nValue, uint32_t bMapped, int32_t x, int32_t y);  // ECX, ESI
};

namespace {
uint32_t R32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
void W32(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }
int32_t Row(const uint8_t* p, int o) { int32_t v; memcpy(&v, p + o, 4); return v; }
}

// 1.13c D2Common+0x68970
extern "C" void D2Common_68970_TileSound(const D2TileSoundSeam* s, uint32_t pRoom, int32_t x, uint32_t pTile, uint32_t nId,
                                         int32_t y, int32_t nFallbackType)
{
    if (pTile && (R32(pTile + 0x14) & 0x20)) return;
    const uint32_t bWall = (pTile ? (int32_t)R32(pTile + 0x1c) : nFallbackType) == 9;
    const uint32_t pOwner = R32(pRoom + 0x58);
    const int32_t i = s->pfnFindRow(bWall, R32(pOwner + 0x1d0), nId);
    if (i == -1) return;
    const uint8_t* e = s->pRows + i * 0x1c;
    const int32_t px = (x - (int32_t)R32(pRoom + 0x34)) * 5 + Row(e, 8);
    const int32_t py = (y - (int32_t)R32(pRoom + 0x38)) * 5 + Row(e, 0xc);
    if (px < 0 || py < 0 || px >= (int32_t)R32(pRoom + 0x3c) * 5 || py >= (int32_t)R32(pRoom + 0x40) * 5) return;
    uint32_t nValue = (uint32_t)Row(e, 0);
    const uint32_t nKind = (uint32_t)Row(e, 4);
    uint32_t bMapped = 0;
    if (nKind == 1) { nValue = s->pfnMapValue(nValue); bMapped = 1; }
    if (!(nKind == 2 && (int32_t)nValue >= 0x5b && (int32_t)nValue <= 0x5c && s->pfnRoll(3, pRoom + 0x14) == 0))
        s->pfnQueue(R32(R32(pOwner + 0x1b4) + 0x478), pRoom, nKind, nValue, bMapped, px, py);
    if (pTile) W32(pTile + 0x14, R32(pTile + 0x14) | 0x20);
}
