// 1.13c D2Game+0x567A0: quest-state gate. param_1 is a quest record (+0x9 flag, +0xC state, +0x10 quest id).
// Callee ids (w[0] = register arg): 1 33F30 (QUESTS_GetQuestData), 2 IsBadCodePtr (A2DA), 3 Ordinal_10265 (0x3D5), 4 Ordinal_10024 (A2C2, fatal message), 5 _exit(-1) (2A3C), 6 call quest handler.
#include <cstdint>
typedef uint32_t W;
struct D2G567A0Seam { W (*call)(void* ctx, W id, const W* w); W (*rd)(W addr); void (*wr8)(W addr, uint8_t v); void* ctx; };
W GAME_567A0(const D2G567A0Seam& s, W rec)
{
    auto C = [&](W id, W a = 0, W b = 0) { const W w[2] = { a, b }; return s.call(s.ctx, id, w); };
    const uint8_t st = s.rd(rec + 0xC) & 0xFF;
    if (st != 5 && (s.rd(rec + 8) >> 8 & 0xFF) != 0) {
        if (st == 0) s.wr8(rec + 0xC, 1);
        return 1;
    }
    const W q = C(1, s.rd(rec + 0x10));
    if (!q) return 0;
    if (C(2, s.rd(rec + 0xF0))) {
        const W m = C(3, 0x3D5);
        C(4, 0, m);
        C(5, 0xFFFFFFFFu);
    }
    return C(6, s.rd(q + 0xF0));
}

// ---- FUNCTION-mode probe for 0x567A0 (src/core/fn/d2game_567a0.cpp): 33F30 and the quest handler log through HookEnv; IsBadCodePtr is 0 (the descriptor pins a module code pointer).
#include <emscripten.h>
#include "../fn/hookenv.h"
static HookEnv* q67H; static W q67Rec, q67Node;
uint32_t D2G567A0_probe(uint32_t rec, uint32_t* lg, const uint32_t* sc, uint32_t nd)
{
    HookEnv h(lg, sc); q67H = &h; q67Rec = rec; q67Node = nd;
    D2G567A0Seam s{};
    s.rd = [](W a) -> W { return HookEnv::rd32(a); };
    s.wr8 = [](W a, uint8_t v) { *(uint8_t*)(uintptr_t)a = v; };
    s.call = [](void*, W id, const W* w) -> W {
        switch (id) {
        case 1: return q67H->h(0x33F30, HookEnv::rd32(q67Rec + 4), w[0]) ? q67Node : 0;
        case 6: return q67H->h(HookEnv::handlerRva(w[0]), 0, q67Node);
        default: return 0;
        }
    };
    return GAME_567A0(s, rec);
}
