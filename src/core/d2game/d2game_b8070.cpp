// 1.13c D2Game+0xB8070: EAX = target, ECX = unit, EDX = mode, stack game, arg, cmd (ret 0xC). The Baal-clone think
// dispatcher: the unit's monster-stats row (sgptDataTables+0xA78, 0x1A8 bytes per class) supplies the skills; modes
// 1-15 pick an action (idle by difficulty, attack, skill casts, a leap to a spot 25 units past the target, the
// minion command). Modes 8 and 14 without a target fall through to mode 0, which idles for 5 frames.
// gdt is the address of the module's sgptDataTables pointer. Callee 0xF2380 takes its mode in AL (the rest of EAX
// is residue of the previous value, passed through as retail leaves it).
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_b8070(uint32_t target, uint32_t unit, uint32_t mode, uint32_t game, uint32_t arg, uint32_t cmd,
                                 uint32_t gdt, const D2Seam* e)
{
    // 1.13c D2Game+0xB8070
    for (;;) {
        if (!unit) return 0;
        const int32_t cls = (int32_t)d2rd32(unit + 4);
        if (cls < 0) return 0;
        const uint32_t dt = d2rd32(gdt);
        if (cls >= (int32_t)d2rd32(dt + 0xA80)) return 0;
        const uint32_t row = d2rd32(dt + 0xA78) + (uint32_t)cls * 0x1A8u;
        if (!row) return 0;
        const uint32_t m1 = mode - 1;
        auto cast = [&](uint32_t al, uint32_t skill, uint32_t w1, uint32_t w2, uint32_t w3) {
            e->call(0xF2380, al, unit, 0, game, 0, skill, w1, w2, w3);
        };
        switch (m1 > 0xE ? 0xFFu : mode) {
        case 1: {
            const uint32_t d = d2rd8(game + 0x6D);
            e->call(0xF22C0, game, 0, 0, 0, unit, d == 1 ? 0xF : d == 2 ? 5 : 0x23);
            return 0;
        }
        case 2: case 6:
            e->call(0xF1D90, 0xC, target, 0, 0, 0, game, unit, 2, 0);
            return 0;
        case 3:
            e->call(0xF26B0, 1, unit, 0, 0, 0, game, target, 6);
            return 0;
        case 4:
            e->call(0xF1B10, 0x10, 0, 0, 0, 0, game, unit);
            return 0;
        case 5:
            if (!cmd) break;
            e->call(0xF2540, 0, game, unit, 0, 0, 0, 2, d2rd32(cmd + 0xC), d2rd32(cmd + 0x10));
            return 0;
        case 8:
            if (!target) { mode = 0; continue; }
            if ((int16_t)d2rd16(row + 0x17A) > 0) {
                const int32_t hp = (int32_t)e->call(0xA4EA, 0, 0, 0, 0, 0, target);
                const uint32_t mx = e->call(0xA3A0, 0, 0, 0, 0, 0, target);
                if ((int32_t)mx > hp || d2rd32(target) != 0) {
                    cast((mx & ~0xFFu) | d2rd8(row + 0x185), (uint32_t)(int32_t)(int16_t)d2rd16(row + 0x17A), target, 0, 0);
                    return 0;
                }
            }
            {
                const int32_t sk = (int16_t)d2rd16(row + 0x17C);
                if (sk <= 0 || d2rd32(target) != 0) return 0;
                cast(((uint32_t)sk & ~0xFFu) | d2rd8(row + 0x186), (uint32_t)sk, target, 0, 0);
            }
            return 0;
        case 9: cast(9, 0x13B, target, 0, 0); return 0;
        case 10:
            e->call(0xF2220, 5, 0, 0, 0, 0, game, target);
            e->edi(0xF2220, unit);
            return 0;
        case 11: {
            const uint32_t r = e->call(0xA9FA, 0, 0, 0, 0, 0, unit, 0x13C, 1, 0);
            cast((r & ~0xFFu) | 0xA, 0x13C, target, 0, 0);
            return 0;
        }
        case 12: cast(8, 0x13D, target, 0, 0); return 0;
        case 13: cast(4, 0x13E, target, 0, 0); return 0;
        case 14: {
            if (!target) { mode = 0; continue; }
            const int32_t ux = (int32_t)e->call(0x1100, 0, unit), uy = (int32_t)e->call(0x1130, 0, unit);
            const int32_t tx = (int32_t)e->call(0x1100, 0, target), ty = (int32_t)e->call(0x1130, 0, target);
            int32_t dist = (int32_t)e->call(0xAB86, 0, 0, 0, 0, 0, unit, target);
            if (!dist) dist = 1;
            const int32_t x = (ux - tx) * 0x19 / dist + ux;
            const int32_t y = (uy - ty) * 0x19 / dist + uy;
            const uint32_t al = (row & ~0xFFu) | d2rd8(row + 0x184);
            const uint32_t sk = (uint32_t)(int32_t)(int16_t)d2rd16(row + 0x178);
            if (cls == 0x2C5) { cast(al, sk, 0, (uint32_t)tx, (uint32_t)ty); return 0; }
            const uint32_t room = e->call(0xA478, 0, 0, 0, 0, 0, unit);
            // the x/y slots are caller-stack locals the callee may move; the hook leaves them as computed
            if (!e->call(0x47450, 0, 0, 0, 0, 0, game, room, 0, (uint32_t)cls)) { mode = 0; continue; }
            cast(al, sk, 0, (uint32_t)x, (uint32_t)y);
            return 0;
        }
        case 15:
            e->call(0xB7D30, unit, 0, 0, 0, 0, game, arg);
            e->call(0xF22C0, game, 0, 0, 0, unit, 5);
            return 0;
        default:
            break;
        }
        e->call(0xF22C0, game, 0, 0, 0, unit, 5);
        return 0;
    }
}
