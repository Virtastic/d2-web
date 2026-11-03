// 1.13c D2Game+0x71060 -- pick a random eligible slot: starting at a seed-rolled index, walks the table ring (up to 256 collected slots) and
// keeps slots for which 0xB0C0(slot, filter) holds and the entry (0x1A8 bytes) has +0x133 set, (u16 +0xF6 < 100 or game u16 +0x78 >= 100) and
// byte +0xFD <= maxLevel; returns a seed-rolled pick, or 0. The roll is the game seed step (D2Game 0x211D0) on game+0xD0.
#include <cstring>
#include "d2game_71060.h"
namespace {
uint32_t roll(uint32_t seedAddr, int32_t n) {
    if (n <= 0) return 0;
    uint32_t s[2]; std::memcpy(s, (const void*)(uintptr_t)seedAddr, 8);
    const uint64_t v = (uint64_t)s[0] * 0x6AC690C5u + s[1];
    s[0] = (uint32_t)v; s[1] = (uint32_t)(v >> 32);
    std::memcpy((void*)(uintptr_t)seedAddr, s, 8);
    return (uint32_t)v % (uint32_t)n;
}
}  // namespace
// 1.13c D2Game+0x71060
uint32_t d2game_port_71060(uint32_t game, int32_t maxLevel, uint32_t filter, const D2Game71060Env* env)
{
    auto rd = [](uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; };
    auto rd8 = [](uint32_t a) { return (uint32_t)*(const uint8_t*)(uintptr_t)a; };
    auto rd16 = [](uint32_t a) { uint16_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 2); return (uint32_t)v; };
    const uint32_t t = env->table();
    if (!t) env->fatal(0x381);
    const int32_t count = (int32_t)rd(t);
    uint16_t picks[256];
    int n = 0;
    int32_t cur = (int32_t)roll(game + 0xd0, count);
    int32_t endIdx = cur - 1;
    if (endIdx < 0) endIdx = count - 1;
    if (cur == endIdx) return 0;
    const uint32_t base = rd(t + 4);
    do {
        if (n > 0xff) break;
        if (env->slotOk((uint32_t)cur, filter)) {
            const uint32_t e = base + (uint32_t)cur * 0x1a8;
            if (rd8(e + 0x133) != 0 && (rd16(e + 0xf6) < 100 || rd16(game + 0x78) >= 100) && (int32_t)rd8(e + 0xfd) <= maxLevel)
                picks[n++] = (uint16_t)cur;
        }
        cur++;
        if (count <= cur) cur = 0;
    } while (cur != endIdx);
    if (n == 0) return 0;
    return picks[roll(game + 0xd0, n)];
}
