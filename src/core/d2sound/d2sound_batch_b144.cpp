// D2sound batch b144 -- statically linked MSVC CRT pieces. Win32/CRT globals are host seams; retail
// 32-bit addresses stay uint32_t and are dereferenced only through Env::rd/wr.
#include <cstdint>
#include <cmath>
#include <cstring>

struct D2soundB144Env {
    uint32_t (*rd)(uint32_t a);              // read dword at retail address
    void     (*wr)(uint32_t a, uint32_t v);
    uint32_t (*heapAlloc)(uint32_t flags, uint32_t size);          // HeapAlloc(DAT_6860,..)
    uint32_t (*heapReAlloc)(uint32_t flags, uint32_t p, uint32_t size);
    void     (*heapFree)(uint32_t flags, uint32_t p);
    uint32_t (*virtualAlloc)(uint32_t size, uint32_t type, uint32_t prot);
    uint32_t (*newGroup)(uint32_t region);   // ___sbh_alloc_new_group (0x395E)
};

// Small-block-heap globals (DAT_6f9c66xx), held by the host.
struct D2soundSbhState {
    uint32_t lastFree;    // 6614
    uint32_t regionCnt;   // 6618
    uint32_t regionArr;   // 661c
    uint32_t curRegion;   // 6624
    uint32_t regionCap;   // 6628
    uint32_t lastGroup;   // 662c
};

// 1.13c D2sound+0x38a7 -- ___sbh_alloc_new_region; entries are 0x14 bytes. 0 on failure.
uint32_t D2sound_38a7(D2soundSbhState* s, const D2soundB144Env* e) {
    if (s->regionCnt == s->regionCap) {
        uint32_t p = e->heapReAlloc(0, s->regionArr, (s->regionCap * 5 + 0x50) * 4);
        if (!p) return 0;
        s->regionCap += 0x10;
        s->regionArr = p;
    }
    uint32_t r = s->regionArr + s->regionCnt * 0x14;
    uint32_t hdr = e->heapAlloc(8, 0x41c4);
    e->wr(r + 16, hdr);
    if (hdr) {
        uint32_t base = e->virtualAlloc(0x100000, 0x2000, 4);
        e->wr(r + 12, base);
        if (base) {
            e->wr(r + 8, 0xffffffffu);
            e->wr(r, 0);
            e->wr(r + 4, 0);
            s->regionCnt++;
            e->wr(e->rd(r + 16), 0xffffffffu);
            return r;
        }
        e->heapFree(0, e->rd(r + 16));
    }
    return 0;
}

// 1.13c D2sound+0x3d43 -- ___sbh_alloc_block; returns retail address of the block payload, 0 on failure.
uint32_t D2sound_3d43(uint32_t reqSize, D2soundSbhState* s, const D2soundB144Env* e) {
    auto rd = [&](uint32_t a) { return e->rd(a); };
    auto wr = [&](uint32_t a, uint32_t v) { e->wr(a, v); };
    auto rb = [&](uint32_t a) { return (uint8_t)(e->rd(a & ~3u) >> ((a & 3) * 8)); };
    auto wb = [&](uint32_t a, uint8_t v) {
        uint32_t sh = (a & 3) * 8, w = e->rd(a & ~3u);
        e->wr(a & ~3u, (w & ~(0xffu << sh)) | ((uint32_t)v << sh));
    };

    uint32_t uVar7 = (reqSize + 0x17) & 0xfffffff0u;
    int32_t iVar8 = ((int32_t)(reqSize + 0x17) >> 4) - 1;
    uint32_t end = s->regionArr + s->regionCnt * 0x14;
    uint32_t reg = s->curRegion;
    uint32_t uVar14, local_c;
    if (iVar8 < 0x20) { uVar14 = 0xffffffffu >> (iVar8 & 0x1f); local_c = 0xffffffffu; }
    else { uVar14 = 0; local_c = 0xffffffffu >> ((iVar8 - 0x20) & 0x1f); }
    auto hit = [&](uint32_t p) { return (rd(p + 4) & local_c) != 0 || (rd(p) & uVar14) != 0; };

    while (reg < end && !hit(reg)) reg += 0x14;
    if (reg == end) {
        uint32_t p = s->regionArr;
        while (p < s->curRegion && !hit(p)) p += 0x14;
        reg = p;
        if (p == s->curRegion) {
            while (p < end && rd(p + 8) == 0) p += 0x14;
            uint32_t q = s->regionArr;
            reg = p;
            if (p == end) {
                while (q < s->curRegion && rd(q + 8) == 0) q += 0x14;
                reg = q;
                if (q == s->curRegion) {
                    reg = D2sound_38a7(s, e);
                    if (!reg) return 0;
                }
            }
            int32_t g = (int32_t)e->newGroup(reg);
            wr(rd(reg + 16), (uint32_t)g);
            if ((int32_t)rd(rd(reg + 16)) == -1) return 0;
        }
    }
    uint32_t h = rd(reg + 16);
    int32_t grp = (int32_t)rd(h);
    auto lo = [&](int32_t g) { return h + 4 * (0x11 + g); };
    auto hi = [&](int32_t g) { return h + 4 * (0x31 + g); };
    if (grp == -1 || ((rd(hi(grp)) & local_c) == 0 && (rd(lo(grp)) & uVar14) == 0)) {
        grp = 0;
        uint32_t p9 = h + 0x44;
        if ((rd(h + 0xc4) & local_c) == 0 && (rd(p9) & uVar14) == 0) {
            uint32_t p12;
            do {
                p12 = p9 + 0x84;
                grp++;
                p9 += 4;
            } while ((rd(p12) & local_c) == 0 && (rd(p9) & uVar14) == 0);
        }
    }
    uint32_t piVar3 = h + 4 * (grp * 0x81 + 0x51);
    int32_t bit = 0;
    uVar14 = rd(lo(grp)) & uVar14;
    if (uVar14 == 0) { uVar14 = rd(hi(grp)) & local_c; bit = 0x20; }
    for (; (int32_t)uVar14 >= 0; uVar14 <<= 1) bit++;
    uint32_t blk = rd(piVar3 + 4 * (bit * 2 + 1));
    int32_t rem = (int32_t)(rd(blk) - uVar7);
    int32_t cls = (rem >> 4) - 1;
    if (cls > 0x3f) cls = 0x3f;
    s->curRegion = reg;
    if (cls != bit) {
        if (rd(blk + 4) == rd(blk + 8)) {
            if (bit < 0x20) {
                uint32_t m = ~(0x80000000u >> (bit & 0x1f));
                wr(lo(grp), m & rd(lo(grp)));
                uint8_t c = rb(h + bit + 4) - 1; wb(h + bit + 4, c);
                if (c == 0) wr(reg, rd(reg) & m);
            } else {
                uint32_t m = ~(0x80000000u >> ((bit - 0x20) & 0x1f));
                wr(hi(grp), rd(hi(grp)) & m);
                uint8_t c = rb(h + bit + 4) - 1; wb(h + bit + 4, c);
                if (c == 0) wr(reg + 4, rd(reg + 4) & m);
            }
        }
        wr(rd(blk + 8) + 4, rd(blk + 4));
        wr(rd(blk + 4) + 8, rd(blk + 8));
        if (rem != 0) {
            uint32_t p1 = piVar3 + cls * 8;
            uint32_t head = rd(p1 + 4);
            wr(blk + 8, p1);
            wr(blk + 4, head);
            wr(p1 + 4, blk);
            wr(rd(blk + 4) + 8, blk);
            if (rd(blk + 4) == rd(blk + 8)) {
                uint8_t c = rb(h + cls + 4); wb(h + cls + 4, c + 1);
                if (cls < 0x20) {
                    if (c == 0) wr(reg, rd(reg) | (0x80000000u >> (cls & 0x1f)));
                    wr(lo(grp), rd(lo(grp)) | (0x80000000u >> (cls & 0x1f)));
                } else {
                    if (c == 0) wr(reg + 4, rd(reg + 4) | (0x80000000u >> ((cls - 0x20) & 0x1f)));
                    wr(hi(grp), rd(hi(grp)) | (0x80000000u >> ((cls - 0x20) & 0x1f)));
                }
            }
        }
    }
    if (rem != 0) {
        wr(blk, (uint32_t)rem);
        wr(blk + (uint32_t)rem - 4, (uint32_t)rem);
    }
    blk += (uint32_t)rem;
    wr(blk, uVar7 + 1);
    wr(blk + uVar7 - 4, uVar7 + 1);
    uint32_t cnt = rd(piVar3);
    wr(piVar3, cnt + 1);
    if (cnt == 0 && reg == s->lastFree && grp == (int32_t)s->lastGroup) s->lastFree = 0;
    wr(h, (uint32_t)grp);
    return blk + 4;
}

// 1.13c D2sound+0x15c5 -- __lock(n): lazily create the table lock, then enter it.
struct D2soundLockEnv {
    uint32_t (*lockPtr)(int n);              // (&DAT_6f9c5038)[n*2]
    int  (*initLockNum)(int n);              // __mtinitlocknum; 0 = failed
    void (*amsgExit)(int code);              // __amsg_exit
    void (*enter)(int n);                    // EnterCriticalSection
};
void D2sound_15c5(int n, const D2soundLockEnv* e) {
    if (e->lockPtr(n) == 0 && e->initLockNum(n) == 0) e->amsgExit(0x11);
    e->enter(n);
}

// 1.13c D2sound+0x295e -- __local_unwind2: run the cleanup handlers of an SEH frame down to `stop`.
// Frame is 32-bit retail memory: +8 table, +0xc current level; table entries are 12 bytes.
void D2sound_295e(uint32_t frame, int32_t stop, const D2soundB144Env* e, void (*prologue29f2)(), void (*callHandler)(uint32_t fn)) {
    for (;;) {
        uint32_t tbl = e->rd(frame + 8);
        int32_t lvl = (int32_t)e->rd(frame + 0xc);
        if (lvl == -1 || lvl == stop) break;
        e->wr(frame + 0xc, e->rd(tbl + lvl * 0xc));
        if (e->rd(tbl + 4 + lvl * 0xc) == 0) {
            prologue29f2();
            callHandler(e->rd(tbl + 8 + lvl * 0xc));
        }
    }
}

// 1.13c D2sound+0x2450 -- __chkstk: stack-page probe; the host stack is not probed, so nothing to do.
void D2sound_2450() {}

// 1.13c D2sound+0x458d -- ___crtMessageBoxA; user32 lookups and the DAT_6f9c63xx cache are host seams.
struct D2soundMsgBoxEnv {
    bool  haveMsgBox;                        // DAT_633c != 0 after (lazy) load; false = load failed
    int  (*messageBox)(int hwnd, const char* text, const char* caption, uint32_t type);
    int  (*getActiveWindow)();               // null seam = DAT_6340 == 0
    int  (*getLastActivePopup)(int hwnd);    // null = DAT_6344 == 0
    bool  hasStationFns;                     // DAT_6348 != 0
    bool  stationInteractive;                // station flags & 1 (GetUserObjectInformationA ok)
    uint32_t osPlatform;                     // DAT_6004 (unused beyond load)
    uint32_t osMajor;                        // DAT_6010
};
int D2sound_458d(const char* text, const char* caption, uint32_t type, const D2soundMsgBoxEnv* e) {
    int hwnd = 0;
    if (!e->haveMsgBox) return 0;
    if (!e->hasStationFns || e->stationInteractive) {
        if (e->getActiveWindow && (hwnd = e->getActiveWindow()) != 0 && e->getLastActivePopup)
            hwnd = e->getLastActivePopup(hwnd);
    } else if (e->osMajor < 4) type |= 0x40000;
    else type |= 0x200000;
    return e->messageBox(hwnd, text, caption, type);
}

// 1.13c D2sound+0xcff7 -- __frnd: round to integer in the current x87 mode (nearest-even default).
double D2sound_cff7(double x) { return std::nearbyint(x); }

// 1.13c D2sound+0xc217 -- __startOneArgErrorHandling: forwards to the error raiser 0xCEFB.
struct D2soundMathErr { int32_t type; /* ... retail struct, doubles at +8/+0x10/+0x18 */ int32_t pad[9]; };
double D2sound_c217(double st0, int code, uint16_t cw, D2soundMathErr* err, void (*cefb)(int, D2soundMathErr*, uint16_t*)) {
    cefb(code, err, &cw);
    return st0;
}

// 1.13c D2sound+0xcefb -- math-error raiser (FUN_6f9bcefb). Exception/FP-control calls are host seams.
struct D2soundCefbEnv {
    bool (*handleExc)(uint32_t flag, double* val, uint32_t cw);   // __handle_exc
    void (*raiseExc)(uint32_t flag, int code, double* a, double* b, uint32_t cw);  // __raise_exc
    void (*ctrlfp)();
    int  (*fpInit)();       // FUN_6f9bda83 (only when DAT_6f9c5c30 == 0)
    int   haveHandler;      // DAT_6f9c5c30
    void (*finish)(int type);   // FUN_6f9bdf4f
};
// err: int[]; type at [0], operands at +8 / +16 (doubles), result at +24.
void D2sound_cefb(int code, int32_t* err, uint16_t* cwp, double* ops, const D2soundCefbEnv* e) {
    uint32_t cw = *cwp, flag = 0;
    bool go = true;
    switch (err[0]) {
        case 1: case 5: flag = 8; break;
        case 2: flag = 4; break;
        case 3: flag = 0x11; break;
        case 4: flag = 0x12; break;
        case 7: err[0] = 1; go = false; break;
        case 8: flag = 0x10; break;
        default: go = false; break;
    }
    if (go && !e->handleExc(flag, ops + 3, cw))
        e->raiseExc(flag, code, ops + 1, ops + 3, cw);   // 0x10/0x16/0x1d also stash the result operand
    e->ctrlfp();
    if (err[0] != 8 && e->haveHandler == 0 && e->fpInit() != 0) return;
    e->finish(err[0]);
}

// 1.13c D2sound+0xcc84 -- libm error-record builder: maps the op code to (type, name) and calls the
// user _matherr hook (PTR_FUN_6f9c5c00); sets errno EDOM(0x21)/ERANGE(0x22) when it returns 0.
struct D2soundMatherr { int32_t type; const char* name; double a1, a2, ret; };
void D2sound_cc84(const double* p1, const double* p2, double* p3, int op,
                  int (*matherr)(D2soundMatherr*), int* err) {
    D2soundMatherr m{}; m.type = 0; m.name = "";
    enum { EDOM_ = 0x21, ERANGE_ = 0x22 };
    int mode = 0;   // 0 = type-from-table error path, 1 = domain(0x21) path, 2 = range(0x22) path, 3 = no hook
    switch (op) {
        case 0xa1: m.type = 3; m.name = "exp2"; mode = 2; break;
        case 0xa2: m.type = 4; m.name = "exp2"; mode = 2; break;
        case 0xa6: m.type = 3; m.name = "exp10"; mode = 2; break;
        case 0xaa: m.type = 2; m.name = "log2"; mode = 2; break;
        case 0xab: m.name = "log2"; mode = 1; *p3 = *p3; break;
        case 0x3e8: m.name = "log"; *p3 = *p1; mode = 1; break;
        case 0x3e9: m.name = "log10"; *p3 = *p1; mode = 1; break;
        case 0x3ea: m.name = "exp"; *p3 = *p1; mode = 1; break;
        case 0x3eb: m.name = "atan"; *p3 = *p1; mode = 1; break;
        case 0x3ec: m.name = "ceil"; *p3 = *p1; mode = 1; break;
        case 0x3ed: m.name = "floor"; *p3 = *p1; mode = 1; break;
        case 0x3ee: m.name = "pow"; mode = 1; break;
        case 0x3ef: m.name = "modf"; *p3 = *p1; mode = 1; break;
        case 2:  m.type = 2; m.name = "log"; mode = 2; break;
        case 3:  m.name = "log"; mode = 1; break;
        case 8:  m.type = 2; m.name = "log10"; mode = 2; break;
        case 9:  m.name = "log10"; mode = 1; break;
        case 0xe: m.type = 3; m.name = "exp"; mode = 2; break;
        case 0xf: m.type = 4; m.name = "exp"; mode = 3; break;
        case 0x18: m.type = 3; m.name = "pow"; mode = 2; break;
        case 0x19: m.type = 4; m.name = "pow"; mode = 3; break;
        case 0x1a: *p3 = 1.0; return;
        case 0x1b: m.type = 2; m.name = "pow"; mode = 2; break;
        case 0x1c: m.name = "pow"; mode = 1; break;
        case 0x1d: m.name = "pow"; *p3 = *p1; mode = 1; break;
        default: return;
    }
    m.a1 = *p1; m.a2 = *p2; m.ret = *p3;
    if (mode == 1) m.type = 1;
    int r = matherr(&m);
    if (mode == 1 && r == 0) *err = EDOM_;
    else if ((mode == 2) && r == 0) *err = ERANGE_;
    *p3 = m.ret;
}

// 1.13c D2sound+0x9f40  _adj_fdiv core (the Pentium FDIV-flaw-safe divide), cdecl: two 80-bit operands on the stack,
// a at [esp+4] (mantissa lo, hi, sign|exponent) and b at [esp+0x10]; returns ST0 = b / a. When a's mantissa bits 59..62
// hit a flaw pattern (table DAT_6f9c59c0 at (hi*2 ^ 0x0e000000) >> 28, checked only when bits 25..27 of hi*2 are 0b111),
// both operands are first multiplied by 15/16 (or 17/16 when b's exponent is 1) at 64-bit precision: exact for every
// operand with at most 60 significant bits, so the quotient is the plain one. A denormal a is classified on a copy scaled by
// 2^63 (DAT_6f9c59d8) while the registers keep the original. Operands here are doubles widened exactly to 80 bits (the
// caller runs at 53-bit precision), so the result is the IEEE quotient. Our port: widen-back and divide.
static double d2s_ext80_to_double(uint32_t lo, uint32_t hi, uint16_t se) {
    uint64_t m = ((uint64_t)hi << 32) | lo;
    uint64_t sign = (uint64_t)(se & 0x8000) << 48;
    int32_t e = se & 0x7fff;
    uint64_t bits;
    if (m == 0) bits = sign;
    else if (e == 0x7fff) bits = sign | 0x7ff0000000000000ull | ((m << 1) >> 12);   // inf (m = 1 << 63) or NaN payload
    else {
        int lz = __builtin_clzll(m);
        m <<= lz; e -= lz;
        int32_t de = e - 16383 + 1023;
        if (de >= 0x7ff) bits = sign | 0x7ff0000000000000ull;
        else if (de >= 1) bits = sign | ((uint64_t)de << 52) | ((m >> 11) & 0x000fffffffffffffull);
        else { int sh = 12 - de; bits = sign | (sh < 64 ? (m >> sh) : 0); }  // double denormal (truncated; not reached from doubles)
    }
    double d; std::memcpy(&d, &bits, 8); return d;
}
double D2sound_9f40(uint32_t a_lo, uint32_t a_hi, uint16_t a_se, uint32_t b_lo, uint32_t b_hi, uint16_t b_se) {
    return d2s_ext80_to_double(b_lo, b_hi, b_se) / d2s_ext80_to_double(a_lo, a_hi, a_se);
}

// 1.13c D2sound+0xb2e9 -- SSE2 table-driven pow(x, y) (CRT __libm_sse2_pow body; callers: thunk 0xB2D0).
// ponytail: libm pow, not the retail table polynomial -- special-case results agree, last-ulp may not.
double D2sound_b2e9(double x, double y) { return std::pow(x, y); }
