// src/core/d2game/d2game_batch_predicates.cpp -- 1.13c D2Game.dll stat-record predicates transcribed from the cached
// Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Base 0x6FC20000.
//
// Style as d2game_batch_d62e0.cpp: register inputs are explicit parameters (fastcall ECX/EDX first); guest
// addresses are uint32_t and go through env->at(). Ordinal_10174 is the unit stat query shared by the first three
// functions; FUN_6fc2e660 is the single-key test used by 0xE4030.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2GamePredicatesEnv {
    void*    (*at)(uint32_t guest);                                  // guest address -> host pointer
    int32_t  (*o10174)(uint32_t unit, uint32_t a, uint32_t b);       // Ordinal_10174: stat query on a unit
    uint32_t (*o10920)(uint32_t id);                                 // Ordinal_10920: unit-table lookup
    uint32_t (*o10331)(uint32_t id);                                 // Ordinal_10331: unit lookup by id
    uint32_t (*o10826)(uint32_t unit);                               // Ordinal_10826
    int8_t   (*o10459)(uint32_t v);                                  // Ordinal_10459
    void     (*o10003)(uint32_t unit, uint32_t a, uint32_t b);       // Ordinal_10003
    int32_t  (*sub2E660)(uint32_t key);                              // FUN_6fc2e660 (rva 0x2E660)
};

template <typename E> static inline uint32_t R32(const E* e, uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (uint8_t*)e->at(a) + o, 4); return v; }
template <typename E> static inline uint8_t  R8 (const E* e, uint32_t a, uint32_t o) { return *((uint8_t*)e->at(a) + o); }

// 1.13c D2Game+0xE28C0 -- stat-record predicate A (sbit 0x16 on the unit, else sbit 0xB).
// p1 = caller record (byte +0xC is its state), p2 = record kind, p4 = unit.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_fn_d2game_E28C0(uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4, const D2GamePredicatesEnv* e)
{
    (void)p3;
    bool r = true;
    if (e->o10174(p4, 0x16, 0) != 0 || p2 != 0xFF || R8(e, p1, 0xC) != 1) {
        r = e->o10174(p4, 0x16, 0xB) != 0;
    }
    return r ? 1u : 0u;
}

// 1.13c D2Game+0xE2AB0 -- stat-record predicate B (sbit 0x15 gates; kind 0xFF tests p1+0xC, kind 0xF5 tests the
// record at p1+0x18). Returns 1 in the low byte when it passes, else the masked value.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_fn_d2game_E2AB0(uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4, const D2GamePredicatesEnv* e)
{
    (void)p3;
    uint32_t r = (uint32_t)e->o10174(p4, 0x15, 0);
    if (r == 0) {
        if (p2 == 0xFF) {
            r = 0;
            if (R8(e, p1, 0xC) == 1) return 1;
        } else if (p2 == 0xF5) {
            int32_t v = e->o10174(p4, 0x15, 4);
            r = 0;
            if (v != 0) {
                r = R32(e, p1, 0x18);
                if (R8(e, r, 0xC) == 0) return (r & 0xFFFFFF00u) | 1u;   // CONCAT31(r >> 8, 1)
            }
        }
    }
    return r & 0xFFFFFF00u;
}

// 1.13c D2Game+0xE4030 -- stat-record predicate for the 0x14 stat group. Kind 0xFE tests key 0x20343367, 0x108 tests
// key 0x2034336A, 0xF5 tests both keys plus sbits 2 and 4. A pass returns 1 in the low byte.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_fn_d2game_E4030(uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4, const D2GamePredicatesEnv* e)
{
    (void)p1; (void)p3;
    auto pass = [](int32_t v) -> uint32_t { return ((uint32_t)v & 0xFFFFFF00u) | 1u; };   // CONCAT31(v >> 8, 1)
    uint32_t r = (uint32_t)e->o10174(p4, 0x14, 0);
    if (r == 0) {
        r = (uint32_t)e->o10174(p4, 0x14, 1);
        if (r == 0) {
            if (p2 == 0xFE) {
                int32_t v = e->sub2E660(0x20343367);
                if (v != 0) return pass(v);
                v = e->o10174(p4, 0x14, 1);
                r = 0;
                if (v != 0) return pass(v);
            } else if (p2 == 0x108) {
                int32_t v = e->sub2E660(0x2034336A);
                r = 0;
                if (v != 0) return pass(v);
            } else if (p2 == 0xF5) {
                int32_t v = e->sub2E660(0x2034336A);
                if (v != 0) {
                    int32_t w = e->o10174(p4, 0x14, 2);
                    v = 0;
                    if (w == 0) return pass(v);
                }
                v = e->sub2E660(0x20343367);
                r = 0;
                if (v != 0) {
                    r = (uint32_t)e->o10174(p4, 0x14, 4);
                    v = 0;
                    if (r == 0) return pass(v);
                }
            }
        }
    }
    return r & 0xFFFFFF00u;
}

// 1.13c D2Game+0xE41D0 -- minion slot flag check. Finds the node of type 0x11 in the caller's list at +0x10F4, reads
// the unit in the slot selected by byte +0x6D, and when that unit has none of sbits 0x13/6/8/7 and the owner's unit
// has type 2, sets sbit 0x13/6. p1 = caller (fastcall ECX), p2 = unit id (EDX).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_E41D0(uint32_t p1, uint32_t p2, const D2GamePredicatesEnv* e)
{
    uint32_t node = R32(e, p1, 0x10F4);
    if (node == 0) return;
    node = R32(e, node, 0);
    if (node == 0) return;
    while (R32(e, node, 0) != 0x11) {
        node = R32(e, node, 0xF4);
        if (node == 0) return;
    }
    uint32_t owner = e->o10920(p2);
    uint32_t slot = R32(e, owner, 0x10 + (uint32_t)R8(e, p1, 0x6D) * 4);
    if (e->o10174(slot, 0x13, 0) == 0 && e->o10174(slot, 0x13, 6) == 0 &&
        e->o10174(slot, 0x13, 8) == 0 && e->o10174(slot, 0x13, 7) == 0) {
        uint32_t unit = e->o10331(p2);
        if (unit != 0) {
            uint32_t k = e->o10826(unit);
            if (e->o10459(k) == 2) e->o10003(slot, 0x13, 6);
        }
    }
}
