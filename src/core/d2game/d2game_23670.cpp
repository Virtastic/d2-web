// 1.13c D2Game+0x23670: widen the stored pair of an owned missile-type unit. Fastcall ECX = game, EDX = unit, two stack words never read
// (ret 8). The unit must be type 3 with stat 0x16 above 0 and bit 10 of the flag word at +0xC8; the owner is 0xE03A0 (EAX +0x94, ECX game,
// EDX +0x98) and must be a type-0x10F-indexed unit (+4). The first pair (stat 0x15, 0x16) moves by the owner's stats 0x17 and 0x18, each
// shifted left 8 and offset -0x100 (clamped at 0), and is stored back through 0xA472.
#include "d2game_seam4e.h"
extern "C" void d2g_23670(uint32_t game, uint32_t unit, const D2Seam* e)
{
    if (!unit || d2rd32(unit) != 3) return;
    const uint32_t a = e->call(0xA42A, 0, 0, 0, 0, 0, unit, 0x15, 0);
    const int32_t b = (int32_t)e->call(0xA42A, 0, 0, 0, 0, 0, unit, 0x16, 0);
    if (b <= 0) return;
    if (!((d2rd32(unit + 0xC8) >> 10) & 1)) return;
    const uint32_t own = e->call(0xE03A0, d2rd32(unit + 0x94), game, d2rd32(unit + 0x98));
    if (!own) return;
    if (d2rd32(own + 4) != 0x10F) return;
    const uint32_t x = e->call(0xA42A, 0, 0, 0, 0, 0, own, 0x17, 0);
    int32_t nx = (int32_t)(a + (x << 8) - 0x100);
    const uint32_t y = e->call(0xA42A, 0, 0, 0, 0, 0, own, 0x18, 0);
    int32_t ny = (int32_t)((y << 8) + (uint32_t)b - 0x100);
    if (nx < 0) nx = 0;
    if (ny < 0) ny = 0;
    e->call(0xA472, 0, 0, 0, 0, 0, unit, 0x15, (uint32_t)nx, 0);
    e->call(0xA472, 0, 0, 0, 0, 0, unit, 0x16, (uint32_t)ny, 0);
}
