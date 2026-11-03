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
    P (*o10744)(P, P), (*o10048)(P), (*o10865)(P), (*o10973)(P, P, P), (*o10913)(P);
    void (*o10887)(P, P, P, P); P (*o10920)(P); P (*o10265)(P); void (*o10024)(const char*, P);
    P (*o10383)(P, P*, P*), (*o10076)(), (*o11129)(P, P), (*o10179)(), (*o10649)(P), (*o10980)();
    void (*o10637)(P, P), (*o10019)(P, P);
    P (*o10494)(P, P), (*o10871)(P, P);
    void (*o10680)(P, P, P);
    // D2Game internal
    P (*f6ad10)(P, P), (*ff620)(P, P), (*f9b0a0_)(P, P), (*f0f40)(P, P, P, P, P, P, P, P), (*f007a0)(P);
    void (*f9b0a0)(P, P, P);   // (game, unit, client)
    void (*fcc00b0)(), (*fed30)(P, P, P); P (*f8f930)(P, P*); void (*f98a90)(P), (*f99100)(P);
    P (*fbeb80)(), (*fb1340)(P, P), (*fab010)(P, P, P, P, P), (*ff5740)(P, P, P), (*fc1790)(P), (*fbfa30)(P, P, P*, P*);
    P (*f21100)(P), (*f21130)(P), (*f003a0)(P, P, P), (*f21ec0)(P), (*f35d0_)(P);
    void (*fcc35d0)(P, P);
    P (*fa454)(P), (*fa3b8)(P), (*fac28)(P), (*fad12)(P), (*fad3c)(P), (*facb2)(P), (*facd0)(P, P);
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
    if (unit == g4b100.f6ad10(ebx, 0)   /* ESI=client, stack 0 */) return;
    P iVar4 = g4b100.ff620(local4, unit);   // stack game, ESI=unit
    if (!iVar4) return;
    if (rd(ebx, 0x1b4) == 0) fatal(0x827);
    P arr, cnt;
    g4b100.o10383(rd(ebx, 0x1b4), &arr, &cnt);
    for (P i = 0; i < cnt; i++) {
        P e = rd(arr, (int)i * 4);
        if (!e) fatal(0x82c);
        if (e == iVar4) return;
    }
    g4b100.f9b0a0(local4, unit, ebx);   // pushes client, unit, game
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
            q = g4b100.f0f40(0x20003, -1, iVar2 + 4, 0, p1, unit, 0, 0);
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

// 1.13c D2Game+0x4C9E0  (eax = chance, ecx = item, edx = seed; stack: game, amount; ret 8)
P D2G_4C9E0(P game, P amount, int chance, P item, P seed) {
    if (!g4b100.o10744(item, 0x2d)) return 0;
    if (g4b100.o10048(item)) {                       // 2A940: the stackable branch
        if (g4b100.o10973(item, 0x46, 0) <= 0) return 0;
        uint32_t v = lcg(seed);
        if ((int)(v % 100) < chance) return g4b100.fc1790(game);
        return 0;
    }
    if (!g4b100.o10865(item)) return 0;              // 2A970
    uint32_t v = lcg(seed);
    if ((int)(v % 100) >= chance) return 0;
    int d = (int)g4b100.o10973(item, 0x48, 0) - (int)amount;
    if (d < 0 || d == 0) return g4b100.ff5740(game, item, seed);
    g4b100.o10887(item, 0x48, d, 0);
    P client = 0;
    if (seed && rd(seed, 0) == 0) client = (P)rd(g4b100.o10920(seed), 0x9c);
    return g4b100.fab010(item, client, 1, 0x48, d);
}

// unit position the way 4D040 inlines it: types 2, 4 and 5 read the path block directly
static P posX(P u) {
    int t = rd(u, 0);
    if (t == 2 || (t > 3 && t <= 5)) return rd(rd(u, 0x2c), 0xc);
    P pa = rd(u, 0x2c);
    return pa ? g4b100.fa454(pa) : 0;
}
static P posY(P u) {
    int t = rd(u, 0);
    if (t == 2 || (t > 3 && t <= 5)) return rd(rd(u, 0x2c), 0x10);
    P pa = rd(u, 0x2c);
    return pa ? g4b100.fa3b8(pa) : 0;
}

// 1.13c D2Game+0x4D040  (ecx = game, edx = unit; stack: skill, lvl; ret 8)
int D2G_4D040(P game, P unit, int skill, int lvl) {
    if (unit) wr(unit, 0xc4, rd(unit, 0xc4) | 0x40);
    P row = skillRow(skill);
    if (!row) return 0;
    g4b100.f007a0(unit);                               // eax = unit, ecx = game
    P owner = g4b100.o10392(rd(unit, 0x2c));
    if (owner == unit || !owner) return 0;
    g4b100.fed30(game, unit, owner);
    int n = (int)g4b100.o10786(unit, rd(row, 0x138), skill, lvl);
    P ox = posX(owner), oy = posY(owner);
    P tx = 2 * ox - posX(unit);
    P uy = posY(unit);
    int mid = (int)g4b100.fb1340(skill, unit);         // eax = skill, stack unit
    if (mid < 0 || mid >= rd(g4b100.dataTables, 0xb6c)) return 0;
    int32_t blk[23] = {0};
    blk[0] = 0x21; blk[1] = (int32_t)unit; blk[4] = mid;
    blk[5] = (int32_t)ox; blk[6] = (int32_t)oy;
    blk[7] = (int32_t)tx; blk[8] = (int32_t)(2 * oy - uy);
    blk[0xb] = skill; blk[0xc] = lvl;
    blk[0x15] = 0x41f50;                               // callback D2Game+0x41F50
    for (int i = 0; i < n; i++) { blk[0x16] = i; g4b100.f8f930(game, (P*)blk); }
    return 1;
}

// 1.13c D2Game+0x4D270  (ecx = game, edx = unit; stack: skill, lvl; ret 8)
int D2G_4D270(P game, P unit, int skill, int lvl) {
    if (unit) wr(unit, 0xc4, rd(unit, 0xc4) | 0x40);
    P row = skillRow(skill);
    if (!row) return 0;
    g4b100.f007a0(unit);                               // eax = unit, ecx = game
    P p = g4b100.o10392(rd(unit, 0x2c));
    P q = (p == unit) ? 0 : p;
    P x = 0, y = 0;
    if (!g4b100.fbfa30(unit, game, &y, &x)) return 0;   // stack = &y, ebx = &x
    int n = (int)g4b100.o10786(unit, rd(row, 0x138), skill, lvl);
    int mid = (int16_t)rd(row, 0x48);
    if ((int)g4b100.fac28(unit) != 1) {
        int16_t alt = (int16_t)(rd(row, 0x48) >> 16);
        if (alt >= 0) mid = alt;
    }
    if (mid < 0 || mid >= rd(g4b100.dataTables, 0xb6c)) return 0;
    int32_t blk[23] = {0};
    blk[0] = q ? 0x20 : 0x420;
    blk[1] = (int32_t)unit; blk[2] = (int32_t)unit; blk[3] = (int32_t)q; blk[4] = mid;
    blk[7] = (int32_t)x; blk[8] = (int32_t)y;
    blk[0xb] = skill; blk[0xc] = lvl;
    if (n) blk[0x15] = 0x4c190;                        // callback D2Game+0x4C190
    blk[0x16] = n;
    P e = g4b100.f8f930(game, (P*)blk);
    if (e) {
        g4b100.o10637(e, q ? 1 : 2);
        P dx = x - g4b100.f21100(unit), dy = y - g4b100.f21130(unit);
        g4b100.o10019(e, (int16_t)dy * 0x10000 + (int16_t)dx);
    }
    return 1;
}

// 1.13c D2Game+0x4DCD0  (ecx = game, edx = unit; stack: skill, lvl; ret 8)
int D2G_4DCD0(P game, P unit, int skill, int lvl) {
    P row = skillRow(skill);
    if (!row) return 0;
    P X = g4b100.o10511(unit);
    if (!X) return 0;
    int range = (int)g4b100.o10786(unit, rd(row, 0x64), skill, lvl);
    P cnt = g4b100.fad12(X);                           // param 1 of the used skill
    if (!cnt) return 0;
    P pty = g4b100.fad3c(X), pid = g4b100.facb2(X);
    P a = g4b100.f003a0(pid, game, pty);               // eax = param 3, ecx = game, edx = param 2
    if (!a) {
        a = g4b100.f0f40(3, pty, range, 0, game, unit, 0, 0);
        if (!a) return 1;
    }
    g4b100.facd0(unit, a);
    P x = 0, y = 0;
    g4b100.fbfa30(unit, game, &y, &x);                 // stack = &y, ebx = &x
    int mid = (int16_t)rd(row, 0x48);
    if ((int)g4b100.fac28(unit) != 1) {
        int16_t alt = (int16_t)(rd(row, 0x48) >> 16);
        if (alt >= 0) mid = alt;
    }
    if (mid < 0 || mid >= rd(g4b100.dataTables, 0xb6c)) return 0;
    int c = (int)g4b100.o10786(unit, rd(row, 0x13c), skill, lvl);
    int32_t blk[23] = {0};
    blk[0] = 0x21; blk[1] = (int32_t)unit;
    blk[4] = mid;
    blk[5] = (int32_t)g4b100.f21100(unit); blk[6] = (int32_t)g4b100.f21130(unit);
    blk[7] = (int32_t)x; blk[8] = (int32_t)y;
    blk[0xb] = skill; blk[0xc] = lvl;
    blk[0x15] = 0x4c190; blk[0x16] = c;
    g4b100.f8f930(game, (P*)blk);
    P left = cnt - 1;
    g4b100.o10243(X, left);                            // SetParam1
    if (left <= 0) return 0;
    P ecx = a ? rd(a, 0xc) : -1;
    P q = g4b100.f0f40(3, ecx, range, 0, game, unit, 0, 0);
    if (!q) return 1;
    g4b100.o10554(X, rd(q, 0));                        // SetParam2
    g4b100.o10748(X, rd(q, 0xc));                      // SetParam3
    g4b100.fcc35d0(unit, g4b100.f21ec0(skill));
    return 1;
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
