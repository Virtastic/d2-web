// 1.13c D2Game+0x4E760 -- skill-driven heal of a unit: ECX = game, stdcall words: source unit, target unit, damage record (word +8 > 0), skill id, level (negative becomes 1).
// Both units need type (word +0) 0 or 1, 0xDFA90(target) must be clear. Skill row = 0x1420(skill id); its short +0x82 (below dt +0xC4) must pass 0xA68E(source, id) and select the record
// 0xA5D4(source, id); owner = 0xE03A0(0xA7FC(rec), game, 0xA84A(rec)); bonus = 0x14D0(0xAC9A(owner, row +0x138, skill id, level), 100, record word +8). With cur = 0xA42A(target, 6, 0) and
// max = 0xA3A0(target) both positive: cur = min(max, max(1, cur + bonus)) goes to 0xA472(target, 6, cur, 0), and short +0x10C (1..dt +0xBC0 - 1) goes to 0xA83E(target, id, 0). Returns 1 or 0.
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f4e760(NE& e, uint32_t dt, uint32_t game, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4, uint32_t p5)
{
    if (!p1 || !p2) return 0;
    uint32_t t = e.rd32(p1);
    if (t != 0 && t != 1) return 0;
    t = e.rd32(p2);
    if (t != 0 && t != 1) return 0;
    if (e.h(0xDFA90, p2)) return 0;
    if (!p3 || (int32_t)e.rd32(p3 + 8) <= 0) return 0;
    const uint32_t row = e.h(0x1420, p4);
    if (!row) return 0;
    int32_t lvl = (int32_t)p5;
    if (lvl < 0) lvl = 1;
    const int32_t id = (int16_t)e.rd16(row + 0x82);
    if (id < 0 || id >= (int32_t)e.rd32(dt + 0xc4)) return 0;
    if (!e.imp(0xA68E, p1, (uint32_t)id)) return 0;
    const uint32_t rec = e.imp(0xA5D4, p1, (uint32_t)(int32_t)(int16_t)e.rd16(row + 0x82));
    if (!rec) return 0;
    const uint32_t q = e.imp(0xA84A, rec);
    const uint32_t a = e.imp(0xA7FC, rec);
    const uint32_t owner = e.h(0xE03A0, a, game, q);
    const uint32_t g = e.imp(0xAC9A, owner, e.rd32(row + 0x138), p4, (uint32_t)lvl);
    int32_t bonus = (int32_t)e.h(0x14D0, g, 100, e.rd32(p3 + 8));
    int32_t cur = (int32_t)e.imp(0xA42A, p2, 6, 0);
    int32_t mx = (int32_t)e.imp(0xA3A0, p2);
    if (cur <= 0 || mx <= 0) return 0;
    cur += bonus;
    if (cur < 1) cur = 1;
    if (cur >= mx) cur = mx;
    e.imp(0xA472, p2, 6, (uint32_t)cur, 0);
    const int32_t s = (int16_t)e.rd16(row + 0x10c);
    if (s > 0 && s < (int32_t)e.rd32(dt + 0xbc0)) e.imp(0xA83E, p2, (uint32_t)s, 0);
    return 1;
}
}
