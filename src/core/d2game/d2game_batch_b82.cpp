// src/core/d2game/d2game_batch_b82.cpp -- 1.13c D2Game.dll 0x79850 0x8A770 0x8B3D0 0x98250,
// transcribed from the cached Ghidra decompiles. Alpha lane: implementation + marker, no vector.
// Units/games are opaque host pointers read at 32-bit retail offsets; pointer fields are read as
// 4 bytes and widened. Callees whose arguments Ghidra hides are seams on D2GameB82Env.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline const uint8_t* rdp(const void* p, uint32_t o) { return (const uint8_t*)(uintptr_t)rd32(p, o); }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, sizeof v); }

struct D2GameB82Env {
    void (*fatal)(uint32_t code);          // Ordinal_10265 + Ordinal_10024 + _exit(-1)
    uintptr_t (*ord10920)(uintptr_t unit);              // unit -> player/monster data
    uintptr_t (*ord10331)(uintptr_t node);
    int (*ord10826)(uintptr_t v);
    uintptr_t (*ord10867)(uintptr_t a);
    uintptr_t (*ord10750)(uintptr_t a);
    void (*ord10637)(uintptr_t a);
    uintptr_t (*ord10511)(uintptr_t a);
    int (*ord10601)(uintptr_t a, const char* file, uint32_t line);
    void (*fn_586F0)(void);
    void* (*fn_8F930)(uintptr_t game, void* args);
    void (*fn_99100)(void* a);
    void (*fn_98A90)(void);
    void (*fn_99730)(void* a, void* b);
    void (*fn_982F0)(void* a, void* b);
    void (*fn_98550)(void* a, void* b, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g);
    void (*fn_8A3E0)(const void* buf, uint32_t n);
    void (*fn_97D10)(void* unit, const void* p);
    void (*fn_8E2A0)(void);
    const uint8_t* (*dataTables)(void);                 // *sgptDataTables
    uint32_t bitMask8;                                  // gdwBitMasks[8]
};

// hash-chain lookup: bucket table at game+base, mask 0x7f, id at +0xc, next at +nextOff
static const uint8_t* findUnit(const uint8_t* game, uint32_t base, uint32_t id, uint32_t nextOff)
{
    for (const uint8_t* n = rdp(game, base + (id & 0x7f) * 4); n; n = rdp(n, nextOff))
        if (rd32(n, 0xc) == id) return n;
    return nullptr;
}

// 1.13c D2Game+0x79850
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_79850(void* p1, const uint8_t* p2, const D2GameB82Env* e)
{
    int mode = 0;
    bool call = false;
    if (!p2) {
        // mode 0, fall to the lookup
    } else {
        mode = (int)rd32(p2, 0x10);
        if (mode >= 7 && (mode <= 8 || mode == 0xb)) call = true;
    }
    if (!call) {
        uintptr_t r = e->ord10511((uintptr_t)p2);
        if (r) {
            int idx = e->ord10601(r, "..\\Source\\D2Game\\PLAYER\\PlrModes.cpp", 0x778);
            const uint8_t* t = e->dataTables();
            if (idx >= 0 && idx < (int)rd32(t, 0xba0)) {
                const uint8_t* rec = rdp(t, 0xb98) + (uintptr_t)idx * 0x23c;
                if (rec && (e->bitMask8 & rec[6])) call = true;
            }
        }
    }
    if (call) { e->fn_99100(p1); e->fn_98A90(); }
    if (mode == 0) e->fn_99730(p1, (void*)p2);
    else if (mode != 0x13) e->fn_982F0(p1, (void*)p2);
    else e->fn_98550(p1, (void*)p2, 0, 4, 0xffffffffu, 0, 0);
}

// 1.13c D2Game+0x8A770  (EAX = 6-byte source)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_8A770(const void* src, const D2GameB82Env* e)
{
    uint8_t b[6]; std::memcpy(b, src, 6);
    e->fn_8A3E0(b, 6);
}

// 1.13c D2Game+0x8B3D0  (EAX = 40-byte source)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_8B3D0(const void* src, const D2GameB82Env* e)
{
    uint8_t b[40]; std::memcpy(b, src, 40);
    e->fn_8A3E0(b, 0x28);
}

// 1.13c D2Game+0x98250  (EAX = unit)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_98250(const uint8_t* game, uintptr_t unit, const D2GameB82Env* e)
{
    uintptr_t pd = e->ord10920(unit);
    const uint8_t* p1 = rdp((void*)pd, 0x44);
    if (!p1) return;
    e->fn_97D10((void*)unit, p1);
    if (unit && rd32((void*)unit, 0) == 0) e->ord10920(unit);
    const uint8_t* t = e->dataTables();
    int n = (int)rd32(t, 0xbf0);
    uint32_t off = 0xe0, o2 = 0xc;
    for (int i = 1; i < n; i++, off += 0xe0, o2 += 0xc) {
        t = e->dataTables(); n = (int)rd32(t, 0xbf0);
        if (i >= n) break;
        const uint8_t* rec = rdp(t, 0xbe8) + off;
        if (!rec || !(e->bitMask8 & rec[4])) continue;
        for (const uint8_t* q = rdp(rdp(p1, 0), o2); q; q = rdp(q, 0x14)) {
            if (*q & 1) continue;
            if (!game) e->fatal(0x2d3);
            if (findUnit(game, 0x1320, rd32(q, 4), 0xe4)) { e->fn_8E2A0(); break; }
        }
    }
}
