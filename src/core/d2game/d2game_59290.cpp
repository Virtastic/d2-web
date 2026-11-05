// 1.13c D2Game+0x59290 (ACT5Q2_CheckWussieCounters): EAX = pGame, stack = unit. Finds the quest node of type 0x20 (list at game+0x10F4, chain +0xF4);
// a live node whose soldier-group lookup (0x58260, ECX = node, EAX = unit) is >= 0 and whose counter dword at data+0x114+idx*4 is non-zero gives 1.
#include <cstdint>
#include <cstring>
static uint32_t rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static uint32_t rd8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }
typedef int32_t (*D2GameFindGroup)(uint32_t node, uint32_t unit);
uint32_t d2g_59290(uint32_t game, uint32_t unit, D2GameFindGroup findGroup) {
    const uint32_t list = rd32(game + 0x10F4);
    if (!list) return 0;
    uint32_t n = rd32(list);
    while (n && rd32(n) != 0x20) n = rd32(n + 0xF4);
    if (!n || !rd8(n + 9)) return 0;
    const int32_t idx = findGroup(n, unit);
    if (idx < 0) return 0;
    return rd32(rd32(n + 0x18) + 0x114 + (uint32_t)idx * 4) ? 1 : 0;
}
