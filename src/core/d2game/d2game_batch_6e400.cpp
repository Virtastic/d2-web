// Alpha batch D2Game+0x6E400..0xA44C0 (12 functions), transcribed from the cached 1.13c decompiles
// (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Base 0x6FC20000. No test run.
// Same pattern as d2game_batch_1EF50.cpp: retail records are read by byte offset; every callee (Ordinal_N
// and FUN_6fc..) is a seam on D2GameBatch6E400Env in retail argument order, so no Ordinal signature is
// declared here and nothing can clash with the shared ORD() declarations. Ghidra register-only arguments
// (in_EAX / unaff_ESI / unaff_EBX / extraout_*) are explicit parameters, named as in the decompile.
// REG? marks a call whose register inputs the decompiler dropped.
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>

typedef uintptr_t A;  // a retail record address
static inline uint32_t rd32(A p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline uint16_t rd16(A p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint8_t  rd8 (A p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline void wr32(A p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline void wr8 (A p, uint32_t o, uint8_t v)  { ((uint8_t*)p)[o] = v; }
static inline A    rdA (A p, uint32_t o) { return (A)rd32(p, o); }

struct D2GameBatch6E400Env {
    A dataTables;                  // *sgptDataTables_exref
    const uint32_t* bitMasks;      // gdwBitMasks
    const char* assertFile;        // DAT_6FD19FF9
    A handlerTable27BB8;           // 0x6FD27BB8: dword per +0x1B3 byte
    A fn8CF90;                     // &FUN_6fc8cf90
    // D2Common / Fog ordinals
    A    (*o10331)(A);  A (*o10265)(A);  void (*o10024)(const char*, A);  void (*o10920)(A);
    void (*o11090)(A, A);  A (*o10695)(A);  A (*o10688)(A);  A (*o10296)(A);  A (*o10826)(A);
    A    (*o10436)(A);  A (*o10373)(A);  A (*o10416)(A);  A (*o10459)(A, A, A, A, A);
    A    (*o10942)(A);  void (*o10141)(A);  A (*o11088)(A);  void (*o10083)(A);  void (*o10782)(A);
    void (*o10588)(A);  void (*o10467)(A);  void (*o10977)(A);  A (*o10202)(A);  A (*o10460)(A);
    A    (*o11147)(A);  A (*o10464)(A);  A (*o10020)(A);  A (*o10494)(A, A);  A (*o10871)(A);
    A    (*o10680)(A, A, A);  void (*o501)(A);  A (*o10802)(A);  A (*o10744)(A, A);  A (*o10106)(A);
    A    (*o10620)(A);  A (*o10366)(A);  A (*o10225)(A);  A (*o10838)(A);  A (*o10973)(A, A, A);
    A    (*o11151)(A);  A (*o10086)(A);  A (*o10213)(A, A, A);  A (*o10048)(A);  void (*o11165)(A);
    A    (*o10587)(A, A, A);  A (*o10379)(A, A, A);  void (*o11103)(A);  void (*o11005)(A);
    // D2Game FUN_
    void (*f6AE60)(A game);                                    // 0x6AE60
    void (*f8EA80)(A game, A unit, A, A, A, A, A);             // FUN_6fcaea80
    A    (*f89E50)(A byteArg);                                 // FUN_6fc89e50
    void (*fE9CB0)(A esi);                                     // FUN_6fce9cb0
    A    (*fD2C70)();                                          // FUN_6fcf2c70
    void (*fAAB20)();                                          // FUN_6fcaab20
    A    (*f210D0)(A id);                                      // FUN_6fc210d0 REG?
    void (*fAB400)(A esi, A eax);                              // FUN_6fcab400 REG?
    A    (*fB6600)();                                          // FUN_6fcb6600 REG?
    A    (*f21130)(A);  A (*f21100)(A);  A (*fD08F0)(A, A);    // 0x21130 / 0x21100 / 0xB08F0
    void (*f95E10)(A, A*);                                     // FUN_6fc95e10
    A    (*f21720)();  void (*f2B800)(A);  void (*fBE170)(A);  // 0x21720 / 0x2B800 / 0x9E170
    void (*f98550)(A, A, A, A, A, A, A);                       // FUN_6fc98550
    void (*fAA3E0)(const void*, A);                            // FUN_6fcaa3e0
};

static void fatal6E400(const D2GameBatch6E400Env* e, A size)
{
    e->o10024(e->assertFile, e->o10265(size));
    std::exit(-1);
}

// 1.13c D2Game+0x6E400
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_6E400(A game, A unit, const D2GameBatch6E400Env* e)
{
    if (unit && rd32(unit, 0x10) == 1) wr32(unit, 0x10, 2);
    e->o10331(unit);
    A seed = rdA(game, 0x10F0);
    e->f6AE60(game);
    uint64_t lo, hi;
    std::memcpy(&lo, (void*)seed, 8);
    (void)hi;
    uint64_t v = (uint64_t)(uint32_t)lo * 0x6ac690c5ull + rd32(seed, 4);
    std::memcpy((void*)seed, &v, 8);
    e->f8EA80(game, unit, 0, rd32(game, 0xA8) + 0xF + (uint32_t)((v & 0xffffffffull) % 0x23), 0, 0, 0);
}

static A chainFind(A game, uint32_t base, uint32_t id)
{
    for (A n = rdA(game, base + (id & 0x7F) * 4); n; n = rdA(n, 0xE4))
        if (rd32(n, 0xC) == id) return n;
    return 0;
}

// this=ECX (id of piVar6 list), param_1 game, param_2 id of piVar5 list, param_3 mode, eax = id for local_c.
// 1.13c D2Game+0x6E680
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_fn_d2game_6E680(uint32_t thisId, A game, uint32_t param2, int16_t param3,
                            uint32_t eax, A esi, const D2GameBatch6E400Env* e)
{
    if (!game) fatal6E400(e, 0x2D3);
    A p5 = chainFind(game, 0x1520, param2);
    A p6 = chainFind(game, 0x1120, thisId);
    A lc = chainFind(game, 0x1720, eax);   // same 0xE4 chain, record id at +0xC
    if (!p6) {
        if (!p5) fatal6E400(e, 0x1109);
        return 1;
    }
    if (!p5) return 1;
    if (rd32(p6, 0) == 0) e->o10920(p6);
    if (param3 == 2) {
        e->fAAB20();
        e->o11090(p5, 0);
        wr32(p6, 0x64, (uint32_t)-1); wr32(p6, 0x68, 6); wr8(p6, 0x6C, 0);
        return 1;
    }
    if (param3 != 3) return 1;
    A rec = e->o10695(lc ? rd32(lc, 4) : 0xFFFFFFFFu);
    bool ok;
    if (rd32(p5, 4) == 0x98) ok = rd32(rec, 0x80) == 0x20747368;
    else ok = e->fD2C70() != 0;
    if (!ok) { e->fAAB20(); return 1; }
    wr32(p6, 0x64, (uint32_t)-1); wr32(p6, 0x68, 6); wr8(p6, 0x6C, 0);
    uint8_t c = (rd32(p5, 4) == 0x98) ? 1 : (uint8_t)e->f89E50(rd8(rec, 0x122));
    e->fAAB20();
    if (c == 1) {
        e->o11090(p5, 1);
        A r2 = e->o10688(rd32(p5, 4));
        if (rd32(p5, 4) == 0x98) {
            e->o11090(p5, 2);
            e->fE9CB0(esi);
            return 1;
        }
        e->f8EA80(lc, p5, 1, (rd32(r2, 0xDC) >> 8) + 1 + rd32(lc, 0xA8), 0, 0, 0);
        return 1;
    }
    e->o11090(p5, 0);
    return 1;
}

// Decompile is stack-garbled around the packet block; semantics kept, REG? args noted.
// eax = node id, ebx = game, esi = unit record, edi = spell id source, retaddr word = skill id.
// 1.13c D2Game+0x6E930
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_6E930(uint32_t eax, A ebx, A esi, A edi, uint16_t retaddr, int16_t sStack8,
                        const D2GameBatch6E400Env* e)
{
    if (!ebx) fatal6E400(e, 0x41);
    A n = rdA(ebx, 0x1520 + (eax & 0x7F) * 4);
    while (n && rd32(n, 0xC) != eax) n = rdA(n, 0xE4);
    if (!n) return;
    A i3 = e->o10296(n);
    if (!i3 || rd8(i3, 0x1B3) >= 0x65) return;
    if (rdA(e->handlerTable27BB8, rd8(i3, 0x1B3) * 4) != e->fn8CF90) return;
    uint32_t sk = retaddr;
    A u2 = e->o10331(n);                               // REG? n
    uint32_t cur = (uint32_t)e->o10826(n);             // REG? n
    wr32(esi, 0x64, (uint32_t)-1); wr32(esi, 0x68, 6); wr8(esi, 0x6C, 0);
    if (cur == sk || sStack8 == 0) return;
    A g = e->f21720();
    if (!e->o10436(sk)) return;                         // REG? skill
    if (!e->o10373(g)) return;                          // REG? u2
    A flag = (e->o10416(sk) == 0 && sk != 0x2E && sk != 0x4A && sk != 0x85 && sk != 0x86 &&
              sk != 0x87 && sk != 0x88) ? 0 : 0xD;
    e->fBE170(ebx);
    A u3 = e->o10331(n);                                // REG?
    if (u3) {
        A xy[2] = {0, 0};
        A idx = e->o10459(sk, sk, flag, (A)&xy[0], (A)&xy[1]);
        A u4 = e->o10942(rd32(ebx, 0xBC + (idx & 0xFF) * 4));
        if (u3 == u4) {
            if (rd32(esi, 0) == 0) e->o10920(esi);
            uint32_t uid = rd32(esi, 0xC);
            e->o10141(u3);                              // REG?
            e->f98550(ebx, esi, 0, 2, (A)xy[0], u2, 0);
            uint8_t b[13] = {};
            b[0] = 0x0D;
            std::memcpy(b + 2, &uid, 4);
            b[6] = 1;
            e->fAA3E0(b, 0xD);
        }
    }
    e->f2B800(esi);
    (void)edi;
}

// 1.13c D2Game+0x70010
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_70010(const D2GameBatch6E400Env* e)
{
    if (e->o11088(0) == 7) {                            // REG? unit
        e->o10083(0); e->o10782(0); e->o10588(0); e->o10467(0); e->o10977(0);
        return;
    }
    if (e->o10202(0) != 0) { e->o10083(0); e->o10782(0); }
}

// 1.13c D2Game+0x70190
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t d2_fn_d2game_70190(A inv, const D2GameBatch6E400Env* e)
{
    int32_t n = 0;
    A it = e->o10460(inv);
    while (it) {
        A rec = e->o11147(it);
        it = e->o10464(it);
        if ((uint8_t)e->o10020(rec) == 3) n++;
    }
    return n;
}

// Quest/item requirement check against one 8-byte condition row at esi+0x14+idx*8.
// esi = condition table, iStack20 = unit's class record (+0x80 mask), iStack10 = required count,
// iStack8 = "exact count" flag (0 => at-least). Both stack values are caller-set (REG?).
// 1.13c D2Game+0x70240
extern "C" EMSCRIPTEN_KEEPALIVE
bool d2_fn_d2game_70240(A param1, A param2, int32_t param3, A param4, A esi,
                        A iStack20, int32_t iStack10, int32_t iStack8, const D2GameBatch6E400Env* e)
{
    if (param2 == 0) return false;
    uint16_t cond0 = rd16(esi, 0x14 + param3 * 8);
    if ((cond0 & 3) == 0) return true;
    int32_t local1c = 0;
    if ((cond0 & 1) && (int16_t)rd16(esi, 0x16 + param3 * 8) != -1)
        local1c = (int32_t)e->o10695((int16_t)rd16(esi, 0x16 + param3 * 8));
    int32_t count = 0;
    A cur = e->o10460(param2);
    const A idxBase = 0x14 + (A)param2 * 8;  // NB: row is indexed by param_2 in the body (as decompiled)
    while (cur) {
        A rec8 = e->o11147(cur);
        cur = e->o10464(cur);
        if (!rec8 || (uint8_t)e->o10020(rec8) != 3) continue;
        local1c++;
        A r9 = e->o10695(rd32(param1, 4));
        if (!r9) continue;
        A cls = e->o10802(param1);
        uint16_t c = rd16(esi, idxBase);
        uint16_t arg = rd16(esi, 0x16 + param2 * 8);
        bool pass1;
        if (!(c & 1)) {
            pass1 = !(c & 2) || e->o10744(param1, arg) != 0;
        } else if (arg == 0xFFFF) {
            pass1 = true;
        } else if ((int8_t)c < 0) {
            A c84 = rd32(r9, 0x84), c88 = rd32(r9, 0x88), c8c = rd32(r9, 0x8C), m = rd32(iStack20, 0x80);
            if (cls == c84)      pass1 = (m == c84);
            else if (cls == c88) pass1 = (m == c84 || m == c88);
            else                 pass1 = (cls == c8c) && (m == c84 || m == c88 || m == c8c);
        } else {
            pass1 = (A)arg == rd32(param1, 4);
        }
        if (!pass1) continue;
        uint8_t b1a = rd8(esi, 0x1A + param2 * 8);
        if (b1a != 0 && e->o10106(param1) != b1a) {
            if (b1a == 9) e->o11165(param1);
            continue;
        }
        if ((rd8(esi, 0x14 + param2 * 8) & 0x40) &&
            (int32_t)e->o10620(param1) != (int32_t)rd16(esi, 0x18 + param2 * 8) - 1) continue;
        c = rd16(esi, idxBase);
        if (c & 4) { if ((uint8_t)e->o10366(param1) != 0) continue; }
        else if ((c & 8) && (uint8_t)e->o10366(param1) == 0) continue;
        c = rd16(esi, idxBase);
        if (c & 0x20) { if (e->o10225(param1) != 0) continue; }
        else if ((c & 0x10) && e->o10225(param1) == 0) continue;
        c = rd16(esi, idxBase);
        if (c & 0x100)      { if (cls != rd32(r9, 0x84)) continue; }
        else if (c & 0x200) { if (cls != rd32(r9, 0x88)) continue; }
        else if (c & 0x400) { if (cls != rd32(r9, 0x8C)) continue; }
        if ((c & 0x800) && (e->o10838(param1) & 0x4000000)) continue;
        if (rd8(esi, 4) == 0x1C && rd8(r9, 0x12A) != 0 && rd8(r9, 0x12B) != 0 &&
            (int32_t)rd8(rec8, 0x6D) <= (int32_t)e->o10973(param1, 0x164, 0)) continue;
        if (param2 == 0 && rd8(esi, 0x54) != 0xFF && rd8(esi, 0x54) != 0xFE &&
            (rd8(esi, 0x4C) & 2) && (uint8_t)e->o11151(param1) == 0) continue;
        if (!(local1c > 0x2F || rd32(param4, local1c * 4 - 4) == 0)) continue;
        if (param2 == 0) {
            uint8_t t = rd8(esi, 4);
            if (t != 0) {
                int32_t id = (int32_t)rd32(esi, 8);
                bool inRange = id > -1 && id <= (int32_t)rd32(e->dataTables, 0xBD4);
                if (t >= 0x0F && t <= 0x1A && inRange) {
                    int k = (t - 0x0F) % 4, g = (t - 0x0F) / 4;
                    A r = e->f210D0(id);
                    if (!r) goto skip;
                    int32_t thr = (int32_t)rd32(esi, 0xC) >> (rd8(r, 0x18) & 0x1F);
                    int32_t val = (int32_t)(g == 0 ? e->o10973(param1, id, 0)
                                          : g == 1 ? e->o10587(param1, id, 0) : e->o10379(param1, id, 0));
                    bool s = k == 0 ? val < thr : k == 1 ? thr < val : k == 2 ? val == thr : val != thr;
                    if (s) goto skip;
                } else if (t == 0x1B && (int32_t)e->o10620(param1) == (int32_t)rd32(esi, 0xC)) {
                    goto skip;
                }
            }
            // three output slots at param3+4: {unit,itemId,flags}
            A p = (A)param3 + 4;
            for (int k = 3; k; k--, p += 12) {
                wr32(p, 4, (uint32_t)e->o10086(param1));
                if (rd8(esi, 0x4C) & 1) wr32(p, (uint32_t)-4, (uint32_t)param1);
                uint8_t m = rd8(esi, 0x54);
                if (m == 0xFF || m == 0xFE) {
                    wr32(p, (uint32_t)-4, (uint32_t)param1);
                    wr32(p, 0, m == 0xFF ? rd32(param1, 4) : 0xFFFFFFFFu);
                    uint32_t k4c = rd16(esi, 0x4C);
                    uint32_t code = ((int8_t)k4c < 0) ? rd32(r9, 0x88) : (k4c & 0x100) ? rd32(r9, 0x8C) : 0x20202020;
                    if (code != 0x20202020) {
                        int32_t id2 = (int32_t)e->o10213(rd32(e->dataTables, 0x94), code, 0);
                        A r2 = id2 > -1 ? e->o10695(id2) : 0;
                        if (r2 && (m == 0xFE || rd32(rec8, 0x70) != 0 || rd16(r2, 0xF6) < 100))
                            wr32(p, 0, (uint32_t)id2);
                    }
                }
            }
        }
        if (local1c < 0x30) wr32(param4, local1c * 4 - 4, 1);
        count++;
        if (e->o10048(param1) != 0) iStack8 = 0;
        skip:;
    }
    if (iStack8 == 0) return iStack10 <= count;
    return count == iStack10;
}

// 1.13c D2Game+0x70BF0
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_70BF0(A eax, A esi, const D2GameBatch6E400Env* e)
{
    uint32_t local[8] = {};
    if (eax != 0) { e->o501((A)local); local[5] &= 0xffffff; }
    if (esi && rd32(esi, 0) == 0) e->o10920(esi);
    e->fAB400(esi, eax);
}

// eax = unit (EAX), ebx = condition table (EBX), esi forwarded to 0x70240.
// 1.13c D2Game+0x70CF0
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_fn_d2game_70CF0(A param1, A param2, A eax, A ebx, A esi, A iStack20,
                            int32_t iStack10, int32_t iStack8, const D2GameBatch6E400Env* e)
{
    if (!ebx || !eax) return 0;
    uint32_t flags[0x30] = {};
    for (int i = 0; i < 7; i++)
        if (!d2_fn_d2game_70240(param1, ebx, i, (A)(flags + 0x30), esi, iStack20, iStack10, iStack8, e))
            return 0;
    (void)param2;
    return 1;
}

// ECX param_1 = skill id, EDX param_2 = unit. in_EAX = flag. 0xA3D60 table: 0x23C-byte rows at +0xB98.
// 1.13c D2Game+0xA3D60
extern "C" EMSCRIPTEN_KEEPALIVE
bool d2_fn_d2game_A3D60(int32_t param1, A param2, A eax, const D2GameBatch6E400Env* e)
{
    A dt = e->dataTables;
    if (param1 < 0 || (int32_t)rd32(dt, 0xBA0) <= param1) return false;
    A row = (A)param1 * 0x23C + rdA(dt, 0xB98);
    if (!row) return false;
    if ((int16_t)rd16(row, 0xBC) == 0) return true;
    uint32_t id = param2 ? rd32(param2, 4) : 0xFFFFFFFFu;
    if ((int32_t)(int16_t)rd16(row, 0xBC) == (int32_t)(id & 0xFFFF)) return false;
    int32_t c = (int8_t)rd8(row, 0xBE);
    if (c > -1 && c < (int32_t)rd32(dt, 0xBF0) && eax != 0)
        return (int32_t)e->fB6600() != c;               // REG? unit
    return true;
}

// 1.13c D2Game+0xA3E10
extern "C" EMSCRIPTEN_KEEPALIVE
bool d2_fn_d2game_A3E10(A param1, A param2, const D2GameBatch6E400Env* e)
{
    if (!param1) fatal6E400(e, 0x36C);
    if (!param2) fatal6E400(e, 0x36D);
    if ((uint8_t)rd32(param1, 0x18) != rd8(param2, 0x18)) return false;
    A y = e->f21130(param1), x = e->f21100(param1);
    return (uint32_t)e->fD08F0(x, y) < 0x3FC;
}

// ecx param_1 unused, param_2 (DL) = level byte, in_EAX = mode, esi = unit.
// 1.13c D2Game+0xA41B0
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_A41B0(A param1, uint8_t param2, int32_t eax, A esi, const D2GameBatch6E400Env* e)
{
    (void)param1;
    if (eax == 1) eax = 7;
    A rec = 0;
    if (esi && rd32(esi, 0) == 1 && rd32(esi, 0x14) != 0) rec = rdA(rd32(esi, 0x14), 0x2C);
    if (eax != 0) wr32(rec, 0x18, (uint32_t)eax);
    if (param2 != 0) {
        if (param2 > 0x4D) param2 = 0x4D;
        wr8(rec, 0x20, param2);
    }
    e->o11103(esi);                                     // REG?
    A st[2] = { rd32(esi, 0x2C), 1 };
    e->o11005(0);                                       // REG?
    e->f95E10(0, st);
}

// param_1 = skill id (ECX), in_EAX = struct (+8, +0x18), unaff_EBX = unit for 0xA3D60.
// 1.13c D2Game+0xA44C0
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_fn_d2game_A44C0(int32_t param1, A eax, A ebx, const D2GameBatch6E400Env* e)
{
    A dt = e->dataTables;
    A unit8 = rdA(eax, 8);
    uint32_t i18 = rd32(eax, 0x18);
    if (!(param1 > -1 && param1 < (int32_t)rd32(dt, 0xBA0))) return 0;
    A row = (A)param1 * 0x23C + rdA(dt, 0xB98);
    if (!row || !d2_fn_d2game_A3D60(param1, ebx, eax, e)) return 0;
    uint8_t cls = rd8(row, 0x230);
    if (i18 == 0) { if (cls == 4 || cls == 0xD) return 0; }
    else if (cls != 4 && cls != 0xD) return 0;
    int16_t s80 = (int16_t)rd16(row, 0x80), s82 = (int16_t)rd16(row, 0x82);
    if (cls == 1 && s80 > 0 && e->o10494(unit8, s80) != 0) return 0;
    if (unit8 == 0 && (cls == 2 || cls == 4 || cls == 5 || cls == 0xB || cls == 0xC || cls == 0xD))
        return 0;
    if (cls == 2 && ((s80 > 0 && e->o10494(unit8, s80) != 0) ||
                     (s82 > 0 && e->o10494(unit8, s82) != 0))) return 0;
    if ((e->bitMasks[8] & rd8(row, 4)) && s80 > 0 && e->o10494(unit8, s80) != 0) {
        A r = e->o10871(unit8);
        if (r && (int32_t)e->o10680(r, (int16_t)rd16(row, 0x54), 0) >= 3) return 0;
    }
    return 1;
}
