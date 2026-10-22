// src/core/d2game/d2game_e8b60.cpp -- 1.13c D2Game+0xE8B60, the a3q4 quest event callback (q+0xcc slot).
// Transcribed from the cached decompile + disassembly (the decompile lost the register inputs).
// Same Env/seam conventions as d2game_batch_e6850.cpp: structs are 32-bit images, pointers via env->h/p,
// env->ord = D2Common ordinal, env->call(rva, logical args...) = D2Game+rva with the register inputs
// listed per call below given in this order.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static inline uint32_t rd32(uintptr_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void     wr32(uintptr_t p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline uint8_t  rd8(uintptr_t p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline void     wr8(uintptr_t p, uint32_t o, uint8_t v) { ((uint8_t*)p)[o] = v; }
static inline uint16_t rd16(uintptr_t p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }

struct D2GameE8B60Env {
    uintptr_t (*ord)(uint32_t n, uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d, uintptr_t e);
    uintptr_t (*call)(uint32_t rva, uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d, uintptr_t e, uintptr_t f, uintptr_t g);
    uint32_t  (*addr)(uint32_t rva);
    uint32_t  (*h)(uintptr_t host);
    uintptr_t (*p)(uint32_t handle);
    uintptr_t (*isBadCodePtr)(uint32_t handle);
    void      (*exit)(int code);
    uintptr_t (*callQ)(uint32_t handle, uintptr_t q);   // __thiscall-like: ecx = q
};
#define O(n, a, b, c) env->ord(n, (uintptr_t)(a), (uintptr_t)(b), (uintptr_t)(c), 0, 0)
#define C(rva, a, b, c, d) env->call(rva, (uintptr_t)(a), (uintptr_t)(b), (uintptr_t)(c), (uintptr_t)(d), 0, 0, 0)
#define A(rva) env->addr(rva)

// 1.13c D2Game+0xE8B60   (q = ecx, msg = edx)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_E8B60(uintptr_t q, uintptr_t msg, const D2GameE8B60Env* env)
{
    const uint16_t ev = rd16(msg, 0x14), sub = rd16(msg, 0x18);
    const uint32_t unit = rd32(msg, 0), game = rd32(msg, 0xc);
    uintptr_t pl = O(10920, game, 0, 0);
    uintptr_t u = unit ? env->p(unit) : 0;
    const uint32_t id = rd32(pl, 0x10 + rd8(u, 0x6d) * 4);
    uintptr_t d = env->p(rd32(q, 0x18));
    const uint32_t cb = A(0xE6BD0);
    if (O(10174, id, 0x14, 0) == 1) return;

    auto q55eb0 = [&](uint32_t n) { C(0x35EB0, n, q, 0, 0); };              // eax=n esi=q
    auto q55910 = [&](uint32_t a) { C(0x35910, a, game, unit, 0); };         // eax=a ecx=game stack=unit
    auto q6b90  = [&]() { C(0xE6B90, game, unit, 0, 0); };                   // eax=game esi=unit
    auto q566a0 = [&](uint32_t tag) { C(0x366A0, tag, unit, game, 0); };     // eax=tag ecx=unit esi=game
    auto q56470 = [&](uint32_t tag) { return C(0x36470, tag, game, unit, 0); };// ecx=tag edx=game stack=(unit,2,1) eax=0

    if (ev == 0xf5) {
        if (sub == 0x20f) {
            O(10003, id, 0x14, 2);
            if (rd8(q, 0xc) == 1) { q55eb0(2); wr8(d, 9, 1); wr32(q, 0xa8, cb); }
            q55910(rd32(msg, 8));
            wr32(d, 0x18, 2);
            q6b90();
        } else if (sub == 0x213) {
            O(10003, id, 0x14, 4);
            wr8(d, 10, 1);
            q55910(rd32(msg, 8));
            wr32(d, 0x18, 4);
            q6b90();
        }
    } else if (ev == 0xfe) {
        if (sub == 0x216) {
            if (!C(0xE660, 0x20343367, game, unit, 0)) return;
            if (O(10174, id, 0x14, 1) != 0 || O(10174, id, 0x14, 0) != 0) return;
            q566a0(0x20343367);
            wr32(d, 0x10, rd32(d, 0x10) - 1);
            if (rd8(q, 9) != 0 && rd8(q, 0xc) != 4) q55eb0(4);
            O(10003, id, 0x14, 1);
            wr8(d, 8, 1);
            wr32(q, 0xa8, cb);
            wr32(d, 0x18, 1);
            q6b90();
        } else if (sub == 0x21a && O(10174, id, 0x14, 0) == 0) {
            if (O(10174, id, 0x14, 1) != 0) {
                O(10593, id, 0x14, 1);
                O(10003, id, 0x14, 0);
                O(10003, id, 0x14, 0xd);
                O(10495, id, rd32(q, 0xe0), 0);
                O(10003, id, 0x14, 5);
                if (rd8(q, 9) != 0) q55eb0(5);
                q56470(0x207a7978);                                           // eax=0, (unit,2,1)
                C(0x34C00, game, unit, 0, 0);                                 // eax=game stack=unit
                C(0x33880, q + 0x1c, game ? rd32(env->p(game), 0xc) : (uint32_t)-1, 0, 0); // eax=&q[0x1c] esi=game id
            }
            if (O(10174, id, 0x14, 0xd) != 0) {
                C(0x33920, 0x14, 0xd, unit, 0);                               // eax=unit stack=(0x14,0xd)
                uint32_t fp = rd32(q, 0xf0);
                if (env->isBadCodePtr(fp)) {
                    uintptr_t r = O(10265, 0x221, 0, 0);
                    O(10024, A(0xF9FF9), r, 0);
                    env->exit(-1);
                }
                env->callQ(fp, q);
            }
        }
    } else if (ev == 0x108 && sub == 0x211) {
        if (!C(0xE660, 0x2034336a, game, unit, 0)) return;
        wr32(q, 0xa8, cb);
        q566a0(0x2034336a);
        wr32(d, 0x10, rd32(d, 0x10) - 1);
        if (q56470(0x20343367)) {
            wr32(d, 0x10, rd32(d, 0x10) + 1);
            q55eb0(3);
            wr8(d, 0xb, 1);
            wr32(q, 0xa8, cb);
        }
    }
}
