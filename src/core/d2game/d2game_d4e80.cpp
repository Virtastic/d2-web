// 1.13c D2Game+0xD4E80 (EAX item id, ECX player, EDX game; stack page, direct, out; ret 0xC)
#include "d2game_d4e80.h"
#include <cstdlib>
#include <cstring>

static uint32_t rd(const void* p, uint32_t off) { uint32_t v; memcpy(&v, (const uint8_t*)p + off, 4); return v; }
static void wr(void* p, uint32_t off, uint32_t v) { memcpy((uint8_t*)p + off, &v, 4); }
static void* P(uint32_t v) { return (void*)(uintptr_t)v; }

uint32_t d2game_d4e80(const D2GameD4E80Env& env, uint32_t itemId, void* player, void* game, uint32_t page, uint32_t direct, uint32_t* out)
{
    if (!game || !player) abort();   // retail: Fog assert, then exit(-1)
    *out = 0;
    env.sync(player);
    uint8_t* item = (uint8_t*)P(rd(game, 0x1720 + (itemId & 0x7f) * 4));
    while (item && rd(item, 0xc) != itemId) item = (uint8_t*)P(rd(item, 0xe4));
    if (!item) { *out = 1; return 0; }
    if (rd(item, 0) != 4) return 0;
    if (rd(item, 0x10) != 4) { *out = 1; return 0; }
    if (!env.itemCheck(item)) return 0;
    const uint32_t inv = rd(player, 0x60);
    if (direct) {
        if (!env.invPlace(inv, item)) return 0;
    } else {
        uint32_t x = direct, y = (uint32_t)(uintptr_t)out;   // retail hands 10114 the argument slots; it never reads them back
        env.itemPos(item, &x, &y, 0x1223);
        if (!env.invPlaceAt(inv, item, page)) return 0;
        env.itemSetPage(item, 0xff);
    }
    if (!env.invAdd(inv, item, 2)) { *out = 1; return 0; }
    env.invSetCursor(inv, 0);
    env.picked(item, player);
    wr(item, 0xc4, rd(item, 0xc4) & ~2u);
    if (env.itemOwned(item, player)) env.refresh(player, item, 0);
    env.itemSetMode(item, 2);
    env.itemSetPage(item, 0xff);
    env.itemFlag(item, 0x400, 1);
    env.unitUpdate(player, 1);
    env.invLink(inv, item);
    return 1;
}
