// 1.13c D2Game+0x203D0: the inactive-unit compressor's item branch. EBX = item unit,
// [esp+4] = game, [esp+8] = list owner, [esp+0xC] = out pointer; ret 0xC. A 0xE-byte header (link 0, ground-removal frame
// from 0xE0A0, owner id from thunk 0xA8DA, word bitstream size) plus the 0x400-byte serialized item (thunk 0xAFD0) is
// copied, size+0xF bytes, into a pool node (thunk 0xA2E6 on the game's memory pool +0x1C, line 0x1E3). When the item has an
// inventory (thunk 0xA9EE, +0x60), each socketed child is unlinked (0xA3DC first / 0xA4CC next / 0xA46C unit / 0xA748
// remove) and removed (0xE20A0); the item itself is removed. The node goes to *out when out is set, else it heads the
// owner's +8 list.
#ifndef D2G_203D0_CPP
#define D2G_203D0_CPP
#include <cstring>
#include "d2g_mem.h"
namespace d2g_203d0 {
using namespace d2g;
struct Env {
    U (*ownerId)(P item);                                    // thunk 0xA8DA (stdcall 1)
    U (*serialize)(P item, uint8_t* buf, I cap, I a, I b, I c); // thunk 0xAFD0 (stdcall 6), low word = size
    U (*removalFrame)(P item, U game);                       // 0xE0A0: ESI = item, EDI = game
    P (*alloc)(U pool, U size, U file, I line, I zero);      // thunk 0xA2E6: ECX pool, EDX size, stack file, line, 0
    I (*hasInventory)(P item);                               // thunk 0xA9EE (stdcall 1)
    U (*first)(U inv);                                       // thunk 0xA3DC
    U (*next)(U node);                                       // thunk 0xA4CC
    U (*nodeUnit)(U node);                                   // thunk 0xA46C
    void (*remove)(U inv, U unit);                           // thunk 0xA748 (stdcall 2)
    void (*removeUnit)(U game, U unit);                      // 0xE20A0: fastcall
    U file;                                                  // the source-file string's address as the caller sees it
};
// 1.13c D2Game+0x203D0
inline void CompressItem_113c(const Env& e, P item, P game, P owner, P out) {
    uint8_t blk[0x410];
    const U id = e.ownerId(item);
    wr(blk, 0, 0);
    const uint16_t n = (uint16_t)e.serialize(item, blk + 0xE, 0x400, 1, 1, 0);
    const U fr = e.removalFrame(item, W(game));
    wr(blk, 4, fr); wr(blk, 8, id); wr16(blk, 0xC, n);
    const U size = (U)n + 0xF;
    P node = e.alloc(rd(game, 0x1C), size, e.file, 0x1E3, 0);
    memcpy(node, blk, size);
    if (e.hasInventory(item)) {
        const U inv = rd(item, 0x60);
        if (inv) for (U it = e.first(inv); it;) {
            const U nx = e.next(it);
            const U u = e.nodeUnit(it);
            e.remove(inv, u);
            e.removeUnit(W(game), u);
            it = nx;
        }
    }
    e.removeUnit(W(game), W(item));
    if (out) { wr(out, 0, W(node)); return; }
    wr(node, 0, rd(owner, 8));
    wr(owner, 8, W(node));
}
}
#endif
