// INVENTORY_CanItemsBeTraded at retail's 32-bit image layout: stdcall(pool, player1, player2, pTradeState), ret 0x10.
// Two 0x40-byte trade inventories are allocated (signature 0x01020304 at +0, pool at +4, owner +8 = 0); each player's main
// grid is copied into its own trade inventory (0x1E7B0); player1's storage items are then reserved in trade inventory 2
// (0x20D20, else *pTradeState = 0) and player2's in trade inventory 1 (else *pTradeState = 1). Both inventories are freed on
// every path (0x21B40: an owner-less, item-less inventory frees its cell arrays, its grid array and itself). Returns 1 only
// when both fit. The native D2MOO port reads the wasm64 unit layout and cannot run on an image unit.
#include <cstdint>
#include <cstring>
#include "inv_place.h"
struct D2Common1E7B0Env { int32_t (*unitRecord)(uint32_t unit); const InvEnv* inv; };
extern "C" uint32_t D2Common_1E7B0_CopyGrid(const D2Common1E7B0Env* e, uint32_t unit, uint32_t trade);
struct D2Common21C50Env { const D2Common1E7B0Env* copy; void (*free)(uint32_t p, void* ctx); };
namespace {
inline uint8_t* P21(uint32_t a) { return (uint8_t*)(uintptr_t)a; }
inline uint32_t& W21(uint32_t a, uint32_t o) { return *(uint32_t*)(P21(a) + o); }
uint32_t NewTradeInventory(const InvEnv& e, uint32_t pool)
{
    uint32_t v = e.alloc(0x40, e.ctx);
    if (!v) return 0;
    memset(P21(v), 0, 0x40);
    W21(v, 0) = 0x01020304u;
    W21(v, 4) = pool;
    return v;
}
// 0x21B40 for a trade inventory: no cursor item, no items, no owner, no nodes, no corpses.
void FreeTradeInventory(const D2Common21C50Env& e, uint32_t v)
{
    if (!v || W21(v, 0) != 0x01020304u) return;
    void* ctx = e.copy->inv->ctx;
    for (int32_t i = 0; i < (int32_t)W21(v, 0x18); ++i) {
        uint32_t cells = W21(W21(v, 0x14) + (uint32_t)i * 0x10, 0xC);
        if (cells) e.free(cells, ctx);
    }
    if (W21(v, 0x14)) e.free(W21(v, 0x14), ctx);
    e.free(v, ctx);
}
}
// 1.13c D2Common+0x21C50 (#10762)
uint32_t D2Common_21C50_CanItemsBeTraded(const D2Common21C50Env* e, uint32_t pool, uint32_t p1, uint32_t p2, uint32_t pState)
{
    const InvEnv& inv = *e->copy->inv;
    uint32_t t1 = NewTradeInventory(inv, pool);
    uint32_t t2 = NewTradeInventory(inv, pool);
    uint32_t ok = 0;
    if (t1 && t2 && D2Common_1E7B0_CopyGrid(e->copy, p1, t1) && D2Common_1E7B0_CopyGrid(e->copy, p2, t2)) {
        if (!D2Common_20D20_ReserveAll(inv, p1, t2, p2)) { if (pState) W21(pState, 0) = 0; }
        else if (!D2Common_20D20_ReserveAll(inv, p2, t1, p1)) { if (pState) W21(pState, 0) = 1; }
        else ok = 1;
    }
    FreeTradeInventory(*e, t1);
    FreeTradeInventory(*e, t2);
    return ok;
}
