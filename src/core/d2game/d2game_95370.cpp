// 1.13c D2Game+0x95370: ebx = unit; stack game, skill, level, state, stat (ret 0x14). skill picks a skills row (dt+0xB98, stride 0x23C, count dt+0xBA0); outside it: 0.
// expire = game +0xA8 + 0xAC9A(unit, row +0x60, skill, level). The unit's list for the state is 0xA5D4(unit, state), else one from 0xA6A0(game +0x1C, 2, expire, unit type
// (6 when no unit), unit id (-1)) (none: 0) set up by 0xA5B6(list, state), 0xA75A(list, callback 0x90F00), 0xA60A(unit, list, 1), 0xA6E2(list, 0x15E, skill, 0),
// 0xA6E2(list, 0x15F, level, 0). Then 0xA7C6(list, expire), 0x8EA80(game, unit; 0xC, expire, 0, 0, 0) and the stack count n = 0xA712(list, stat, 0): at most 3 it
// goes up by one (0xA6E2(list, stat, n, 0), then, with row short +0x56 a state below dt+0xBD4, 0xAF7C(list, that state, 0xAC9A(unit, row +0x6C, skill, level), 0)
// and 0xA3CA / 0xA706(unit, state, 1)). Returns 1.
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_95370(uint32_t unit, uint32_t game, uint32_t skill, uint32_t level, uint32_t state, uint32_t stat, uint32_t dt, const D2Seam* e)
{
    if ((int32_t)skill < 0 || (int32_t)skill >= (int32_t)d2rd32(dt + 0xBA0)) return 0;
    const uint32_t row = skill * 0x23C + d2rd32(dt + 0xB98);
    if (!row) return 0;
    const uint32_t exp = d2rd32(game + 0xA8) + e->call(0xAC9A, 0, 0, 0, 0, 0, unit, d2rd32(row + 0x60), skill, level);
    uint32_t sl = e->call(0xA5D4, 0, 0, 0, 0, 0, unit, state);
    if (!sl) {
        const uint32_t ty = unit ? d2rd32(unit) : 6, id = unit ? d2rd32(unit + 0xC) : ~0u;
        sl = e->call(0xA6A0, 0, 0, 0, 0, 0, d2rd32(game + 0x1C), 2, exp, ty, id);
        if (!sl) return 0;
        e->call(0xA5B6, 0, 0, 0, 0, 0, sl, state);
        e->call(0xA75A, 0, 0, 0, 0, 0, sl, 0xB2090F00);
        e->call(0xA60A, 0, 0, 0, 0, 0, unit, sl, 1);
        e->call(0xA6E2, 0, 0, 0, 0, 0, sl, 0x15E, skill, 0);
        e->call(0xA6E2, 0, 0, 0, 0, 0, sl, 0x15F, level, 0);
    }
    e->call(0xA7C6, 0, 0, 0, 0, 0, sl, exp);
    e->call(0x8EA80, 0, game, unit, 0, 0, 0xC, exp, 0, 0, 0);
    const int32_t cur = (int32_t)e->call(0xA712, 0, 0, 0, 0, 0, sl, stat, 0);
    int32_t nx = cur + 1; if (nx > 3) nx = 3;
    if (cur == nx) return 1;
    e->call(0xA6E2, 0, 0, 0, 0, 0, sl, stat, (uint32_t)nx, 0);
    const int32_t w = (int16_t)d2rd16(row + 0x56);
    if (w >= 0 && w < (int32_t)d2rd32(dt + 0xBD4)) {
        const uint32_t v = e->call(0xAC9A, 0, 0, 0, 0, 0, unit, d2rd32(row + 0x6C), skill, level);
        e->call(0xAF7C, 0, 0, 0, 0, 0, sl, (uint32_t)w, v, 0);
    }
    e->call(0xA3CA, 0, 0, 0, 0, 0, unit, state, 1);
    e->call(0xA706, 0, 0, 0, 0, 0, unit, state, 1);
    return 1;
}
