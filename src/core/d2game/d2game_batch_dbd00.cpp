// src/core/d2game/d2game_batch_dbd00.cpp -- twelve 1.13c D2Game.dll functions transcribed from the cached
// Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Base 0x6FC20000.
// Same env-seam style as d2game_batch_1EF50.cpp: retail records are 32-bit addresses read by byte offset,
// register-only args (in_EAX / unaff_ESI / unaff_EDI) are explicit parameters, unmapped callees and the
// 0x6FD2E628.. weight table are seams on D2GameBatchDBD00Env. The env instance is NOT defined here: it is
// supplied by the caller as the last argument of every d2_fn_d2game_<RVA> entry (extern to this file).
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <emscripten.h>

static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)(a + o), 4); return v; }
static inline uint8_t  rd8 (uint32_t a, uint32_t o) { return *(const uint8_t*)(uintptr_t)(a + o); }
static inline void     wr32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)(uintptr_t)(a + o), &v, 4); }

struct D2GameBatchDBD00Env {
    // 0xDBD00
    int32_t  (*subDB3C0)();                                   // no visible args
    int32_t  (*sub14D0)(int32_t pct, int32_t v);
    int32_t  (*subDAD00)(uint32_t thisp);
    // 0xDC940 / 0xDCAB0
    int32_t  (*ordinal10871)(uint32_t unit);
    uint32_t (*ordinal10265)(uint32_t size);
    void     (*ordinal10024)(const char* file, uint32_t v);
    const char* assertFile;                                   // DAT_6FD19FF9
    void     (*sub8E6F0)();
    void     (*sub8EA80)(uint32_t game, uint32_t unit, uint32_t type, uint32_t frame, uint32_t a, uint32_t b, uint32_t c);
    void     (*ordinal10945)(uint32_t unit, uint32_t skill, uint32_t one);
    uint32_t (*ordinal11013)(uint32_t a, uint32_t b, uint32_t c, uint32_t d);
    void     (*ordinal10261)(uint32_t h, uint32_t stat, int32_t val, uint32_t z);
    void     (*ordinal10785)(uint32_t h, uint32_t skill);
    void     (*ordinal10911)(uint32_t h, uint32_t fn);
    void     (*ordinal10807)(uint32_t unit, uint32_t h, uint32_t one);
    int32_t  (*ordinal10680)(int32_t a, uint32_t stat, uint32_t z);
    void     (*ordinal11043)(uint32_t game, uint32_t frame);
    void     (*ordinal10188)(uint32_t unit, uint32_t stat, int32_t val, uint32_t z);
    uint32_t subBF7A0;                                        // retail fn address 0x6FCBF7A0
    // 0xDCD80
    int32_t  (*ordinal10973)();
    uint32_t (*subDBED0)(uint32_t a, int32_t b, uint32_t edi, int32_t d, uint32_t esi);
    // 0xDEBA0
    void*    (*sub572C0)(uint32_t p);
    uint32_t (*sub32EE0)();
    uint32_t weightCount;                                     // DAT_6FD2E628
    const uint32_t* weightTable;                              // DAT_6FD2E62C: {id, weight} pairs
    uint32_t (*ordinal11139)(uint32_t a, uint32_t id);
    int32_t  (*ordinal10744)(uint32_t a, uint32_t b);
    // 0xDFDC0
    uint32_t (*ordinal10867)(uint32_t unit);
    uint32_t (*ordinal10750)(uint32_t unit);
    uint32_t (*ordinal10331)(uint32_t unit);
    void     (*ordinal10561)(uint32_t h, uint32_t out[2]);
    // 0xE0410
    uint32_t (*ordinal10920)(uint32_t unit);
    void     (*sub8E8C0)(uint32_t fn);
    // 0xE0570
    uint32_t (*subE03A0)(uint32_t a, uint32_t b);
    uint32_t (*ordinal10666)(uint32_t a, uint32_t b);
    int16_t  (*subBBEB0)(uint32_t a);
    // 0xE2600
    void     (*ordinal10153)(uint32_t h, uint32_t pos, uint32_t* out);
    // 0xE2900
    uint32_t (*ordinal10826)(uint32_t h);
    // 0xE2A40
    uint32_t (*sub53F30)(uint32_t id);
    int32_t  (*isBadCodePtr)(uint32_t fn);
    uint32_t (*callPtr)(uint32_t fn);
};

static void dbd00Fatal(const D2GameBatchDBD00Env* e, uint32_t size)
{
    uint32_t v = e->ordinal10265(size);
    e->ordinal10024(e->assertFile, v);
    std::exit(-1);
}

// 1.13c D2Game+0xDBD00  thiscall(this, param_1), EAX = pointer to an offset cell.
// GUESS: DB3C0 and 14D0 take stack/register args the decompile does not show; DB3C0 called with none.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_DBD00(uint32_t thisp, int32_t param1, uint32_t eax, const D2GameBatchDBD00Env* e)
{
    uint32_t cell = rd32(thisp, 0x18) + rd32(eax, 0);
    int32_t v = (int32_t)rd32(cell, 0);
    if (v < 1) { wr32(cell, 0, 0); return 0; }
    int32_t f = e->subDB3C0();
    if (param1 == 0) v -= (int32_t)rd32(thisp, rd32(eax, 0x18) * 4 + 0x1c);
    else if (0 < f) f = 0;
    if (0 < v && f != 0) v = e->sub14D0(100, v);
    if (param1 == 0) v = e->subDAD00(thisp);
    wr32(cell, 0, (uint32_t)v);
    return v;
}

// Shared body of 0xDC940 (skill 0x73) and 0xDCAB0 (skill 2); the two functions are identical otherwise.
// GUESS: the comments reads iRam0000007f / uRam00000073 (null-address artefacts); taken as the game
// (iVar2) and the skill constant, per the surrounding calls.
static void dbd00Aura(uint32_t thisp, uint32_t p1, uint32_t unit, int32_t p3, int32_t p4, uint32_t skill,
                      const D2GameBatchDBD00Env* e)
{
    if (!(0 < p4 && 0 < p3)) return;
    int32_t kind = e->ordinal10871(unit);
    if (thisp == 0) dbd00Fatal(e, 0xa2a);
    uint32_t game = rd32(thisp, 0x80);
    if (unit && rd32(unit, 0) == 1) {
        for (uint32_t n = rd32(unit, 0x37 * 4); n;) {
            uint8_t t = rd8(n, 0);
            n = rd32(n, 0x24);
            if (t == 3) { e->sub8E6F0(); p3 = (int32_t)p1; }
        }
        e->sub8EA80(game, unit, 3, rd32(game, 0xa8) + 1, 0, 0, 0);
    }
    uint32_t frame = rd32(game, 0xa8) + unit;
    if (kind == 0) {
        e->ordinal10945(unit, skill, 1);
        uint32_t h = e->ordinal11013(rd32(game, 0x1c), 2, frame, skill);
        e->sub8EA80(game, unit, 0xc, frame, 0, 0, 0);
        e->ordinal10261(h, 0x4a, -p3, 0);
        e->ordinal10785(h, skill);
        e->ordinal10911(h, e->subBF7A0);
        e->ordinal10807(unit, h, 1);
        return;
    }
    int32_t cur = e->ordinal10680(kind, 0x4a, 0);
    if (-p3 == cur || -cur < p3) {
        e->ordinal11043(game, frame);
        e->sub8EA80(game, unit, 0xc, frame, 0, 0, 0);
        e->ordinal10188(unit, 0x4a, -p3, 0);
    }
}

// 1.13c D2Game+0xDC940  thiscall(this, p1, unit, p3, p4)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_DC940(uint32_t thisp, uint32_t p1, uint32_t unit, int32_t p3, int32_t p4, const D2GameBatchDBD00Env* e)
{ dbd00Aura(thisp, p1, unit, p3, p4, 0x73, e); }

// 1.13c D2Game+0xDCAB0  thiscall(this, p1, unit, p3, p4)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_DCAB0(uint32_t thisp, uint32_t p1, uint32_t unit, int32_t p3, int32_t p4, const D2GameBatchDBD00Env* e)
{ dbd00Aura(thisp, p1, unit, p3, p4, 2, e); }

// 1.13c D2Game+0xDCD80  ESI/EDI are register args. GUESS: Ordinal_10973 args not shown (none passed).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_DCD80(uint32_t param1, uint32_t param2, uint32_t esi, uint32_t edi, const D2GameBatchDBD00Env* e)
{
    (void)param1;
    uint32_t stk = 0;
    int32_t a = e->ordinal10973();
    if (a * 0x100 < 8) return param2 & (((int32_t)param2 < 0) - 1);
    int32_t b = e->ordinal10973();
    if (param2 != 0xffffffffu) { stk = (uint32_t)e->ordinal10973(); esi += stk; }
    return e->subDBED0(stk, a * 0x100, edi, b << 8, esi);
}

// 1.13c D2Game+0xDEBA0  EAX = unit A, EBX = unit B (rng seed at B+0x20/+0x24). Weighted pick over
// weightTable; the LCG is seed = lo*0x6AC690C5 + hi, written back as a 64-bit pair, low 32 bits used.
// GUESS: the first block's sub572C0 arg and sub32EE0's (none) args; return of the early-out paths follows
// the decompile's EAX leftovers.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_DEBA0(uint32_t eax, uint32_t ebx, const D2GameBatchDBD00Env* e)
{
    auto roll = [&](uint32_t mod, bool pow2) -> uint32_t {
        uint64_t s = (uint64_t)rd32(ebx, 0x20) * 0x6ac690c5ull + rd32(ebx, 0x24);
        wr32(ebx, 0x20, (uint32_t)s); wr32(ebx, 0x24, (uint32_t)(s >> 32));
        return pow2 ? ((mod - 1) & (uint32_t)s) : (uint32_t)((s & 0xffffffffull) % mod);
    };
    if (eax && rd32(eax, 0) == 0) {
        eax = e->sub572C0(eax) ? e->sub32EE0() : 0;
    }
    if (ebx && rd32(ebx, 0) == 0 && (eax = rd32(ebx, 0x18)) != 0) {
        uint32_t slot[7] = {0}, total = 0;
        int32_t n = (int32_t)e->weightCount;
        for (int32_t i = 0; i < n; ++i) {
            uint32_t u = e->ordinal11139(eax ? rd32(ebx, 0x18) : 0, e->weightTable[i * 2]);
            eax = 0;
            if (u && e->ordinal10744(u, 0x32)) { eax = e->weightTable[i * 2 + 1]; slot[i] = u; total += eax; }
        }
        if (n > 0 && (int32_t)total > 0) {
            uint32_t idx = roll(e->weightCount, (e->weightCount & (e->weightCount - 1)) == 0);
            uint32_t w = roll(total, (total & (total - 1)) == 0);
            int32_t nx;
            do {
                if (slot[idx]) {
                    if ((int32_t)w < (int32_t)e->weightTable[idx * 2 + 1]) return e->sub32EE0();
                    w -= e->weightTable[idx * 2 + 1];
                }
                nx = (int32_t)idx + 1;
                idx = (uint32_t)nx % e->weightCount;
            } while (-1 < (int32_t)w);
            return (uint32_t)nx / e->weightCount;
        }
    }
    return eax;
}

// 1.13c D2Game+0xDFDC0  ESI = unit A, EDI = unit B. Swaps type-dependent id/class fields into each other's
// +0x14 sub-record (+0x20/+0x24), then copies the Ordinal_10331 point (via 10561) to +0x18/+0x1C.
// GUESS: 10561's out buffer is a two-dword local, first call's output is the stale uStack_14/_10 pair.
static uint32_t dbd00Field(uint32_t u, uint32_t off, bool second, const D2GameBatchDBD00Env* e)
{
    uint32_t t = rd32(u, 0);
    if (t == 2 || (3 < t && t < 6)) return rd32(rd32(u, 0x2c), off);
    if (rd32(u, 0x2c) == 0) return 0;
    return second ? e->ordinal10750(u) : e->ordinal10867(u);
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_DFDC0(uint32_t esi, uint32_t edi, const D2GameBatchDBD00Env* e)
{
    if (!edi || !esi) dbd00Fatal(e, 0);
    wr32(rd32(edi, 0x14), 0x20, dbd00Field(esi, 0xc, false, e));
    wr32(rd32(edi, 0x14), 0x24, dbd00Field(esi, 0x10, true, e));
    wr32(rd32(esi, 0x14), 0x20, dbd00Field(edi, 0xc, false, e));
    wr32(rd32(esi, 0x14), 0x24, dbd00Field(edi, 0x10, true, e));
    uint32_t out[2] = {0, 0};
    uint32_t h = e->ordinal10331(edi);
    if (h) { e->ordinal10561(h, out); wr32(rd32(edi, 0x14), 0x18, out[0]); wr32(rd32(edi, 0x14), 0x1c, out[1]); }
    h = e->ordinal10331(esi);
    if (h) { e->ordinal10561(h, out); wr32(rd32(esi, 0x14), 0x18, out[0]); wr32(rd32(esi, 0x14), 0x1c, out[1]); }
}

// 1.13c D2Game+0xE0410  fastcall(ecx unused, unit)
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_E0410(uint32_t ecx, uint32_t unit, const D2GameBatchDBD00Env* e)
{
    (void)ecx;
    if (unit && rd32(unit, 0) == 0) {
        uint32_t r = e->ordinal10920(unit);
        if (r) { wr32(r, 0x148, 0); wr32(r, 0x14c, 0); e->sub8E8C0(0x6fd00410); return 1; }
    }
    return 0;
}

// 1.13c D2Game+0xE0570  fastcall(unit a = ECX), EAX = unit b. Resolve owner via E03A0, then compare.
// GUESS: subBBEB0 takes the unit's +0x80 field (decompile shows no args).
static uint32_t dbd00Owner(uint32_t u, const D2GameBatchDBD00Env* e)
{
    if (u && rd32(u, 0) == 1 && rd32(u, 0x14)) {
        uint32_t r = rd32(u, 0x14);
        uint32_t s = rd32(r, 0x28);
        if (s && rd32(s, 0x28)) { uint32_t o = e->subE03A0(rd32(s, 0x28), rd32(s, 0x2c)); if (o) return o; }
    }
    return u;
}
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_E0570(uint32_t a, uint32_t b, const D2GameBatchDBD00Env* e)
{
    a = dbd00Owner(a, e);
    b = dbd00Owner(b, e);
    if (a != b) {
        if (!a || rd32(a, 0) != 0 || !b || rd32(b, 0) != 0) return e->ordinal10666(a, b);
        int16_t s2 = rd32(a, 0x80) == 0 ? -1 : e->subBBEB0(rd32(a, 0x80));
        int16_t s3 = (rd32(b, 0x80) == 0 || rd32(b, 0) != 0) ? -1 : e->subBBEB0(rd32(b, 0x80));
        if (s2 != s3 || s3 == -1) return 0;
    }
    return 1;
}

// 1.13c D2Game+0xE2600  (game, pct), ESI = unit. Re-times a unit's frame schedule: drops its type-0/1
// timers, then queues type-0 events (tile value 1..4) along the path and a final type-1.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_E2600(uint32_t game, int32_t pct, uint32_t esi, const D2GameBatchDBD00Env* e)
{
    if (esi) {
        for (int ty = 0; ty < 2; ++ty)
            for (uint32_t n = rd32(esi, 0x37 * 4); n;) {
                uint8_t t = rd8(n, 0);
                n = rd32(n, 0x24);
                if (t == ty) e->sub8E6F0();
            }
    }
    uint32_t frame = rd32(game, 0xa8);
    int32_t base = (int32_t)(((int32_t)(frame - ((int32_t)rd32(esi, 0x11 * 4) >> 8)) * (100 - pct)) / 100);
    wr32(esi, 0x11 * 4, (frame - base) * 0x100);
    int32_t step, limit;
    if (rd32(esi, 0xc * 4) == 0) { step = (int16_t)rd32(esi, 0x13 * 4); limit = (int32_t)rd32(esi, 0x12 * 4); }
    else { step = (int32_t)rd32(esi, 0xf * 4); limit = (int32_t)rd32(esi, 0xd * 4); }
    if (step == 0) { e->sub8EA80(game, esi, 1, rd32(game, 0xa8) + 1, 0, 0, 0); return; }
    uint32_t tbl = rd32(esi, 0x14 * 4);
    int32_t lo = base - 1, pos = base * 0x100, nxt = pos + step;
    while (nxt < limit) {
        int32_t a = pos + step, b = nxt + step, hi = a >> 8;
        ++frame;
        for (; lo <= hi; ++lo) {
            uint32_t v;
            if (rd32(esi, 0xc * 4) == 0) v = rd8(tbl, lo + 0x10);
            else e->ordinal10153(rd32(esi, 0xc * 4), (uint32_t)lo << 8, &v);
            if (0 < (int32_t)v && (int32_t)v < 5) e->sub8EA80(game, esi, 0, frame, 0, v, 0);
        }
        pos = a; nxt = b;
    }
    if (frame == rd32(game, 0xa8)) ++frame;
    e->sub8EA80(game, esi, 1, frame + 1, 0, 0, 0);
}

// Walk the game's +0x10f4 list head for a type-0x13 node (shared by E2900 / E2960).
static uint32_t dbd00Find13(uint32_t eax)
{
    uint32_t head = rd32(eax, 0x10f4);
    if (head) for (uint32_t n = rd32(head, 0); n; n = rd32(n, 0x3d * 4)) if (rd32(n, 0) == 0x13) return n;
    return 0;
}

// 1.13c D2Game+0xE2900  EAX = game, param_1 = unit
extern "C" EMSCRIPTEN_KEEPALIVE bool d2_fn_d2game_E2900(uint32_t param1, uint32_t eax, const D2GameBatchDBD00Env* e)
{
    uint32_t n = dbd00Find13(eax);
    uint32_t h = e->ordinal10331(param1);
    if ((h == 0 || e->ordinal10826(h) != 0x65) && n) return rd8(rd32(n, 6 * 4), 0xc) == 0;
    return false;
}

// 1.13c D2Game+0xE2960  EAX = game; true when no type-0x13 node, else its +0x18 record byte +0xC == 0
extern "C" EMSCRIPTEN_KEEPALIVE bool d2_fn_d2game_E2960(uint32_t eax)
{
    uint32_t n = dbd00Find13(eax);
    return n ? rd8(rd32(n, 6 * 4), 0xc) == 0 : true;
}

// 1.13c D2Game+0xE2A40  fastcall(p = ECX). The "0x6FC1FFB7" callee is the IsBadCodePtr import.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_E2A40(uint32_t p, const D2GameBatchDBD00Env* e)
{
    if (rd8(p, 0xc) != 7 && rd8(p, 9) != 0) return 1;
    uint32_t r = e->sub53F30(rd32(p, 0x10));
    if (!r) return 0;
    if (e->isBadCodePtr(rd32(p, 0xf0))) dbd00Fatal(e, 0x1f5);
    return e->callPtr(rd32(r, 0x3c * 4));
}
