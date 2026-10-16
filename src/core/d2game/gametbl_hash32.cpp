// 1.13c D2Game+0x2060 (handle alloc), +0x2130 (Ptr), +0x2090 (New), +0x21D0 (MonitorFullness), +0x22D0 (Grow), +0x24B0 +0x25A0 +0x25F0 +0x2510 +0x2630 +0x1FE0 +0x2180 +0x1F60
// Storm TSExportTableSimpleReuse/TSHashTable instantiated for the game table, on retail's 32-bit field layout
//. Table words are 32-bit; pointers are the low 32 bits.
#include <emscripten.h>
#include <cstdint>
#include <cstdlib>
typedef uint32_t W;
static inline W& w(W a, int o) { return *(W*)(uintptr_t)(a + o); }

// 0x2130: fastcall(edx=table, esi=hash) -> node or 0
W d2game_2130_ptr(W t, W h) {
    W s = w(t, 0x1c) + (h & w(t, 0x24)) * 12;
    int32_t n = (int32_t)w(s, 8);
    if (n < 1) n = 0;
    while (n > 0) {
        if (w((W)n, 0) == h) return (W)n;
        n = (int32_t)w((W)n + w(s, 0), 4);
    }
    return 0;
}

// 0x2060: GenerateUniqueHandle (edi=table): returns the new sequence at +0x48
W d2game_2060_genhandle(W t) {
    for (;;) {
        W seq;
        while ((seq = ++w(t, 0x48)) == 0) w(t, 0x4c) = 1;
        if (w(t, 0x4c) == 0) break;
        if (!d2game_2130_ptr(t, seq)) break;
    }
    return w(t, 0x48);
}

// Storm TSExplicitList link ops (nodes: link = node+listOffset; {next, prev-or-~head-slot}); the table's slots are 12-byte lists {offset, head link, ~first}.
// 1.13c D2Game+0x1FE0 / +0x2180 / +0x1F60 / +0x24B0 / +0x25A0 (list uninit, unlink-all, element destroy/construct), all on 32-bit words.
static void unlinkLink(W l) {
    W n = w(l, 0);
    if (!n) return;
    int32_t b = (int32_t)w(l, 4);
    W p = b < 0 ? (W)~b : l - w(n, 4) + (W)b;
    w(p, 0) = n; w(n, 4) = w(l, 4); w(l, 0) = 0; w(l, 4) = 0;
}
static void unlinkAll(W list) { for (int32_t n; (n = (int32_t)w(list, 8)) > 0;) unlinkLink(w(list, 0) + (W)n); }   // 0x2180
static void listUninit(W list) { unlinkAll(list); unlinkLink(list + 4); }   // 0x1FE0 (and its 0x1F60 wrapper)
static void listInit(W list) { w(list, 0) = 0xdddddddd; w(list, 4) = list + 4; w(list, 8) = ~(list + 4); }   // 0x24B0

// Storage for the slot array (SMemReAlloc/SMemAlloc of 0x2630): old block, byte size -> new block. The default hands back a fresh zeroed block:
// retail's realloc keeps the old bytes, whose list heads point into the old block, but Grow (0x22D0) re-inits every slot whose +0 is not 4 and the
// nodes are already parked, so a zeroed block is the safe equal. The capture hook returns a pre-zeroed blob the same way.
W (*d2game_tbl_realloc)(W old, W bytes) = [](W old, W bytes) -> W { void* p = calloc(1, bytes); free((void*)(uintptr_t)old); return (W)(uintptr_t)p; };

// 0x25F0 (eax = n, stack = array hdr), 0x2630 (ebx = cap, esi = hdr), 0x2510 (eax = hdr, edi = n): the growable array {cap, count, data, align} at table+0x14.
static W arrAlign(W n, W hdr) {
    if (n >= 0x15) { w(hdr, 0xc) = 0x15; return 0x15; }
    for (W c = (n - 1) & n; c; c = (n - 1) & n) n = c;
    return n < 1 ? 1 : n;
}
static void arrSetCap(W hdr, W cap) {   // the OOM copy-construct fallback of 0x2630 (via 0x26F0/0x2740) is not modelled
    W old = w(hdr, 8);
    for (W i = cap; i < w(hdr, 4); ++i) listUninit(old + i * 12);
    w(hdr, 0) = cap;
    w(hdr, 8) = d2game_tbl_realloc(old, cap * 12);
}
static void arrSetCount(W hdr, W n) {
    W c = w(hdr, 4);
    if (n > c) {
        if (n > w(hdr, 0)) {
            W a = w(hdr, 0xc) ? w(hdr, 0xc) : arrAlign(n, hdr), r = n % a;
            arrSetCap(hdr, r ? n + a - r : n);
        }
        for (W i = w(hdr, 4); i < n; ++i) listInit(w(hdr, 8) + i * 12);
    } else if (n < c) {
        for (W i = n; i < w(hdr, 4); ++i) listUninit(w(hdr, 8) + i * 12);
    }
    w(hdr, 4) = n;
}

// 0x22D0 Grow: park every node on a temp list, double the slot array (mask = 2n-1), re-init slots, re-hash the nodes (node+0 = hash).
void d2game_22d0_grow(W t) {
    W n = w(t, 0x24) + 1, nn = n * 2;
    W T[3]; W tt = (W)(uintptr_t)T;
    listInit(tt);
    w(tt, 0) = 4; w(tt, 4) = tt + 4; w(tt, 8) = ~(tt + 4);
    W cur = tt + 4;
    for (W i = 0; i < n; ++i) {
        W slot = w(t, 0x1c) + i * 12;
        for (int32_t h; (h = (int32_t)w(slot, 8)) > 0;) {
            W l = (W)h + w(tt, 0);
            unlinkLink(l);
            w(l, 0) = cur; w(l, 4) = w(cur, 4); w(cur, 4) = (W)h; cur = l;
        }
    }
    arrSetCount(t + 0x14, nn);
    for (W i = 0; i < nn; ++i) {
        W s = w(t, 0x1c) + i * 12;
        if (w(s, 0) != 4) { unlinkAll(s); w(s, 4) = s + 4; w(s, 0) = 4; w(s, 8) = ~(s + 4); }
    }
    w(t, 0x24) = nn - 1;
    for (int32_t h; (h = (int32_t)w(tt, 8)) > 0;) {
        W s = w(t, 0x1c) + (w((W)h, 0) & w(t, 0x24)) * 12;
        W l = w(s, 0) + (W)h;
        unlinkLink(l);
        W term = w(s, 4);
        w(l, 0) = term; w(l, 4) = w(term, 4); w(term, 4) = (W)h; w(s, 4) = l;
    }
    listUninit(tt);
}

// 0x2090: New slot node link (edi=table): hash, extrabytes, flags; internalNew = vtable[1].
// MonitorFullness (0x21D0, ecx = table, eax = slot): chain length > 13 resets the counter and grows the table (0x22D0).
typedef W (*InternalNewFn)(W list, W extra, W flags);
int d2game_21d0_fullness(W t, W slot) {
    if (w(t, 0x24) > 0x3fe) return 0;
    W& f = w(t, 0x10);
    f = f < 4 ? 0 : f - 3;
    W s = w(t, 0x1c) + slot * 12;
    int32_t n = (int32_t)w(s, 8);
    if (n < 1) n = 0;
    while (n > 0) {
        if (++f > 0xd) { f = 0; d2game_22d0_grow(t); return 1; }
        n = (int32_t)w((W)n + w(s, 0), 4);
    }
    return 0;
}

W d2game_2090_new(W t, W hash, W extra, W flags, InternalNewFn internalNew) {
    W slot = hash & w(t, 0x24);
    if (d2game_21d0_fullness(t, slot)) slot = hash & w(t, 0x24);
    W node = internalNew(w(t, 0x1c) + slot * 12, extra, flags);
    W l = node ? w(t, 4) + node : t + 8;  // link of the node in the full list
    if (w(l, 0)) {
        int32_t b = (int32_t)w(l, 4), p;
        p = b < 0 ? (int32_t)~b : (int32_t)(l - w(w(l, 0), 4) + (W)b);
        w((W)p, 0) = w(l, 0);
        w(w(l, 0), 4) = w(l, 4);
        w(l, 0) = 0; w(l, 4) = 0;
    }
    W h = w(t, 8);
    w(l, 0) = h; w(l, 4) = w(h, 4); w(h, 4) = node; w(t, 8) = l;
    w(node, 0) = hash;
    return node;
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_2060(uint32_t* r) {  // r[0] = table (edi)
    r[4] = d2game_2060_genhandle(r[0]);
}
