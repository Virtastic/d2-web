// D2Game+0xD76A0: move an item (type 4, mode 4) into an inventory. ECX = file, EDX = line (passed to 0xAA9C), stack a0 = game, a1 = unit, a2 = item id,
// a3, a4 = cell x, y (a5 == 0), a5 = place at cell vs auto, a6 = finish (mode/dirty/notify), a7 = inventory (0: [unit + 0x60]); ret 0x20.
// Written from the 1.13c disassembly. kind = low byte of 0xA6C4(item); retail stores the byte over the low byte of the dead a1 (unit) slot and reloads the dword, so the
// kind argument of 0xA634 / 0xAA9C / 0xAA36 carries the unit pointer's upper three bytes. The capture masks them (hookmask 0xA634 0xF1F); the port passes kind alone. F2 = 1 (cleared when kind == 1 and [unit] == 0), F1 = 0 (set when kind == 2 and
// [unit] == 0). Lookup game[0x1720 + (id & 0x7F) * 4] chain through item[0xE4] on item[0xC] == id.
#include "d2game_seam4e.h"
static const uint32_t kFile = 0xB20FB14Cu;   // the source-file string at D2Game+0xFB14C, recorded as a module address
// 1.13c D2Game+0xD76A0
extern "C" uint32_t d2game_d76a0(uint32_t file, uint32_t line, uint32_t game, uint32_t unit, uint32_t id, uint32_t a3, uint32_t a4, uint32_t a5,
                                 uint32_t a6, uint32_t a7, const D2Seam* e)
{
    if (!game || !unit) return 0;
    e->call(0xD29F0, 0, 0, 0, 0, 0, unit);
    uint32_t it = d2rd32(game + 0x1720 + (id & 0x7F) * 4);
    while (it && d2rd32(it + 0xC) != id) it = d2rd32(it + 0xE4);
    if (!it || d2rd32(it) != 4 || d2rd32(it + 0x10) != 4) return 0;
    const uint8_t kind = (uint8_t)e->call(0xA6C4, 0, 0, 0, 0, 0, it);
    uint32_t inv = a7;
    if (!inv) { inv = d2rd32(unit + 0x60); if (!inv) return 0; }
    const uint32_t kd = kind;
    uint32_t f1 = 0, f2 = 1;
    const uint32_t v = e->call(0xA634, 0, 0, 0, 0, 0, unit, kd, d2rd32(game + 0x70));
    if (d2rd32(unit) == 0) { if (kind == 1) f2 = 0; else if (kind == 2) f1 = 1; }
    uint32_t r;
    if (a5) {
        r = e->call(0xAA9C, 0, 0, 0, 0, 0, inv, it, v, 0, kd, file, line);
    } else {
        const uint32_t x = (int32_t)a3 < 0 ? 0 : a3, y = (int32_t)a4 < 0 ? 0 : a4;
        r = e->call(0xAA36, 0, 0, 0, 0, 0, inv, it, x, y, v, 0, kd);
    }
    if (!r) return 0;
    if (!e->call(0xA9B8, 0, 0, 0, 0, 0, inv, it, 1)) return 0;
    if ((uint8_t)e->call(0xA6C4, 0, 0, 0, 0, 0, it) != 4) e->call(0xD4670, it, unit, 0, 0, 0);
    d2wr32(it + 0xC4, d2rd32(it + 0xC4) & 0xFFFFFFFDu);
    if (e->call(0xA640, 0, 0, 0, 0, 0, it, unit)) e->call(0xD47C0, 0, 0, 0, 0, 0, 0);
    if (f2) e->call(0xAA54, 0, 0, 0, 0, 0, inv, 0);
    e->call(0xA4A2, 0, 0, 0, 0, 0, it, 0);
    if (a6) {
        e->call(0xA916, 0, 0, 0, 0, 0, it, 2, 1);
        if (e->call(0xD1BC0, it, 0, 0, 0, 0)) e->call(0xA65E, 0, 0, 0, 0, 0, it, 1, 1);
        if (e->call(0xA8A4, 0, 0, 0, 0, 0, it, 0x4000, 0x8EF, kFile)) e->call(0xA65E, 0, 0, 0, 0, 0, it, 0x4000, 0);
        e->call(0xA9A6, 0, 0, 0, 0, 0, unit, 1);
        e->call(0xA994, 0, 0, 0, 0, 0, inv, it);
    }
    if (f1 == 1) e->call(0x71B00, it, game, 0, 0, 0, unit);
    if (e->call(0xA640, 0, 0, 0, 0, 0, it, unit)) {
        e->call(0xD51A0, 0, 0, 0, 0, 0, game, unit, 0);
        e->call(0xA9A6, 0, 0, 0, 0, 0, unit, 1);
    }
    return 1;
}
