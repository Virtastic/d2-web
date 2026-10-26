// Storm STRANS.CPP transparency-mask helpers (batch z13). Retail globals/callees are seam params.
// Records are uint32_t[]; 32-bit links are uint32_t. Register args (EAX/ECX/EBX/ESI) are explicit params.
#include <cstdint>
#include <cstring>
typedef uint32_t u32;
typedef int32_t i32;
typedef uint8_t u8;
struct StormZ13Seam {
    u32* (*newRec)(void* pool, int, int);          // Storm+0x14A0 on &DAT_6fc458b0
    void* pool;
    void* (*alloc)(u32 size, const char* file, int line, int);  // Storm+0x1C580
    void (*free_)(void* p, const char* file, int line);         // Storm+0x1B960
    void (*setLastError)(u32);
    u32* lastErr;                                   // DAT_6fc45388
    void** cacheBuf;                                // DAT_6fc45364
    u32* cacheCap;                                  // DAT_6fc45368
    void* rowTable;                                 // DAT_6fc45360 (int* per-row base table)
    u32 shiftRow, mask, shiftCol, bits;             // DAT_6fc415f0/415f4/415e8/415ec
    void (*blit)(i32* rc, u32 a, int b, u32 c, char d, int e, char* out, u32* sz, u32* x); // Storm+0x36A50
};
static const char* kStrans = "..\\3rdParty\\STORM\\SOURCE\\STRANS.CPP";
static void z13err(const StormZ13Seam& s) { *s.lastErr = 0x57; s.setLastError(0x57); }

// 1.13c Storm+0x36F00: new record, copy fields 3..11 from src (EAX).
u32* Storm_Z13_36F00(const StormZ13Seam& s, const u32* src) {
    u32* r = s.newRec(s.pool, 0, 0);
    if (src) for (int i = 3; i <= 11; i++) r[i] = src[i];
    return r;
}
// 1.13c Storm+0x36F60: grow buf (EBX={ptr,cap,len,grow}) so len+need fits; fix up two interior pointers.
void Storm_Z13_36F60(const StormZ13Seam& s, u32 need, u32* buf, u32* p1, u32* p2) {
    u32 cap = buf[1], n = cap;
    if (cap < buf[2] + need) do n += buf[3]; while (n < need + buf[2]);
    if (n == cap) return;
    u8* nb = (u8*)s.alloc(n, kStrans, 0x4f, 0);
    memcpy(nb, (void*)(uintptr_t)buf[0], buf[2]);
    s.free_((void*)(uintptr_t)buf[0], kStrans, 0x51);
    if (p1 && *p1) *p1 = (*p1 - buf[0]) + (u32)(uintptr_t)nb;
    if (p2 && *p2) *p2 = (*p2 - buf[0]) + (u32)(uintptr_t)nb;
    buf[1] = n; buf[0] = (u32)(uintptr_t)nb;
}
// 1.13c Storm+0x37010: init buf (ESI) reusing the cached scratch buffer if any.
void Storm_Z13_37010(const StormZ13Seam& s, u32* buf) {
    void* c = *s.cacheBuf;
    buf[3] = 0x1000; buf[2] = 0;
    if (c) { buf[0] = (u32)(uintptr_t)c; buf[1] = *s.cacheCap; *s.cacheBuf = 0; *s.cacheCap = 0; return; }
    buf[1] = 0x1000;
    buf[0] = (u32)(uintptr_t)s.alloc(0x1000, kStrans, 0x42, 0);
}
// 1.13c Storm+0x37060 (ordinal 443): combine two masks. GUESSED: type-propagation warning; transcribed literally.
u32 Storm_Z13_37060(const StormZ13Seam& s, u32* a, u32* b, i32 p3, i32 p4, u32 p5, u32** out) {
    if (out) *out = 0;
    u8* cur = (u8*)*s.cacheBuf;
    if (!a || !b || !out) { z13err(s); return 0; }
    u32 f0 = p5 & 1, f1 = (p5 >> 1) & 1;
    u32 L[12];
    u32 *p7 = L + (f1 != 0) + 4, *p11 = L + (f1 == 0) + 4, *p10;
    for (int i = 0; i < 2; i++) {
        int j = 0; p10 = p11;
        do {
            bool c;
            if (f0 == 0) { *p7 = (j == 0); c = (i == j); } else { *p7 = (i == j); c = (j == 1); }
            j++; p7 += 2; p11 = p10 + 2; *p10 = c; p10 = p11;
        } while (j < 2);
    }
    const u8* pa = (const u8*)(uintptr_t)(a[2] + a[5]);
    const u8* pb = (const u8*)(uintptr_t)(b[5] + b[2]);
    for (i32 k = p4; k < 0; k++) { int16_t t; do { t = *(const int16_t*)pb; pb += 2; } while (t != 0); }
    u32 len = 0, cap;
    if (!*s.cacheBuf) { cap = 0x1000; cur = (u8*)s.alloc(0x1000, kStrans, 0x42, 0); }
    else { cap = *s.cacheCap; *s.cacheBuf = 0; *s.cacheCap = 0; }
    u8* base = cur; u8* w = cur;
    for (i32 row = 0; row < (i32)a[7]; row++) {
        u32 need = a[6] * 2 + 2 + len;
        if (cap < need) {
            need = (((need - cap) - 1 >> 12) + 1) * 0x1000 + cap;
            if (need != cap) {
                u8* nb = (u8*)s.alloc(need, kStrans, 0x4f, 0);
                memcpy(nb, base, len);
                s.free_(base, kStrans, 0x51);
                cap = need;
                if (w) w = w + (nb - base);
                base = nb;
            }
        }
        if (row < p4 || (i32)b[7] - 1 + p4 < row) {
            if (f0 == f1) {
                int16_t t;
                do { t = *(const int16_t*)pa; *(int16_t*)w = t; len += 2; w += 2; pa += 2; } while (t != 0);
            } else {
                for (i32 n = a[6]; n != 0;) { i32 st = n < 0xfd ? n : 0xfc; w[0] = 0; w[1] = (u8)st; len += 2; w += 2; n -= st; }
                *(int16_t*)w = 0; w += 2; len += 2;
                int16_t t; do { t = *(const int16_t*)pa; pa += 2; } while (t != 0);
            }
        } else {
            u32 c6 = 0; L[0] = L[1] = L[2] = 0;
            bool done = false;
            u32 c8 = ((i32)p3 < 0 ? 0u : 0xffffffffu) & (u32)p3;
            L[3] = c8;
            if (p3 < 0) {
                u32 rem = (u32)-p3;
                while (L[2] = c6, L[3] = c8, rem != 0) {
                    c6 = pb[0]; u8 hi = pb[1]; pb += 2; L[2] = c6; L[3] = hi;
                    if (c6 == 0 && hi == 0) { c8 = 0x7fffffff; L[3] = c8; done = true; break; }
                    for (int j = 0; j < 2; j++) {
                        c6 = L[j + 2]; c8 = rem;
                        if ((i32)c6 <= (i32)rem) c8 = c6;
                        rem -= c8; L[j + 2] = c6 - c8;
                        c6 = L[2]; c8 = L[3];
                    }
                }
            }
            for (;;) {
                u8 x = pa[0], y = pa[1]; pa += 2; L[0] = x; L[1] = y;
                if (x == 0 && y == 0) break;
                u32* p68 = L; u32* p60 = L;
                for (int m = 2; m != 0; m--) {
                    p60 += 4;
                    u32 v = *p68;
                    while (v != 0) {
                        if (c6 == 0 && c8 == 0) {
                            c6 = pb[0]; c8 = pb[1]; pb += 2; L[2] = c6; L[3] = c8;
                            if (c6 == 0 && c8 == 0) { c8 = 0x7fffffff; L[3] = c8; done = true; }
                        }
                        p5 &= 0xffff0000u;
                        u32* p64 = p60;
                        for (int j = 0; j < 2; j++, p64 += 2) {
                            v = 0;
                            if (*p64 != 0) v = c6;
                            if (p64[1] != 0 && (*p64 != 0 || c6 == 0)) v += c8;
                            c6 = *p68;
                            if ((i32)c6 <= (i32)v) v = c6;
                            ((u8*)&p5)[j] = (u8)v;
                            *p68 = c6 - v;
                            for (int q = 0; q < 2; q++) {
                                c6 = L[q + 2]; c8 = v;
                                if ((i32)c6 <= (i32)v) c8 = c6;
                                L[q + 2] = c6 - c8; v -= c8;
                            }
                            c6 = L[2]; c8 = L[3];
                        }
                        if ((int16_t)p5 != 0) { *(int16_t*)w = (int16_t)p5; w += 2; len += 2; }
                        v = *p68;
                    }
                    p68++;
                }
            }
            *(int16_t*)w = 0; w += 2; len += 2;
            if (!done) { int16_t t; do { t = *(const int16_t*)pb; pb += 2; } while (t != 0); }
        }
    }
    u32* r = s.newRec(s.pool, 0, 0);
    for (int i = 3; i <= 11; i++) r[i] = a[i];
    r[2] = (u32)(uintptr_t)base; r[3] = cap; r[4] = len; r[5] = 0;
    *out = r;
    return 1;
}
// 1.13c Storm+0x37620 (ordinal 437). GUESSED: carry-over of local_14 / local_8 init and the 0x37750 row-copy
// shortcut transcribed literally; bit-run encoder on rowTable.
u32 Storm_Z13_37620(const StormZ13Seam& s, u32* a, int base, u8 bit, u32** out) {
    if (out) *out = 0;
    if (!a || !base || !bit || !out) { z13err(s); return 0; }
    if (!s.rowTable) return 0;
    u32 buf[4];
    Storm_Z13_37010(s, buf);
    u8* src = (u8*)(uintptr_t)(a[5] + a[2]);
    u8* prevOut = (u8*)(uintptr_t)buf[0];   // local_20
    u8* prevOut2 = prevOut;                 // local_1c
    u32 row = 0;
    u8* prevSrc = src;                      // pbVar11
    u32 len = buf[2];                       // local_8
    u32 bufRef[1];
    (void)bufRef;
    u8* p10 = (u8*)(uintptr_t)buf[0];
    u8* l20 = prevOut; u8* l1c = prevOut2; u8* l24 = src;
    u8* l14 = prevSrc;
    if (0 < (i32)a[7]) {
        do {
            // grow, with fix-up of l20/l1c, which alias buf[0] (EBX frame)
            u32 p1 = (u32)(uintptr_t)l20, p2 = (u32)(uintptr_t)l1c;
            Storm_Z13_36F60(s, a[6] * 2 + 2, buf, &p1, &p2);
            l20 = (u8*)(uintptr_t)p1; l1c = (u8*)(uintptr_t)p2;
            p10 = (u8*)(uintptr_t)buf[0];
            u8* outp; u8* ps;
            bool same = ((s.mask - 1) & row) == 0 ? false : (*(i32*)l24 == *(i32*)prevSrc);
            if (same) {
                i32 rem = (i32)(l24 - prevSrc) - 4;
                const u8 *x = l24 + 4, *y = prevSrc + 4; bool eq = true;
                do { if (rem == 0) break; rem--; eq = *x == *y; x++; y++; } while (eq);
                same = eq;
            }
            if (!same) {
                u32 col = 0; ps = l24; outp = l20;
                u8* tb = (u8*)(uintptr_t)(((u32*)s.rowTable)[(i32)row >> s.shiftRow] + base);
                u8 x, y;
                do {
                    x = ps[0]; y = ps[1]; ps += 2;
                    if (x == 0) {
                        outp[0] = 0; outp[1] = y; outp += 2; len += 2;
                    } else {
                        u8 run = 0; bool st = true, tog = true, emitted = false; u8 rem = x;
                        for (;;) {
                            bool brk = false;
                            while (rem != 0) {
                                if (((*tb & bit) != 0) != st) { brk = true; break; }
                                if (rem < s.bits - col) { col += rem; run += rem; rem = 0; }
                                else { u8 c = (u8)(s.bits - col); run += c; rem -= c; col = 0; tb++; }
                            }
                            if (!brk && run == 0) break;
                            *outp++ = run; len++; run = 0; st = !tog; tog = st;
                        }
                        emitted = true; (void)emitted;
                        if (y != 0) { if (st) { *outp++ = 0; len++; } *outp++ = y; len++; }
                        else if (!st) { *outp++ = y; len++; }
                        if (y == 0) { /* fallthrough */ }
                    }
                    if (x == 0 && y != 0) { tb += (col + y) >> s.shiftCol; col = (col + y) & (s.bits - 1); }
                } while (x != 0 || y != 0);
                l14 = l24;
            } else {
                u32 n = (u32)(l20 - l1c);
                outp = l20;
                memcpy(outp, l1c, n);
                l14 = l24; len += n;
                ps = l24 + (l24 - prevSrc);
                outp = l20 + n;
            }
            l20 = outp; l24 = ps; l1c = (u8*)(uintptr_t)buf[0]; row++;
            p10 = (u8*)(uintptr_t)buf[0];
            prevSrc = l14;
        } while ((i32)row < (i32)a[7]);
    }
    u32* r = Storm_Z13_36F00(s, a);
    r[3] = buf[1]; r[2] = (u32)(uintptr_t)p10; r[4] = len; r[5] = 0;
    *out = r;
    return 1;
}
// 1.13c Storm+0x378E0 (ordinal 438): strip empty (0,0) pairs per row into a fresh buffer.
u32 Storm_Z13_378E0(const StormZ13Seam& s, u32* a, u32** out) {
    if (out) *out = 0;
    if (!a || !out) { z13err(s); return 0; }
    u32 sz = (a[4] + a[7] * 2) - a[5];
    u8* buf = (u8*)s.alloc(sz, kStrans, 0x32d, 0);
    const char* p = (const char*)(uintptr_t)(a[2] + a[5]);
    u8* w = buf; i32 n = 0;
    for (i32 rows = a[7]; rows != 0; rows--) {
        char lo = 0, hi;
        do {
            hi = *p;
            if (lo != 0 || hi != 0) { w[0] = lo; w[1] = hi; w += 2; n += 2; }
            lo = p[1]; p += 2;
        } while (lo != 0 || hi != 0);
        w[0] = w[1] = 0; w += 2; n += 2;
    }
    u32* r = Storm_Z13_36F00(s, a);
    r[4] = n; r[3] = sz; r[2] = (u32)(uintptr_t)buf; r[5] = 0;
    *out = r;
    return 1;
}
// 1.13c Storm+0x379D0 (ordinal 436): duplicate a record's data.
u32 Storm_Z13_379D0(const StormZ13Seam& s, u32* a, u32** out) {
    if (out) *out = 0;
    if (!a || !out) { z13err(s); return 0; }
    void* d = s.alloc(a[3], kStrans, 0x315, 0);
    memcpy(d, (void*)(uintptr_t)a[2], a[4]);
    u32* r = Storm_Z13_36F00(s, a);
    r[2] = (u32)(uintptr_t)d;
    *out = r;
    return 1;
}
// 1.13c Storm+0x37A60: build a mask record. eax8 = in_EAX (must be 8), flags = ECX, rc = ESI (i32[4] or null).
u32 Storm_Z13_37A60(const StormZ13Seam& s, u32 flags, i32 eax8, i32* rc, i32 p1, u32 p2, u32 p3, i32 p4, u32** p5) {
    if (eax8 != 8) return 0;
    char c4 = (flags & 0x1000000) ? (char)flags : 0;
    u32 L[6] = {0, 0, 0, 0, 0, 0};   // local_18,local_14,local_10..local_4
    s.blit(rc, p2, p1, p3, c4, p4, nullptr, &L[0], &L[1]);
    u32 sz = L[0];
    char* buf = (char*)s.alloc(sz, kStrans, 0x115, 0);
    s.blit(rc, p2, p1, p3, c4, p4, buf, nullptr, nullptr);
    u32* r = s.newRec(s.pool, 0, 0);
    r[8] = L[2]; r[9] = L[3]; r[10] = L[4]; r[11] = L[5];
    r[2] = (u32)(uintptr_t)buf; r[3] = sz; r[4] = sz; r[5] = L[1];
    if (rc) p2 = rc[2] - rc[0];
    r[6] = p2;
    r[7] = rc ? (u32)(rc[3] - rc[1]) : p3;
    *p5 = r;
    return 1;
}
static u32 z13_37A60Wrap(const StormZ13Seam& s, i32 p1, u32 p2, u32 p3, i32 rcmode, i32* rc, u32 flags, u32** out, i32 p4) {
    if (out) *out = 0;
    if (!p1 || !p2 || !p3 || !out) { z13err(s); return 0; }
    return Storm_Z13_37A60(s, flags, 8, rc, p1, p2, p3, p4, out);
}
// 1.13c Storm+0x37B90 (ordinal 446)
u32 Storm_Z13_37B90(const StormZ13Seam& s, i32 p1, u32 p2, u32 p3, u32, i32* rc, u32 flags, u32** out) { return z13_37A60Wrap(s, p1, p2, p3, 0, rc, flags, out, 1); }
// 1.13c Storm+0x37C10 (ordinal 445)
u32 Storm_Z13_37C10(const StormZ13Seam& s, i32 p1, u32 p2, u32 p3, u32, i32* rc, u32 flags, u32** out) { return z13_37A60Wrap(s, p1, p2, p3, 0, rc, flags, out, 0); }
// 1.13c Storm+0x37C90 (ordinal 444): inclusive rect (p5) -> exclusive, then 37B90.
void Storm_Z13_37C90(const StormZ13Seam& s, i32 p1, u32 p2, u32 p3, u32 p4, const i32* r5, u32 flags, u32** out) {
    i32 rc[4]; i32* pr = nullptr;
    if (r5) { rc[0] = r5[0]; rc[1] = r5[1]; rc[2] = r5[2] + 1; rc[3] = r5[3] + 1; pr = rc; }
    Storm_Z13_37B90(s, p1, p2, p3, p4, pr, flags, out);
}
// 1.13c Storm+0x37CF0 (ordinal 433): as 37C90 but via 37C10.
void Storm_Z13_37CF0(const StormZ13Seam& s, i32 p1, u32 p2, u32 p3, u32 p4, const i32* r5, u32 flags, u32** out) {
    i32 rc[4]; i32* pr = nullptr;
    if (r5) { rc[0] = r5[0]; rc[1] = r5[1]; rc[2] = r5[2] + 1; rc[3] = r5[3] + 1; pr = rc; }
    Storm_Z13_37C10(s, p1, p2, p3, p4, pr, flags, out);
}
