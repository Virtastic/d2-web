// 1.13c D2Win+0x167c0 -- stdcall(msg): type-7 window message handler (fatal 0x18f otherwise); ignores msg[1] == 0x102.
// msg[2] 0xd: refresh (0x17a90(0)) and call the current child's handler (+0x204). 0x26 / 0x28: step the current child (+0x68) back / forward through the child list
// (head +0x64, next +0x220; 0x26 wraps to the last, 0x28 to the head), recount its index (+0x54) and scroll the window offset (+0x4c) to keep it within +0x58 rows.
// Any other key: the first child whose key word (+0x21c) equals msg[2] and has a handler gets the message.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2w {
static uint32_t f167c0_scroll(GE& e, uint32_t* w)
{
    int32_t n = (int32_t)w[0x15];
    if (n < (int32_t)w[0x13]) { w[0x13] = (uint32_t)n; e.hs(0x17a90, 0, 0, 0, 0, 0, { 0 }); return 1; }
    int32_t d = n - (int32_t)w[0x13];
    if ((int32_t)w[0x16] < d) { do { --d; ++w[0x13]; } while ((int32_t)w[0x16] < d); }
    e.hs(0x17a90, 0, 0, 0, 0, 0, { 0 });
    return 1;
}
uint32_t f167c0(GE& e, uint32_t* msg)
{
    uint32_t* w = (uint32_t*)(uintptr_t)msg[0];
    if (w[0] != 7) abort();
    if (msg[1] == 0x102) return 0;
    const uint32_t m = (uint32_t)(uintptr_t)msg;
    auto nxt = [&](uint32_t c) { return e.rd32(c + 0x220); };
    if (msg[2] == 0xd) {
        e.hs(0x17a90, 0, 0, 0, 0, 0, { 0 });
        const uint32_t h = e.rd32(w[0x1a] + 0x204);
        if (h) return e.hs(h & 0xFFFFFFu, 0, 0, 0, 0, 0, { m }, 0, 1);
    } else if (msg[2] == 0x26) {
        uint32_t head = w[0x19], i2 = nxt(head), i5 = head;
        if (head == w[0x1a]) { for (; i2; i2 = nxt(i2)) i5 = i2; }
        else if (i2 != w[0x1a]) { do { i5 = i2; i2 = nxt(i5); } while (nxt(i5) != w[0x1a]); }
        w[0x1a] = i5;
        w[0x15] = 0;
        for (uint32_t c = head; c != i5; c = nxt(c)) ++w[0x15];
        return f167c0_scroll(e, w);
    } else if (msg[2] == 0x28) {
        uint32_t c4 = nxt(w[0x1a]);
        if (!c4) c4 = w[0x19];
        w[0x1a] = c4;
        w[0x15] = 0;
        for (uint32_t c = w[0x19]; c != c4; c = nxt(c)) ++w[0x15];
        return f167c0_scroll(e, w);
    }
    for (uint32_t c = w[0x19]; c; c = nxt(c))
        if ((e.rd32(c + 0x21c) & 0xFFFFu) == msg[2] && e.rd32(c + 0x204))
            return e.hs(e.rd32(c + 0x204) & 0xFFFFFFu, 0, 0, 0, 0, 0, { m }, 0, 1);
    return 0;
}
}
