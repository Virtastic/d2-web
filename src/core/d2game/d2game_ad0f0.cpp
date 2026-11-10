// 1.13c D2Game+0xAD0F0 (EDI unit, ESI item; stack game, a1, a2, a3; ret 0x10)
#include "d2game_ad0f0.h"
#include <cstring>

static uint32_t rd(const void* p, uint32_t off) { uint32_t v; memcpy(&v, (const uint8_t*)p + off, 4); return v; }

// The item-use table at D2Game+0x107238: {use, fallback} handler RVAs per use index; 0x1F entries, the rest empty.
static const uint32_t kUse[0x1f][2] = {
    { 0, 0 }, { 0xACFD0, 0xADAC0 }, { 0, 0xAF450 }, { 0, 0xAEDC0 }, { 0, 0xAEA40 }, { 0, 0xAE6D0 },
    { 0, 0xAF370 }, { 0, 0xAD9C0 }, { 0, 0xACA20 }, { 0, 0xAE4D0 }, { 0, 0xADF20 }, { 0, 0xADEE0 },
};

static void notify(const D2GameAD0F0Env& env, void* unit, void* item)
{
    env.afterUse(unit);
    const uint32_t id = rd(item, 0xc);
    const uint8_t type = (uint8_t)rd(item, 0);
    // The reply sender takes AL = 0x7C and CL = the item type; only their low bytes are defined (the rest is register residue).
    env.send(0x7c, type, id, rd(unit, 0) == 0 ? rd(env.playerData(unit), 0x9c) : 0);
}

uint32_t d2game_ad0f0(const D2GameAD0F0Env& env, void* unit, void* item, uint32_t game, uint32_t a1, uint32_t a2, uint32_t a3)
{
    if (!unit || (rd(unit, 0) != 0 && rd(unit, 0) != 1)) return 0;
    if (!item || rd(item, 0) != 4) return 0;
    int32_t idx = -1, arg = -1;
    if (env.itemIsType(item, 0x12) || env.itemIsType(item, 0x16)) {
        const uint8_t* rec = (const uint8_t*)env.useRecord((uint16_t)env.itemField(item, 0));
        if (rec) { idx = (int32_t)rd(rec, 4); arg = (int32_t)rd(rec, 0xc); }
    }
    if (idx <= 0) {
        const uint8_t* rec = (const uint8_t*)env.itemRecord(rd(item, 4));
        if (rec) idx = (int32_t)rd(rec, 0x94);
        if (idx <= 0) return 0;
    }
    if (idx >= 0x1f) return 0;
    if (kUse[idx][0] && !env.itemFlagTest(item, 4, 0x46b)) {
        if (env.handler(kUse[idx][0], game, unit, item, 0, a2, a3, (uint32_t)arg)) return 0;
        notify(env, unit, item);
        return 0;
    }
    if (!kUse[idx][1]) return 0;
    env.itemFlagSet(item, 4, 1);
    const uint32_t r = env.handler(kUse[idx][1], game, unit, item, a1, a2, a3, (uint32_t)arg);
    if (r) return r;
    notify(env, unit, item);
    return 0;
}
