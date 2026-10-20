// src/core/d2game/d2game_batch_b6d30.cpp -- twelve 1.13c D2Game.dll functions (AI/skill helpers around
// 0xB6D30..0xBA850) transcribed from the cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c).
// Base 0x6FC20000. Pointers are retail 32-bit addresses read through env->r8/r32/w8/w32. Register-only
// arguments (in_EAX, unaff_ESI/EBP, __fastcall ECX/EDX) are explicit parameters. Callees src/ does not
// implement are seams on D2GameBatchB6D30Env in retail argument order. Where Ghidra lost a stack/register
// argument it is marked "unrecovered" and passed as the nearest live value; no test runner has run these yet.
#include <cstdint>
#include <emscripten.h>

struct D2GameBatchB6D30Env {
    uint8_t  (*r8)(uint32_t a);
    uint32_t (*r32)(uint32_t a);
    void     (*w8)(uint32_t a, uint8_t v);
    void     (*w32)(uint32_t a, uint32_t v);
    uint32_t sgptDataTables;                                  // *(sgptDataTables_exref)
    uint32_t (*s1100)(uint32_t unit);                         // 0x1100
    uint32_t (*s1130)(uint32_t unit);                         // 0x1130
    uint32_t (*sB08F0)(uint32_t a, uint32_t b);               // 0xB08F0
    void     (*fatal)(uint32_t code);                         // Ordinal_10265 + 10024 + _exit(-1); no return
    uint32_t (*sB6C90)(uint32_t arg);                         // 0xB6C90
    uint32_t (*ord11027)(uint32_t unit);
    void     (*ord10507)(uint32_t unit);
    void     (*ord10828)(uint32_t unit);
    uint32_t (*ord10494)(uint32_t unit, uint32_t state);
    void     (*ord10945)(uint32_t unit, uint32_t state, uint32_t v);
    uint32_t (*ord10920)(uint32_t p);
    uint32_t (*ord10174)(uint32_t a, uint32_t b, uint32_t c);
    uint32_t (*ord10186)(uint32_t unit);
    uint32_t (*ord10913)(uint32_t unit);
    uint32_t (*ord10522)(uint32_t unit);
    uint32_t (*ord10331)(uint32_t unit, uint32_t a, uint32_t b);
    void     (*ord10302)(uint32_t unit, uint32_t a, uint32_t b, uint32_t c, const char* file, uint32_t line);
    void     (*sBD230)(uint32_t a, uint32_t b);               // 0x9D230
    void     (*s41290)(uint32_t unit, uint32_t buf);          // 0x21290
    void     (*sF22C0)(uint32_t v);                           // 0xF22C0
    void     (*sF1D90)(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);   // 0xF1D90
    void     (*sF26B0)(uint32_t a, uint32_t b, uint32_t c, uint32_t d);               // 0xF26B0
    void     (*sF1B10)(uint32_t a, uint32_t b);               // 0xF1B10
    void     (*sF2540)(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint8_t f, uint32_t g);
    void     (*sF2380)(uint32_t unit);                        // 0xF2380
    void     (*sF2220)(uint32_t a, uint32_t b);               // 0xF2220
    void     (*sB7D30)(uint32_t a);                           // 0xB7D30
    uint32_t (*s47450)(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g);
    void     (*s53EB0)(uint32_t a, uint32_t b, uint32_t c);   // 0x33EB0
    uint32_t (*s2E660)(uint32_t tag);                         // 0xE660
    void     (*s554F0)(uint32_t a, uint32_t b);               // 0x354F0
    void     (*sFFCC0)(uint32_t node, uint32_t v);            // 0xDFCC0
    int16_t  (*sBBEB0)();                                     // 0x9BEB0
    void     (*sBC160)(uint32_t cb);                          // 0x9C160
    uint32_t (*sEEF10)(uint32_t* arg);                        // 0xEEF10
};

// FUN_6fcd7xxx shared tail: weighted pick among 16 weights; the unit's 64-bit seed lives at +0x20/+0x24.
static int pickWeighted(const D2GameBatchB6D30Env* e, uint32_t seedUnit, const int32_t w[16])
{
    uint32_t sum = 0;
    for (int i = 0; i < 16; i++) sum += (uint32_t)w[i];
    if ((int32_t)sum < 1) sum = 0;
    else {
        uint64_t s = (uint64_t)e->r32(seedUnit + 0x20) * 0x6ac690c5ull + e->r32(seedUnit + 0x24);
        e->w32(seedUnit + 0x20, (uint32_t)s); e->w32(seedUnit + 0x24, (uint32_t)(s >> 32));
        sum = (sum & (sum - 1)) == 0 ? ((sum - 1) & (uint32_t)s) : (uint32_t)s % sum;
    }
    int32_t acc = 0;
    for (int i = 0; i < 16; i++) { acc += w[i]; if ((int32_t)sum < acc) return i; }
    return 1;
}

// 1.13c D2Game+0xB6D30  ECX = unit, EDX = record
extern "C" EMSCRIPTEN_KEEPALIVE bool d2_fn_d2game_B6D30(uint32_t unit, uint32_t rec, const D2GameBatchB6D30Env* e)
{
    if (!unit) e->fatal(0x11d);
    if (!rec) e->fatal(0x11e);
    if (e->r8(unit + 0x18) != e->r8(rec + 0x18)) return false;
    uint32_t y = e->s1130(unit), x = e->s1100(unit);
    return e->sB08F0(x, y) < 0x3fc;
}

// 1.13c D2Game+0xB75D0  returns the picked index. local_40[i-2] is weight i; weights 0/1 (local_40[-2],[-1])
// are stack words Ghidra did not name and are taken as 0.
extern "C" EMSCRIPTEN_KEEPALIVE int d2_fn_d2game_B75D0(uint32_t p1, uint32_t p2, int p3, const D2GameBatchB6D30Env* e)
{
    int32_t l[16] = {0};
    l[2] = l[3] = l[4] = l[6] = 0x14; l[8] = 0x50; l[9] = 0x46;
    int32_t base = e->r8(p1 + 0x6d) == 0 ? 0x7d : 100;
    if (p3 < 2) { l[2] = 0x2d; l[10] = 0x19; }
    l[0] = base + (100 - (int32_t)e->ord11027(p2));
    e->ord10507(p2);
    bool hit = e->sB6C90(p2) != 0;
    if (!hit) { e->ord10828(p2); hit = e->sB6C90(p2) != 0; if (!hit && p2 == 0) goto tail; }
    if (p2 != 0) { l[0xb] = 0x19; l[0] = 0; l[4] = 0; l[1] = 0x23; l[9] = 0x19; }
    else { l[0xb] = 0x32; l[0xc] = 0x32; }
tail:
    if (p3 != 0) l[3] = 0x3c;
    int32_t w[16] = {0};
    for (int i = 2; i < 16; i++) w[i] = l[i - 2];
    return pickWeighted(e, p2, w);
}

// 1.13c D2Game+0xB7750  The 16-entry weight table is built by the caller's frame (stack0xffffffb8); the
// two #11027 calls and #10494(unit,0xb) feed it. The caller supplies the table, the seed unit is ESI.
extern "C" EMSCRIPTEN_KEEPALIVE int d2_fn_d2game_B7750(uint32_t unit, uint32_t esiUnit, const int32_t weights[16],
                                                       const D2GameBatchB6D30Env* e)
{
    e->ord11027(unit); e->ord11027(unit);   // unrecovered args
    e->ord10494(unit, 0xb);
    return pickWeighted(e, esiUnit, weights);
}

// 1.13c D2Game+0xB78A0  ECX = param_1, EDX = unit; flagA is [ebp-4], flagB is EBP (both live registers),
// buf the &stack0xffffffdc argument. The two list walks in the decompile have no effect except
// the second's "found 10" test, reproduced below.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_B78A0(uint32_t p1, uint32_t unit, uint32_t flagA, uint32_t flagB,
                                                        uint32_t buf, const D2GameBatchB6D30Env* e)
{
    if (e->ord10494(unit, 0xc)) e->ord10945(unit, 0xc, 0);
    e->sBD230(p1, unit);
    if (unit && e->r32(unit) == 1 && e->r32(unit + 0x14)) {
        uint32_t a = e->r32(e->r32(unit + 0x14) + 0x28);
        uint32_t head = a ? e->r32(a + 0x20) : 0;
        if (head) {
            uint32_t n = e->r32(head);
            while (e->r32(n + 8) != 10 && n != head) n = e->r32(n);
            if (e->r32(n + 8) == 10) goto done;
        }
    }
    if (flagA && flagB) e->s41290(unit, buf);
done:
    e->sF22C0(1);
}

// 1.13c D2Game+0xB8070  ECX = rec (param_1), EAX = target, EDX = mode, param_3 = game/owner, param_5 = extra;
// ebp is unaff_EBP (case 0xe).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_B8070(uint32_t rec, uint32_t mode, uint32_t p3, uint32_t p4,
                                                        uint32_t p5, uint32_t target, int32_t ebp,
                                                        const D2GameBatchB6D30Env* e)
{
    if (!rec) return;
    int32_t idx = (int32_t)e->r32(rec + 4);
    if (idx < 0 || idx >= (int32_t)e->r32(e->r32(e->sgptDataTables) + 0xa80)) return;
    uint32_t row = idx * 0x1a8 + e->r32(e->r32(e->sgptDataTables) + 0xa78);
    if (!row) return;
    auto rd16 = [&](uint32_t a) { return (int16_t)(e->r32(a) & 0xffff); };
    switch (mode) {
    case 1: {
        uint8_t d = e->r8(p3 + 0x6d);
        if (d == 2) { e->sF22C0(5); return; }
        e->sF22C0(d == 1 ? 0xf : 0x23); return;
    }
    case 2: case 6: e->sF1D90(target, p3, rec, 2, 0); return;
    case 3: e->sF26B0(rec, p3, target, 6); return;
    case 4: e->sF1B10(p3, rec); return;
    case 5:
        if (p5) { e->sF2540(p3, rec, 0, 2, e->r32(p5 + 0xc), (uint8_t)e->r32(p5 + 0x10), 1); return; }
        break;
    case 8:
        if (target) {
            if (0 < rd16(row + 0x17a)) {
                int a = (int)e->ord10186(rec), b = (int)e->ord10913(rec);
                if (a < b || e->r32(target) != 0) { e->sF2380(rec); return; }
            }
            if (0 < rd16(row + 0x17c) && e->r32(target) == 0) { e->sF2380(rec); return; }
            return;
        }
        break;
    case 9: case 0xc: case 0xd: e->sF2380(rec); return;
    case 10: e->sF2220(p3, target); return;
    case 0xb:
        e->ord10302(rec, 0x13c, 1, 0, "..\\Source\\D2Game\\Ai\\AiBaal.cpp", 0x452);
        e->sF2380(rec); return;
    case 0xe:
        if (target) {
            int32_t x = (int32_t)e->s1100(rec); e->s1130(rec);
            int32_t lx = (int32_t)e->s1100(target); uint32_t ly = e->s1130(target);
            int32_t d = (int32_t)e->ord10522(rec);
            if (d == 0) d = 1;
            int32_t loc[4] = {(x - ebp) * 0x19 / d + x, 0, 0, 0};
            (void)lx; (void)ly;
            if (e->r32(rec + 4) == 0x2c5) { e->sF2380(rec); return; }
            uint32_t pv = e->ord10331(rec, 0, e->r32(rec + 4));
            // unrecovered stack args of 0x47450: loc buffer (not addressable as a retail pointer) is passed by value.
            if (e->s47450(p3, pv, (uint32_t)loc[0], 0, 0, rec, target)) { e->sF2380(rec); return; }
        }
        break;
    case 0xf: e->sB7D30(p3); e->sF22C0(5); return;
    default: break;
    }
    e->sF22C0(5);   // default (also reached by the mode=0 re-dispatch)
}

// 1.13c D2Game+0xB88C0  EAX = game
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_B88C0(uint32_t game, const D2GameBatchB6D30Env* e)
{
    if (e->r32(game + 0x70)) return 0;
    uint32_t l = e->r32(game + 0x10f4);
    if (!l) return 1;
    for (uint32_t n = e->r32(l); n; n = e->r32(n + 0xf4))
        if (e->r32(n) == 0x17) return e->r8(e->r32(n + 0x18) + 0x14) != 0;
    return 1;
}

// 1.13c D2Game+0xB8CE0  ECX = state, EDX = ev
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_B8CE0(uint32_t s, uint32_t ev, const D2GameBatchB6D30Env* e)
{
    uint32_t self = e->r32(ev), ev3 = e->r32(ev + 0xc);
    uint32_t t = e->r32(e->ord10920(ev3) + 0x10 + e->r8(self + 0x6d) * 4);
    int32_t other = -1;
    uint32_t a = e->r32(s + 0xe0);
    e->ord10174(t, a, 0);
    // the second #10174(t) in the decompile has no recovered arguments and its result is unused
    uint32_t b = e->r32(s + 0xe0);
    if (e->ord10174(t, b, 1)) { e->s53EB0(s, e->r32(ev + 0x14), a); return; }
    if (ev3) other = (int32_t)e->r32(ev3 + 0xc);
    for (int i = 0; i < (int)(e->r32(s + 0x9c) & 0xffff); i++)
        if ((int32_t)e->r32(s + 0x1c + i * 4) == other) { e->s53EB0(s, e->r32(ev + 0x14), a); return; }
    uint8_t c = e->r8(s + 0xc);
    if (c && (c < 4 || t != 0) && e->ord10174(t, 0x1b, 1) == 0 && e->r8(s + 9)) {
        if (c == 1) {
            if (e->s2E660(0x2073736d) == 0) {
                if (e->r8(e->r32(s + 0x18) + 0x1c)) return;
                if (e->r8(s + 0xb) > 2) return;
            }
        } else {
            if (e->s2E660(0x2073736d)) return;
            if (e->r8(s + 0xb) > 2) return;
            if (e->r8(e->r32(s + 0x18) + 0x1c)) return;
        }
        e->s53EB0(s, e->r32(ev + 0x14), b);
    }
}

static uint32_t stateGate(const D2GameBatchB6D30Env* e, uint32_t unit, uint32_t p2, uint32_t cat)
{
    uint32_t t = e->r32(e->ord10920(p2) + 0x10 + e->r8(unit + 0x6d) * 4);
    if ((e->ord10174(t, cat, 0) || e->ord10174(t, cat, 0xf)) && !e->ord10174(t, cat, 0xd) && !e->ord10174(t, cat, 0xe))
        return 0;
    e->s554F0(p2, unit);
    return 0;
}
// 1.13c D2Game+0xB9970
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_B9970(uint32_t unit, uint32_t p2, const D2GameBatchB6D30Env* e)
{ return stateGate(e, unit, p2, 0x1b); }
// 1.13c D2Game+0xB9D20
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_B9D20(uint32_t unit, uint32_t p2, const D2GameBatchB6D30Env* e)
{ return stateGate(e, unit, p2, 0x19); }

// 1.13c D2Game+0xBA080  EAX = game
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_BA080(uint32_t game, const D2GameBatchB6D30Env* e)
{
    uint32_t l = e->r32(game + 0x10f4);
    if (!l) return;
    for (uint32_t n = e->r32(l); n; n = e->r32(n + 0xf4)) {
        if (e->r32(n) != 0x16) continue;
        if (!e->r8(n + 9) || e->r8(n + 0xb) > 1) return;
        e->w8(n + 0x14, 0); e->w8(n + 0xb, 2);
        e->sFFCC0(n, 0);
        return;
    }
}

// 1.13c D2Game+0xBA180  cbAddr is the retail address of the callback 0x6FCD94F0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_BA180(uint32_t unit, uint32_t p2, uint32_t cbAddr,
                                                            const D2GameBatchB6D30Env* e)
{
    uint32_t t = e->r32(e->ord10920(p2) + 0x10 + e->r8(unit + 0x6d) * 4);
    if (e->ord10174(t, 0x19, 0xd) && p2 && e->r32(p2 + 0x80) && e->r32(p2) == 0 && e->sBBEB0() != -1)
        e->sBC160(cbAddr);
    return 0;
}

// 1.13c D2Game+0xBA850  p3 is a record whose byte +0x11 is set; returns the node from 0xEEF10 (tried 3 times)
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_BA850(uint32_t p1, uint32_t p2, uint32_t p3,
                                                            const D2GameBatchB6D30Env* e)
{
    (void)p2;
    uint32_t a = p1, n = 0;
    for (int i = 0; i < 3 && !n; i++) { a = p1; n = e->sEEF10(&a); }
    if (!n) return 0;
    e->w32(n + 0xc4, e->r32(n + 0xc4) | 0x3000000);
    e->w8(p3 + 0x11, 1);
    return n;
}
