// 1.13c D2Game+0x78DD0: PlrModes weapon pass after a skill/state change. Registers: EAX = unit; two stack words (ret 8): ctx and player (EBP). The player's
// stack word is reused by the compiler as a scratch dword whose low byte is the 0xA97C result, so `slot` below is that dword (player with the low byte replaced).
// Order: gate on 0xA940 / 0xA9AC / stat 0x7D (0xA802), bail when stat 0x46 > 0; with stat 0x7D set clear 0x46 and return; with 0xA430(0x2D) and 0xAFDC set, 0xD5740 unless
// 0xA8A4 reports the flag; otherwise refresh through 0xA4A2 / 0xD8970 / 0xA916, walk the player's inventory (0xB036) for the unit's own entry (0xA8A4 == 0 and 0xA7AE == 0)
// and hand it to 0xD57F0, falling back to 0x78B60. Addresses are 32-bit words as in the FN probes.
#include <cstdint>
#include <cstring>
struct D2G78DD0Seams {
    uint32_t (*a940)(uint32_t unit);
    uint32_t (*a9ac)(uint32_t unit);
    int32_t  (*getStat)(uint32_t unit, uint32_t stat, uint32_t layer);          // 0xA802 (0x7D) and 0xA42A (0x46): a802 below, this is 0xA42A
    int32_t  (*a802)(uint32_t unit, uint32_t stat, uint32_t layer);
    void     (*setStat)(uint32_t unit, uint32_t stat, uint32_t layer, uint32_t v); // 0xA472
    uint32_t (*a430)(uint32_t unit, uint32_t v);
    uint32_t (*afdc)(uint32_t unit);
    uint32_t (*a8a4)(uint32_t item, uint32_t a, uint32_t line, uint32_t file);
    void     (*d5740)(uint32_t unit, uint32_t player, uint32_t ctx);            // EAX unit, ECX player, EDX ctx; plain ret
    uint32_t (*b02a)(uint32_t unit);
    uint32_t (*aa66)(uint32_t unit);
    uint32_t (*a97c)(uint32_t unit);
    uint32_t (*b024)(uint32_t unit);
    void     (*a4a2)(uint32_t player, uint32_t v);
    void     (*d8970)(uint32_t ctx, uint32_t player, uint32_t slot, uint32_t one, uint32_t* out); // ECX ctx, EDX player; stack slot, 1, &out (ret 0xC)
    void     (*a916)(uint32_t unit, uint32_t a, uint32_t b);
    uint32_t (*b036)(uint32_t inv, uint32_t key, uint32_t prev);
    uint32_t (*a7ae)(uint32_t inv, uint32_t slotByte);
    uint32_t (*d57f0)(uint32_t player, uint32_t ctx, uint32_t id, uint32_t slot); // EAX player; stack ctx, id, slot (ret 0xC)
    void     (*s78b60)(uint32_t local, uint32_t ctx, uint32_t player);          // EAX = local with the low byte = slot byte; stack ctx, player (ret 8)
};
static inline uint32_t r32_78dd0(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(p + o), 4); return v; }
// 1.13c D2Game+0x78DD0
extern "C" void d2game_78dd0(uint32_t unit, uint32_t ctx, uint32_t player, const D2G78DD0Seams* s) {
    const uint32_t file = 0xB20FBBC8u;   // the module string "..\\Source\\D2Game\\PLAYER\\PlrModes.cpp", tagged as the capture does
    if (!s->a940(unit) && !s->a9ac(unit) && !s->a802(unit, 0x7d, 0)) return;
    if (s->getStat(unit, 0x46, 0) > 0) return;
    if (s->a802(unit, 0x7d, 0)) { s->setStat(unit, 0x46, 0, 0); return; }
    if (s->a430(unit, 0x2d) && s->afdc(unit)) {
        if (!s->a8a4(unit, 0x100, 0x512, file)) s->d5740(unit, player, ctx);
        return;
    }
    const uint32_t edi = s->b02a(unit), key = s->aa66(unit);
    const uint32_t slot = (player & ~0xffu) | (s->a97c(unit) & 0xff);
    const uint32_t local = s->b024(unit);
    s->a4a2(player, 1);
    uint32_t out = 0;
    s->d8970(ctx, player, slot, 1, &out);
    s->a916(unit, 1, 1);
    if (edi) {
        for (uint32_t it = s->b036(r32_78dd0(player, 0x60), key, 0); it; it = s->b036(r32_78dd0(player, 0x60), key, it)) {
            const uint32_t self = unit ? r32_78dd0(unit, 4) : 0xffffffffu, other = r32_78dd0(it, 4);
            if (self == other && !s->a8a4(it, 0x100, 0x530, file)) {
                if (r32_78dd0(it, 4) == self && !s->a7ae(r32_78dd0(player, 0x60), slot & 0xff) && s->d57f0(player, ctx, r32_78dd0(it, 0xc), slot)) return;
                break;
            }
        }
    }
    if (local) s->s78b60((local & ~0xffu) | (slot & 0xff), ctx, player);
}
