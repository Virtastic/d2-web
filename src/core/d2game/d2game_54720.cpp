// 1.13c D2Game+0x54720: ebx = game, esi = request record (packet fields). Builds the 0x84-byte item-create block (zeroed; fields copied from the record; the NUL-terminated string at record +0x32 copied to block +0x58), hands it to #11490 (stdcall: game, block, 0); a null item returns 0, otherwise #B1AA(item, record[0x20]) and #A92E(item, byte record[0x30]) (stdcall, 2 words each) and the item is returned.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_54720(uint32_t ebx, uint32_t esi, const D2Seam* e)
{
    uint32_t blk[33] = {};
    uint8_t* b = (uint8_t*)blk;
    auto W = [&](uint32_t o, uint32_t v) { memcpy(b + o, &v, 4); };
    W(0x08, ebx);
    W(0x0C, d2rd32(esi + 2));
    W(0x14, d2rd32(esi + 6));
    W(0x18, d2rd8(esi + 0xA));
    W(0x1C, d2rd32(esi + 0x18));
    W(0x20, d2rd32(esi + 0x1C));
    W(0x24, 0);
    b[0x28] = 1; b[0x29] = 0;
    const uint32_t w42 = d2rd16(esi + 0x42);
    b[0x2A] = (uint8_t)w42; b[0x2B] = (uint8_t)(w42 >> 8);
    W(0x2C, 1);
    W(0x30, d2rd8(esi + 0xB));
    W(0x34, d2rd32(esi + 0xC));
    W(0x38, d2rd32(esi + 0x10));
    W(0x3C, d2rd32(esi + 0x14));
    W(0x40, d2rd32(esi + 0x20));
    W(0x44, d2rd32(esi + 0x24));
    W(0x48, d2rd32(esi + 0x28));
    W(0x4C, d2rd32(esi + 0x2C));
    for (uint32_t i = 0;; ++i) { const uint8_t c = (uint8_t)d2rd8(esi + 0x32 + i); b[0x58 + i] = c; if (!c) break; }
    b[0x50] = (uint8_t)d2rd8(esi + 0x31);
    const uint32_t item = e->call(0x11490, 0, 0, 0, 0, 0, ebx, (uint32_t)(uintptr_t)blk, 0);
    e->blk(0x11490, blk, 33);
    if (!item) return 0;
    e->call(0xB1AA, 0, 0, 0, 0, 0, item, d2rd32(esi + 0x20));
    e->call(0xA92E, 0, 0, 0, 0, 0, item, d2rd8(esi + 0x30));
    return item;
}
