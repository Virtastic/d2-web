// 1.13c D2Client+0xA9E30 (206 B): centre the 48x48 light window on the tracked unit and step every light node.
// DAT 0x11BBFC = the unit (type +0, path pointer +0x2C). Type 2 and 4..5 read the static path (x at +0xC, y at +0x10); every other type asks C3BE(path) / C2EC(path)
// for x and y (0 when the path is null). The window DAT 0x104DF0..FC = (x - 0x18, y - 0x18, x + 0x18, y + 0x18). Then A9C90(0), A8880(), edi = A8800(),
// DAT 0x11BBF8 = 0 and every node of the list at DAT 0x11BBEC (next link +0x28, read before the call) is counted and handed to A9B50(EAX = node, stack edi).
#include <cstring>
#include <emscripten.h>
#include "d2client_a9_env.h"
struct D2cA9e30G { uint32_t head; uint32_t pad[2]; uint32_t counter; uint32_t unitP; };   // RVAs 0x11BBEC.. 0x11BBF8 (counter) 0x11BBFC (unit)
static uint32_t rd32(uint32_t p, uint32_t off) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)(p + off), 4); return v; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_a9e30(D2cA9e30G* g, int32_t* rect, const D2cA9Env* e) {
    const int32_t type = (int32_t)rd32(g->unitP, 0);
    const uint32_t path = rd32(g->unitP, 0x2c);
    const bool st = type == 2 || (type > 3 && type <= 5);
    uint32_t x, y;
    if (st) x = rd32(path, 0xc); else x = path ? e->call(e->ctx, 0xC3BE, 0, 0, 0, 0, path, 0, 0, 0, 0) : 0;
    if (st) y = rd32(path, 0x10); else y = path ? e->call(e->ctx, 0xC2EC, 0, 0, 0, 0, path, 0, 0, 0, 0) : 0;
    rect[0] = (int32_t)(x - 0x18); rect[1] = (int32_t)(y - 0x18); rect[2] = (int32_t)(x + 0x18); rect[3] = (int32_t)(y + 0x18);
    e->call(e->ctx, 0xA9C90, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    e->call(e->ctx, 0xA8880, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    const uint32_t v = e->call(e->ctx, 0xA8800, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    uint32_t n = g->head;
    g->counter = 0;
    while (n) {
        ++g->counter;
        const uint32_t next = rd32(n, 0x28);
        e->call(e->ctx, 0xA9B50, n, 0, 0, 0, v, 0, 0, 0, 0);
        n = next;
    }
}
