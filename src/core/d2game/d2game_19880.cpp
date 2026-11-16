// 1.13c D2Game+0x19880: a monster's think step toward its target, with the nearest-target scan. ECX = game, EDX = unit, one stack word (ret 4) = a pointer to a
// pointer to the record whose dword +0x18 is the idle countdown; void.
// The target is 0xF30C0(ESI unit; game, &scratch). With a target other than the unit: the distance is 0xB0A20(EBX unit; target); the AI control is the unit's +0x14 +0x30
// (a type 1 unit; else 0, then read as a null base). A type 0 target that 0x37680(ESI target) calls 1 goes idle with 10 (0xF22C0(EAX game, ESI unit; 0xA)); so does any AI
// control whose first node chain (first dword, node dword +8 next) holds a node whose first dword is not 0. Otherwise with the countdown record = *arg: a distance below 0x18
// (signed) and a countdown of 0 sets it to 0x3C and, for a type 0 target, 0xDFFD0(ESI unit; 0x12, target), then idles with 0x14; a distance below 0x18 with a nonzero countdown, or
// any distance above, decrements it (a distance of 0x18 or more first restores 0x3C when it is not above 0) and idles with 0x14. Without a target (or the target being the unit): a
// null unit idles; class dword +4 == 0xFB asks 0xC4AA0(EAX game; unit), then 0x15BB0(ESI unit; game), then runs 0xF21E0(EAX 0; game, 0, 0) and 0xDFCC0(EAX 0, EBX callback
// 0xC4CC0, EDI game; 0); class 0x196 does the same with 0xB9C00 and callback 0xB9C60; any other class idles with 0x14. A failed ask idles with 0x14.
#include "d2game_seam4e.h"
static void idle(const D2Seam* e, uint32_t game, uint32_t unit, uint32_t n)
{
    e->call(0xF22C0, game, 0, 0, 0, unit, n);
}
extern "C" void d2g_19880(uint32_t game, uint32_t unit, uint32_t arg, const D2Seam* e)
{
    uint32_t scratch = 0;
    const uint32_t target = e->call(0xF30C0, 0, 0, 0, 0, unit, game, (uint32_t)(uintptr_t)&scratch);
    if (target && target != unit) {
        const uint32_t dist = e->call(0xB0A20, 0, 0, 0, unit, 0, target);
        uint32_t ai = 0;
        if (unit && d2rd32(unit) == 1) {
            const uint32_t p = d2rd32(unit + 0x14);
            if (p) ai = d2rd32(p + 0x30);
        }
        bool away = false;
        if (d2rd32(target) == 0 && e->call(0x37680, 0, 0, 0, 0, target) == 1) away = true;
        if (!away) {
            for (uint32_t n = d2rd32(ai); n; n = d2rd32(n + 8))
                if (d2rd32(n) != 0) { away = true; break; }
        }
        if (away) { idle(e, game, unit, 0xA); return; }
        const uint32_t rec = d2rd32(arg);
        if ((int32_t)dist >= 0x18) {
            if ((int32_t)d2rd32(rec + 0x18) <= 0) d2wr32(rec + 0x18, 0x3C);
            d2wr32(rec + 0x18, d2rd32(rec + 0x18) - 1);
        } else if (d2rd32(rec + 0x18)) {
            d2wr32(rec + 0x18, d2rd32(rec + 0x18) - 1);
        } else {
            d2wr32(rec + 0x18, 0x3C);
            if (d2rd32(target) == 0) e->call(0xDFFD0, 0, 0, 0, 0, unit, 0x12, target);
        }
        idle(e, game, unit, 0x14);
        return;
    }
    if (!unit) { idle(e, game, unit, 0x14); return; }
    const uint32_t cls = d2rd32(unit + 4);
    if (cls == 0xFB || cls == 0x196) {
        const bool a = cls == 0xFB;
        if (!e->call(a ? 0xC4AA0 : 0xB9C00, game, 0, 0, 0, 0, unit)) { idle(e, game, unit, 0x14); return; }
        if (!e->call(0x15BB0, 0, 0, 0, 0, unit, game)) { idle(e, game, unit, 0x14); return; }
        e->call(0xF21E0, 0, 0, 0, 0, 0, game, 0, 0);
        e->call(0xDFCC0, 0, 0, 0, 0xB2000000u | (a ? 0xC4CC0u : 0xB9C60u), 0, 0);
        e->edi(0xDFCC0, game);
        return;
    }
    idle(e, game, unit, 0x14);
}
