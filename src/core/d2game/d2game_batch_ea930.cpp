// src/core/d2game/d2game_batch_ea930.cpp -- twelve 1.13c D2Game.dll functions transcribed from the cached
// Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Base 0x6FC20000.
// Same env seam as d2game_batch_e6850.cpp. Register-passed args (EAX/ESI/EDI/EBX) are explicit parameters,
// named for the register. Ordinal calls whose arguments the decompile does not show (EAD10/EAE20) pass zeros
// and are flagged `ponytail:`; a disassembly pass must fill them before VERIFIED.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static inline uint32_t rd32(uintptr_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void     wr32(uintptr_t p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }

struct D2GameBatchEA930Env {
    uintptr_t (*ord)(uint32_t n, uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d, uintptr_t e);
    uintptr_t (*call)(uint32_t rva, uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d, uintptr_t e, uintptr_t f, uintptr_t g);
    uint32_t  (*addr)(uint32_t rva);
    uint32_t  (*h)(uintptr_t host);
    uintptr_t (*p)(uint32_t handle);
    void      (*exit)(int code);
    uint32_t  (*str)(const char* s);     // image handle of a literal C string
};
typedef const D2GameBatchEA930Env* E;
#define O(n, a, b, c, d, e_) env->ord(n, (uintptr_t)(a), (uintptr_t)(b), (uintptr_t)(c), (uintptr_t)(d), (uintptr_t)(e_))
#define C(rva, a, b, c, d, e_, f, g) env->call(rva, (uintptr_t)(a), (uintptr_t)(b), (uintptr_t)(c), (uintptr_t)(d), (uintptr_t)(e_), (uintptr_t)(f), (uintptr_t)(g))
#define P(x) env->p((uint32_t)(x))
#define A(rva) env->addr(rva)
#define X extern "C" EMSCRIPTEN_KEEPALIVE

[[noreturn]] static void fatal(E env, uint32_t line)
{
    uintptr_t r = O(10265, line, 0, 0, 0, 0);
    O(10024, A(0xF9FF9), r, 0, 0, 0);
    env->exit(-1);
    for (;;) {}
}
static uint32_t unitId(uintptr_t u) { return u ? rd32(u, 0xc) : 0xffffffffu; }

// 1.13c D2Game+0xEA930  (eax=obj, ebx=ctx)
X void d2_fn_d2game_EA930(uintptr_t eax, uintptr_t ebx, E env)
{
    C(0xEA730, ebx, eax, 0x23, 0x28, 0, 0, 0);
    uint32_t t = rd32(eax, 0);
    if (t != 2 && (t < 4 || t > 5) && rd32(eax, 0x2c)) O(10750, rd32(eax, 0x2c), 0, 0, 0, 0);
    t = rd32(eax, 0);
    if (t != 2 && (t < 4 || t > 5) && rd32(eax, 0x2c)) {
        O(10867, rd32(eax, 0x2c), 0, 0, 0, 0);
        C(0xA0000, ebx, 0, 0, 0, 0, 0, 0);
        return;
    }
    C(0xA0000, ebx, 0, 0, 0, 0, 0, 0);
}

// 1.13c D2Game+0xEAA30  (eax=game, esi=unit)
X void d2_fn_d2game_EAA30(uintptr_t eax, uintptr_t esi, E env)
{
    int32_t k = esi ? (int32_t)rd32(esi, 4) : -1;
    if (k == 0x2c0) wr32(eax, 0x1df0, 1);
    else if (k == 0x2c1) wr32(eax, 0x1dec, 1);
    else if (k == 0x2c5) wr32(eax, 0x1de8, 1);
    else fatal(env, 0xe6);
    if (rd32(eax, 0x1df0) && rd32(eax, 0x1dec) && rd32(eax, 0x1de8)) {
        uint32_t local4 = 0;
        wr32(esi, 0xb8, 0x20326d63);
        uintptr_t r = C(0x11A70, esi, 7, env->h((uintptr_t)&local4), 0, 0xffffffff, 0, 0);
        C(0xD2380, 0, r, 0, 0, 0, 0, 0);
        wr32(esi, 0xb8, 0x20647473);
        for (int32_t n = (int32_t)rd32(eax, 0x8c); n > 0; --n)
            C(0x11A70, esi, 2, env->h((uintptr_t)&local4), 0, 0xffffffff, 0, 0);
    }
}

// 1.13c D2Game+0xEAB10  (fastcall: ecx ignored, edx=param_2, stack param_3)
X void d2_fn_d2game_EAB10(uint32_t p1, uint32_t p2, uintptr_t p3, E env)
{
    (void)p1;
    uint32_t b[0x17] = {};
    b[0] = 0x8000; b[1] = p2; b[2] = p2;
    // local_4c = b[4] (0x4c = 0x5c-0x10 -> index 4), local_2c = b[8], local_10 = b[0x13]
    b[4] = 299; b[12] = 1; b[0x13] = 100;
    C(0x6F930, p3, env->h((uintptr_t)b), 0, 0, 0, 0, 0);
}

// EAD10 / EAE20 (eax=count, esi=unit); callers EAF80. ponytail: Ordinal_10551/10973/10887 args not in decompile.
// 1.13c D2Game+0xEAD10
X void d2_fn_d2game_EAD10(int32_t eax, uintptr_t esi, uintptr_t sgptDataTables, E env)
{
    if (!eax || !esi) return;
    int32_t i = (int32_t)rd32(esi, 4);
    uintptr_t dt = P(rd32(sgptDataTables, 0));
    if (i < 0 || i >= (int32_t)rd32(dt, 0xbc8) || !(i * 0xc4 + rd32(dt, 0xbc4))) return;
    O(10551, 0, 0, 0, 0, 0); O(10551, 0, 0, 0, 0, 0); O(10551, 0, 0, 0, 0, 0);
    if (eax > 0) O(10551, 0, 0, 0, 0, 0);
    if ((int32_t)O(10973, 0, 0, 0, 0, 0) < (int32_t)O(10973, 0, 0, 0, 0, 0)) O(10887, 0, 0, 0, 0, 0);
    O(10551, 0, 0, 0, 0, 0);
    if (eax > 0) O(10551, 0, 0, 0, 0, 0);
    if ((int32_t)O(10973, 0, 0, 0, 0, 0) < (int32_t)O(10973, 0, 0, 0, 0, 0)) O(10887, 0, 0, 0, 0, 0);
}
// 1.13c D2Game+0xEAE20
X void d2_fn_d2game_EAE20(int32_t eax, uintptr_t esi, uintptr_t sgptDataTables, E env)
{
    if (!eax || !esi) return;
    int32_t i = (int32_t)rd32(esi, 4);
    uintptr_t dt = P(rd32(sgptDataTables, 0));
    if (i < 0 || i >= (int32_t)rd32(dt, 0xbc8) || !(i * 0xc4 + rd32(dt, 0xbc4))) return;
    O(10551, 0, 0, 0, 0, 0); O(10551, 0, 0, 0, 0, 0);
    if (eax > 0) O(10551, 0, 0, 0, 0, 0);
    O(10551, 0, 0, 0, 0, 0);
    if ((int32_t)O(10973, 0, 0, 0, 0, 0) < (int32_t)O(10973, 0, 0, 0, 0, 0)) O(10887, 0, 0, 0, 0, 0);
}

// 1.13c D2Game+0xEDF20  (esi, edi, stack retaddr-slot value passed through)
X void d2_fn_d2game_EDF20(uintptr_t esi, uintptr_t edi, uintptr_t stk, E env)
{
    uintptr_t u = O(10331, 0, 0, 0, 0, 0);
    if (!O(11025, u, 0, 0, 0, 0)) return;
    O(10581, u, stk, 0, 0, 0);
    C(0xEDDB0, esi, edi, 0, 0, 0, 0, 0);
}

// player-list node (0x14): +0 unitId, +8 ?, +0xc state, +0x10 next; list head at playerData+0x38
static uintptr_t findNode(E env, uintptr_t pd, uint32_t id)
{
    for (uintptr_t n = rd32(pd, 0x38) ? P(rd32(pd, 0x38)) : 0; n; n = rd32(n, 0x10) ? P(rd32(n, 0x10)) : 0)
        if (rd32(n, 0) == id) return n;
    return 0;
}
static uintptr_t lookup(E env, uintptr_t pd, uintptr_t esi, uintptr_t edi)
{
    return edi != esi ? findNode(env, pd, unitId(esi)) : 0;
}

// 1.13c D2Game+0xF0060  (esi=unit, edi=other; Ordinal_10920 arg is a register not shown: edi)
X uintptr_t d2_fn_d2game_F0060(uintptr_t esi, uintptr_t edi, E env)
{
    return lookup(env, O(10920, edi, 0, 0, 0, 0), esi, edi);
}

// 1.13c D2Game+0xF0130  (eax, edi)
X void d2_fn_d2game_F0130(uintptr_t eax, uintptr_t edi, E env)
{
    uintptr_t pd = O(10920, edi, 0, 0, 0, 0);
    uintptr_t n = lookup(env, pd, eax, edi);
    if (unitId(edi) != unitId(eax) && n) {
        if (edi && rd32(edi, 0) == 0) O(10920, edi, 0, 0, 0, 0);
        uint32_t buf[6] = {}; // ponytail: stack0xfffffff4 payload layout not recovered
        C(0x8A3E0, env->h((uintptr_t)buf), 6, 0, 0, 0, 0, 0);
    }
}

// 1.13c D2Game+0xF0220  (esi, edi, stk=new state)
X void d2_fn_d2game_F0220(uintptr_t esi, uintptr_t edi, uint32_t stk, E env)
{
    uintptr_t pd = O(10920, edi, 0, 0, 0, 0);
    uintptr_t n = lookup(env, pd, esi, edi);
    if (edi != esi && n) wr32(n, 0xc, stk);
}

// 1.13c D2Game+0xF02F0  (fastcall ecx=param_1, eax=in_EAX)
X void d2_fn_d2game_F02F0(uintptr_t ecx, uintptr_t eax, E env)
{
    uintptr_t pd = O(10920, ecx, 0, 0, 0, 0);
    if (ecx == eax) return;
    if (O(10494, 0, 0, 0, 0, 0)) return;
    if (O(10494, ecx, 7, 0, 0, 0)) return;
    if (d2_fn_d2game_F0060(eax, ecx, env)) return;
    uintptr_t n = O(10045, env->str("..\\Source\\D2Game\\UNIT\\PlayerList.cpp"), 0x6d, 0, 0, 0);
    uintptr_t nn = P((uint32_t)n);
    for (int i = 0; i < 5; i++) wr32(nn, i * 4, 0);
    wr32(nn, 0, unitId(eax));
    wr32(nn, 8, 0);
    int16_t s = (int16_t)C(0xDFA60, 0, 0, 0, 0, 0, 0, 0);
    wr32(nn, 0xc, s != -1);
    wr32(nn, 0x10, rd32(pd, 0x38));
    wr32(pd, 0x38, (uint32_t)n);
}

// 1.13c D2Game+0xF03E0  (param_1=game, param_2 passed to Ordinal_10920)
X void d2_fn_d2game_F03E0(uintptr_t game, uint32_t param2, E env)
{
    uintptr_t pd = O(10920, param2, 0, 0, 0, 0);
    for (uintptr_t n = rd32(pd, 0x38) ? P(rd32(pd, 0x38)) : 0; n; n = rd32(n, 0x10) ? P(rd32(n, 0x10)) : 0) {
        if (rd32(n, 0xc) != 2) continue;
        if (!game) fatal(env, 0x2d3);
        for (uintptr_t u = rd32(game, 0x1120 + (rd32(n, 0) & 0x7f) * 4) ? P(rd32(game, 0x1120 + (rd32(n, 0) & 0x7f) * 4)) : 0;
             u; u = rd32(u, 0xe4) ? P(rd32(u, 0xe4)) : 0) {
            if (rd32(u, 0xc) == rd32(n, 0)) {
                int16_t s = (int16_t)C(0x9BEB0, u, 0, 0, 0, 0, 0, 0);
                wr32(n, 0xc, s != -1);
                d2_fn_d2game_F0130(u, param2, env);
                break;
            }
        }
    }
}

// 1.13c D2Game+0xF0480  (fastcall ecx, edx=param_2, stack param_3)
X void d2_fn_d2game_F0480(uintptr_t ecx, uintptr_t p2, uintptr_t p3, E env)
{
    (void)ecx;
    d2_fn_d2game_F02F0(p2, p2, env);
    d2_fn_d2game_F02F0(p3, p3, env);
    C(0x8C570, 0, 0, 0, 0, 0, 0, 0);
    uintptr_t pd = O(10920, p2, 0, 0, 0, 0);
    uintptr_t n = lookup(env, pd, p3, p2);
    if (p2 != p3 && n) wr32(n, 0xc, 0);
    if (p2 && rd32(p2, 0x80) && rd32(p2, 0) == 0) C(0x9BEB0, p2, 0, 0, 0, 0, 0, 0);
    d2_fn_d2game_F0220(p3, p2, 0, env);
    d2_fn_d2game_F0130(p3, p2, env);
    d2_fn_d2game_F0130(p2, p3, env);
    pd = O(10920, p2, 0, 0, 0, 0);
    for (uintptr_t m = rd32(pd, 0x38) ? P(rd32(pd, 0x38)) : 0; m; m = rd32(m, 0x10) ? P(rd32(m, 0x10)) : 0) {
        if (p3 && rd32(p3, 0) == 0) O(10920, p3, 0, 0, 0, 0);
        uint32_t buf[6] = {}; // ponytail: stack0xffffffec payload layout not recovered
        C(0x8A3E0, env->h((uintptr_t)buf), 0xb, 0, 0, 0, 0, 0);
    }
}
