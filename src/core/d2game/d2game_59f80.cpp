// 1.13c D2Game+0x59F80 (ACT5Q3_SetNodeState2): EAX = pGame. Walk the quest list at game+0x10F4 (head word, nodes chained at +0xF4) for the node of type 0x21;
// a live node below state 2 goes to state 2, clears byte +0x14 and broadcasts.
// Retail 32-bit record offsets read through the probe's blob addresses; the 0xDFCC0 broadcast is the seam.
#include <cstdint>
#include <cstring>
static uint32_t rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static uint32_t rd8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }
static void wr8(uint32_t a, uint32_t v) { *(uint8_t*)(uintptr_t)a = (uint8_t)v; }
typedef void (*D2GameNodeBroadcast)(uint32_t node, uint32_t callbackRva, uint32_t game);
void d2g_59f80(uint32_t game, D2GameNodeBroadcast broadcast) {
    const uint32_t list = rd32(game + 0x10F4);
    if (!list) return;
    uint32_t n = rd32(list);
    while (n && rd32(n) != 0x21) n = rd32(n + 0xF4);
    if (!n) return;

    if (!rd8(n + 9)) return;
    if (rd8(n + 0xB) >= 2) return;
    wr8(n + 0xB, 2); wr8(n + 0x14, 0);
    broadcast(n, 0x591D0, rd32(n + 4));
}
