// Batch b63: plain transcriptions of cached 1.13c D2Game functions. Guest addresses are 32-bit ints;
// hidden register inputs are explicit leading parameters (mapped per function). Callees are weak
// externs (Ordinal_N = D2Common/Fog imports, FUN_ = D2Game RVA) with args as in the decompile.
#include <cstdint>
#include <initializer_list>
#include <cstdlib>
typedef int32_t i32;
typedef uint32_t u32;
template<class T> static inline intptr_t P(T* p) { return (intptr_t)p; }
static inline intptr_t P(intptr_t v) { return v; }
#define W(a, o) (*(i32*)((intptr_t)(a) + (o)))
#define B(a, o) (*(uint8_t*)((intptr_t)(a) + (o)))
// Callees: typed weak externs from call sites; (...) kept where another TU declares the same extern "C" symbol variadic.
extern "C" __attribute__((weak)) uint32_t Ordinal_10265(uint32_t); extern "C" __attribute__((weak)) void Ordinal_10024(const void*, uint32_t);
extern "C" __attribute__((weak)) intptr_t Ordinal_10750(...);
extern "C" __attribute__((weak)) intptr_t Ordinal_10867(...);
extern "C" __attribute__((weak)) intptr_t Ordinal_10920(...);
extern "C" __attribute__((weak)) intptr_t Ordinal_11090(...);
extern "C" __attribute__((weak)) intptr_t Ordinal_10826(...);
extern "C" __attribute__((weak)) intptr_t Ordinal_10973(...);
extern "C" __attribute__((weak)) intptr_t Ordinal_10331(...);
extern "C" __attribute__((weak)) intptr_t Ordinal_10045(...);
extern "C" __attribute__((weak)) intptr_t Ordinal_10158(...);
extern "C" __attribute__((weak)) int32_t Ordinal_11017(int32_t);
extern "C" __attribute__((weak)) intptr_t Ordinal_10174(intptr_t,intptr_t,intptr_t);
extern "C" __attribute__((weak)) intptr_t Ordinal_10057(intptr_t);
extern "C" __attribute__((weak)) intptr_t Ordinal_10061(intptr_t);
extern "C" __attribute__((weak)) intptr_t Ordinal_10111(intptr_t,intptr_t);
extern "C" __attribute__((weak)) intptr_t Ordinal_10141(intptr_t,intptr_t);
extern "C" __attribute__((weak)) intptr_t Ordinal_10258(intptr_t);
extern "C" __attribute__((weak)) intptr_t Ordinal_10296(intptr_t);
extern "C" __attribute__((weak)) intptr_t Ordinal_10344(intptr_t);
extern "C" __attribute__((weak)) intptr_t Ordinal_10346(intptr_t,intptr_t=0);
extern "C" __attribute__((weak)) intptr_t Ordinal_10436(intptr_t,intptr_t);
extern "C" __attribute__((weak)) intptr_t Ordinal_10445(intptr_t);
extern "C" __attribute__((weak)) intptr_t Ordinal_10459(intptr_t=0);
extern "C" __attribute__((weak)) intptr_t Ordinal_10460(intptr_t);
extern "C" __attribute__((weak)) intptr_t Ordinal_10464(intptr_t);
extern "C" __attribute__((weak)) intptr_t Ordinal_10551(intptr_t,intptr_t,intptr_t,intptr_t);
extern "C" __attribute__((weak)) intptr_t Ordinal_10591(intptr_t,intptr_t,intptr_t,intptr_t,intptr_t,intptr_t,intptr_t,intptr_t);
extern "C" __attribute__((weak)) intptr_t Ordinal_10594(intptr_t,intptr_t);
extern "C" __attribute__((weak)) intptr_t Ordinal_10621(intptr_t);
extern "C" __attribute__((weak)) intptr_t Ordinal_10654(intptr_t,intptr_t,intptr_t,intptr_t,intptr_t,intptr_t,intptr_t);
extern "C" __attribute__((weak)) intptr_t Ordinal_10688(intptr_t);
extern "C" __attribute__((weak)) intptr_t Ordinal_10758(intptr_t,intptr_t,intptr_t);
extern "C" __attribute__((weak)) intptr_t Ordinal_10804(intptr_t,intptr_t,intptr_t);
extern "C" __attribute__((weak)) intptr_t Ordinal_10858();
extern "C" __attribute__((weak)) intptr_t Ordinal_10910(intptr_t,intptr_t,intptr_t);
extern "C" __attribute__((weak)) intptr_t Ordinal_11030(intptr_t,intptr_t);
extern "C" __attribute__((weak)) intptr_t Ordinal_11060(intptr_t);
extern "C" __attribute__((weak)) intptr_t Ordinal_11088(intptr_t);
extern "C" __attribute__((weak)) intptr_t Ordinal_11147(intptr_t);
extern "C" __attribute__((weak)) intptr_t Ordinal_11153(intptr_t,intptr_t);
extern "C" __attribute__((weak)) intptr_t Ordinal_11154(intptr_t,intptr_t);
extern "C" __attribute__((weak)) intptr_t Ordinal_10027(intptr_t,intptr_t);
extern "C" __attribute__((weak)) intptr_t Ordinal_10049(intptr_t);
extern "C" __attribute__((weak)) intptr_t Ordinal_10052(intptr_t);
extern "C" __attribute__((weak)) intptr_t Ordinal_10020(intptr_t);
extern "C" __attribute__((weak)) intptr_t FUN_6fc54130(intptr_t,intptr_t,intptr_t,intptr_t);
extern "C" __attribute__((weak)) intptr_t FUN_6fc21280();
extern "C" __attribute__((weak)) intptr_t FUN_6fd01d90(intptr_t,intptr_t,intptr_t,intptr_t,intptr_t);
extern "C" __attribute__((weak)) intptr_t FUN_6fcc22e0(intptr_t,intptr_t,intptr_t);
extern "C" __attribute__((weak)) intptr_t FUN_6fc53920(intptr_t,intptr_t);
extern "C" __attribute__((weak)) intptr_t FUN_6fd003a0(intptr_t,uint32_t);
extern "C" __attribute__((weak)) intptr_t FUN_6fc8a330(intptr_t,intptr_t);
extern "C" __attribute__((weak)) intptr_t FUN_6fc8f930(intptr_t,intptr_t);
extern "C" __attribute__((weak)) intptr_t FUN_6fcaaa70();
extern "C" __attribute__((weak)) intptr_t FUN_6fcffae0(intptr_t,intptr_t);
extern "C" __attribute__((weak)) intptr_t FUN_6fcaea80(intptr_t,intptr_t,intptr_t,intptr_t,intptr_t,intptr_t,intptr_t);
extern "C" __attribute__((weak)) intptr_t FUN_6fc737d0(intptr_t,intptr_t,intptr_t,intptr_t,intptr_t);
extern "C" __attribute__((weak)) intptr_t FUN_6fcac810();
extern "C" __attribute__((weak)) intptr_t FUN_6fcf2d90(intptr_t);
extern "C" __attribute__((weak)) intptr_t FUN_6fcf4570();
extern "C" __attribute__((weak)) intptr_t FUN_6fc32df0();
extern "C" __attribute__((weak)) intptr_t FUN_6fc21670(intptr_t);
extern "C" __attribute__((weak)) intptr_t FUN_6fcfffd0(intptr_t,intptr_t);
extern "C" __attribute__((weak)) intptr_t FUN_6fd00110();
extern "C" __attribute__((weak)) intptr_t FUN_6fc4f300();
extern "C" __attribute__((weak)) intptr_t FUN_6fd00f40(intptr_t,intptr_t,intptr_t,intptr_t);
extern "C" __attribute__((weak)) intptr_t FUN_6fcdc050(intptr_t);
extern "C" intptr_t g_b63_sgptDataTables; // &sgptDataTables
extern "C" intptr_t g_b63_vtbl6A330[0x65]; // DAT_6fd27bb8
[[noreturn]] static void b63_fatal(i32 line) { Ordinal_10024(0, Ordinal_10265(line)); std::abort(); }

// 1.13c D2Game+0x674A0
i32 D2GAME_674A0(i32 game)
{
    i32 st[8] = {0}; // iStack_14.. position; st[6]=uStack_10 mapped loosely
    i32 plr = W(game, 0x10f4);
    if (W(plr, 8) == 0) b63_fatal(0x82c);
    i32 unit = 0; // iStack_4 = out param of Ordinal_10174
    i32 r = (i32)Ordinal_10174(W(plr, 0xc), 4, 0xb);
    if (r == 0 && Ordinal_10174(W(Ordinal_10920(unit) + 0x10 + B(game, 0x6d) * 4, 0), 4, 10) == 0) {
        bool go = false;
        if (W(game, 0x70) == 0) {
            go = Ordinal_10174(W(Ordinal_10920(unit) + 0x10 + B(game, 0x6d) * 4, 0), 0x1a, 0) != 0;
            if (!go && W(game, 0x70) == 0) goto fail;
        }
        if (!go && Ordinal_10174(W(Ordinal_10920(unit) + 0x10 + B(game, 0x6d) * 4, 0), 0x28, 0) == 0) goto fail;
        i32 u = (i32)Ordinal_10331(unit);
        if (u && Ordinal_10826(u) == 1) {
            u32 f = 0;
            Ordinal_10141(unit, P(&st[0]));
            FUN_6fc54130(u, P(&st[0]), 3, 0x400);
            if (f) {
                i32 s18 = st[3];
                if (unit) { Ordinal_10057(f); s18 = st[3]; }
                if ((char)Ordinal_10459(0x27) != (char)Ordinal_10459(Ordinal_10826(f))) b63_fatal(0xe5c);
                i32 p[2] = {0, 0};
                i32 ok = (i32)Ordinal_10654(f, P(p), P(p), 3, 0x3e01, 0xc01, 0);
                if (ok && FUN_6fc21280()) {
                    i32 m = (i32)FUN_6fd01d90(2, 0x3c, (uint8_t)p[0], s18, game);
                    Ordinal_11090(m, 1);
                    m = (i32)FUN_6fcc22e0(game, m, 0x27);
                    if (m) {
                        f = ((u32)(uint8_t)Ordinal_10258(m)) | 3;
                        Ordinal_10111(m, f);
                        Ordinal_10346(ok, 0);
                        Ordinal_10346(Ordinal_10331(m, 0));
                        FUN_6fc53920(4, 0xb);
                        return 1;
                    }
                }
            }
        }
    }
fail:
    if (unit) {
        *(int16_t*)((intptr_t)unit + 0x6e) = 0x14;
        W(unit, 0x70) = unit;
        Ordinal_10158(unit);
        W(unit, 0xc4) |= 0x400;
        return 0;
    }
    b63_fatal(0x558);
}

// 1.13c D2Game+0x69B00  EAX(in_EAX)->p_eax first; __fastcall ECX=param_1.
void D2GAME_69B00(i32 p_eax, i32 param_1, i32* param_2, i32* param_3)
{
    i32 in_eax = p_eax;
    *param_3 = (i32)Ordinal_10621(P(param_2));
    if (param_1 == 0) b63_fatal(0x963);
    if (in_eax == 0) {
        in_eax = (i32)Ordinal_10061(param_2[0x18]);
    }
    if (in_eax != 0 && Ordinal_11088(in_eax) == 0x26) *param_3 = 0;
    i32 iVar1 = (i32)Ordinal_10804(P(param_2), P(in_eax), param_1);
    i32 iVar3 = (i32)Ordinal_10910(P(param_2), 0x77, 0);
    i32 t = (i32)Ordinal_10858();
    i32 iVar5 = 0;
    iVar1 = *param_3 + (((iVar5 - 1) * W(t, 0x19c) + W(t, 0x198) + iVar1 + iVar3) * *param_3) / 100;
    *param_3 = iVar1;
    if (param_2 && *param_2 == 0) {
        i32 id = param_2[1];
        i32 dt = (i32)W(g_b63_sgptDataTables, 0);
        if (id >= 0 && id < W(dt, 0xbc8)) {
            i32 rec = id * 0xc4 + W(dt, 0xbc4);
            if (rec) *param_3 = W(rec, 0x3c) + iVar1;
        }
    }
}

// 1.13c D2Game+0x6A330  EAX=out ptr (in_EAX), EBX=param_a, EDI=unit; stack retaddr slot=skill id (unaff_retaddr).
i32 D2GAME_6A330(i32* in_eax, i32 ebx, i32* edi, i32 param_1, u32 param_2, i32 skillId)
{
    (void)param_1;
    *in_eax = 1;
    i32 iVar2 = (i32)FUN_6fd003a0(ebx, param_2);
    i32 iVar3 = (i32)Ordinal_10296(iVar2);
    if (edi && *edi == 0 &&
        ((char)edi[0x1b] != 0 || W(Ordinal_10920(), 0x4c) != 0 ||
         (Ordinal_11017(edi[0x18]) != 0 && skillId != 0x10b)))
        return 0;
    uint8_t b = B(iVar3, 0x1b3);
    if (b < 0x65) {
        if (skillId != 0x79 && skillId != 0x7a && skillId != 0x16 && g_b63_vtbl6A330[b]) {
            Ordinal_10331(iVar2);
            return (i32)((intptr_t(*)())g_b63_vtbl6A330[b])();
        }
        return 0;
    }
    b63_fatal(0xcf8);
}

// 1.13c D2Game+0x6A720  __thiscall: this -> thisp.
i32 D2GAME_6A720(i32* thisp, i32 param_1, u32 param_2)
{
    i32 iVar1 = (i32)FUN_6fd003a0(param_1, param_2);
    if (!iVar1) return 0;
    if (thisp) {
        if (*thisp == 1 && B(Ordinal_10688(W(iVar1, 4)), 0x16d) == 0) return 1;
        if (Ordinal_10594(P(thisp), iVar1) == 0) return 1;
    }
    FUN_6fc8a330(0, param_2);
    return 1;
}

// 1.13c D2Game+0x6C900  __fastcall param_1; EBX=env (unaff_EBX), EDI=units array (unaff_EDI).
void D2GAME_6C900(i32* p, i32 ebx, i32* edi)
{
    auto uw = [&](i32 off_a, i32 off_b, intptr_t (*f)(...)) -> i32 {
        i32* u = (i32*)(intptr_t)p[1];
        i32 k = *u;
        if (k == 2 || (k > 3 && k < 6)) return W(u[0xb], off_a);
        return u[0xb] ? (i32)f(u[0xb]) : 0;
        (void)off_b;
    };
    i32 uVar2 = (i32)Ordinal_10331();
    i32 uVar3 = uw(0xc, 0, Ordinal_10867);
    i32 uStack34 = uw(0x10, 0, Ordinal_10750);
    i32 a44[14] = {0};
    i32 uStack30 = W(ebx, 8);
    a44[0] = 0x583; a44[3] = uVar3;
    i32 d0[4], cnt = 0; // auStack_d0 / iStack_dc: results list
    Ordinal_10591(W(*p, 0x1c), P(d0), uVar2, uVar3, uStack34, uStack30, 0, P(a44));
    Ordinal_10052(P(d0));
    for (i32 i = 0; i < cnt; i++) {
        i32* u = (i32*)(intptr_t)edi[i];
        if (u && (*u == 1 || *u == 0) && (B(u, 0xc6) & 1) == 0) {
            bool hit = (*u == 0) ? (u[4] != 0 && u[4] != 0x11) : (u[4] != 0 && u[4] != 0xc);
            if (hit) {
                i32 v = (i32)Ordinal_10973(u, 6, 0);
                i32 q = (i32)(((int64_t)(W(ebx, 4) * (v >> 8)) * -0x51eb851f) >> 32);
                Ordinal_10551(P(u), 6, ((q >> 5) - (q >> 31)) * 0x100, 0);
            }
        }
    }
    i32 st = p[1];
    Ordinal_10344(P(&st));
    u32 uVar12 = 1; i32 iVar11 = -5, iVar5 = 5;
    do {
        bool b7 = true; u32 u4 = uVar12 & 1; i32 iVar13 = -5, iVar8 = 5;
        do {
            u32 a[23] = {0};
            a[0] = 3; a[1] = p[2]; a[4] = 0x3e;  // idx = (0xcc - stack offset)/4
            a[5] = uw(0xc, 0, Ordinal_10867);
            a[6] = uw(0x10, 0, Ordinal_10750);
            a[7] = u4 ? iVar5 : iVar11;
            a[8] = b7 ? iVar8 : iVar13;
            i32 lvl = (i32)Ordinal_10973(a[1], 0xc, 0) / 5;
            a[12] = lvl < 1 ? 1 : lvl > 8 ? 8 : lvl;
            FUN_6fc8f930(P(*p), P(a));
            iVar8 += 5; iVar13 -= 5; b7 = !b7;
        } while (iVar8 < 0x19);
        iVar5 += 5; uVar12++; iVar11 -= 5;
    } while (iVar5 < 0x19);
}

// 1.13c D2Game+0x6CF90  __fastcall param_1. stack0xffffffd8/d9 are local out buffers.
i32 D2GAME_6CF90(i32* p)
{
    i32 u3 = (i32)Ordinal_10331(p[1]);
    i32 u4 = (i32)Ordinal_10826(u3);
    i32 uVar1 = W(Ordinal_10920(p[2]), 0x1c + B(*p, 0x6d) * 4);
    char buf[40];
    if (Ordinal_10436(u4, P(buf)) != 0) Ordinal_11154(uVar1, u3);
    i32 u = p[1];
    if (u && W(u, 0x10) != 0) {
        if ((W(u, 0x10) == 2 || W(u, 0x10) == 1)) {
            i32* q = (i32*)(intptr_t)p[2];
            if (!q || *q != 0 ||
                ((char)q[0x1b] == 0 && Ordinal_11017(q[0x18]) == 0 && W(Ordinal_10920(q), 0x4c) == 0)) {
                Ordinal_10027(uVar1, P(buf + 1));
                if (q && *q == 0) Ordinal_10920(q);
                FUN_6fcaaa70();
                FUN_6fcffae0(2, p[1] == 0 ? -1 : W(p[1], 0xc));
            }
        }
        return 1;
    }
    Ordinal_11090(u, 1);
    i32 d = (i32)Ordinal_10688(p[4]);
    FUN_6fcaea80(*p, p[1], 1, (i32)((u32)W(d, 0xdc) >> 8) + 1 + W(*p, 0xa8), 0, 0, 0);
    return 1;
}

// 1.13c D2Game+0x70FB0  EBX=player (unaff_EBX), EDI=param (unaff_EDI); Ordinal_10920 arg hidden (EAX/ECX).
i32 D2GAME_70FB0(i32 ebx, i32 edi, i32 unitArg)
{
    i32 r = (i32)Ordinal_10920(unitArg);
    if (W(r, 0x5c) != 0) return (u32)W(r, 0x5c) & 0xffffff00u;
    i32* t = (i32*)Ordinal_10045("..\\Source\\D2Game\\PLAYER\\PlrTrade.cpp", 0x803, 0);
    i32 s = (i32)Ordinal_10045("..\\Source\\D2Game\\PLAYER\\PlrTrade.cpp", 0x804, 0);
    t[1] = s;
    t[0] = (i32)FUN_6fc737d0(s, W(ebx, 0x60), edi, ebx, 0x2000);
    if (t[0] < 1) b63_fatal(0x806);
    t[2] = 0; t[3] = 0; t[5] = 0;
    i32 v = (i32)Ordinal_10973();
    t[4] = v;
    W(r, 0x5c) = (i32)(intptr_t)t;
    return (v & ~0xff) | 1;
}

// 1.13c D2Game+0x711D0  EAX=player (in_EAX), EBX=count (unaff_EBX), stack retaddr slot=arg for FUN_6fcf2d90.
i32 D2GAME_711D0(i32 in_eax, i32 ebx, i32 arg)
{
    if (W(in_eax, 0x60) == 0) return 0;
    i32 local8[2] = {0, 0};
    i32 it = (i32)Ordinal_10460(W(in_eax, 0x60));
    while (it) {
        i32 u = (i32)Ordinal_11147(it);
        it = (i32)Ordinal_10464(it);
        if (u && (char)Ordinal_10020(u) == 3) {
            local8[0] = 0;
            Ordinal_10758(P(local8), 0x20, 1);
            Ordinal_11030(u, 3);
            FUN_6fcac810();
            FUN_6fcf2d90(arg);
            ebx++;
        }
    }
    return ebx;
}

// 1.13c D2Game+0x717E0  __fastcall ECX=param_1, EDX=param_2; EAX=in_EAX id, SI=unaff_SI op.
void D2GAME_717E0(u32 in_eax, i32 param_1, i32 param_2, i32 param_4, int16_t op)
{
    if (param_2 == 0) b63_fatal(0x2d3);
    i32 n = W(param_2, 0x1520 + (in_eax & 0x7f) * 4);
    if (!n) return;
    while ((u32)W(n, 0xc) != in_eax) { n = W(n, 0xe4); if (!n) return; }
    if (W(n, 4) != 0x10b) return;
    i32 a = (i32)Ordinal_10331(param_1);
    if (!a || !Ordinal_10057(a)) return;
    i32 b = (i32)Ordinal_10331(n);
    if (!b || !Ordinal_10057(b)) return;
    if (op == 0x12) {
        if (B(param_1, 0x6c) != 0 && W(param_1, 0x68) == 2) {
            W(param_1, 100) = -1; W(param_1, 0x68) = 6; B(param_1, 0x6c) = 0;
        }
        FUN_6fcf4570();
    } else if (op == 0x13) {
        if (param_4 > 0 && param_4 <= (i32)Ordinal_10973(param_1, 0xf, 0)) {
            i32 x = (i32)Ordinal_10973(param_1, 0xe, 0);
            i32 y = (i32)Ordinal_10049(param_1);
            if (x + param_4 <= y) { FUN_6fc32df0(); FUN_6fc21670(-param_4); return; }
            FUN_6fcfffd0(0x13, param_1);
        }
    } else if (op == 0x14 && param_4 > 0) {
        i32 e = (i32)Ordinal_10973(param_1, 0xe, 0);
        if (param_4 <= e) {
            u32 mx = (u32)Ordinal_11060(param_1);
            u32 f = (u32)Ordinal_10973(param_1, 0xf, 0);
            if (f + param_4 <= mx) { FUN_6fc21670(-param_4); FUN_6fc21670(param_4); return; }
            if (f < mx) { FUN_6fc21670(mx - f); FUN_6fc21670(f - 0xe); }
        }
    }
}

// 1.13c D2Game+0xBAFF0  EAX=in_eax (pointer to {game, ?, unit}); Ordinal args hidden in regs.
void D2GAME_BAFF0(i32* in_eax)
{
    i32 pos[2] = {0, 0};
    Ordinal_10141(in_eax[2], P(pos));
    pos[1] += 5;
    Ordinal_10331(in_eax[2]);
    i32 iVar2 = (i32)Ordinal_10331();
    i32 res = (i32)FUN_6fc54130(iVar2, in_eax[2], P(pos), 2);
    if (!res) return;
    i32 g = *in_eax;
    i32* m = (i32*)Ordinal_11153(W(g, 0x1c), 2);
    if (!m) return;
    m[0] = 2; m[1] = 0x236; m[2] = W(g, 0x1c); m[0x20] = g;
    m[0x32] = W(g, 0x70) == 0 ? (m[0x32] & 0xfdffffff) : (m[0x32] | 0x2000000);
    m[0x32] |= 0x4000000;
    i32 u = (i32)Ordinal_10826();
    uint8_t c = (uint8_t)Ordinal_10459();
    *(uint8_t*)(m + 6) = c;
    m[7] = W(g, 0xbc + c * 4);
    m[0x1d] = 0;
    FUN_6fd00110();
    m[0x31] |= 0x10; m[0x34] = 0xb;
    i32 gen = W(g, 0x98) + 1; if (gen == -1) gen = 1;
    W(g, 0x98) = gen;
    i32* z = (i32*)Ordinal_10045("..\\Source\\D2Game\\UNIT\\SUnit.cpp", 0x265, 0);
    m[5] = (i32)(intptr_t)z;
    for (int i = 0; i < 0xe; i++) z[i] = 0;
    *(i32*)(intptr_t)m[5] = (i32)Ordinal_10688(0x236);
    m[4] = 1;
    Ordinal_10445(P(m));
    FUN_6fc4f300();
    FUN_6fd00f40(res, P(m), u, g);
    // uRam000000a8/a9 = 0/1 in the decompile is a Ghidra artefact (null-based store); omitted.
    m[0x31] |= 0x3000000;
}

static i32 b63_bc_common(i32* p, i32 dx, i32 dy, i32 ox, i32 oy)
{
    i32 iVar1 = p[1];
    if ((!iVar1 || W(iVar1, 0x10) == 0) && W(*p, 0x10f4) != 0) {
        i32* n = (i32*)(intptr_t)*(i32*)(intptr_t)W(*p, 0x10f4);
        if (!n) return 0;
        while (*n != 0x17) { n = (i32*)(intptr_t)n[0x3d]; if (!n) return 0; }
        i32 s = n[6];
        Ordinal_10141(iVar1, s + ox);
        W(s, ox) += dx;
        W(s, oy) += dy;
        FUN_6fcdc050((i32)(intptr_t)p);
    }
    return 0;
}
// 1.13c D2Game+0xBC510  __fastcall param_1
i32 D2GAME_BC510(i32* p) { return b63_bc_common(p, -0x27, 0x21, 0x2c, 0x30); }
// 1.13c D2Game+0xBC580  __fastcall param_1
i32 D2GAME_BC580(i32* p) { return b63_bc_common(p, -0xc, -0x34, 0x24, 0x28); }
