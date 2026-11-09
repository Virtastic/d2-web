// 1.13c D2Game+0xD60D0 (EAX item id, ECX player; stack game, loc, force, out; ret 0x10)
#include "d2game_d60d0.h"
#include <cstdlib>
#include <cstring>

static uint32_t rd(const void* p, uint32_t off) { uint32_t v; memcpy(&v, (const uint8_t*)p + off, 4); return v; }
static void wr(void* p, uint32_t off, uint32_t v) { memcpy((uint8_t*)p + off, &v, 4); }
static void* P(uint32_t v) { return (void*)(uintptr_t)v; }

uint32_t d2game_d60d0(const D2GameD60D0Env& env, uint32_t itemId, void* player, void* game, uint32_t loc, uint32_t force, uint32_t* out)
{
    const uint8_t body = (uint8_t)loc;
    const bool twoH = body == 0xb || body == 0xc;   // the two weapon-swap slots
    if (!game || !player) abort();   // retail: Fog assert, then exit(-1)
    if (out) *out = 0;
    env.sync(player);
    uint8_t* item = (uint8_t*)P(rd(game, 0x1720 + (itemId & 0x7f) * 4));
    while (item && rd(item, 0xc) != itemId) item = (uint8_t*)P(rd(item, 0xe4));
    if (!item) { if (out) *out = 1; return 0; }
    if (rd(item, 0) != 4) return 0;
    if (rd(item, 0x10) != 4) { if (out) *out = 1; return 0; }
    if (env.canEquip(player, body, item, force) != 1) { if (out) *out = 0; return 0; }
    if (!force && !env.canUse(item, player)) { if (out) *out = 1; return 0; }
    const uint32_t inv = rd(player, 0x60);
    if (!twoH) {
        const uint32_t r = env.invOffhand(inv);
        if (r) env.unequip(player, r);
    }
    if (!env.invEquip(inv, item, body)) return 1;
    if (!env.invAdd(inv, item, twoH ? 4 : 3)) { if (out) *out = 1; return 0; }
    env.itemSetLoc(item, loc);
    if (!twoH) {
        env.invUnlink(inv, item);
        env.refresh(player, item, 0);
    }
    env.invSetCursor(inv, 0);
    wr(item, 0xc4, rd(item, 0xc4) & ~2u);
    env.itemSetMode(item, 1);
    env.itemSetPage(item, 0xff);
    env.itemFlag(item, 8, 1);
    env.itemFlagSet(item, 1, 1);
    if (env.itemFlagTest(item, 0x4000, 0xb4f)) env.itemFlagSet(item, 0x4000, 0);
    env.invLink(inv, item);
    env.unitUpdate(player, 1);
    env.stats(player);
    env.send(game, player, 0);
    return 1;
}
