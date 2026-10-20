// src/core/d2game/d2game_batch_f05b0.cpp -- 1.13c D2Game.dll 0xF05B0..0xF1730 (12 functions),
// transcribed from the cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c).
// Alpha lane (OBJECTIVE v8.3): implementation + marker only, no vector.
//
// Host model: units/structs are opaque host pointers read at Ghidra's offsets. Register-passed
// hidden arguments (EAX/EBX/ESI/stack) are explicit parameters. Callees whose arguments Ghidra
// does not show are zero-arg seams on D2GameF05B0Env, called in retail order.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>

static inline void* rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, sizeof v); }

struct D2GameF05B0Env {
    void* (*ord10920)(void* pUnit);                 // unit -> player data
    uintptr_t (*ord10392)(uintptr_t a);             // F1490/F1420 lookup
    void (*ord11129)(void);
    uint32_t (*fn_DFC10)(void* args16);             // 0xDFC10 (4-dword arg block)
    void (*fn_8B400)(uint32_t v, uint32_t kind, uint32_t grp);  // 0x8B400, grp = switch arm
    void (*fn_102F0)(void* pUnit);
    void (*fn_10130)(void);
    uint32_t (*fn_101D0)(void);
    void (*fn_10220)(void);
    uint32_t (*fn_102A0)(void);
    void (*fn_8D120)(void);
    void (*fn_103E0)(void* a, void* b);
    uint16_t (*fn_9BEB0)(void* pUnit);
    uint16_t (*fn_9BF70)(void);
    void (*fn_9C4F0)(uint16_t v, void* pUnit);
    void (*fn_9C160)(const void* cb);
    void (*fn_9C840)(void);
    uintptr_t (*fn_9E810)(void* a, int b);
    void (*fn_7A440)(int16_t a, int8_t b);
    int (*ord10494)(void);
    int (*fn_F14F0)(void* a, void* b, uint32_t c, void* d, void* e, uint32_t f);
    void (*fn_E07A0)(uintptr_t a);
    void* (*fn_E09D0)(uintptr_t a);
    const void* cb_10820;
};

static inline uint32_t unitId(void* u) { return u ? rd32(u, 0xc) : 0xFFFFFFFFu; }

static void* findNode(const D2GameF05B0Env* e, void* self, void* other)
{
    void* pd = e->ord10920(self);
    uint32_t id = unitId(other);
    if (self == other) return nullptr;
    for (void* n = rdptr(pd, 0x38); n; n = rdptr(n, 0x10))
        if (rd32(n, 0) == id) return n;
    return nullptr;
}

// 1.13c D2Game+0xF05B0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_F05B0(void* self, void* other, const D2GameF05B0Env* e)
{
    if (findNode(e, self, other)) { uint32_t a[4] = {}; e->fn_DFC10(a); }
}

// 1.13c D2Game+0xF0640
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_F0640(void* self, int set, uint32_t mask, void* other, const D2GameF05B0Env* e)
{
    void* n = findNode(e, self, other);
    if (self != other && n) {
        uint32_t f = rd32(n, 4);
        wr32(n, 4, set == 0 ? (f & ~mask) : (f | mask));
        uint32_t a[4] = {}; e->fn_DFC10(a);
    }
}

// 1.13c D2Game+0xF0720
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_F0720(uint32_t v, uint8_t kind, const D2GameF05B0Env* e)
{
    uint32_t grp = (kind == 1 || kind == 2) ? 1 : kind == 3 ? 3 : kind == 4 ? 4 :
                   (kind == 5 || kind == 7 || kind == 8) ? 5 : 6;
    e->fn_8B400(v, kind, grp);
}

static uint32_t playerField9c(void* u, const D2GameF05B0Env* e)
{
    return (u && rd32(u, 0) == 0) ? rd32(e->ord10920(u), 0x9c) : 0;
}

// 1.13c D2Game+0xF0870
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_F0870(void* a, void* b, const D2GameF05B0Env* e)
{
    e->fn_102F0(a);
    e->fn_102F0(b);
    if ((int16_t)e->fn_101D0() == 0) {
        e->fn_10220();
        e->fn_10220();
        d2_fn_d2game_F0720(playerField9c(b, e), 5, e);
    }
    e->fn_10130();
    e->fn_10130();
}

static void callIfPlaced(void* u, const D2GameF05B0Env* e)
{
    if (u && rd32(u, 0x80) != 0 && rd32(u, 0) == 0) e->fn_9BEB0(u);
}

// 1.13c D2Game+0xF0900
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_F0900(void* a, void* b, const D2GameF05B0Env* e)
{
    e->fn_102F0(a);
    e->fn_102F0(b);
    if ((int16_t)e->fn_101D0() == 4) {
        callIfPlaced(b, e);
        e->fn_10220();
        callIfPlaced(a, e);
        e->fn_10220();
        d2_fn_d2game_F0720(playerField9c(b, e), 6, e);
    }
    e->fn_10130();
    e->fn_10130();
}

// 1.13c D2Game+0xF09E0 (EBX = unit, param = other)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_F09E0(void* other, void* unit, const D2GameF05B0Env* e)
{
    if (unitId(unit) == unitId(other)) return;
    bool skip = !(unit && rd32(unit, 0x80) != 0 && rd32(unit, 0) == 0) || e->fn_9BEB0(unit) == 0xFFFF;
    if (skip && (int16_t)e->fn_101D0() == 4) { e->fn_10220(); return; }
    e->fn_10220();
    e->fn_10220();
}

// 1.13c D2Game+0xF0C60 (EAX = unitB)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_F0C60(void* a, int p2, uint32_t unused, uint32_t p4, void* unitB, void* other, const D2GameF05B0Env* e)
{
    (void)unused;
    e->fn_102F0(a);
    e->fn_102F0(unitB);
    if (e->fn_102A0() != p4) d2_fn_d2game_F0640(a, p2, 0, other, e);  // mask is in EAX at call; seam-level
    e->fn_8D120();
    e->fn_8D120();
}

// 1.13c D2Game+0xF0D50
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_F0D50(void* p1, void* a, void* b, const D2GameF05B0Env* e)
{
    uint16_t id = (b && rd32(b, 0x80) != 0 && rd32(b, 0) == 0) ? e->fn_9BEB0(b) : 0xFFFF;
    e->fn_102F0(a);
    e->fn_102F0(b);
    if ((int16_t)e->fn_101D0() != 2) { e->fn_10130(); e->fn_10130(); return; }
    if (id == 0xFFFF) {
        id = e->fn_9BF70();
        e->fn_9C4F0(id, b);
        e->fn_9C4F0(id, a);
        e->fn_103E0(p1, b);
    } else {
        e->fn_9C4F0(id, a);
    }
    e->fn_103E0(p1, a);
    d2_fn_d2game_F0720(playerField9c(a, e), 8, e);
    uint32_t args[4] = {};
    e->fn_DFC10(args);
}

// 1.13c D2Game+0xF10F0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_F10F0(void* unit, const D2GameF05B0Env* e)
{
    callIfPlaced(unit, e);
    e->fn_9C160(e->cb_10820);
    e->fn_9C840();
}

// 1.13c D2Game+0xF1420 (EAX = a, ESI = unit, EBX = out)
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_F1420(uintptr_t a, void* unit, uint32_t* out, const D2GameF05B0Env* e)
{
    e->fn_E07A0(a);
    void* slot = rdptr(unit, 0x2c);
    uintptr_t r = e->ord10392(0);
    if (r == (uintptr_t)unit || r == 0) {
        uint32_t* p = (uint32_t*)e->fn_E09D0(a);
        e->ord11129();
        if (out) {
            *out = p ? p[0] : 6;
            wr32(slot, 0, p ? p[3] : 0xFFFFFFFFu);
        }
    }
    return 1;
}

// 1.13c D2Game+0xF1490 (ESI = unit, EBX = out, retaddr slot = out2)
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_F1490(uintptr_t self, void* unit, int p1, uint32_t* out, uint32_t* out2, const D2GameF05B0Env* e)
{
    e->fn_E07A0(self);
    e->ord10392((uintptr_t)rdptr(unit, 0x2c));
    uint32_t* p = (uint32_t*)e->fn_9E810(unit, p1);
    if (!p) return 0;
    e->ord11129();
    if (out) { *out = p[0]; *out2 = p[3]; }
    return 1;
}

// 1.13c D2Game+0xF1730
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_F1730(void* inEax, uint32_t p2, uint32_t p3, void* retaddrArg, void* p3ptr, uint32_t ebx, const D2GameF05B0Env* e)
{
    if (!inEax) return 0;
    void* pv = rdptr(inEax, 0x80);
    if (pv && e->ord10494() == 0 && e->fn_F14F0(inEax, pv, p2, retaddrArg, p3ptr, ebx) != 0) {
        e->fn_7A440((int16_t)p2, (int8_t)(uintptr_t)retaddrArg);
        return 1;
    }
    return 0;
}
