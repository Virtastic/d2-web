// MSVC CRT pieces linked into D2sound.dll (small-block heap, memmove, FP exception helpers).
// Retail addresses are 32-bit; the small-block heap works on 32-bit addresses relative to `base`
// (the arena). Win32 services (VirtualAlloc/VirtualFree/HeapAlloc/HeapFree/RaiseException/lock/errno/x87 status)
// are host seams passed in.
#include <cstdint>
#include <cstring>

typedef uint32_t u32;
struct D2SndSbh {
    uint8_t* base;
    u32 hdrList;      // DAT_6f9c661c  region header array (5 dwords each)
    int hdrCount;     // DAT_6f9c6618
    u32 deferHdr;     // DAT_6f9c6614
    int deferInd;     // DAT_6f9c662c
    u32 scan;         // DAT_6f9c6624
    u32 threshold;    // DAT_6f9c6620
    int heapMode;     // DAT_6f9c6864
    u32 (*newRegion)(D2SndSbh*);              // ___sbh_alloc_new_region (0x38A7)
    bool (*virtualAlloc)(u32 addr, u32 size); // commit, PAGE_READWRITE
    void (*virtualFree)(u32 addr, u32 size, u32 type);
    void (*heapFree)(u32 addr);
    void* (*heapAlloc)(size_t);               // HeapAlloc(DAT_6f9c6860,0,n)
    void (*lock)(int);
    void (*unlock)(void);
};
#define R(a) (*(u32*)(s.base + (u32)(a)))
#define W(a, v) (*(u32*)(s.base + (u32)(a)) = (u32)(v))
#define B(a) (*(int8_t*)(s.base + (u32)(a)))

static void sbhClr(D2SndSbh& s, u32 hdr, u32 h, u32 grp, u32 i) {
    if (i < 0x20) {
        u32 m = ~(0x80000000u >> (i & 31));
        W(h + 0x44 + grp * 4, R(h + 0x44 + grp * 4) & m);
        if (--B(h + i + 4) == 0) W(hdr, R(hdr) & m);
    } else {
        u32 m = ~(0x80000000u >> ((i - 0x20) & 31));
        W(h + 0xc4 + grp * 4, R(h + 0xc4 + grp * 4) & m);
        if (--B(h + i + 4) == 0) W(hdr + 4, R(hdr + 4) & m);
    }
}
static void sbhSet(D2SndSbh& s, u32 hdr, u32 h, u32 grp, u32 i) {
    int8_t c = B(h + i + 4);
    B(h + i + 4) = c + 1;
    if (i < 0x20) {
        u32 b = 0x80000000u >> (i & 31);
        if (c == 0) W(hdr, R(hdr) | b);
        W(h + 0x44 + grp * 4, R(h + 0x44 + grp * 4) | b);
    } else {
        u32 b = 0x80000000u >> ((i - 0x20) & 31);
        if (c == 0) W(hdr + 4, R(hdr + 4) | b);
        W(h + 0xc4 + grp * 4, R(h + 0xc4 + grp * 4) | b);
    }
}

// 1.13c D2sound+0x395E
int D2sound_395E(D2SndSbh& s, u32 region) {
    u32 h = R(region + 0x10);
    int g = 0;
    for (int32_t v = (int32_t)R(region + 8); v >= 0; v <<= 1) g++;
    u32 tab = g * 0x204 + 0x144 + h;
    u32 q = tab;
    for (int n = 0x3f; n; n--, q += 8) { W(q + 8, q); W(q + 4, q); }
    u32 la = g * 0x8000 + R(region + 0xc);
    if (!s.virtualAlloc(la, 0x8000)) return -1;
    u32 p = la + 0x10;
    for (int n = 8; n; n--, p += 0x1000) {
        W(p - 8, -1); W(p + 0x3fb * 4, -1);
        W(p, p + 0x3ff * 4); W(p - 4, 0xff0);
        W(p + 4, p - 0x401 * 4); W(p + 0x3fa * 4, 0xff0);
    }
    W(tab + 0x1fc, la + 0xc);   W(la + 0x14, tab + 0x1f8);
    W(tab + 0x200, la + 0x700c); W(la + 0x7010, tab + 0x1f8);
    W(h + 0x44 + g * 4, 0);
    W(h + 0xc4 + g * 4, 1);
    int8_t c = B(h + 0x43);
    B(h + 0x43) = c + 1;
    if (c == 0) W(region + 4, R(region + 4) | 1);
    W(region + 8, R(region + 8) & ~(0x80000000u >> (g & 31)));
    return g;
}

// 1.13c D2sound+0x3D43 -- returns the 32-bit arena address of the block, 0 on failure
u32 D2sound_3D43(D2SndSbh& s, u32 size) {
    u32 sz = (size + 0x17) & ~0xfu;
    int32_t bi = ((int32_t)(size + 0x17) >> 4) - 1;
    u32 end = s.hdrList + s.hdrCount * 0x14;
    u32 p = s.scan, m0, m1;
    if (bi < 0x20) { m0 = 0xffffffffu >> (bi & 31); m1 = ~0u; }
    else { m0 = 0; m1 = 0xffffffffu >> ((bi - 0x20) & 31); }
    for (; p < end && (R(p + 4) & m1) == 0 && (R(p) & m0) == 0; p += 0x14) {}
    u32 q = s.hdrList;
    if (p == end) {
        for (; q < s.scan && (R(q + 4) & m1) == 0 && (R(q) & m0) == 0; q += 0x14) {}
        p = q;
        if (q == s.scan) {
            for (; q < end && R(q + 8) == 0; q += 0x14) {}
            u32 q2 = s.hdrList;
            p = q;
            if (q == end) {
                for (; q2 < s.scan && R(q2 + 8) == 0; q2 += 0x14) {}
                p = q2;
                if (q2 == s.scan && (p = s.newRegion(&s)) == 0) return 0;
            }
            int g = D2sound_395E(s, p);
            W(R(p + 0x10), g);
            if ((int32_t)R(R(p + 0x10)) == -1) return 0;
        }
    }
    u32 h = R(p + 0x10);
    int32_t loc = (int32_t)R(h);
    if (loc == -1 || ((R(h + 4 * (loc + 0x31)) & m1) == 0 && (R(h + 4 * (loc + 0x11)) & m0) == 0)) {
        loc = 0;
        u32 a = h + 4 * 0x11;
        if ((R(h + 4 * 0x31) & m1) == 0 && (R(a) & m0) == 0) {
            u32 b;
            do { b = a + 0x84; loc++; a += 4; } while ((R(b) & m1) == 0 && (R(a) & m0) == 0);
        }
    }
    u32 tab = h + 4 * (loc * 0x81 + 0x51);
    int32_t bin = 0;
    u32 u = R(h + 4 * (loc + 0x11)) & m0;
    if (u == 0) { u = R(h + 4 * (loc + 0x31)) & m1; bin = 0x20; }
    for (; (int32_t)u >= 0; u <<= 1) bin++;
    u32 blk = R(tab + (bin * 2 + 1) * 4);
    int32_t rem = (int32_t)(R(blk) - sz);
    int32_t nb = (rem >> 4) - 1;
    if (nb > 0x3f) nb = 0x3f;
    s.scan = p;
    if (nb != bin) {
        if (R(blk + 4) == R(blk + 8)) sbhClr(s, p, h, loc, bin);
        W(R(blk + 8) + 4, R(blk + 4));
        W(R(blk + 4) + 8, R(blk + 8));
        if (rem != 0) {
            u32 l = tab + nb * 8;
            u32 t = R(l + 4);
            W(blk + 8, l); W(blk + 4, t); W(l + 4, blk);
            W(R(blk + 4) + 8, blk);
            if (R(blk + 4) == R(blk + 8)) sbhSet(s, p, h, loc, nb);
        }
    }
    if (rem != 0) { W(blk, rem); W(rem - 4 + blk, rem); }
    blk += rem;
    W(blk, sz + 1);
    W(blk + sz - 4, sz + 1);
    u32 cnt = R(tab);
    W(tab, cnt + 1);
    if (cnt == 0 && p == s.deferHdr && loc == s.deferInd) s.deferHdr = 0;
    W(h, loc);
    return blk + 4;
}

// 1.13c D2sound+0x5280 -- memmove (retail picks copy direction by overlap)
void* D2sound_5280(void* dst, const void* src, size_t n) { return memmove(dst, src, n); }

// 1.13c D2sound+0x358F
void D2sound_358F(D2SndSbh& s, u32 hdr, u32 blk) {
    u32 h = R(hdr + 0x10);
    u32 a = blk - 4;
    u32 grp = (blk - R(hdr + 0xc)) >> 15;
    u32 g = grp * 0x204 + 0x144 + h;
    u32 sz = R(a) - 1;
    if (sz & 1) return;
    u32 nx = sz + a, nsz = R(nx), psz = R(blk - 8);
    if ((nsz & 1) == 0) {
        u32 i = ((int32_t)nsz >> 4) - 1;
        if (i > 0x3f) i = 0x3f;
        if (R(nx + 4) == R(nx + 8)) sbhClr(s, hdr, h, grp, i);
        sz += nsz;
        W(R(nx + 8) + 4, R(nx + 4));
        W(R(nx + 4) + 8, R(nx + 8));
    }
    u32 idx = ((int32_t)sz >> 4) - 1;
    if (idx > 0x3f) idx = 0x3f;
    u32 pidx = hdr;
    if ((psz & 1) == 0) {
        a -= psz;
        pidx = ((int32_t)psz >> 4) - 1;
        if (pidx > 0x3f) pidx = 0x3f;
        sz += psz;
        idx = ((int32_t)sz >> 4) - 1;
        if (idx > 0x3f) idx = 0x3f;
        if (pidx != idx) {
            if (R(a + 4) == R(a + 8)) sbhClr(s, hdr, h, grp, pidx);
            W(R(a + 8) + 4, R(a + 4));
            W(R(a + 4) + 8, R(a + 8));
        }
    }
    if ((psz & 1) != 0 || pidx != idx) {
        u32 l = g + idx * 8;
        u32 t = R(l + 4);
        W(a + 8, l); W(a + 4, t); W(l + 4, a);
        W(R(a + 4) + 8, a);
        if (R(a + 4) == R(a + 8)) sbhSet(s, hdr, h, grp, idx);
    }
    W(a, sz);
    W(sz - 4 + a, sz);
    W(g, R(g) - 1);
    if (R(g) == 0) {
        u32 d = s.deferHdr;
        if (d != 0) {
            s.virtualFree(s.deferInd * 0x8000 + R(d + 0xc), 0x8000, 0x4000);
            W(d + 8, R(d + 8) | (0x80000000u >> (s.deferInd & 31)));
            W(R(d + 0x10) + 0xc4 + s.deferInd * 4, 0);
            B(R(d + 0x10) + 0x43)--;
            if (B(R(d + 0x10) + 0x43) == 0) W(d + 4, R(d + 4) & 0xfffffffe);
            if (R(d + 8) == 0xffffffffu) {
                s.virtualFree(R(d + 0xc), 0, 0x8000);
                s.heapFree(R(d + 0x10));
                D2sound_5280(s.base + d, s.base + d + 0x14, s.hdrCount * 0x14 - d - 0x14 + s.hdrList);
                s.hdrCount--;
                if (d < hdr) hdr -= 0x14;
                s.scan = s.hdrList;
            }
        }
        s.deferHdr = hdr;
        s.deferInd = grp;
    }
}

// 1.13c D2sound+0x2861
void* D2sound_2861(D2SndSbh& s, size_t size) {
    if (s.heapMode == 3 && size <= s.threshold) {
        s.lock(4);
        u32 a = D2sound_3D43(s, (u32)size);
        s.unlock();
        if (a) return s.base + a;
    }
    if (size == 0) size = 1;
    if (s.heapMode != 1) size = (size + 0xf) & ~(size_t)0xf;
    return s.heapAlloc(size);
}

// 1.13c D2sound+0x29F2 -- saves caller frame info (EAX, EBP, [EBP+8]) into globals 5420/541c/5424
struct D2SndFrame { u32 g541c, g5420, g5424; };
void D2sound_29F2(D2SndFrame& f, u32 eax, u32 ebp, u32 arg8) { f.g5420 = arg8; f.g541c = eax; f.g5424 = ebp; }

// 1.13c D2sound+0xDF4F -- maps FP error kind (1 = domain, 2..3 = range) to errno via errno-pointer seam
void D2sound_DF4F(int kind, int* (*errnoPtr)()) {
    if (kind == 1) *errnoPtr() = 0x21;
    else if (kind > 1 && kind < 4) *errnoPtr() = 0x22;
}

// 1.13c D2sound+0xDF8E -- __ctrlfp: returns the x87 control word (sign-extended 16 bit); seam arg
int D2sound_DF8E(uint16_t fpuControlWord) { return (int)(int16_t)fpuControlWord; }

// (superseded by d2sound_n95.cpp) D2sound 0xDFB2
void D2sound_DFB2(void) {}

// 1.13c D2sound+0xD129 -- __decomp: frexp-style split; returns mantissa in [0.5,1), *exp = exponent
double D2sound_D129(double x, int* exp) {
    uint64_t b; memcpy(&b, &x, 8);
    uint32_t hi = (uint32_t)(b >> 32);
    int e;
    if (x == 0.0) { *exp = 0; return 0.0; }
    if ((hi & 0x7ff00000) == 0) {            // denormal: normalise until bit 52 is set
        uint64_t sign = b & 0x8000000000000000ull;
        uint64_t m = b & 0x7fffffffffffffffull;
        int n = 0;
        while (!(m & 0x10000000000000ull)) { m <<= 1; n++; }
        e = -1021 - n;
        b = sign | (m & 0xffefffffffffffffull);
    } else {
        e = (int)((hi >> 20) & 0x7ff) - 0x3fe;
    }
    b = (b & 0x800fffffffffffffull) | (0x3feull << 52);   // __set_exp(x, 0)
    memcpy(&x, &b, 8);
    *exp = e;
    return x;
}

// 1.13c D2sound+0xDD2B -- __handle_exc(status bits, operand, control word); true = no exception left unhandled
bool D2sound_DD2B(uint32_t st, double* v, uint32_t cw) {
    uint32_t r = st & 0x1f;
    if ((st & 8) && (cw & 1)) { r = st & 0x17; goto tail; }
    if ((st & 4) && (cw & 4)) { r = st & 0x1b; goto tail; }
    if ((st & 1) && (cw & 8)) {
        double mx = 1.7976931348623157e308, inf = __builtin_inf();
        double x = *v;
        switch (cw & 0xc00) {
        case 0x000: *v = x <= 0 ? -inf : inf; break;
        case 0x400: *v = x <= 0 ? -inf : mx; break;
        case 0x800: *v = x <= 0 ? -mx : inf; break;
        default:    *v = x <= 0 ? -mx : mx; break;
        }
        r = st & 0x1e;
        goto tail;
    }
    if ((st & 2) && (cw & 0x10)) {
        bool sticky = (st & 0x10) != 0;
        if (*v != 0.0) {
            int e;
            double m = D2sound_D129(*v, &e);
            uint64_t mb; memcpy(&mb, &m, 8);
            e -= 0x600;
            uint64_t out;
            if (e < -0x432) { out = mb & 0x8000000000000000ull; sticky = true; }
            else {
                out = (mb & 0xfffffffffffffull) | 0x10000000000000ull;
                if (e < -0x3fd) {
                    for (int n = -0x3fd - e; n; n--) { if (out & 1) sticky = true; out >>= 1; }
                }
                if (m < 0.0) out ^= 0x8000000000000000ull;
            }
            memcpy(v, &out, 8);
        }
        r &= ~2u;
    }
tail:
    if ((st & 0x10) && (cw & 0x20)) r &= ~0x10u;
    return r == 0;
}

// 1.13c D2sound+0xDA86 -- __raise_exc: builds the exception record and raises it through seams
struct D2SndFpSeams {
    uint32_t (*statfp)(void);
    void (*clrfp)(void);
    void (*raise)(uint32_t code, uint32_t* rec);   // RaiseException(code,0,1,&rec)
};
void D2sound_DA86(uint32_t* rec, uint32_t* cwp, uint32_t flags, int op, const uint64_t* a, uint64_t* b,
                  const D2SndFpSeams& fp) {
    uint32_t code = flags, f = flags;
    rec[1] = rec[2] = rec[3] = 0;
    if (f & 0x10) { rec[1] |= 1;  code = 0xc000008f; }
    if (f & 2)    { rec[1] |= 2;  code = 0xc0000093; }
    if (f & 1)    { rec[1] |= 4;  code = 0xc0000091; }
    if (f & 4)    { rec[1] |= 8;  code = 0xc000008e; }
    if (f & 8)    { rec[1] |= 0x10; code = 0xc0000090; }
    rec[2] ^= (~(*cwp << 4) ^ rec[2]) & 0x10;
    rec[2] ^= (~(*cwp << 1) ^ rec[2]) & 8;
    rec[2] ^= (~(*cwp >> 1) ^ rec[2]) & 4;
    rec[2] ^= (~(*cwp >> 3) ^ rec[2]) & 2;
    rec[2] ^= (~(*cwp >> 5) ^ rec[2]) & 1;
    uint32_t st = fp.statfp();
    if (st & 1)    rec[3] |= 0x10;
    if (st & 4)    rec[3] |= 8;
    if (st & 8)    rec[3] |= 4;
    if (st & 0x10) rec[3] |= 2;
    if (st & 0x20) rec[3] |= 1;
    switch (*cwp & 0xc00) {
    case 0x000: rec[0] &= ~3u; break;
    case 0x400: rec[0] = (rec[0] & ~2u) | 1; break;
    case 0x800: rec[0] = (rec[0] & ~1u) | 2; break;
    case 0xc00: rec[0] |= 3; break;
    }
    switch (*cwp & 0x300) {
    case 0x000: rec[0] = (rec[0] & 0xffffffebu) | 8; break;
    case 0x200: rec[0] = (rec[0] & 0xffffffe7u) | 4; break;
    case 0x300: rec[0] &= 0xffffffe3u; break;
    }
    rec[0] ^= (((uint32_t)op << 5) ^ rec[0]) & 0x1ffe0;
    rec[8] |= 1; rec[8] = (rec[8] & 0xffffffe3u) | 2;
    memcpy(rec + 4, a, 8);
    rec[0x18] |= 1; rec[0x18] = (rec[0x18] & 0xffffffe3u) | 2;
    memcpy(rec + 0x14, b, 8);
    fp.clrfp();
    fp.raise(code, rec);
    if (rec[2] & 0x10) *cwp &= ~1u;
    if (rec[2] & 8)    *cwp &= ~4u;
    if (rec[2] & 4)    *cwp &= ~8u;
    if (rec[2] & 2)    *cwp &= ~0x10u;
    if (rec[2] & 1)    *cwp &= ~0x20u;
    switch (rec[0] & 3) {
    case 0: *cwp &= 0xfffff3ffu; break;
    case 1: *cwp = (*cwp & 0xfffff7ffu) | 0x400; break;
    case 2: *cwp = (*cwp & 0xfffffbffu) | 0x800; break;
    case 3: *cwp |= 0xc00; break;
    }
    switch ((rec[0] >> 2) & 7) {
    case 0: *cwp = (*cwp & 0xfffff3ffu) | 0x300; break;
    case 1: *cwp = (*cwp & 0xfffff3ffu) | 0x200; break;
    case 2: *cwp &= 0xfffff3ffu; break;
    }
    memcpy(b, rec + 0x14, 8);
}
