// 1.13c D2Game+0x5A200 (ACT5Q2_SetNodeState5): EAX = pGame. Walk the quest list at game+0x10F4 (head word, nodes chained at +0xF4) for the node of type 0x20;
// a live node (byte +9) not yet in state 5 whose +0x18 record has a signed word at +0xA8 below 5 goes to state 5, clears byte +0x14 and broadcasts
// through 0xDFCC0 (EAX 0, ECX node, EBX callback 0x593F0, EDI the node's game word at +4, stack 0). Callee is the seam.
#include <cstdint>
#include <cstring>
static uint32_t rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static uint32_t rd8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }
static void wr8(uint32_t a, uint32_t v) { *(uint8_t*)(uintptr_t)a = (uint8_t)v; }
typedef void (*D2GameNodeBroadcast)(uint32_t node, uint32_t callbackRva, uint32_t game);
void d2g_5a200(uint32_t game, D2GameNodeBroadcast broadcast) {
    const uint32_t list = rd32(game + 0x10F4);
    if (!list) return;
    uint32_t n = rd32(list);
    while (n && rd32(n) != 0x20) n = rd32(n + 0xF4);
    if (!n) return;
    if (!rd8(n + 9)) return;
    if (rd8(n + 0xB) == 5) return;
    if ((int32_t)rd32(rd32(n + 0x18) + 0xA8) >= 5) return;
    wr8(n + 0xB, 5); wr8(n + 0x14, 0);
    broadcast(n, 0x593F0, rd32(n + 4));
}
