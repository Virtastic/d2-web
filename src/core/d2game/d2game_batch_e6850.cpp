// src/core/d2game/d2game_batch_e6850.cpp -- twelve 1.13c D2Game.dll functions transcribed from the cached
// Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Base 0x6FC20000.
//
// Retail structs are 32-bit images: fields are read/written with rd32/wr32 and every pointer stored in or
// loaded from a field goes through env->h / env->p (image handle <-> host pointer), never a bare cast.
// env->ord(n, ...) is D2Common ordinal n (five args); env->call(rva, ...) calls D2Game+rva with up to seven
// args; env->addr(rva) is the image handle of a D2Game function/data at +rva (callback tables, strings).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static inline uint32_t rd32(uintptr_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void     wr32(uintptr_t p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline uint8_t  rd8(uintptr_t p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline void     wr8(uintptr_t p, uint32_t o, uint8_t v) { ((uint8_t*)p)[o] = v; }
static inline void     wr16(uintptr_t p, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, 2); }

struct D2GameBatchE6850Env {
    uintptr_t (*ord)(uint32_t n, uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d, uintptr_t e);
    uintptr_t (*call)(uint32_t rva, uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d, uintptr_t e, uintptr_t f, uintptr_t g);
    uint32_t  (*addr)(uint32_t rva);     // image handle of D2Game+rva
    uint32_t  (*h)(uintptr_t host);      // host pointer -> image handle
    uintptr_t (*p)(uint32_t handle);     // image handle -> host pointer
    void      (*exit)(int code);         // _exit
};
#define O(n, a, b, c, d, e) env->ord(n, (uintptr_t)(a), (uintptr_t)(b), (uintptr_t)(c), (uintptr_t)(d), (uintptr_t)(e))
#define C(rva, a, b, c, d, e, f, g) env->call(rva, (uintptr_t)(a), (uintptr_t)(b), (uintptr_t)(c), (uintptr_t)(d), (uintptr_t)(e), (uintptr_t)(f), (uintptr_t)(g))
#define P(x) env->p((uint32_t)(x))
#define A(rva) env->addr(rva)

[[noreturn]] static void fatal(const D2GameBatchE6850Env* env, uint32_t line)
{
    uintptr_t r = O(10265, line, 0, 0, 0, 0);
    O(10024, A(0xF9FF9), r, 0, 0, 0);
    env->exit(-1);
    for (;;) {}
}

// quest-state list at game+0x10f4: head ptr at [0], next at +0xf4, type dword at +0
static uintptr_t findQuestState(uintptr_t game, uint32_t type, const D2GameBatchE6850Env* env)
{
    uintptr_t list = rd32(game, 0x10f4) ? P(rd32(game, 0x10f4)) : 0;
    if (!list) return 0;
    for (uintptr_t n = rd32(list, 0) ? P(rd32(list, 0)) : 0; n; n = rd32(n, 0xf4) ? P(rd32(n, 0xf4)) : 0)
        if (rd32(n, 0) == type) return n;
    return 0;
}

// E6850/E6980 share a body; E6B90 is the same without the Ordinal_10174 test.
static uint32_t timerIf(uintptr_t self, uintptr_t unit, uint32_t sub, uint32_t cbRva, const D2GameBatchE6850Env* env)
{
    uintptr_t pl = O(10920, unit, 0, 0, 0, 0);
    uintptr_t v = O(10174, rd32(pl, 0x10 + rd8(self, 0x6d) * 4), sub, 0xd, 0, 0);
    if (v && unit && rd32(unit, 0x80) && rd32(unit, 0) == 0 && (int16_t)C(0x9BEB0, 0, 0, 0, 0, 0, 0, 0) != -1)
        C(0x9C160, A(cbRva), 0, 0, 0, 0, 0, 0);
    return 0;
}
// 1.13c D2Game+0xE6850
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_E6850(uintptr_t self, uintptr_t unit, const D2GameBatchE6850Env* env)
{ return timerIf(self, unit, 0x16, 0xE5B60, env); }
// 1.13c D2Game+0xE6980
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_E6980(uintptr_t self, uintptr_t unit, const D2GameBatchE6850Env* env)
{ return timerIf(self, unit, 0x15, 0xE37D0, env); }

// 1.13c D2Game+0xE6B90   (unit arrives in EAX)
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_E6B90(uintptr_t unit, const D2GameBatchE6850Env* env)
{
    if (unit && rd32(unit, 0x80) && rd32(unit, 0) == 0 && (int16_t)C(0x9BEB0, 0, 0, 0, 0, 0, 0, 0) != -1)
        C(0x9C160, A(0xE3C70), 0, 0, 0, 0, 0, 0);
    return 0;
}

// quest-record init: zero 0xf dwords at +0xa0, then caller-specific fields.
static void zero0xa0(uintptr_t q) { std::memset((void*)(q + 0xa0), 0, 0x3c); }

// 1.13c D2Game+0xE7CE0   (a3q2)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_E7CE0(uintptr_t q, const D2GameBatchE6850Env* env)
{
    zero0xa0(q);
    wr8(q, 10, 1); wr8(q, 11, 1);
    wr32(q, 0xa0, A(0xE4C90));
    wr32(q, 0xdc, A(0x100538));
    wr8(q, 12, 0);
    wr32(q, 0xe0, 0x12);
    wr8(q, 13, 0);
    wr32(q, 0xe8, A(0xE4AB0)); wr32(q, 0xec, A(0xE4F60)); wr32(q, 0xf0, A(0xE2E80));
    wr32(q, 0x10, 0xf);
    wr32(q, 0xcc, A(0xE7940)); wr32(q, 0xac, A(0xE7A10)); wr32(q, 0xd4, A(0xE49E0));
    wr32(q, 200, A(0xE2EF0)); wr32(q, 0xb0, A(0xE6190));
    uintptr_t d = O(10045, "..\\Source\\D2Game/Quests/a3q2.cpp", 0x39b, 0, 0, 0);
    wr32(q, 0x18, env->h(d));
    std::memset((void*)d, 0, 0x2c * 4);
    wr32(d, 8, 0);
    wr16(d, 0xac, 0);
}

// 1.13c D2Game+0xE8310   (ctx: [0]=game [2]=a [5]=b [6]=c)
// ponytail: Ghidra shows FUN_6fd0ef10 called on an unstored stack slot (stack0xffffffcc); a zeroed 0x34-byte
// block is passed here -- resolve with a FUNCTION-mode trace when this row is vectored.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_E8310(uintptr_t ctx, const D2GameBatchE6850Env* env)
{
    uintptr_t game = P(rd32(ctx, 0));
    uintptr_t qs = P(rd32(game, 0x10f4));
    if (rd32(qs, 8) == 0) fatal(env, 0);
    if (O(10174, rd32(qs, 0xc), 0x16, 0, 0, 0) != 0) return;
    uintptr_t st = findQuestState(game, 0x14, env);
    if (!st) return; // retail dereferences null here
    uintptr_t rec = P(rd32(st, 0x18));
    if (rd8(rec, 0x20)) {
        if (!game) fatal(env, 0x2d3);
        for (uintptr_t n = rd32(game, 0x1320 + (rd32(rec, 0x2c) & 0x7f) * 4) ? P(rd32(game, 0x1320 + (rd32(rec, 0x2c) & 0x7f) * 4)) : 0;
             n; n = rd32(n, 0xe4) ? P(rd32(n, 0xe4)) : 0) {
            if (rd32(n, 0xc) == rd32(rec, 0x2c)) {
                if (rd32(n, 4) == 0x129) return;
                break;
            }
        }
    }
    uint32_t blk[13] = {0};
    uintptr_t u = C(0xEEF10, (uintptr_t)blk, 0, 0, 0, 0, 0, 0);
    if (!u) {
        u = C(0xEF650, 0x129, 1, rd32(ctx, 0), rd32(ctx, 8), rd32(ctx, 20), rd32(ctx, 24), 3 /* + trailing 0 */);
        if (!u) { wr8(rec, 0x20, 0); return; }
    }
    wr8(rec, 0x20, 1);
    wr32(rec, 0x2c, rd32(u, 0xc));
    if (rd32(rec, 0x24) && rd32(P(rd32(rec, 0x24)), 4) && rd8(rec, 0x30) == 0) {
        C(0x20DE0, 0, 0, 0, 0, 0, 0, 0);
        O(10171, 0, 0, 0, 0, 0);
        wr8(rec, 0x30, 1);
    }
}

// 1.13c D2Game+0xE84A0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_E84A0(uintptr_t ctx, const D2GameBatchE6850Env* env)
{
    uintptr_t game = P(rd32(ctx, 0));
    uintptr_t st = findQuestState(game, 0x13, env);
    uintptr_t rec = P(rd32(st, 0x18));
    if (rd8(rec, 0xc)) O(11090, rd32(ctx, 4), 2, 0, 0, 0);
    if (rd8(rec, 0x28) == 0) {
        uint32_t s[10] = { rd32(ctx, 0), rd32(ctx, 8), 0, 0x16e, 1, 0, rd32(ctx, 20), rd32(ctx, 24), 0xffffffffu, 0 };
        uintptr_t u = C(0xEEF10, (uintptr_t)s, 0, 0, 0, 0, 0, 0);
        if (u) {
            wr32(u, 0xc4, rd32(u, 0xc4) | 0x20000);
            wr32(rec, 0x2c, rd32(u, 0xc));
            wr8(rec, 0x28, 1);
        }
    }
}

// 1.13c D2Game+0xE87E0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_E87E0(uintptr_t self, uintptr_t unit, const D2GameBatchE6850Env* env)
{
    static const uint32_t tags[5] = { 0x20796571, 0x20726871, 0x20726271, 0x20316671, 0x20326671 };
    auto sweep = [&]() { for (uint32_t t : tags) if (C(0xE660, t, 0, 0, 0, 0, 0, 0)) C(0x365E0, unit, self, 0, 0, 0, 0, 0); };
    uintptr_t pl = O(10920, unit, 0, 0, 0, 0);
    uint32_t id = rd32(pl, 0x10 + rd8(self, 0x6d) * 4);
    if (O(10174, id, 0x12, 0, 0, 0) == 1) { sweep(); return; }
    uintptr_t u = O(10331, unit, 0, 0, 0, 0);
    if (!u) return;
    if ((char)O(10459, O(10826, u, 0, 0, 0, 0), 0, 0, 0, 0) != 2) return;
    O(10003, id, 0x12, 0, 0, 0);
    O(10003, id, 0x12, 0xd, 0, 0);
    if (C(0x70D60, unit, self, 0, 0, 0, 0, 0) == 0) sweep();
    if (O(10174, id, 0x15, 4, 0, 0) != 0 && O(10174, id, 0x15, 0, 0, 0) == 0) O(10003, id, 0x15, 0, 0, 0);
}

// 1.13c D2Game+0xE92D0   (a3q4)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_E92D0(uintptr_t q, const D2GameBatchE6850Env* env)
{
    zero0xa0(q);
    wr8(q, 12, 0); wr8(q, 13, 0); wr16(q, 0x9c, 0);
    wr32(q, 0xa0, A(0xE3E30));
    wr32(q, 0xdc, A(0x101568));
    wr8(q, 10, 1);
    wr32(q, 0xe0, 0x14);
    wr32(q, 0xec, A(0xE4030)); wr32(q, 0xe8, A(0xE3CE0)); wr32(q, 0xf0, A(0xE2B70));
    wr32(q, 0x10, 0x10);
    wr32(q, 0xb0, A(0xE7640)); wr32(q, 0xcc, A(0xE8B60)); wr32(q, 0xac, A(0xE3E00));
    wr32(q, 0xd4, A(0xE3B40)); wr32(q, 200, A(0xE3B30)); wr32(q, 0xd8, A(0xE6A60)); wr32(q, 0xc4, A(0xE6B20));
    uintptr_t d = O(10045, "..\\Source\\D2Game/Quests/a3q4.cpp", 0x340, 0, 0, 0);
    wr32(q, 0x18, env->h(d));
    std::memset((void*)d, 0, 7 * 4);
    wr8(d, 12, 1); wr8(d, 1, 1);
}

// 1.13c D2Game+0xE93C0   (a3q1)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_E93C0(uintptr_t q, const D2GameBatchE6850Env* env)
{
    zero0xa0(q);
    wr32(q, 0xdc, A(0x100088));
    wr8(q, 10, 1); wr8(q, 12, 0);
    wr32(q, 0xe0, 0x11);
    wr8(q, 13, 4);
    wr32(q, 0xe8, A(0xE6E00)); wr32(q, 0xec, A(0xE53F0)); wr32(q, 0xf0, A(0xE2F00));
    wr32(q, 0x10, 0x13);
    uintptr_t d = O(10045, "..\\Source\\D2Game/Quests/a3q1.cpp", 0x3ad, 0, 0, 0);
    wr32(q, 0x18, env->h(d));
    wr32(q, 0xb0, A(0xE7AF0)); wr32(q, 0xb4, A(0xE7A40)); wr32(q, 0xa0, A(0xE5270));
    wr32(q, 0xcc, A(0xE8FE0)); wr32(q, 0xac, A(0xE7BE0)); wr32(q, 0xd4, A(0xE50C0));
    wr32(q, 0xd8, A(0xE6F60)); wr32(q, 200, A(0xE5220)); wr32(q, 0xc4, A(0xE6FE0));
    std::memset((void*)d, 0, 0x28 * 4);
    wr8(d, 1, 0); wr8(d, 2, 0); wr16(d, 0x9c, 0); wr8(d, 0, 1);
}

// 1.13c D2Game+0xE9960   (self in ECX/stack, unit in EDI)
// ponytail: Ordinal_10331/11090/10408 take register args Ghidra did not name; unit (EDI) is passed.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_E9960(uintptr_t self, uintptr_t unit, const D2GameBatchE6850Env* env)
{
    uintptr_t rec = P(rd32(self, 0x18));
    if (rd32(rec, 0xc) == 2) {
        uint32_t mode = 0;
        if (unit) { mode = rd32(unit, 0x10); if (mode == 2) return; }
        uintptr_t pl = O(10331, unit, 0, 0, 0, 0);
        for (uintptr_t n = rd32(pl, 0x74) ? P(rd32(pl, 0x74)) : 0; n; n = rd32(n, 0xe8) ? P(rd32(n, 0xe8)) : 0) {
            if (rd32(n, 0) == 0 && (uint32_t)O(10407, n, 0, 0, 0, 0) < 0x12) {
                if (mode == 0) { wr32(rec, 0x10, 2); O(11090, unit, 2, 0, 0, 0); }
                else if (mode == 1) { wr32(rec, 0x10, 2); O(11090, unit, 2, 0, 0, 0); O(10408, unit, 0, 0, 0, 0); }
                break;
            }
        }
    }
    uintptr_t game = P(rd32(self, 4));
    C(0x8EA80, game, unit, 7, rd32(game, 0xa8) + 0x18, 0, 0, 0);
}

// 1.13c D2Game+0xE9F60   (ctx: [0]=game [1]=unit [4]=skill)
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_E9F60(uintptr_t ctx, const D2GameBatchE6850Env* env)
{
    uintptr_t unit = rd32(ctx, 4) ? P(rd32(ctx, 4)) : 0;
    if (unit && rd32(unit, 0x10)) return 0;
    uintptr_t game = P(rd32(ctx, 0));
    uintptr_t st = findQuestState(game, 0x10, env);
    if (!st) return 0;
    uintptr_t rec = P(rd32(st, 0x18));
    if (rd8(rec, 0) != 1) return 0;
    uint32_t key = rd32(rec, 4);
    uintptr_t n = rd32(game, 0x1520 + (key & 0x7f) * 4) ? P(rd32(game, 0x1520 + (key & 0x7f) * 4)) : 0;
    while (n && rd32(n, 0xc) != key) n = rd32(n, 0xe4) ? P(rd32(n, 0xe4)) : 0;
    if (!n) return 0;
    O(11090, unit, 1, 0, 0, 0);
    uintptr_t sk = O(10688, rd32(ctx, 16), 0, 0, 0, 0);
    C(0x8EA80, game, unit, 1, (rd32(sk, 0xdc) >> 8) + rd32(game, 0xa8), 0, 0, 0);
    wr32(rec, 8, 2);
    C(0x8EA80, game, unit, 7, rd32(game, 0xa8) + 0x1e, 0, 0, 0);
    uintptr_t qs = P(rd32(P(rd32(st, 4)), 0x10f4));
    wr8(qs, 0x20, 9);
    C(0xDFCC0, qs, 0, 0, 0, 0, 0, 0);
    return 0;
}

// 1.13c D2Game+0xEA730
// ponytail: the decompile aliases the request block, its results and the loop slots on one stack frame
// (loop bound shows as `(int)param_2`). Here the block is q[14]; the Ordinal_10052 result list and count come
// back through out[2]. Confirm both against a FUNCTION-mode trace before vectoring.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_EA730(uintptr_t game, uintptr_t unit, int32_t lo, int32_t hi, const D2GameBatchE6850Env* env)
{
    uintptr_t ctx = O(10331, unit, 0, 0, 0, 0);
    uint32_t t = rd32(unit, 0);
    bool uni = t == 2 || (t > 3 && t < 6);
    uint32_t a = uni ? rd32(P(rd32(unit, 0x2c)), 0xc) : rd32(unit, 0x2c) ? (uint32_t)O(10867, unit, 0, 0, 0, 0) : 0;
    uint32_t b = uni ? rd32(P(rd32(unit, 0x2c)), 0x10) : rd32(unit, 0x2c) ? (uint32_t)O(10750, unit, 0, 0, 0, 0) : 0;
    if (!(hi != 0 && lo < hi)) fatal(env, 0);
    uint32_t q[14] = {0};
    q[0] = 0x583; q[2] = env->h(unit); q[3] = a; q[4] = b; q[5] = env->h(unit);
    O(10591, rd32(game, 0x1c), (uintptr_t)q, ctx, 0, 0);
    uint32_t out[2] = {0, 0};
    O(10052, (uintptr_t)out, 0, 0, 0, 0);
    for (int32_t i = 0; i < (int32_t)out[1]; i++) {
        uintptr_t u = rd32((uintptr_t)out, 0) ? P(rd32(P(out[0]), i * 4)) : 0;
        if (!u || rd32(u, 0) != 1) continue;
        bool ok = O(10830, u, 0, 0, 0, 0) == 0;
        if (q[11] != 0 && O(10239, u, 0, 0, 0, 0) == 0) ok = false;
        if (rd32(u, 4) != 0x14d && ok) {
            uint32_t id = (uint32_t)C(0x11D0, unit + 0x20, 0, 0, 0, 0, 0, 0);
            C(0x8EA80, game, u, 7, rd32(game, 0xa8) + id + q[9], 0, 0, 0);
            C(0x232B0, u, 0, 0, 0, 0, 0, 0);
        }
    }
    O(10344, (uintptr_t)&a, 0, 0, 0, 0);
}
