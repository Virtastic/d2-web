// src/core/d2game/d2game_batch_bd880.cpp -- twelve 1.13c D2Game.dll functions transcribed from the cached
// Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Base 0x6FC20000.
//
// Style as d2game_batch_104f0.cpp: register-only arguments (in_EAX / unaff_E**) are explicit parameters,
// guest 32-bit addresses are uint32_t and are dereferenced through env->at(), callees we do not own are
// seams in retail argument order. Where the decompile hides an ordinal's arguments the seam takes the
// registers the disassembly implies and says so.
#include <cstdint>
#include <cstring>
#include <strings.h>
#include <emscripten.h>

struct D2GameBatchBD880Env {
    void*    (*at)(uint32_t guest);                                  // guest address -> host pointer
    void     (*fatal)(uint32_t line);                                // Ordinal_10265(line); Ordinal_10024; _exit(-1)
    // D2Game internals
    void     (*subD62E0)(uint32_t unit, uint32_t id, uint32_t a, uint32_t* out, uint32_t z0, uint32_t z1); // 0xD62E0
    void     (*subEE70)();                                           // 0xEE70
    void     (*o10201)();  void (*o10050)(uint32_t v);  void (*o10892)();
    uint32_t (*subFAA0)(uint32_t v);                                 // 0x8FAA0
    void     (*subDFC10)(uint32_t unit);                             // 0xDFC10
    void     (*subE20A0)(uint32_t game, uint32_t unit);              // 0xE20A0
    uint32_t (*sub8F5C0)(uint32_t v);                                // 0x8F5C0
    uint32_t (*o10460)(uint32_t v); uint32_t (*o11147)(uint32_t v); uint32_t (*o10464)(uint32_t v);
    uint8_t  (*subBCAA0)(uint32_t unit);                             // 0xBCAA0
    uint32_t (*o10450)(uint32_t v, uint32_t* out2);                  // D2Common #10450, 8-byte out
    uint32_t (*sub11D0)(uint32_t rng);                               // 0x11D0
    uint32_t (*sub11980)(uint32_t a, uint32_t b, uint32_t c, uint32_t edi, uint32_t game, uint32_t d,
                         int32_t e, uint32_t f, uint32_t g, uint32_t h);
    int32_t  (*o10106)(uint32_t u); uint32_t (*o10620)(uint32_t u); const char* (*o10961)(uint32_t v);
    uint32_t (*o10802)(uint32_t u); uint8_t (*o11075)(uint32_t u); void (*o10012)(uint32_t u, uint32_t v);
    void     (*subBD9B0)(uint32_t u, uint32_t game);
    void     (*sub1130)(uint32_t u); void (*sub1100)(uint32_t u);
    uint32_t (*subD76A0)(uint32_t game, uint32_t edi, uint32_t v);
    void     (*subBCE60)();
    void     (*sub90260)(uint32_t u);
    uint32_t (*subBCB20)(uint32_t unit);                             // 0xBCB20
    uint32_t (*subBD220)(uint32_t unit, uint32_t* a, const void* stackArg); // 0xBD220
    uint32_t (*sub003A0)(uint32_t game, uint32_t v);                 // FUN_6fd003a0
    uint32_t (*subBE640)(uint32_t game, uint32_t client, uint32_t node, uint32_t a, int16_t b);
    void     (*subC1510)(uint32_t game, uint32_t client);
    // Failure tail shared by BF200 / C1890: if(*client==0) #10920(); #10973(..); send {0x0e,0,..} len 0xf.
    void     (*badPacketTail)(uint32_t client);
    uint32_t (*o10630)(uint32_t unit); int32_t (*o10353)(uint32_t v); uint32_t (*o10507)(uint32_t unit);
    void     (*o10252)(uint32_t v, int32_t n);
    void     (*subC2100)(uint32_t unit, uint32_t a, uint32_t b, int32_t c);
    void     (*subAD570)(uint32_t reg, uint32_t unitId, uint8_t v);  // reg: EBP if *unit==0, else EBX
    void     (*o10920)();
    uint32_t (*tick)();                                              // GetTickCount
    void     (*o10973)(uint32_t a, uint32_t b, uint32_t c);
    uint32_t (*sub21780)(uint32_t rng);                              // 0x1780
    uint32_t (*o10349)(uint32_t v); uint32_t (*o10463)(uint32_t u, uint32_t z); void (*o10887)(uint32_t u, uint32_t a, uint32_t b);
};

static inline uint32_t R32(const D2GameBatchBD880Env* e, uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (uint8_t*)e->at(a) + o, 4); return v; }
static inline uint8_t  R8 (const D2GameBatchBD880Env* e, uint32_t a, uint32_t o) { return ((uint8_t*)e->at(a))[o]; }
static inline uint16_t R16(const D2GameBatchBD880Env* e, uint32_t a, uint32_t o) { uint16_t v; std::memcpy(&v, (uint8_t*)e->at(a) + o, 2); return v; }
static inline void     W32(const D2GameBatchBD880Env* e, uint32_t a, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)e->at(a) + o, &v, 4); }

// 1.13c D2Game+0xBD880 -- EAX unit, EBX id, EDI game.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_BD880(int32_t flag, uint32_t unit, uint32_t id, uint32_t game, const D2GameBatchBD880Env* env)
{
    uint32_t l4;
    if (!id) env->fatal(0x1a2);
    if (!game) env->fatal(0x1a3);
    if (!unit) env->fatal(0x1a4);
    env->subD62E0(game, id, R32(env, unit, 0xc), &l4, 0, 0);
    if (flag) {
        W32(env, unit, 200, R32(env, unit, 200) | 0x10);
        env->o10201(); env->o10050(R32(env, game, 0x60)); env->o10892();
        return;
    }
    env->subEE70();
}

// 1.13c D2Game+0xBD920 -- EAX game, ESI unit.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_BD920(uint32_t p1, int32_t flag, uint32_t game, uint32_t unit, const D2GameBatchBD880Env* env)
{
    if (!unit || !game) return;
    uint32_t v = env->subFAA0(p1);
    if (!v) return;
    uint32_t l;
    env->subD62E0(game, p1, R32(env, unit, 0xc), &l, 0, 0);
    if (flag) {
        W32(env, unit, 0xc8, R32(env, unit, 0xc8) | 0x10);
        env->o10201(); env->o10050(v); env->o10892();
        return;
    }
    env->subDFC10(unit);
    env->subE20A0(p1, unit);
}

// 1.13c D2Game+0xBDD80 -- ECX this, EAX unit; EBX/EDI are BD880's id/game, passed through (the decompile
// shows them live from the caller). BD920's EAX/ESI are not shown at that call; EDI (game) and the unit are inferred.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_BDD80(uint32_t thisv, int32_t p1, uint32_t p2, int32_t p3, uint32_t unit,
                        uint32_t ebx, uint32_t edi, const D2GameBatchBD880Env* env)
{
    (void)p1;
    uint32_t a = env->sub8F5C0(thisv);
    if (R32(env, a, 0xc) != 0 && p3 == 2) {
        uint32_t node = R32(env, a, 8);
        int32_t key = p2 == 0 ? -1 : (int32_t)R32(env, p2, 0xc);
        for (; node; node = R32(env, node, 8)) {
            if ((int32_t)R32(env, node, 4) != key) continue;
            if (R32(env, node, 0) != 0) {
                uint32_t it = env->o10460(R32(env, node, 0));
                while (it) {
                    uint32_t v = env->o11147(it);
                    it = env->o10464(it);
                    if (v == unit) { d2_fn_d2game_BD920(thisv, 1, edi, unit, env); return; }
                }
            }
            break;
        }
    }
    if (env->subBCAA0(unit) == 0) d2_fn_d2game_BD880(1, unit, ebx, edi, env);
}

// 1.13c D2Game+0xBEE30 -- ECX this, EAX game, EDI guest pointer; returns the created unit or 0.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_fn_d2game_BEE30(uint32_t thisv, int32_t p1, int32_t p2, int32_t p3, uint32_t game, uint32_t edi,
                            const D2GameBatchBD880Env* env)
{
    if (!game) env->fatal(0xfd);
    if (!p1) env->fatal(0xfe);
    uint32_t out[2] = {0, 0}, scratch[2];
    uint32_t rec = env->o10450(thisv, out);
    uint8_t m = R8(env, game, 0x6d);
    if (m != 0 && 0x19 < p3) {
        uint32_t rng = R32(env, game, 0x1d24) + 8;
        uint32_t u = env->sub11D0(rng), v;
        if (m == 1) {
            if (u < (uint32_t)p2 * 0x40 + 4000u && (v = R32(env, rec, 0x88)) != 0 && v != 0x20202020) {
                env->o10450(v, scratch); thisv = R32(env, rec, 0x88);
            } else if (R32(env, rec, 0x19c) != 0x20787878) {
                env->o10450(R32(env, rec, 0x19c), scratch); thisv = R32(env, rec, 0x19c);
            }
        } else {
            if (R32(env, game, 0x70) == 0 || (uint32_t)p2 * 0x10 + 1000u <= u ||
                (v = R32(env, rec, 0x8c)) == 0 || v == 0x20202020) {
                if (u < (uint32_t)p2 * 0x80 + 5000u && (v = R32(env, rec, 0x88)) != 0x20202020 && v != 0) {
                    env->o10450(v, scratch); thisv = R32(env, rec, 0x88);
                }
            } else {
                env->o10450(v, scratch); thisv = R32(env, rec, 0x8c);
            }
            if (R32(env, rec, 0x1a0) != 0x20787878) {
                env->o10450(R32(env, rec, 0x1a0), scratch); thisv = R32(env, rec, 0x1a0);
            }
        }
    }
    uint32_t t = 0;
    p3 = 0;
    do {
        int32_t tries = 0;
        for (;;) {
            bool ok = true;
            t = env->sub11980(1, 0, out[1], edi, game, 4, p1, 0, 0, 0);
            if (env->o10106(t) == 1 && strcasecmp(env->o10961(env->o10620(t)), "Cracked") == 0) {
                env->subDFC10(t); env->subE20A0(game, t); t = 0; ok = false;
            }
            tries++;
            if (ok) break;
            if (4 < tries) return 0;
        }
        if (thisv == env->o10802(t)) p3 = 2;
        else { env->subDFC10(t); env->subE20A0(game, t); p1 = 2; t = 0; }
        p3++;
    } while (p3 < 2);
    if (!t) return t;
    uint8_t b = env->o11075(t);
    p1 = b;
    if (b == 0xff) { env->subEE70(); return 0; }
    env->o10012(t, p1);
    env->subBD9B0(t, game);
    uint32_t id = R32(env, t, 0xc);
    env->sub1130(t); env->sub1100(t);
    if (env->subD76A0(game, edi, id) == 0) {
        if ((uint8_t)out[1] == 1) {
            env->o10012(t, 2);
            id = R32(env, t, 0xc);
            env->sub1130(t); env->sub1100(t);
            if (env->subD76A0(game, edi, id) != 0) { env->subBCE60(); return t; }
        }
        env->sub90260(t);
        return 0;
    }
    env->subBCE60();
    return t;
}

// 1.13c D2Game+0xBF170 -- EBX unit; the stack word is forwarded to 0xBD220.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_fn_d2game_BF170(uint32_t unit, const void* stackArg, const D2GameBatchBD880Env* env)
{
    uint32_t l4 = env->subBCB20(unit);
    for (int i = 0; i <= 4; i++)
        if (env->subBD220(unit, &l4, stackArg) == 0) break;
    return 0;
}

// Shared by BF200 / C1890: the client's node lookup in the game's 128-bucket table.
static uint32_t findNode(const D2GameBatchBD880Env* env, uint32_t game, uint32_t client, uint32_t id, uint32_t* match)
{
    uint32_t want = R8(env, client, 0x6c) == 0 ? 0 : env->sub003A0(game, R32(env, client, 0x64));
    if (!game) env->fatal(0);
    for (uint32_t n = R32(env, game, 0x1320 + (id & 0x7f) * 4); n; n = R32(env, n, 0xe4))
        if (R32(env, n, 0xc) == id) { *match = (want == n) ? n : 0; return 1; }
    *match = 0;
    return 1;
}

// 1.13c D2Game+0xBF200 -- EAX game, EBX id, ESI client. Fatal code is not recoverable from the decompile (0).
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_fn_d2game_BF200(uint32_t p1, uint32_t p2, int16_t p3, uint32_t game, uint32_t id, uint32_t client,
                            const D2GameBatchBD880Env* env)
{
    (void)p2;
    if (!game) env->fatal(0);
    uint32_t n; findNode(env, game, client, id, &n);
    if (n) return env->subBE640(game, client, n, p1, p3);
    env->badPacketTail(client);
    return 1;
}

// 1.13c D2Game+0xC1890 -- same lookup, success runs 0xC1510(game, client).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_C1890(uint32_t game, uint32_t id, uint32_t client, const D2GameBatchBD880Env* env)
{
    uint32_t n; findNode(env, game, client, id, &n);
    if (n) env->subC1510(game, client); else env->badPacketTail(client);
}

// 1.13c D2Game+0xBF2E0 -- stdcall (game, p2, unit, tbl), ret 0x10; from the disassembly (the decompile's parameter and local names are wrong:
// the item-code table is tbl+0xC, the fixed-code list tbl+0x14, the 0x8F5C0 record A is looked up by the unit's id, and the roll 0x1780 takes
// EAX = max+1, ECX = rng, EDI = min). Seams are the callees; the probe records them with the capture's hook recorder.
struct D2GameBF2E0Seams {
    void*    (*at)(uint32_t guest);
    void     (*fatal)(uint32_t line);
    uint32_t (*tick)();                                                  // GetTickCount (call [IAT])
    uint32_t (*sub8F5C0)(uint32_t game, uint32_t unitId, uint32_t* out); // EAX = out slot, ECX = game, ESI = unit id (-1 for a null unit)
    uint32_t (*a42a)(uint32_t p2, uint32_t a, uint32_t b);               // Ordinal 10973: 3 stack words
    uint32_t (*subBCB20)(uint32_t unit, uint32_t game);                  // EAX = unit, ECX = game
    uint32_t (*aa42)(uint32_t key, uint32_t* out);                       // Ordinal 10450: record for an item code, id written through out
    uint32_t (*roll)(uint32_t rng, uint32_t lo, uint32_t hiPlus1, uint32_t lvl); // 0x1780: EAX = hi+1, ECX = rng, EDI = lo (EBP = lvl, logged by REGS Y)
    uint32_t (*ac16)(uint32_t id);                                       // Ordinal 10349
    uint32_t (*bee30)(uint32_t game, uint32_t code, uint32_t unit, uint32_t one, uint32_t q, uint32_t lvl, uint32_t x); // EAX = game, ECX = code, 5 stack words
    uint32_t (*aa06)(uint32_t item, uint32_t z);                         // Ordinal 10463
    void     (*a472)(uint32_t item, uint32_t ev, uint32_t v);            // Ordinal 10887
};

extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_BF2E0(uint32_t G, uint32_t p2, uint32_t U, uint32_t T, const D2GameBF2E0Seams* s)
{
    auto rd32 = [&](uint32_t a) { uint32_t v; memcpy(&v, s->at(a), 4); return v; };
    auto rd16 = [&](uint32_t a) { uint16_t v; memcpy(&v, s->at(a), 2); return (uint32_t)v; };
    auto rd8 = [&](uint32_t a) { return (uint32_t)*(const uint8_t*)s->at(a); };
    auto wr32 = [&](uint32_t a, uint32_t v) { memcpy(s->at(a), &v, 4); };
    const uint32_t rng = rd32(G + 0x1d24) + 8;
    auto next = [&]() { uint64_t v = (uint64_t)rd32(rng) * 0x6ac690c5u + rd32(rng + 4); wr32(rng, (uint32_t)v); wr32(rng + 4, (uint32_t)(v >> 32)); return (uint32_t)v; };
    wr32(T + 8, s->tick());
    int32_t fail = 0;
    uint32_t slot = 0;
    const uint32_t A = s->sub8F5C0(G, U ? rd32(U + 4) : 0xffffffffu, &slot);
    if (!A) { s->fatal(0x2c4); return; }
    const uint32_t X = s->a42a(p2, 0xc, 0);
    const int32_t lvl = (int32_t)s->subBCB20(U, G);
    uint32_t off = 0;
    for (int32_t i = 0; i < (int32_t)rd32(A + 0x30); i++, off += 12) {
        uint32_t id;
        const uint32_t rec = s->aa42(rd32(rd32(T + 0xc) + off + 4), &id);
        if ((int32_t)rd8(rec + 0xfd) > lvl) continue;
        uint32_t cnt = 0;
        if (lvl < 0x19) { const uint32_t row = rd32(A + 0x2c) + off; cnt = s->roll(rng, rd8(row), rd8(row + 1) + 1, (uint32_t)lvl); }
        if (!(rd16(rec + 0xf6) < 100) && rd16(G + 0x78) < 100) continue;
        for (uint32_t k = 0; k < cnt; k++) {
            const uint32_t r = next() % 100;
            uint32_t q = 2;
            if (lvl < 5) { if (r > 0x5a) q = 1; }
            else if (lvl < 10) { if (r > 0x55) q = 3; }
            else if (r >= 0x4b) q = 3;
            if (!s->bee30(G, rd32(rd32(A + 0x2c) + off + 4), U, 1, q, (uint32_t)lvl, X)) fail++;
            if (fail > 0x20) return;
        }
        uint32_t id2;
        s->aa42(rd32(rd32(T + 0xc) + off + 4), &id2);
        if (s->ac16(id2) != 1) continue;
        if ((int32_t)rd8(rd32(T + 0xc) + off + 8) > lvl) continue;
        uint32_t m = 1;
        if (lvl >= 0x19) m = s->roll(rng, 1, 3, (uint32_t)lvl) + 1;
        const uint32_t row = rd32(A + 0x2c) + off, lo = rd8(row + 2);
        const int32_t hi = (int32_t)(rd8(row + 3) + m);
        uint32_t v = lo;
        if (hi > (int32_t)lo) {
            const uint32_t d = (uint32_t)hi - lo, nx = next();
            v = ((d & (d - 1)) == 0 ? ((d - 1) & nx) : nx % d) + lo;
        }
        for (; v; v--)
            if (!s->bee30(G, rd32(rd32(A + 0x2c) + off + 4), U, 1, 4, (uint32_t)lvl, X)) fail++;
    }
    for (int32_t j = 0; j < (int32_t)rd32(A + 0x38); j++) {
        const uint32_t item = s->bee30(G, rd32(rd32(A + 0x34) + j * 4), U, 1, 2, (uint32_t)lvl, X);
        if (!item) fail++;
        else {
            uint32_t id3;
            const uint32_t rec = s->aa42(rd32(rd32(T + 0x14) + j * 4), &id3);
            const uint32_t k = rd32(rec + 0x80);
            if (k == 0x20767163 || k == 0x20767161) s->a472(item, 0x46, s->aa06(item, 0));
        }
        if (fail > 0x20) break;
    }
}

// 1.13c D2Game+0xBF8F0 -- EAX unit; EBX / EBP are the two AD570 register arguments (EBP when *unit==0).
// #10630/#10507 take the unit (inferred; the decompile hides their arguments).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_BF8F0(int32_t p1, uint32_t unit, uint32_t ebx, uint32_t ebp, const D2GameBatchBD880Env* env)
{
    if (p1 < 0) return;
    uint32_t i1 = env->o10630(unit);
    if (!i1) return;
    int32_t v = env->o10353(i1) - p1;
    if (v < 1) {
        if (env->o10507(unit) == i1) env->subC2100(unit, 0, 0, -1);
        v = 0;
    }
    env->o10252(i1, v);
    uint32_t id;
    uint32_t reg = ebx;
    if (!unit) id = 0xffffffff;
    else {
        id = R32(env, unit, 0xc);
        if (R32(env, unit, 0) == 0) { env->o10920(); reg = ebp; }
    }
    env->subAD570(reg, id, (uint8_t)v);
}

// Game quest/state list at game+0x10f4: head pointer at [0]; node type [0]==0xd, next at +0xf4.
static uint32_t findD(const D2GameBatchBD880Env* env, uint32_t game)
{
    uint32_t l = R32(env, game, 0x10f4);
    if (!l) return 0;
    for (uint32_t n = R32(env, l, 0); n; n = R32(env, n, 0xf4))
        if (R32(env, n, 0) == 0xd) return n;
    return 0;
}

// 1.13c D2Game+0xC1A00 -- EAX game. 0 when the node is active and its state < 4, else 1.
extern "C" EMSCRIPTEN_KEEPALIVE
uint8_t d2_fn_d2game_C1A00(uint32_t game, const D2GameBatchBD880Env* env)
{
    uint32_t n = findD(env, game);
    return !(n && R8(env, n, 9) != 0 && R8(env, n, 0xc) < 4);
}

// 1.13c D2Game+0xC1A60 -- EAX game.
extern "C" EMSCRIPTEN_KEEPALIVE
uint8_t d2_fn_d2game_C1A60(uint32_t game, const D2GameBatchBD880Env* env)
{
    uint32_t n = findD(env, game);
    if (!n || R8(env, n, 9) == 0) return 0;
    return R8(env, R32(env, n, 0x18), 0xb) == 0;
}

// 1.13c D2Game+0xC1B60 -- EAX game. As C1A00 with state < 2.
extern "C" EMSCRIPTEN_KEEPALIVE
uint8_t d2_fn_d2game_C1B60(uint32_t game, const D2GameBatchBD880Env* env)
{
    uint32_t n = findD(env, game);
    return !(n && R8(env, n, 9) != 0 && R8(env, n, 0xc) < 2);
}
