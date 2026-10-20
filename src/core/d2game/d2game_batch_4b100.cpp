// Batch 0x4B100..0x4E960. Written from the cached 1.13c decompile; register-passed
// arguments the decompiler dropped (unaff_*/in_EAX) are explicit parameters.
// Every callee (D2Common ordinals, D2Game FUN_6fc*) goes through the Seam table below;
// nothing is invented. Struct fields are read at their 1.13c byte offsets via rd/wr.
// Unrecovered: call-argument lists the decompiler lost (marked "seam arg" below).
#include <cstdint>
#include <cstring>
#include <cstdlib>

typedef intptr_t P;
struct D2G4B100Seams {
    // D2Common ordinals
    P (*o10392)(P), (*o10511)(P), (*o11133)(P), (*o10786)(P, P, P, P);
    void (*o10243)(P, P), (*o10554)(P, P), (*o10748)(P, P), (*o10867)(P), (*o10750)(P);
    P (*o10744)(P, P), (*o10048)(), (*o10865)(P), (*o10973)(P, P, P), (*o10913)(P);
    void (*o10887)(P, P, P, P), (*o10920)(P); P (*o10265)(P); void (*o10024)(const char*, P);
    P (*o10383)(P, P*, P*), (*o10076)(), (*o11129)(P, P), (*o10179)(), (*o10649)(P), (*o10980)();
    void (*o10637)(P, P), (*o10019)(P, P);
    P (*o10494)(P, P), (*o10871)(P, P);
    void (*o10680)(P, P, P);
    // D2Game internal
    P (*f6ad10)(P), (*ff620)(), (*f9b0a0_)(P, P), (*f0f40)(P, P, P, P, P, P), (*f007a0)(P);
    void (*f9b0a0)(P, P);
    void (*fcc00b0)(), (*fed30)(P, P, P); P (*f8f930)(P, P*); void (*f98a90)(P), (*f99100)(P);
    P (*fbeb80)(), (*fb1340)(P), (*fab010)(), (*ff5740)(P, P), (*fc1790)(), (*fbfa30)(P, P*);
    P (*f21100)(P), (*f21130)(P), (*f003a0)(P, P), (*f21ec0)(), (*f35d0_)(P);
    void (*fcc35d0)(P);
    P (*f41260)(P), (*ffa90)(), (*f21220)();
    void (*f21c50)();
    P (*f214d0)(P, P);
    void (*f6e390)(P, P, P);
    P dataTables;  // *sgptDataTables
};
D2G4B100Seams g4b100;
static inline int32_t rd(P b, int o) { int32_t v; memcpy(&v, (char*)b + o, 4); return v; }
static inline void wr(P b, int o, int32_t v) { memcpy((char*)b + o, &v, 4); }
static inline P skillRow(int i) {  // 0xBA0 count, 0xB98 base, 0x23C stride
    if (i < 0 || i >= rd(g4b100.dataTables, 0xba0)) return 0;
    return (P)rd(g4b100.dataTables, 0xb98) + (P)i * 0x23c;
}
static void fatal(P code) { g4b100.o10024("", g4b100.o10265(code)); exit(-1); }
// 1.13c lcg step on seed {lo@0x20,hi@0x24}; returns the new low word
static uint32_t lcg(P s) {
    uint64_t v = (uint64_t)(uint32_t)rd(s, 0x20) * 0x6ac690c5ull + (uint32_t)rd(s, 0x24);
    memcpy((char*)s + 0x20, &v, 8);
    return (uint32_t)v;
}

// 1.13c D2Game+0x4B100
void D2G_4B100(P unit, P ebx) {
    P local4 = rd(ebx, 0x1a8);
    if (unit == g4b100.f6ad10(0)) return;
    P iVar4 = g4b100.ff620();
    if (!iVar4) return;
    if (rd(ebx, 0x1b4) == 0) fatal(0x827);
    P arr, cnt;
    g4b100.o10383(rd(ebx, 0x1b4), &arr, &cnt);
    for (P i = 0; i < cnt; i++) {
        P e = rd(arr, (int)i * 4);
        if (!e) fatal(0x82c);
        if (e == iVar4) return;
    }
    g4b100.f9b0a0(local4, unit);
}

// 1.13c D2Game+0x4C220
int D2G_4C220() { g4b100.fcc00b0(); return 1; }

// 1.13c D2Game+0x4C4F0  (unaff_ESI/EDI/uStack_8 are seam args)
int D2G_4C4F0(P p1, P unit, int idx, P esi, P edi, P stk8) {
    P row = skillRow(idx);
    if (!row || !g4b100.o10511(unit)) return 0;
    P iVar2 = g4b100.o11133(unit);
    g4b100.f007a0(p1);
    P iVar1 = rd(unit, 0x2c);
    P q = g4b100.o10392(iVar1);
    P arg = p1;
    if (q == unit || q == 0) {
        arg = esi;
        if (q == unit || q == 0) {
            q = g4b100.f0f40(-1, iVar2 + 4, arg, unit, 0, 0);
            if (!q) { g4b100.o10243(edi, 0); return 0; }
        }
    }
    P r = g4b100.o10786(unit, rd(row, 0x138), stk8, p1);
    g4b100.o10243(iVar1, r);
    g4b100.o10554(iVar1, q ? rd(q, 0) : 6);
    g4b100.o10748(iVar1, q ? rd(q, 12) : -1);
    return 1;
}

// 1.13c D2Game+0x4C840
int D2G_4C840(P p1, P ecx) {
    if (g4b100.fbeb80()) return 1;
    g4b100.f99100(p1);
    g4b100.f98a90(ecx);
    return 0;
}

// 1.13c D2Game+0x4C9E0  (eax = chance)
P D2G_4C9E0(P unit, P seed, int chance) {
    P r = g4b100.o10744(unit, 0x2d);
    if (!r) return 0;
    if (!g4b100.o10048()) {
        if (!g4b100.o10865(unit)) return 0;
        uint32_t v = lcg(seed);
        if ((int)(v % 100) < chance) {
            P d = g4b100.o10973(unit, 0x48, 0) - unit;  // as decompiled
            if (d >= 0 && d != 0) {
                g4b100.o10887(unit, 0x48, d, 0);
                if (seed && rd(seed, 0) == 0) g4b100.o10920(seed);
                return g4b100.fab010();
            }
            return g4b100.ff5740(seed, unit);
        }
        return v / 100;
    }
    P s = g4b100.o10973(unit, 0x46, 0);
    if (s > 0) {
        uint32_t v = lcg(seed);
        if ((int)(v % 100) < chance) return g4b100.fc1790();
        return v / 100;
    }
    return s;
}

// 1.13c D2Game+0x4D040  (esi/regs is seam arg; local 23-dword zeroed struct)
int D2G_4D040(P p1, P unit, int idx, P esi) {
    if (unit) wr(unit, 0xc4, rd(unit, 0xc4) | 0x40);
    P row = skillRow(idx);
    if (!row) return 0;
    g4b100.f007a0(p1);
    P o = g4b100.o10392(rd(unit, 0x2c));
    if (o == unit || !o) return 0;
    g4b100.fed30(0, unit, o);
    P n = g4b100.o10786(unit, rd(row, 0x138), idx, idx);
    auto ok = [](P u) { int t = rd(u, 0); return t != 2 && (t < 4 || t > 5) && rd(u, 0x2c) != 0; };
    if (ok(o)) g4b100.o10867(rd(o, 0x2c));
    if (ok(o)) g4b100.o10750(rd(o, 0x2c));
    if (ok(unit)) g4b100.o10867(rd(unit, 0x2c));
    if (ok(unit)) g4b100.o10750(rd(unit, 0x2c));
    int s = (int)g4b100.fb1340(unit);
    if (s < 0 || s >= rd(g4b100.dataTables, 0xb6c)) return 0;
    uint32_t blk[23] = {0};  // seam arg: fields set by the lost stack writes
    for (int i = 0; i < n; i++) g4b100.f8f930(esi, (P*)blk);
    return 1;
}

// 1.13c D2Game+0x4D270  (ebx flag, ebp, si/di coords are register args)
int D2G_4D270(P p1, P unit, int idx, P ebx, P ebp, short si, short di) {
    if (unit) wr(unit, 0xc4, rd(unit, 0xc4) | 0x40);
    P row = skillRow(idx);
    if (!row) return 0;
    g4b100.f007a0(p1);
    P iVar1 = rd(unit, 0x2c);
    g4b100.o10392(0);
    int tmp[8];
    if (!g4b100.fbfa30(ebp, (P*)tmp)) return 0;
    P n = g4b100.o10786(unit, rd(row, 0x138), idx, idx);
    int s = (short)rd(row, 0x48) & 0xffff; s = (int16_t)s;
    if (g4b100.o10076() != 1 && (int16_t)(rd(row, 0x48) >> 16) >= 0) s = (int16_t)(rd(row, 0x48) >> 16);
    // NB: +0x4A is the high half of the dword at +0x48
    if (s < 0 || s >= rd(g4b100.dataTables, 0xb6c)) return 0;
    uint32_t blk[23] = {0};  // seam arg: blk[3]=s, [?]=idx, n!=0 -> callback LAB_6fc6c190
    P r = 0;
    P e = g4b100.f8f930(iVar1, (P*)blk);
    (void)n; (void)r;
    if (e) {
        g4b100.o10637(e, ebx ? 1 : 2);
        P x = g4b100.f21100(unit), y = g4b100.f21130(unit);
        g4b100.o10019(e, (int16_t)(si - (int16_t)y) * 0x10000 + (int16_t)(di - (int16_t)x));
    }
    return 1;
}

// 1.13c D2Game+0x4DCD0  (esi is register arg; stack structs partially lost)
int D2G_4DCD0(P p1, P unit, int idx, P esi) {
    P row = skillRow(idx);
    if (!row) return 0;
    P th = g4b100.o10511(unit);
    if (!th) return 0;
    P iVar3 = rd(row, 100);
    g4b100.o10786(unit, iVar3, idx, 0);
    if (!g4b100.o10980()) return 0;
    P u2 = g4b100.o10179();
    g4b100.o10649(th);
    P cnt = iVar3;
    P a = g4b100.f003a0(iVar3, u2);
    if (!a) {
        a = g4b100.f0f40(u2, esi, iVar3, unit, 0, 0);
        if (!a) return 1;
    }
    g4b100.o11129(unit, a);
    uint32_t blk[23] = {0};
    g4b100.fbfa30(th, (P*)blk);
    int s = (int16_t)rd(row, 0x48);
    if (g4b100.o10076() != 1 && (int16_t)(rd(row, 0x48) >> 16) >= 0) s = (int16_t)(rd(row, 0x48) >> 16);
    if (s < 0 || s >= rd(g4b100.dataTables, 0xb6c)) return 0;
    g4b100.o10786(unit, rd(row, 0x13c), 0, 0);
    memset(blk, 0, sizeof blk);
    blk[0] = 0x21;
    g4b100.f21100(unit);
    g4b100.f21130(unit);
    g4b100.f8f930(a, (P*)blk);
    g4b100.o10243(0, cnt - 1);
    if (cnt - 1 > 0) {
        P ar = a ? rd(a, 12) : -1;
        P q = g4b100.f0f40(ar, th, 0, unit, 0, 0);
        if (q) {
            g4b100.o10554(0, rd(q, 0));
            g4b100.o10748(0, rd(q, 12));
            g4b100.fcc35d0(g4b100.f21ec0());
        }
        return 1;
    }
    return 0;
}

// 1.13c D2Game+0x4DF20  (eax = amount; ecx unit/stat regs are seam args)
int D2G_4DF20(int amount, P unit) {
    if (amount < 1) return 0;
    int cur = (int)g4b100.o10973(unit, 0, 0);
    int max = (int)g4b100.o10913(unit);
    if (max < cur + amount) amount += max - (cur + amount);
    g4b100.o10887(unit, 0, amount, 0);  // seam arg: stat id / value
    return amount;
}

// 1.13c D2Game+0x4E500
int D2G_4E500(P unit, P, P p3) {
    if (!unit || !p3 || rd(unit, 0) != 1 || rd(p3, 0x4c) <= 0) return 0;
    P u = g4b100.f41260(unit);
    if (!u || g4b100.ffa90()) return 0;
    int v = (int)g4b100.o10973(u, 6, 0);
    if (v <= 0xff) return 0;
    P t = g4b100.f21220();
    if (!t || (int16_t)(rd(t, 0x170) & 0xffff) < 0) return 0;
    g4b100.f21c50();
    int d = (int)g4b100.f214d0(100, rd(p3, 0x4c));
    v -= d;
    if (v < 0x100) v = 0x100;
    g4b100.o10887(u, 6, v, 0);
    int n = rd(p3, 0x4c) - d;
    wr(p3, 0x4c, n);
    wr(p3, 0x4c, n > 0 ? n : 0);
    return 1;
}

// 1.13c D2Game+0x4E900  (ebx = this; ordinal args lost)
void D2G_4E900(P ebx) {
    if (g4b100.o10494(0, 0x61)) {
        P h = g4b100.o10871(0, 0x61);
        if (h) { g4b100.o10680(h, 0x15e, 0); g4b100.o10680(h, 0x15f, 0); }
    }
    g4b100.f6e390(ebx, 0, 1);
}

// 1.13c D2Game+0x4E960  (eax = skill index, retaddr slot = level)
void D2G_4E960(P self, int idx, int level) {
    if (g4b100.o10494(self, 0x61)) {
        P h = g4b100.o10871(self, 0x61);
        if (h) { g4b100.o10680(h, 0x15e, 0); g4b100.o10680(h, 0x15f, 0); }
    }
    int r = 0;
    if (level > 2) {
        P row = skillRow(idx);
        int th = row ? rd(row, 0x148) : 0;
        r = (int)(lcg(self) % 100) < th ? 1 : 0;
    }
    g4b100.f6e390(self, r, 0);
}
