// src/core/d2client/name_a5d90.cpp -- 1.13c D2Client+0xA5D90 (alpha lane: implementation + marker).
// Source: build-wasm/ghidra-d2client/decomp/0xA5D90.c. Retail __usercall: EAX = obj (32-bit retail
// layout, read through rd32 so no host pointers enter offsets). Every callee is an env seam; the
// Storm/Fog/D2Common ordinals are unverified names.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

struct D2ClientA5d90Env {
    uint32_t  (*rd32)(uintptr_t addr);
    uint8_t   (*rd8)(uintptr_t addr);
    void      (*fatal)(uint32_t line);            // Fog#10265(line) + Fog#10024 + _exit(-1)
    uintptr_t (*empty)();                         // Ordinal 10003 (returns a Unicode*), no args
    uintptr_t (*s16e0)();                         // 0x16E0 -> Unicode*
    int32_t   (*ustrlen)(uintptr_t s);            // Unicode::strlen
    void      (*s1230)();                         // 0x1230, result ignored
    uintptr_t (*o10695)(uint32_t v);              // Ordinal 10695(obj+4) -> Unicode*; retail then returns o10003()
    uint32_t  (*o10331)();
    void      (*o10383)(uint32_t a, uint32_t* count, uintptr_t* arr);
    int32_t   (*o10934)();
    uintptr_t (*a5340)(uintptr_t node);           // FindType5; arg per call is unverified
    uintptr_t (*a5310)(uintptr_t obj, uint32_t a);// name from record; EAX=obj
};
// Retail obj offsets: +0 kind, +4 id, +0x10 sub, +0x14 -> (ptr -> record).
static constexpr uint32_t kKind = 0, kId = 4, kSub = 0x10, kRef = 0x14;
static_assert(kRef == 5 * sizeof(uint32_t));

// 1.13c D2Client+0xA5D90
extern "C" EMSCRIPTEN_KEEPALIVE
uintptr_t d2_client_a5d90(uintptr_t obj, const D2ClientA5d90Env* e) {
    if (!obj) { e->fatal(0x99d); return 0; }
    switch (e->rd32(obj + kKind)) {
    default: return 0;
    case 1: {
        uintptr_t s = e->s16e0();
        if (e->ustrlen(s) == 0) { e->s1230(); return e->empty(); }
        return s;
    }
    case 4: e->o10695(e->rd32(obj + kId)); return e->empty();
    case 2: break;
    }
    uintptr_t ref = e->rd32(obj + kRef);
    uintptr_t rec = e->rd32(ref);
    if (e->rd8(rec + 0x1b8) && e->rd32(obj + kSub) == 2) {
        uint32_t me = e->o10331();
        if (e->a5340(me)) return e->a5310(obj, me);
        uint32_t n = 0; uintptr_t arr = 0;
        e->o10383(me, &n, &arr);
        for (uint32_t i = 0; i < n; ++i) {
            uint32_t m = e->rd32(arr + i * 4);
            if (m != me && e->a5340(m)) return e->a5310(obj, m);
        }
    }
    if (e->rd8(rec + 0x13a) == 0) {
        if (e->o10934()) return e->empty();
        rec = e->rd32(e->rd32(obj + kRef));
        if (e->rd8(rec + 0x171) && (int8_t)e->rd8(ref + 4) < 0) return e->empty();
        return rec + 0x40;
    }
    return e->empty();   // sub 0/4/5/6 and everything else all return Ordinal_10003()
}
