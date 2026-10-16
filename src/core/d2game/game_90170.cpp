// 1.13c D2Game+0x90170 (EAX = game, stack = slot, ret 4): releases what a 0x44-byte game slot owns.
// Retail 32-bit slot layout; every release is a seam call (ids below) so the replay can run it on blobs without live game state.
#include <cstddef>
#include <cstdint>
#include <emscripten.h>

#pragma pack(push, 1)
struct D2G90170Slot { uint32_t pad0, handle, head8, cnt0C, pad10, head14; };
struct D2G90170NodeA { uint32_t handle, f4, next; };          // chain at slot+8
struct D2G90170NodeB { uint32_t unit, f4, f8, kind, next; };  // chain at slot+0x14
#pragma pack(pop)
static_assert(offsetof(D2G90170Slot, handle) == 4 && offsetof(D2G90170Slot, head8) == 8 && offsetof(D2G90170Slot, cnt0C) == 0xC && offsetof(D2G90170Slot, head14) == 0x14, "slot fields");
static_assert(offsetof(D2G90170NodeA, next) == 8 && offsetof(D2G90170NodeB, kind) == 0xC && offsetof(D2G90170NodeB, next) == 0x10, "node fields");
// seam ids: 1 drain-inventory-list(game, 8F470), 2 free-inventory(10820, handle), 3 pool-free(10046), 4 per-unit sweep(DFC10), 5 unit free(E20A0)
struct D2G90170Seam { void (*call)(uint32_t id, uint32_t a); };

template <class Ptr> static void GAME_90170(const D2G90170Seam& s, uint32_t game, D2G90170Slot* e, Ptr node)
{
    if (e->cnt0C) {
        for (D2G90170NodeA* n = node(e->head8); n;) {
            D2G90170NodeA* nx = node(n->next);
            if (n->handle) { s.call(1, game); s.call(2, n->handle); n->handle = 0; }
            s.call(3, 0);
            n = nx;
        }
        e->head8 = 0;
    }
    s.call(1, game); s.call(2, e->handle); e->handle = 0;
    D2G90170NodeB* b = (D2G90170NodeB*)node(e->head14);
    while (b) {
        D2G90170NodeB* nx = (D2G90170NodeB*)node(b->next);
        if (b->kind == 1 && b->unit) { s.call(4, b->unit); s.call(5, b->unit); }
        s.call(3, 0);
        b = nx;
    }
    e->head14 = 0;
}

// Replay probe (FN path): fake chains cannot be pinned in a blob, so only empty chains are vectored.
static void d2g90170Nop(uint32_t, uint32_t) {}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_90170(uint32_t* r)
{
    auto none = [](uint32_t) -> D2G90170NodeA* { return nullptr; };
    GAME_90170(D2G90170Seam{ d2g90170Nop }, r[0], (D2G90170Slot*)(uintptr_t)r[1], none);
    r[4] = 0;
}
