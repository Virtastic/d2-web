// D2Game: a unit's inventory items are walked and each is refreshed by its location, in the retail 32-bit layout (addresses are the low 32 bits of a pointer).
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

static uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static void wr32(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }

struct D2Game37C90Env {
    uint32_t (*firstItem)(uint32_t inv);                                      // 0xA3DC thunk (stdcall 1)
    uint32_t (*itemData)(uint32_t item);                                      // 0xA46C thunk (stdcall 1), result unused
    uint32_t (*location)(uint32_t item);                                      // 0xA736 thunk (stdcall 1)
    void (*refreshBelt)(uint32_t mode, uint32_t unit, uint32_t ctx);          // 0x8B0F0: AL = 6 (EAX = 6), ECX = unit; stack ctx, 0, 0 (ret 0xC)
    uint32_t (*findUnit)(uint32_t game, uint32_t id, uint32_t type);          // 0xE03A0: ECX = game, EDX = id, EAX = type
    void (*refreshBody)(uint32_t unit, uint32_t ctx);                         // 0x8DF70: EAX = unit, ECX = ctx; stack 0 (ret 4)
    void (*refreshGrid)(uint32_t unit, uint32_t ctx);                         // 0x8DE50: EAX = unit, ECX = ctx; stack 0 (ret 4)
    uint32_t (*nextItem)(uint32_t item);                                      // 0xA4CC thunk (stdcall 1)
};

// The ctx's current unit: +0x174 kept as is when bit 0 of +0x3D4 is set, else looked up again (game +0x1A8,
// type +0x16C, id +0x170) and cleared when the lookup fails.
static uint32_t currentUnit(uint32_t ctx, const D2Game37C90Env& env)
{
    if (*(const uint8_t*)(uintptr_t)(ctx + 0x3D4) & 1) return rd32(ctx + 0x174);
    if (rd32(ctx + 0x174) == 0) return 0;
    const uint32_t u = env.findUnit(rd32(ctx + 0x1A8), rd32(ctx + 0x170), rd32(ctx + 0x16C));
    if (u == 0) wr32(ctx + 0x174, 0);
    return u;
}

// 1.13c D2Game+0x37C90
// EDI = ctx; stack unit; ret 4. A unit without an inventory (+0x60) is a Fog assert (exit), and so is a null ctx
// once an item outside location 3 is met. Location 3 items go to 0x8B0F0; when the ctx's current unit is this unit,
// location 1 items go to 0x8DF70 and location 2 items to 0x8DE50.
void INVENTORY_RefreshItems_37C90(uint32_t ctx, uint32_t unit, const D2Game37C90Env& env)
{
    const uint32_t inv = rd32(unit + 0x60);
    if (inv == 0) _exit(0x186);
    for (uint32_t it = env.firstItem(inv); it != 0; it = env.nextItem(it)) {
        env.itemData(it);
        if (env.location(it) == 3) { env.refreshBelt(6, unit, ctx); continue; }
        if (ctx == 0) _exit(0xFB);
        if (currentUnit(ctx, env) == unit && env.location(it) == 1) { env.refreshBody(unit, ctx); continue; }
        if (currentUnit(ctx, env) == unit && env.location(it) == 2) env.refreshGrid(unit, ctx);
    }
}
