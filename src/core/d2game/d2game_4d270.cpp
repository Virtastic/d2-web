// D2Game+0x4D270 -- SkillSpawnMissileAtOffset: spawn the skill's missile from a unit and aim it at an offset.
// ECX = game, EDX = unit, stack (skill id, arg1), ret 8. unit +0xC4 |= 0x40. Skill id outside 0..count (dt +0xBA0) or no row table (dt +0xB98): 0.
// 0xE07A0(EAX = unit); p = 0xA598(unit +0x2C); ally = p unless p == unit (then 0); 0x9FA30(EAX = unit, ECX = game, EBX = &x; &y) zero: 0.
// ac = 0xAC9A(unit, row +0x138, skill, arg1); missile = signed word row +0x48, or (when 0xAC28(ECX = unit) != 1) the word at row +0x4A if not negative;
// outside 0..dt +0xB6C: 0. Request (23 dwords): [0] 0x20 (0x420 without ally), [1] [2] unit, [3] ally, [4] missile, [7] x, [8] y, [11] skill, [12] arg1,
// [21] D2Game+0x4C190 and [22] ac when ac is set. m = 0x6F930(ECX = game, EDX = request); if m: 0xA50E(m, ally ? 1 : 2), then
// 0xAF5E(m, ((y - 0x1130(unit)) << 16) + (x - 0x1100(unit)) as sign-extended words). Returns 1.
#include "d2game_seam4e.h"
// 1.13c D2Game+0x4D270
extern "C" uint32_t d2game_4d270(uint32_t game, uint32_t unit, int32_t skill, uint32_t arg1, uint32_t dt, const D2Seam* e)
{
    if (unit) d2wr32(unit + 0xC4, d2rd32(unit + 0xC4) | 0x40);
    if (skill < 0 || skill >= (int32_t)d2rd32(dt + 0xBA0)) return 0;
    const uint32_t row = (uint32_t)skill * 0x23C + d2rd32(dt + 0xB98);
    if (!row) return 0;
    e->call(0xE07A0, unit);
    const uint32_t p = e->call(0xA598, 0, 0, 0, 0, 0, d2rd32(unit + 0x2C));
    const uint32_t ally = p == unit ? 0 : p;
    uint32_t x = 0, y = 0;
    if (!e->call(0x9FA30, unit, game, 0, (uint32_t)(uintptr_t)&x, 0, (uint32_t)(uintptr_t)&y)) return 0;
    const uint32_t ac = e->call(0xAC9A, 0, 0, 0, 0, 0, unit, d2rd32(row + 0x138), (uint32_t)skill, arg1);
    int32_t id = (int16_t)d2rd16(row + 0x48);
    if (e->call(0xAC28, 0, unit) != 1) {
        const int16_t w = (int16_t)d2rd16(row + 0x4A);
        if (w >= 0) id = w;
    }
    if (id < 0 || id >= (int32_t)d2rd32(dt + 0xB6C)) return 0;
    uint32_t q[23] = {};
    q[0] = 0x20; q[1] = unit; q[2] = unit; q[3] = ally; q[4] = (uint32_t)id; q[7] = x; q[8] = y; q[11] = (uint32_t)skill; q[12] = arg1;
    if (ac) q[21] = 0xB2000000u | 0x4C190u;
    q[22] = ac;
    if (!ally) q[0] = 0x420;
    const uint32_t m = e->call(0x6F930, 0, game, (uint32_t)(uintptr_t)q);
    e->blk(0x6F930, q, 23);
    if (m) {
        e->call(0xA50E, 0, 0, 0, 0, 0, m, ally ? 1 : 2);
        const uint32_t dx = x - e->call(0x1100, 0, unit);
        const uint32_t dy = y - e->call(0x1130, 0, unit);
        e->call(0xAF5E, 0, 0, 0, 0, 0, m, (uint32_t)((int32_t)(int16_t)dy << 16) + (uint32_t)(int32_t)(int16_t)dx);
    }
    return 1;
}
