// n59 batch: D2sound CRT float/ldtoa helpers (80-bit "12-byte" long-double arithmetic) and the 0x8830 curve.
// Host seams (DAT tables, ctype, decimal point, x87 status) are passed as arguments.
// The 12-byte value is 3 little-endian uint32: mantissa in bytes 0..9 (bytes 0-1 guard), sign|exp15 in bytes 10-11.
#include <cstdint>
#include <cstring>
#include <cmath>

typedef uint32_t u32;
typedef uint16_t u16;

// internal (n45 owns) D2SOUND+0x0D1E5
int n59_addl(u32 a, u32 b, u32* out) {
    u32 s = a + b;
    int c = (s < a || s < b) ? 1 : 0;
    *out = s;
    return c;
}

// internal (n45 owns) D2SOUND+0x0D206
void n59_add_12(u32* a, const u32* b) {
    if (n59_addl(a[0], b[0], &a[0])) {
        if (n59_addl(a[1], 1, &a[1])) a[2]++;
    }
    if (n59_addl(a[1], b[1], &a[1])) a[2]++;
    n59_addl(a[2], b[2], &a[2]);
}

// internal (n45 owns) D2SOUND+0x0D264
void n59_shl_12(u32* p) {
    u32 a = p[0], b = p[1];
    p[0] = a << 1;
    p[1] = (b << 1) | (a >> 31);
    p[2] = (p[2] << 1) | (b >> 31);
}

// internal (n45 owns) D2SOUND+0x0D292
void n59_shr_12(u32* p) {
    u32 b = p[1];
    p[1] = (b >> 1) | (p[2] << 31);
    p[2] >>= 1;
    p[0] = (p[0] >> 1) | (b << 31);
}

// internal (n45 owns) D2SOUND+0x0DF77  __statfp: x87 status word is a host seam (none in wasm)
int n59_statfp(uint16_t fpsw) { return (int)(int16_t)fpsw; }

// internal (n45 owns) D2SOUND+0x0DF82  __clrfp: returns the status word then clears it
int n59_clrfp(uint16_t* fpsw) { int r = (int)(int16_t)*fpsw; *fpsw = 0; return r; }

// 1.13c D2SOUND+0x05B4A  _isdigit: ctype[c] & _DIGIT(4); mb_cur_max>1 path goes through isctype seam
int n59_isdigit(int c, int mb_cur_max, const uint16_t* ctype, int (*isctype_mt)(int c, int mask)) {
    if (mb_cur_max > 1) return isctype_mt(c, 4);
    return ctype[c] & 4;
}

// internal (n45 owns) D2SOUND+0x0D2BF  decimal digit string -> 12-byte value (exponent base 0x404E, normalised)
void n59_digits_to_ld12(const char* s, int n, u32* out) {
    int16_t e = 0x404e;
    out[0] = out[1] = out[2] = 0;
    for (int i = 0; i < n; i++) {
        u32 t[3] = {out[0], out[1], out[2]};
        n59_shl_12(out); n59_shl_12(out);
        n59_add_12(out, t);
        n59_shl_12(out);
        u32 d[3] = {(u32)(uint8_t)s[i], 0, 0};
        n59_add_12(out, d);
    }
    if (out[2] == 0) {
        u32 hi;
        do {
            e -= 0x10;
            hi = out[1] >> 16;
            out[1] = (out[0] >> 16) | (out[1] << 16);
            out[0] <<= 16;
        } while (hi == 0);
        out[2] = hi;
    }
    while ((out[2] & 0x8000) == 0) { n59_shl_12(out); e--; }
    u16 ee = (u16)e;
    std::memcpy((uint8_t*)out + 10, &ee, 2);
}

// 1.13c D2SOUND+0x0E008  12-byte multiply: a *= b
void n59_mul_12(u32* a, const u32* b) {
    const u16* A = (const u16*)a; const u16* B = (const u16*)b;
    u16 ea = A[5] & 0x7fff, eb = B[5] & 0x7fff;
    u16 sign = (A[5] ^ B[5]) & 0x8000;
    u16 e = ea + eb;
    u16 res_e;
    if (!(ea < 0x7fff && eb < 0x7fff && e < 0xbffe)) goto overflow;
    if (e < 0x3fc0) goto zero;
    if (ea == 0) {
        e++; res_e = 0;
        if (!((a[2] & 0x7fffffffu) || a[1] || a[0])) { ((u16*)a)[5] = res_e; return; }
    }
    if (eb == 0 && (e++, (b[2] & 0x7fffffffu) == 0 && !b[1] && !b[0])) goto zero;
    {
        u16 L[8] = {0}; bool sticky = false;
        for (int i = 0; i < 5; i++)
            for (int j = 0; j < 5 - i; j++) {
                u32 cur = L[i] | ((u32)L[i + 1] << 16);
                u32 add = (u32)B[4 - j] * (u32)A[i + j];
                u32 s = cur + add;
                if (s < cur || s < add) L[i + 2]++;
                L[i] = (u16)s; L[i + 1] = (u16)(s >> 16);
            }
        u32* T = (u32*)L;  // limbs 0..5 = 12 bytes
        res_e = (u16)(e + 0xc002);
        if ((int16_t)res_e >= 1) {
            do {
                if (L[5] & 0x8000) break;
                n59_shl_12(T); res_e--;
            } while ((int16_t)res_e > 0);
        }
        if ((int16_t)res_e < 1) {
            res_e--;
            if ((int16_t)res_e < 0) {
                u32 n = (u16)-(int16_t)res_e; res_e = 0;
                do { if (L[0] & 1) sticky = true; n59_shr_12(T); } while (--n);
                if (sticky) L[0] |= 1;
            }
        }
        if (L[0] > 0x8000 || (L[0] == 0x8000 && (L[1] & 1))) {
            u32 lo = L[1] | ((u32)L[2] << 16), hi = L[3] | ((u32)L[4] << 16);
            u16 top = L[5];
            if (lo == 0xffffffffu) {
                lo = 0;
                if (hi == 0xffffffffu) {
                    hi = 0;
                    if (top == 0xffff) { res_e++; top = 0x8000; } else top++;
                } else hi++;
            } else lo++;
            L[1] = (u16)lo; L[2] = (u16)(lo >> 16); L[3] = (u16)hi; L[4] = (u16)(hi >> 16); L[5] = top;
        }
        if (res_e > 0x7ffe) goto overflow;
        std::memcpy((uint8_t*)a, (uint8_t*)L + 2, 10);
        res_e |= sign;
        ((u16*)a)[5] = res_e;
        return;
    }
zero:
    a[0] = a[1] = a[2] = 0;
    return;
overflow:
    a[0] = 0; a[1] = 0;
    a[2] = (sign ? 0x80000000u : 0) + 0x7fff8000u;
}

// internal (n45 owns) D2SOUND+0x0E23E  scale a by 10^e using retail's power tables (host seam: tab_pos=0x6f9c5cd8, tab_neg=0x6f9c5e38)
void n59_scale10(u32* a, int e, int keep_low, const uint8_t* tab_pos, const uint8_t* tab_neg) {
    if (e == 0) return;
    const uint8_t* t = tab_pos;
    u32 n = (u32)e;
    if (e < 0) { n = (u32)-e; t = tab_neg; }
    if (!keep_low) { u16 z = 0; std::memcpy(a, &z, 2); }
    while (n) {
        u32 r = n & 7;
        n = (u32)((int)n >> 3);
        t += 0x54;
        if (r) {
            const uint8_t* ent = t + r * 12;
            uint8_t tmp[12];
            u16 lo; std::memcpy(&lo, ent, 2);
            if (lo > 0x7fff) {
                std::memcpy(tmp, ent, 12);
                u32 m; std::memcpy(&m, tmp + 2, 4); m--; std::memcpy(tmp + 2, &m, 4);
                ent = tmp;
            }
            u32 b[3]; std::memcpy(b, ent, 12);
            n59_mul_12(a, b);
        }
    }
}

// 1.13c D2SOUND+0x0D3A7  __strgtold12: string -> 12-byte long double; returns flags (0 ok,1 underflow,2 overflow,4 none)
// Mirrors the retail state machine (states 0..0xB) label for label.
int n59_strgtold12(uint8_t* out12, const uint8_t** endp, const uint8_t* s, int mulflag, int a5, int a6, int a7,
                   uint8_t dp, int (*isdigit_)(int), const uint8_t* tab_pos, const uint8_t* tab_neg) {
    int state = 0, sgnexp = 1, expv = 0, dexp = 0, ret = 0;
    u16 sign = 0; u32 nd = 0;
    bool anyd = false, fracd = false, expd = false;
    char dig[32] = {0}; char* p = dig;
    const uint8_t* save = s;
    uint8_t c; const uint8_t* nx;
    while (*s == ' ' || *s == 9 || *s == 10 || *s == 13) s++;
top:
    c = *s; nx = s + 1;
    switch (state) {
    case 0:
        if (c > '0' && c < ':') goto L427;
        if (c == dp) goto L436;
        state = 2;
        if (c == '+') { sign = 0; s = nx; goto top; }
        if (c == '-') { sign = 0x8000; s = nx; goto top; }
        break;
    case 1:
        state = 1; anyd = true;
        if (c > '0' && c < ':') goto L427;
        if (c == dp) goto L485;
        if (c == '+' || c == '-') goto L4c6;
        s = nx;
        if (c != '0') goto L49f;
        goto top;
    case 2:
        if (c > '0' && c < ':') goto L427;
        s = save;
        if (c == dp) goto L436;
        break;
    case 3:
        while (isdigit_(c)) {
            if (nd < 0x19) { nd++; *p++ = (char)(c - '0'); } else dexp++;
            c = *nx++;
        }
        if (c != dp) goto L575;
        goto L485;
    case 4:
        fracd = true;
        if (nd == 0) while (c == '0') { dexp--; c = *nx++; }
        while (isdigit_(c)) {
            if (nd < 0x19) { nd++; *p++ = (char)(c - '0'); dexp--; }
            c = *nx++;
        }
        goto L575;
    case 5:
        fracd = true;
        s = save;
        if (isdigit_(c)) { state = 4; goto L646; }
        goto L603;
    case 6:
        save = s - 1;
        if (c < '1' || c > '9') {
            if (c == '+') goto L67b;
            if (c == '-') goto L66f;
            s = save;
            if (c == '0') goto L5dd;
            goto L603;
        }
        goto L644;
    case 7:
        if (c > '0' && c < ':') goto L644;
        s = save;
        if (c != '0') goto L603;
        goto L5dd;
    case 8:
        expd = true;
        while (c == '0') c = *nx++;
        if (c > '0' && c < ':') goto L644;
        goto L5ff;
    case 9:
        expd = true; expv = 0;
        goto L6c2;
    default:
        goto Ld;
    case 0xb:
        if (a7 != 0) {
            save = s;
            if (c == '+') goto L67b;
            if (c != '-') goto L603;
            goto L66f;
        }
        state = 10; nx = s;
    Ld:
        s = nx;
        if (state != 10) goto top;
        goto L603;
    }
    if (c != '0') goto L603;
    state = 1; s = nx; goto top;
L427: state = 3; goto L646;
L436: state = 5; s = nx; goto top;
L485: anyd = true; state = 4; s = nx; goto top;
L575: if (c == '+' || c == '-') goto L4c6;
L49f: anyd = true;
    if (c < 'D' || (c > 'E' && (c < 'd' || c > 'e'))) goto L5ff;
    state = 6; s = nx; goto top;
L4c6: anyd = true; state = 0xb; s = nx - 1; goto top;
L644: state = 9;
L646: s = nx - 1; goto top;
L5dd: state = 8; s = nx; goto top;
L67b: state = 7; s = nx; goto top;
L66f: sgnexp = -1; state = 7; s = nx; goto top;
L5ff: s = nx - 1; goto L603;
L6c2:
    while (isdigit_(c)) {
        expv = (char)c - '0' + expv * 10;
        if (expv > 0x1450) { expv = 0x1451; break; }
        c = *nx++;
    }
    while (isdigit_(c)) c = *nx++;
    s = nx - 1;
L603:
    *endp = s;
    u32 m[3] = {0, 0, 0};
    if (!anyd) { ret = 4; goto zero; }
    if (nd > 0x18) {
        if (dig[23] > 4) dig[23]++;
        p--; dexp++; nd = 0x18;
    }
    if (nd != 0) {
        while (*--p == 0) { nd--; dexp++; }
        n59_digits_to_ld12(dig, (int)nd, m);
        if (sgnexp < 0) expv = -expv;
        int e = expv + dexp;
        if (!expd) e += a5;
        if (!fracd) e -= a6;
        if (e > 0x1450) {
            std::memset(out12, 0, 12);
            u32 mant = 0x80000000u; std::memcpy(out12 + 6, &mant, 4);
            u16 ee = 0x7fff | sign; std::memcpy(out12 + 10, &ee, 2);
            return 2;
        }
        if (e > -0x1451) {
            n59_scale10(m, e, mulflag, tab_pos, tab_neg);
            std::memcpy(out12, m, 12);
            u16 ee; std::memcpy(&ee, out12 + 10, 2); ee |= sign; std::memcpy(out12 + 10, &ee, 2);
            return 0;
        }
        ret = 1;
    }
zero:
    std::memset(out12, 0, 12);
    { u16 ee = sign; std::memcpy(out12 + 10, &ee, 2); }
    return ret;
}

// 1.13c D2SOUND+0x0D7E7  12-byte value -> decimal digits (_fltout core). out: i16 decpt, +2 sign, +3 len, +4 digits.
int n59_ld12_digits(u32 lo, u32 mid, u16 se, int ndigits, uint8_t flags, uint8_t* out,
                    const uint8_t* tab_pos, const uint8_t* tab_neg) {
    static const uint8_t tenth[12] = {0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xfb,0x3f};
    u32 ex = se & 0x7fff;
    out[2] = (se & 0x8000) ? '-' : ' ';
    auto setzero = [&]() {
        int16_t z = 0; std::memcpy(out, &z, 2);
        out[2] = ' '; out[3] = 1; out[5] = 0;
    };
    if ((int16_t)ex == 0 && mid == 0 && lo == 0) { out[4] = '0'; setzero(); return 1; }
    if ((int16_t)ex == 0x7fff) {
        int16_t one = 1; std::memcpy(out, &one, 2);
        const char* str; uint8_t len = 5;
        if ((mid == 0x80000000u && lo == 0) || (mid & 0x40000000u)) {
            if (!(se & 0x8000) || mid != 0xc0000000u) {
                if (mid != 0x80000000u || lo != 0) { str = "1#QNAN"; len = 6; } else str = "1#INF";
            } else if (lo != 0) { str = "1#QNAN"; len = 6; } else str = "1#IND";
        } else { str = "1#SNAN"; len = 6; }
        std::strcpy((char*)out + 4, str);
        out[3] = len;
        return 0;
    }
    uint8_t T[12] = {0};
    std::memcpy(T + 2, &lo, 4); std::memcpy(T + 6, &mid, 4);
    T[10] = (uint8_t)ex; T[11] = (uint8_t)(ex >> 8);
    int16_t dec = (int16_t)(((((ex >> 8) + (mid >> 24) * 2) * 0x4d) + (u32)-0x134312f4 + ex * 0x4d10) >> 16);
    n59_scale10((u32*)T, -(int)dec, 1, tab_pos, tab_neg);
    u16 te; std::memcpy(&te, T + 10, 2);
    if (te > 0x3ffe) { dec++; u32 t10[3]; std::memcpy(t10, tenth, 12); n59_mul_12((u32*)T, t10); }
    std::memcpy(out, &dec, 2);
    if ((flags & 1) && (ndigits += dec, ndigits < 1)) { out[4] = '0'; setzero(); return 1; }
    if (ndigits > 0x15) ndigits = 0x15;
    std::memcpy(&te, T + 10, 2);
    int sh = (int)te - 0x3ffe;
    T[10] = T[11] = 0;
    for (int i = 0; i < 8; i++) n59_shl_12((u32*)T);
    if (sh < 0) for (u32 n = (u32)-sh & 0xff; n; n--) n59_shr_12((u32*)T);
    uint8_t* d = out + 4; uint8_t* p = d;
    for (int n = ndigits + 1; n > 0; n--) {
        u32 c[3]; std::memcpy(c, T, 12);
        n59_shl_12((u32*)T); n59_shl_12((u32*)T);
        n59_add_12((u32*)T, c);
        n59_shl_12((u32*)T);
        *p++ = (uint8_t)(T[11] + '0');
        T[11] = 0;
    }
    uint8_t* q = p - 2;
    if (p[-1] < '5') {
        while (d <= q && *q == '0') q--;
        if (q < d) { *d = '0'; setzero(); return 1; }
    } else {
        while (d <= q && *q == '9') *q-- = '0';
        if (q < d) { q++; int16_t v; std::memcpy(&v, out, 2); v++; std::memcpy(out, &v, 2); }
        (*q)++;
    }
    out[3] = (uint8_t)(q - out - 3);
    out[(uint8_t)(q - out - 3) + 4] = 0;
    return 1;
}

// 1.13c D2SOUND+0x0B0FD  __cfltcvt dispatch: 'e'/'E' -> 0xAEEB, 'f' -> 0xAFF7, else -> 0xB05E (host seams)
int n59_cfltcvt(int fmtch, void* arg, char* buf, int fmt, int prec,
                int (*efmt)(void*, char*, int, int), char* (*ffmt)(void*, char*, int), int (*gfmt)(void*, char*, int, int)) {
    if (fmtch == 0x65 || fmtch == 0x45) return efmt(arg, buf, fmt, prec);
    if (fmtch == 0x66) return (int)(intptr_t)ffmt(arg, buf, fmt);
    return gfmt(arg, buf, fmt, prec);
}

// 1.13c D2SOUND+0x08830  distance curve: near/far are the fld results of retail's early exits (ST0 not in decompile)
double n59_curve_8830(float a, float b, float c, float bias, float k0988, double nearv, double farv) {
    if (a + bias >= c) return nearv;
    if (!((b - bias) < c || (b - bias) == c)) return farv;
    double div = ((double)c != (double)k0988) ? (double)c : (double)b;
    return std::log2(((double)b + (double)a) / div);
}
