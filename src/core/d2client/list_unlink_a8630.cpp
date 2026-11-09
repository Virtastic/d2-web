// 1.13c D2Client+0xA8630 (140 B, plain ret): ESI = node. Unlinks the node from the light list (head 0x11BBEC, next at +0x28); a node that is not
// on the list takes the fatal path (0x17A). Then releases node+0x30 when set (line 0x181) and the node (line 0x183) through the Fog free
// (Ordinal_10043 thunk 0xBF86: ECX = block, EDX = 0x6FB84104 the source-file string, stack line, 0; ret 8) and bumps the counter 0x11BBF4.
#include <cstdint>
#include <emscripten.h>

static inline uint32_t R(uintptr_t p) { return *(const uint32_t*)p; }

// 1.13c D2Client+0xA8630
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a8630(uintptr_t node, uint32_t* head, uint32_t* freed, void (*fatal)(uint32_t),
                                                     void (*fogfree)(uint32_t ecx, uint32_t edx, uint32_t line, uint32_t zero)) {
    if (node == 0) return;
    uintptr_t prev = 0, cur = (uintptr_t)*head;
    for (;;) {
        if (cur == 0) { fatal(0x17a); return; }
        if (cur == node) break;
        prev = cur;
        cur = (uintptr_t)R(cur + 0x28);
        if (cur == 0) { fatal(0x17a); return; }
    }
    if (prev) *(uint32_t*)(prev + 0x28) = R(cur + 0x28);
    else *head = R(cur + 0x28);
    uint32_t blk = R(node + 0x30);
    if (blk) fogfree(blk, 0x6FB84104u, 0x181, 0);
    fogfree((uint32_t)node, 0x6FB84104u, 0x183, 0);
    *freed += 1;
}
