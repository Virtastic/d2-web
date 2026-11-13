// 1.13c D2Multi+0x9E30 -- ECX = key, one stack word = text, ret 4: the chat-line list at image offset 0x3A0F0 (node: text[0x200], key at +0x200, next at +0x204; kept ascending by key) is searched for the key. A missing key gets a 0x208-byte node from Fog 10042 (thunk 0x7BE4), inserted in key order; then the text is copied into the node with strncpy (0x200 bytes, padded with zeros), the byte at +0x200 is cleared and the list is re-drawn by 0x9D60.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2multi_3a0f0;
namespace d2multi {
void f9e30(GE& e, uint32_t key, uint32_t text)
{
    uint32_t node = g_d2multi_3a0f0;
    while (node && e.rd32(node + 0x200) != key) node = e.rd32(node + 0x204);
    if (!node) {
        node = e.h(0x7be4, 0, 0x208, 0xB201798cu, 0, 0, 0x1b21, 0);
        e.wr32(node + 0x200, key);
        const uint32_t head = g_d2multi_3a0f0;
        if (!head || (int32_t)e.rd32(head + 0x200) > (int32_t)key) {
            e.wr32(node + 0x204, head);
            g_d2multi_3a0f0 = node;
        } else {
            uint32_t prev = head, cur = head;
            for (;;) {
                if ((int32_t)e.rd32(cur + 0x200) >= (int32_t)key) break;
                prev = cur;
                cur = e.rd32(cur + 0x204);
                if (!cur) break;
            }
            e.wr32(node + 0x204, cur);
            e.wr32(prev + 0x204, node);
        }
    }
    // _strncpy(node, text, 0x200): copy to the NUL, pad with zeros
    bool end = false;
    for (uint32_t i = 0; i < 0x200; i++) {
        const uint32_t c = end ? 0 : e.rd8(text + i);
        if (!c) end = true;
        e.wr8(node + i, c);
    }
    e.wr8(node + 0x200, 0);
    e.h(0x9d60);
}
}
