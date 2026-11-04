// D2Game save-file item reader (one item and its socketed children) over retail-layout (32-bit) memory. Written from the 1.13c disassembly.
// Convention: EAX = game, stack (owner, out item*, cursor*, end, version) (ret 0x14); returns 1 with *cursor advanced and *out item set, else 0.
// Each item is a size check (A3EE: cursor, bytes left, 1, out status, version; status -1 fails) then the read (EAE0: ESI = out info
// block whose word 0 is the size-check status and +0x14 the socket count; 0, cursor, bytes left, 1, in/out consumed bytes, version). Sockets are put in mode 4 and
// inserted (D4A70: EAX = socket id; game, owner, item id, &consumed, 0, 1, 0, 0); a socket failure frees the item (E20A0: ECX = game,
// EDX = item). An item with flag 0x4000000 and no inventory owner is handed to D3280 (EAX = item, ECX = owner; game) and the out item is 0.
#include "d2game_seam4e.h"

// 1.13c D2Game+0xED710
extern "C" uint32_t d2game_ed710(uint32_t game, uint32_t owner, uint32_t outItem, uint32_t cur, uint32_t end, uint32_t ver, const D2Seam* e)
{
    volatile uint32_t used = 0, info[6] = { 0, 0, 0, 0, 0, 0 };   // retail: the size-check status is info word 0 (one local)
    const uint32_t pUsed = (uint32_t)(uintptr_t)&used, pInfo = (uint32_t)(uintptr_t)info, pStatus = pInfo;
    uint32_t p = d2rd32(cur);
    uint32_t left = end - p;
    d2wr32(outItem, 0);
    used = left;
    {
        const uint32_t w[5] = { p, left, 1, pStatus, ver };
        e->call(0xA3EE, 0, 0, 0, 0, 0, w[0], w[1], w[2], w[3], w[4]);
        e->blk(0xA3EE, w, 5);
    }
    if (info[0] == 0xFFFFFFFFu) return 0;
    uint32_t item;
    {
        const uint32_t w[6] = { 0, p, left, 1, pUsed, ver };
        item = e->call(0xEAE0, 0, 0, 0, 0, pInfo, w[0], w[1], w[2], w[3], w[4], w[5]);
        e->blk(0xEAE0, w, 6);
    }
    if (!item) return 0;
    p += used;
    const int32_t count = (int32_t)info[5];
    for (int32_t i = 0; i < count; ++i) {
        left = end - p;
        used = left;
        {
            const uint32_t w[5] = { p, left, 1, pStatus, ver };
            e->call(0xA3EE, 0, 0, 0, 0, 0, w[0], w[1], w[2], w[3], w[4]);
            e->blk(0xA3EE, w, 5);
        }
        uint32_t sock;
        {
            const uint32_t w[6] = { 0, p, left, 1, pUsed, ver };
            sock = e->call(0xEAE0, 0, 0, 0, 0, pInfo, w[0], w[1], w[2], w[3], w[4], w[5]);
            e->blk(0xEAE0, w, 6);
        }
        if (!sock) { e->call(0xE20A0, 0, game, item); return 0; }
        p += used;
        e->call(0xA4A2, 0, 0, 0, 0, 0, sock, 4);
        const uint32_t w[8] = { game, owner, item ? d2rd32(item + 0xC) : 0xFFFFFFFFu, pUsed, 0, 1, 0, 0 };
        const uint32_t ok = e->call(0xD4A70, d2rd32(sock + 0xC), 0, 0, 0, 0, w[0], w[1], w[2], w[3], w[4], w[5], w[6], w[7]);
        e->blk(0xD4A70, w, 8);
        if (!ok) { e->call(0xE20A0, 0, game, item); return 0; }
    }
    if ((e->call(0xA412, 0, 0, 0, 0, 0, item) & 0x4000000u) && !e->call(0xA3C4, 0, 0, 0, 0, 0, item)) {
        e->call(0xD3280, item, owner, 0, 0, 0, game);
        item = 0;
    }
    d2wr32(cur, p);
    d2wr32(outItem, item);
    return 1;
}
