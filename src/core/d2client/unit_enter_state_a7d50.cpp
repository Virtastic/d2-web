// 1.13c D2Client+0xA7D50 (208 B, plain ret): EDI = unit. A unit whose first dword is 0 and whose owner resolves (Ordinal_10331, thunk 0xC1F0)
// and whose kind (+0x10) is 1 or 5 is moved into the entered state: the list node of 0xC5C2 (unit, 0) is released through 0xA8630 (ESI = node),
// 0x197D0 (EAX = unit, stack unit == player), 0xC778 (unit, 0), 0xCD78 (+0x2C id), 0xA6210 (ESI = unit); +0xC4 := (+0xC4 & ~0x40) | 2; and when
// 0xC43C (unit, tier) answers, 0x6A220 (EAX = unit, stack 0), 0x2A590 (ECX = tier, EDX = unit) and 0x24490 (EAX = unit, stack tier, 0) run.
// tier = 1 + 4 when 0xC208 (owner of the unit) answers non-zero. Callee `ret` widths: 0x197D0 4, 0x6A220 4, 0x24490 8, the others plain.
// The 0xCD2A call (kind 0x13) follows the kind gate, which only admits 1 and 5, so it is kept as retail orders it and never runs.
#include <cstdint>
#include <emscripten.h>

struct D2ClientA7D50Seams {
    uint32_t (*c1f0)(uintptr_t unit);
    uint32_t (*c5c2)(uintptr_t unit, uint32_t zero);
    void (*a8630)(uint32_t node);
    void (*f197d0)(uintptr_t unit, uint32_t isPlayer);
    void (*c778)(uintptr_t unit, uint32_t zero);
    void (*cd2a)(uint32_t id);
    void (*cd78)(uint32_t id);
    uint32_t (*c208)(uint32_t owner);
    void (*a6210)(uintptr_t unit);
    uint32_t (*c43c)(uintptr_t unit, uint32_t tier);
    void (*f6a220)(uintptr_t unit, uint32_t zero);
    void (*f2a590)(uint32_t tier, uintptr_t unit);
    void (*f24490)(uintptr_t unit, uint32_t tier, uint32_t zero);
};
static inline uint32_t R(uintptr_t p) { return *(const uint32_t*)p; }

// 1.13c D2Client+0xA7D50
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a7d50(uintptr_t unit, uint32_t player, const D2ClientA7D50Seams* s) {
    if (unit == 0 || R(unit) != 0) return;
    uint32_t kind = R(unit + 0x10);
    if (s->c1f0(unit) == 0) return;
    if (kind != 1 && kind != 5) return;
    uint32_t node = s->c5c2(unit, 0);
    if (node != 0) s->a8630(node);
    s->f197d0(unit, (uint32_t)unit == player ? 1u : 0u);
    s->c778(unit, 0);
    uint32_t id = R(unit + 0x2c);
    if (R(unit + 0x10) == 0x13) s->cd2a(id);
    s->cd78(id);
    uint32_t tier = (s->c208(s->c1f0(unit)) != 0 ? 4u : 0u) + 1u;
    s->a6210(unit);
    *(uint32_t*)(unit + 0xc4) = (R(unit + 0xc4) & 0xffffffbfu) | 2u;
    if (s->c43c(unit, tier) != 0) {
        s->f6a220(unit, 0);
        s->f2a590(tier, unit);
        s->f24490(unit, tier, 0);
    }
}
