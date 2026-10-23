// src/core/d2client/d2client_inv_sync_82fe0.cpp -- 1.13c D2Client+0x82FE0: reconcile a unit's equipped-item
// slots (inventory = unit+0x60) after an inventory change. Alpha batch lane: implementation + marker only.
// Ordinals / unrecovered callees are host seams (same Env shape as d2client_batch_82550.cpp).
// Decompiler register noise (unaff_EBX/retaddr) resolved as: iVar3 after the first loop = inventory handle.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>

typedef uintptr_t U;
struct D2cEnv82fe {
    U (*ord)(uint32_t n, U a, U b, U c, U d);
    U (*fn)(uint32_t rva, U a, U b, U c, U d);
};
template <class T> static T rd(const uint8_t* p, size_t o) { T v; memcpy(&v, p + o, sizeof v); return v; }
static U P(const void* p) { return (U)p; }

// 1.13c D2Client+0x82FE0   (param_1 = unit)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_82fe0(const D2cEnv82fe* e, uint8_t* p1) {
    if (!p1) return;
    int32_t type = rd<int32_t>(p1, 0);
    uint32_t flags = rd<uint32_t>(p1, 0xC4);
    U inv = rd<U>(p1, 0x60);
    if (!((type == 0 || ((flags >> 9) & 1)) && inv)) return;
    int changed = 0;
    e->fn(0x81b90, 0, 0, 0, 0);
    // pass 1: walk the inventory list; unequip/flag items that no longer fit
    for (U n = e->ord(10460, inv, 0, 0, 0); n; n = e->ord(10464, n, 0, 0, 0)) {
        U it = e->ord(11147, n, 0, 0, 0);
        if (it && e->ord(10698, n, 0, 0, 0) == 1 && e->ord(11057, it, P(p1), 0, 0) &&
            e->ord(11160, it, 0, 0, 0) == 0 && e->fn(0x81c30, 0, 0, 0, 0)) {
            e->ord(10977, it, 0x4000, 0, 0);
            e->fn(0x81ff0, 0, 0, 0, 0);
        }
    }
    // pass 2: equipped slots 0..10 that are not (already) flagged get flagged and re-applied
    for (int s = 0; s < 0xb; s++) {
        U it = e->ord(11139, inv, s, 0, 0);
        if (it && (e->ord(10202, it, 0x100, 0x203, 0) == 0 || e->ord(11160, it, 0, 0, 0) != 0) &&
            (e->ord(10202, it, 0x4000, 0x208, 0) == 0 || e->ord(11160, it, 0, 0, 0) != 0) &&
            e->fn(0x81c30, 0, 0, 0, 0) == 0) {
            e->ord(10977, it, 0x4000, 1, 0);
            e->ord(10921, P(p1), it, 0, 0);
            e->fn(0x82610, 0, 0, 0, 0);
            changed = 1;
        }
    }
    // pass 3: re-seat items that now satisfy requirements, repeat up to 100 times while anything moved
    int guard = 100;
    bool moved;
    do {
        moved = false;
        for (int s = 0; s < 0xb; s++) {
            U it = e->ord(11139, inv, s, 0, 0);
            if (!it || e->ord(10202, it, 0x100, 0x226, 0) != 0) continue;
            if (!(e->ord(10202, it, 0x4000, 0x22a, 0) != 0 || e->ord(11160, it, 0, 0, 0) == 0)) continue;
            if (!e->ord(10244, it, P(p1), 0, 0)) continue;
            bool ok;
            U need = e->ord(11144, it, 0, 0, 0);
            if (need == 0) ok = true;
            else {
                U o = e->ord(11139, inv, 4, 0, 0);
                if (o == it) o = e->ord(11139, inv, 5, 0, 0);
                ok = o && e->ord(10744, o, need, 0, 0) != 0;
            }
            if (!ok) continue;
            e->ord(10977, it, 0x4000, 0, 0);
            e->ord(10921, P(p1), it, 0, 0);
            uint8_t c = (uint8_t)e->ord(10417, it, 0, 0, 0);
            if (inv && c != 11 && c != 12) {
                e->ord(10694, inv, it, 0, 0);
                e->fn(0x69a90, P(p1), 0, 0, 0);
                if (s == 1 && e->ord(11160, it, 0, 0, 0) == 0) e->fn(0x81ff0, 0, 0, 0, 0);
                e->ord(10138, inv, it, 0, 0);
            }
            moved = true;
            changed = 1;
        }
    } while (moved && guard-- != 0);
    // pass 4: type-5 items not in flagged state are dropped from the inventory
    for (int s = 0; s < 0xb; s++) {
        U it = e->ord(11139, inv, s, 0, 0);
        if (it && e->ord(10106, it, 0, 0, 0) == 5 && e->ord(10202, it, 0x100, 0x247, 0) == 0 &&
            e->ord(10202, it, 0x4000, 0x248, 0) == 0) {
            if (rd<int32_t>(p1, 0) == 0) e->ord(10061, rd<U>(p1, 0x60), 0, 0, 0);
            e->ord(10202, it, 0x100, 0x96, 0);
            e->ord(11127, P(p1), it, 0, 0);
            e->fn(0x81ff0, 0, 0, 0, 0);
        }
    }
    e->fn(0x82410, 0, 0, 0, 0);
    if (changed) {
        if (rd<int32_t>(p1, 0) == 0) {
            if ((rd<uint32_t>(p1, 0xC4) & 0x10000u) == 0 && rd<int32_t>(p1, 0x10) != 0 && rd<int32_t>(p1, 0x10) != 0x11)
                e->fn(0x19830, 7, P(p1), 0, 0);
            e->fn(0x6a220, 0, 0, 0, 0);
            return;
        }
        e->fn(0x4dc70, 7, P(p1), 0, 0);
    }
}
