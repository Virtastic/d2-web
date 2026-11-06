// 1.13c D2Game+0xBA080 -- EAX = pGame. Walk the quest list at game+0x10F4 (head word, nodes chained at +0xF4) for the node of type 0x16;
// a live node (byte +9) below state 2 (byte +0xB) gets its flag byte +0x14 cleared, its state set to 2, and is broadcast through 0xDFCC0.
// The broadcast is the seam: (node, callback RVA, EDI word = node+4).
#include <cstdint>
#include <cstring>
typedef void (*GBA080Broadcast)(uint32_t node, uint32_t callbackRva, uint32_t edi);
static uint32_t rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static uint32_t rd8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }
static void wr8(uint32_t a, uint32_t v) { *(uint8_t*)(uintptr_t)a = (uint8_t)v; }
// 1.13c D2Game+0xBA080
void g_ba080(uint32_t game, GBA080Broadcast broadcast)
{
    const uint32_t list = rd32(game + 0x10F4);
    if (!list) return;
    uint32_t n = rd32(list);
    if (!n) return;
    while (rd32(n) != 0x16) { n = rd32(n + 0xF4); if (!n) return; }
    if (!rd8(n + 9)) return;
    if (rd8(n + 0xB) >= 2) return;
    wr8(n + 0x14, 0); wr8(n + 0xB, 2);
    broadcast(n, 0xB9D20, rd32(n + 4));
}
