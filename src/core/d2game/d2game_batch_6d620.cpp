// Batch 6d620: 12 D2Game 1.13c functions from cached decompiles (no spec/vector).
// Raw-offset implementations. Callees are weak extern "C" hooks named by RVA
// (ordinals: 10331 GetUnitType-ish, etc. kept as d2ord_<n>) so the file links before
// the callee rows are mapped.
// GUESSED INPUTS (register-only/unaff_* in the decompile, taken as named here):
//  96D70: EAX=unit(in_EAX), param_2=owner unit ptr; 97730: EBX=scratch ptr;
//  97D10: ESI=game; 97F50: EAX=skill index; 983B0/987D0/988B0/98490: EAX/ESI/EDI/EBX
//  register args are exposed as explicit parameters; stack0xffffffe0 locals are
//  scratch buffers passed to FUN_DFC10 (uninitialised in retail).
#include <cstdint>
#define W extern "C" __attribute__((weak))
W int d2ord_10331(int); W int d2ord_10826(int); W int d2ord_11090(int, int); W int d2ord_10688(int);
W int d2ord_10920(void*); W int d2ord_10265(int); W void d2ord_10024(const void*, int);
W int d2ord_10421(); W int d2ord_10577(); W int d2ord_10501(int); W int d2ord_10786(...);
W int d2ord_10913(...); W int d2ord_10973(...); W int d2ord_10887(...); W int d2ord_10186(...);
W int d2ord_10945(...); W int d2ord_10051(); W int d2ord_11104(); W int d2ord_10830();
W int d2ord_10871(int, int); W int d2ord_10680(int, int, int); W int d2ord_11111(int, int);
W int d2ord_11108(int); W int d2ord_10195(int, int);
W void d2game_be170(void*); W void d2game_aea80(int, int*, int, int, int, int, int);
W void d2game_dfc10(void*); W void d2game_ab220(int, int, int, int, int, int, int, int);
W void d2game_ab290(int, int, int, int, int); W void d2game_97420(int*, void*, unsigned, unsigned);
W void d2game_96fa0(int, int*); W void d2game_97b50(int, int, unsigned); W void d2game_97c20(int*, int);
W int d2game_97a20(int*, int*, int*); W void d2game_96ea0(int, int); W int d2game_96540(int*);
W unsigned d2game_21180(uint64_t*); W void* d2game_572c0(int); W int d2game_bf120();
W void d2game_e0460(); W void d2game_efc60(); W int d2game_214d0(int, int);
extern "C" char* sgptDataTables_exref;
static inline int dt(int off) { return *(int*)(*(int*)sgptDataTables_exref + off); }
static inline int rd(int p, int o) { return *(int*)(p + o); }
static inline int* ip(int p, int o) { return (int*)(p + o); }
[[noreturn]] static void fatal(int id) { d2ord_10024((void*)"", d2ord_10265(id)); __builtin_trap(); }

// 1.13c D2Game+0x6D620
extern "C" int d2game_6d620(int* a)
{
    int t = d2ord_10826(d2ord_10331(a[1]));
    int u = a[1];
    if (!u || !rd(u, 0x10)) {
        d2ord_11090(u, 1);
        int r = d2ord_10688(a[4]);
        d2game_aea80(*a, (int*)a[1], 1, (int)((unsigned)rd(r, 0xdc) >> 8) + 1 + rd(*a, 0xa8), 0, 0, 0);
        return 0;
    }
    if (t != 0x4a && t != 0x36) return 0;
    d2game_be170((void*)*a);
    int* l = (int*)rd(*a, 0x10f4);
    if (!l || !*l) return 0;
    int* p = (int*)*l;
    while (*p != 0xb) { p = (int*)p[0x3d]; if (!p) return 0; }
    int s = p[6];
    short* a42 = (short*)(s + 0x42); short* a44 = (short*)(s + 0x44);
    if (t == 0x4a) { if (!*a44) { *a44 = 1; *a42 = 2; } }
    else if (!*a42) { *a42 = 1; *a44 = 2; }
    return 0;
}

// 1.13c D2Game+0x966E0
extern "C" void d2game_966e0(int, int* u, int* p)
{
    int v = (u && !*u) ? rd(d2ord_10920(u), 0x9c) : 0;
    if (!p[4] && !p[5]) d2game_ab290(v, (char)p[3], p[0], p[1], (short)p[2]);
    else d2game_ab220(p[3], p[0], v, (char)p[3], p[1], (short)p[2], p[4], p[5]);
}

// 1.13c D2Game+0x96D70
extern "C" int d2game_96d70(int owner, int unit)
{
    int pl = d2ord_10920((void*)unit);
    if (!pl) fatal(0x402);
    int* h = (int*)rd(pl, 0x44);
    if (!h) return 0;
    int n = dt(0xbf0);
    for (int i = 1, o = 0xc; i < n; i++, o += 0xc) {
        for (unsigned* q = *(unsigned**)(*h + o); q; q = (unsigned*)q[5]) {
            unsigned id = owner ? *(unsigned*)(owner + 0xc) : 0xffffffffu;
            if (q[1] == id) { *q &= ~1u; char scratch[32]; d2game_dfc10(scratch); return 1; }
        }
    }
    return 0;
}

// 1.13c D2Game+0x97730
extern "C" void d2game_97730(int unit, int* ebx)
{
    int pl = d2ord_10920((void*)unit);
    if (!pl) fatal(0x12f);
    int* h = (int*)rd(pl, 0x44);
    if (!h) return;
    int n = dt(0xbf0);
    for (int i = 1, o = 0xc; i < n; i++, o += 0xc) {
        void* e = (o < 0 || i >= n) ? 0 : (void*)(*h + o);
        d2game_97420(ebx, e, i == 7, i == 7);
        n = dt(0xbf0);
    }
    d2game_96fa0(unit, ebx);
}

// 1.13c D2Game+0x97D10
extern "C" void d2game_97d10(int param_1, int* tbl, int game)
{
    int n = dt(0xbf0);
    for (int i = 1, o = 0xc; i < n; i++, o += 0xc, n = dt(0xbf0)) {
        if (o == 0x54) continue;
        int e = *(int*)(*tbl + o);
        while (e) {
            int next = rd(e, 0x14);
            unsigned id = rd(e, 4);
            if (!game) fatal(0x2d3);
            bool skip = false;
            for (int* q = *(int**)(game + 0x1320 + (id & 0x7f) * 4); q; q = (int*)q[0x39]) {
                if ((unsigned)q[3] != id) continue;
                if (*(uint8_t*)((uintptr_t)q + 0xc6) & 1) { skip = true; break; }
                if (*q == 0) { if (!q[4]) { skip = true; break; } if (q[4] != 0x11) skip = true; }
                else { if (*q != 1 || !q[4]) { skip = true; break; } if (q[4] != 0xc) skip = true; }
                break;
            }
            (void)skip;
            // retail: loop exits on "break" paths, else FUN_97B50 then continues with e=next
            d2game_97b50(game, param_1, id);
            e = next;
        }
    }
}

// 1.13c D2Game+0x97F50
extern "C" void d2game_97f50(int p1, int* unit, int, int p4, int idx)
{
    if (!unit || *unit || idx <= 0 || idx >= dt(0xbf0)) return;
    d2game_97c20(unit, idx);
    d2game_96ea0(p1, p4);
    int pl = d2ord_10920(unit);
    if (!pl || !rd(pl, 0x44) || !d2game_96540((int*)rd(pl, 0x44))) return;
    int st[6];
    if (!d2game_97a20(unit, unit, st)) return;
    int b[3] = { unit[3], unit[3], (unsigned short)unit[1] };
    d2game_dfc10(b);
}

// 1.13c D2Game+0x983B0
extern "C" int d2game_983b0()
{
    int p = d2ord_10421();
    if (p) {
        int n = d2ord_10577();
        for (int i = 0; i < n; i++) if (*(char*)(p + 5) == 0) return i - 1;
    }
    return 7;
}

// 1.13c D2Game+0x98490
extern "C" int d2game_98490(int unit, int skill, int force, int game)
{
    if (skill < 0 || skill >= dt(0xba0) || !unit || !d2ord_10501(unit)) return 0;
    if (!force && d2ord_10786() <= (int)d2game_21180((uint64_t*)(game + 0x20))) return 0;
    d2ord_10786(); d2ord_10913(); d2ord_10973(); d2ord_10887(); d2ord_10786(); d2ord_10186();
    d2ord_10973(); d2ord_10887(); d2ord_10945(8, 99, 1);
    return 1;
}

// 1.13c D2Game+0x98680
extern "C" int d2game_98680(int* a, int unit)
{
    int* args = (int*)a[3]; int sk = args[0], lvl = args[1]; int g = a[1];
    if (sk < 0 || sk >= dt(0xba0)) return 0;
    int row = sk * 0x23c + dt(0xb98);
    if (!row || !unit || !d2ord_10501(unit)) return 0;
    int c = d2ord_10786(g, rd(row, 0x138), sk, lvl);
    if ((int)d2game_21180((uint64_t*)(g + 0x20)) >= c) return 0;
    int m = d2ord_10786(g, rd(row, 0x13c), sk, lvl);
    d2ord_10913(g);
    int v = d2ord_10973(g, 6, 0) + m * 0x100; if (v > g) v = g;
    d2ord_10887(g, 6, v, 0);
    int x = d2ord_10786(g, rd(row, 0x140), sk, 6);
    int mx = d2ord_10186(g);
    int w = d2ord_10973(g, 8) + x * 0x100; if (w > mx) w = mx;
    d2ord_10887(g, 8, w, 0);
    d2ord_10945(0, 99, 1);
    *ip(g, 8) += 1;
    return 1;
}

// 1.13c D2Game+0x987D0
extern "C" int d2game_987d0(int game)
{
    int lo, hi;
    if (!d2game_572c0(game)) {
        lo = d2ord_10973(); if (lo < 1) lo = 1;
        hi = d2ord_10973(); if (hi < 2) hi = 2;
    } else if (d2ord_10051() == 2) { lo = d2ord_10973(); hi = d2ord_10973(); }
    else { lo = d2ord_10973(); hi = d2ord_10973(); }
    lo *= 0x100; hi *= 0x100;
    if (lo < 1) lo = 0x100;
    if (hi <= lo) hi = lo + 0x100;
    unsigned r = hi - lo;
    if ((int)r < 1) return lo;
    uint64_t* seed = (uint64_t*)(game + 0x20);
    uint64_t s = (uint64_t)(uint32_t)*seed * 0x6ac690c5ull + (uint32_t)(*seed >> 32);
    *seed = s;
    return (r & (r - 1)) ? (int)((uint32_t)s % r) + lo : (int)((r - 1) & (uint32_t)s) + lo;
}

// 1.13c D2Game+0x988B0
extern "C" int d2game_988b0(int game, int* ebx, int skill)
{
    if (skill < 0 || skill >= dt(0xba0)) return 0;
    int row = skill * 0x23c + dt(0xb98);
    if (!row || d2ord_11104() || !ebx || *ebx != 1 || d2ord_10830() || !d2game_bf120()) return 0;
    int c = d2ord_10786(game, rd(row, 0x138));
    return c > (int)d2game_21180((uint64_t*)(game + 0x20));
}

// 1.13c D2Game+0x98950
extern "C" void d2game_98950(int unit, int p2)
{
    d2ord_10945(unit, p2, 0);
    d2game_e0460();
    if (!unit) fatal(0xa2a);
    d2game_efc60();
    int it = d2ord_10871(unit, 0x6d);
    if (!it) return;
    d2ord_10680(it, 0xb0, 0); d2ord_10680(it, 0xb1, 0);
    int a = d2ord_10913(unit);
    d2ord_10973(unit, 6, 0);
    int b = 1;
    if (a >> 8) b = d2game_214d0(a >> 8, it);
    d2ord_10887(unit, 0xc, 0xb1, 0); d2ord_10887(unit, 6, b << 8, 0); d2ord_10887(unit, 7, unit, 0);
    d2ord_11111(unit, it); d2ord_11108(it);
    for (int x = d2ord_10195(unit, 8); x; x = d2ord_10195(unit, 8)) { d2ord_11111(unit, x); d2ord_11108(x); }
}
