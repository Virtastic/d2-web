// src/core/d2game/d2game_batch_F6CA1.cpp -- ten 1.13c D2Game.dll functions (MSVC CRT 12-byte extended-float
// helpers used by strtold/_fltout) transcribed from the cached Ghidra decompiles
// (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Base 0x6FC20000.
//
// A "fp12" is 12 raw bytes: u16 guard @0, 8-byte mantissa @2, u16 sign|exp @10; as three u32 words for
// the shift/add helpers. __security_check_cookie (0x751C) is a stack-protector no-op here.
// Data seams: the power-of-ten tables (0x6fd2f9f0 positive, 0x6fd2fb50 negative), the locale radix char
// (DAT_6fd1f994) and DAT_6fd39fa4 are set by the embedder.
#include <cstdint>
#include <cstring>

static inline uint32_t r32(const void* p) { uint32_t v; std::memcpy(&v, p, 4); return v; }
static inline uint16_t r16(const void* p) { uint16_t v; std::memcpy(&v, p, 2); return v; }
static inline void w32(void* p, uint32_t v) { std::memcpy(p, &v, 4); }
static inline void w16(void* p, uint16_t v) { std::memcpy(p, &v, 2); }

extern "C" {
const uint8_t* d2game_fp_pow10_pos = nullptr;   // 0x6fd2f9f0
const uint8_t* d2game_fp_pow10_neg = nullptr;   // 0x6fd2fb50
uint8_t  d2game_radix_char = '.';               // DAT_6fd1f994
uint32_t d2game_DAT_6fd39fa4 = 0;

uint32_t d2game_addl(uint32_t a, uint32_t b, uint32_t* out);       // forward decls
void d2game_add_12(uint32_t* a, const uint32_t* b);
void d2game_shl_12(uint32_t* a);
void d2game_shr_12(uint32_t* a);
void d2game_fp12_mul(uint8_t* a, const uint8_t* b);
void d2game_fp12_scale10(uint8_t* fp, int32_t n, int32_t p3);
void d2game_fp12_from_digits(const char* digits, int32_t n, uint32_t* out);

uint32_t d2game_addl(uint32_t a, uint32_t b, uint32_t* out) {   // 1.13c D2Game+0xF6CA1
    uint32_t s = a + b;
    uint32_t c = (s < a || s < b) ? 1 : 0;
    *out = s;
    return c;
}

void d2game_add_12(uint32_t* a, const uint32_t* b) {   // 1.13c D2Game+0xF6CC2
    if (d2game_addl(a[0], b[0], &a[0])) {
        if (d2game_addl(a[1], 1, &a[1])) a[2] += 1;
    }
    if (d2game_addl(a[1], b[1], &a[1])) a[2] += 1;
    d2game_addl(a[2], b[2], &a[2]);
}

void d2game_shl_12(uint32_t* a) {   // 1.13c D2Game+0xF6D20
    uint32_t lo = a[0], mid = a[1];
    a[0] = lo * 2;
    a[1] = mid * 2 | lo >> 31;
    a[2] = a[2] << 1 | mid >> 31;
}

void d2game_shr_12(uint32_t* a) {   // 1.13c D2Game+0xF6D4E
    uint32_t mid = a[1];
    a[1] = mid >> 1 | a[2] << 31;
    a[2] = a[2] >> 1;
    a[0] = a[0] >> 1 | mid << 31;
}

// digit string (values 0..9) -> normalised fp12
void d2game_fp12_from_digits(const char* digits, int32_t n, uint32_t* out) {   // 1.13c D2Game+0xF6D7B
    int16_t exp = 0x404e;
    out[0] = out[1] = out[2] = 0;
    for (int32_t i = n; i != 0; --i) {
        uint32_t t[3] = { out[0], out[1], out[2] };
        d2game_shl_12(out);
        d2game_shl_12(out);
        d2game_add_12(out, t);
        d2game_shl_12(out);
        t[0] = (uint32_t)(int32_t)*digits++; t[1] = 0; t[2] = 0;
        d2game_add_12(out, t);
    }
    if (out[2] == 0) {
        uint32_t hi;
        do {
            exp -= 0x10;
            hi = out[1] >> 16;
            out[1] = out[0] >> 16 | out[1] << 16;
            out[0] = out[0] << 16;
        } while (hi == 0);
        out[2] = hi;
    }
    while ((out[2] & 0x8000) == 0) { d2game_shl_12(out); exp -= 1; }
    w16((uint8_t*)out + 10, (uint16_t)exp);
}

// fp12 multiply: a *= b
void d2game_fp12_mul(uint8_t* a, const uint8_t* b) {   // 1.13c D2Game+0xF753F
    int32_t sticky = 0;
    uint8_t buf[16] = {0};   // retail's 12-byte local; byte 12+ is the stack cookie (a carry there never happens)
    uint16_t eb = r16(b + 10) & 0x7fff;
    uint16_t ea = r16(a + 10) & 0x7fff;
    uint16_t sign = (r16(b + 10) ^ r16(a + 10)) & 0x8000;
    uint16_t e3 = (uint16_t)(eb + ea);
    uint16_t le;   // local_18
    if (ea < 0x7fff && eb < 0x7fff && e3 < 0xbffe) {
        if (e3 < 0x3fc0) goto zero;
        if (ea == 0) {
            e3++;
            le = 0;
            if ((r32(a + 8) & 0x7fffffff) != 0 || r32(a + 4) != 0 || r32(a) != 0) goto mulstep;
        } else {
        mulstep:
            if (eb == 0) {
                e3++;
                if ((r32(b + 8) & 0x7fffffff) == 0 && r32(b + 4) == 0 && r32(b) == 0) {
                zero:
                    w32(a + 8, 0); w32(a + 4, 0); w32(a, 0);
                    return;
                }
            }
            for (int i = 0; i < 5; i++) {
                for (int j = 0; j < 5 - i; j++) {
                    uint32_t prod = (uint32_t)r16(b + 8 - 2 * j) * (uint32_t)r16(a + 2 * (i + j));
                    // accumulate into u32 @ buf+2*i, carry into u16 @ buf+2*i+4
                    uint32_t acc = r32(buf + 2 * i);
                    uint32_t s = acc + prod;
                    int c = (s < acc || s < prod);
                    w32(buf + 2 * i, s);
                    if (c) w16(buf + 2 * i + 4, r16(buf + 2 * i + 4) + 1);
                }
            }
            le = (uint16_t)(e3 + 0xc002);
            if ((int16_t)le < 1) goto denorm;
            do {
                if (buf[11] & 0x80) break;
                d2game_shl_12((uint32_t*)buf);
                le--;
            } while (0 < (int16_t)le);
            if ((int16_t)le < 1) {
            denorm:
                le--;
                if ((int16_t)le < 0) {
                    uint32_t cnt = (uint16_t)-le;
                    le = 0;
                    do {
                        if (buf[0] & 1) sticky++;
                        d2game_shr_12((uint32_t*)buf);
                        cnt--;
                    } while (cnt != 0);
                    if (sticky) buf[0] |= 1;
                }
            }
            {
                uint16_t g = r16(buf);
                uint32_t A = r32(buf + 2), B = r32(buf + 6);
                uint16_t C = r16(buf + 10);
                if (g > 0x8000 || (r32(buf) & 0x1ffff) == 0x18000) {
                    if (A == 0xffffffffu) {
                        A = 0;
                        if (B == 0xffffffffu) {
                            if (C == 0xffff) { le++; C = 0x8000; B = 0; }
                            else { C++; B = 0; }
                        } else B++;
                    } else A++;
                }
                w32(buf + 2, A); w32(buf + 6, B); w16(buf + 10, C);
            }
            if (0x7ffe < le) goto ovf;
            std::memcpy(a, buf + 2, 10);
            le |= sign;
        }
        w16(a + 10, le);
    } else {
    ovf:
        w32(a + 4, 0); w32(a, 0);
        w32(a + 8, (sign ? 0x80000000u : 0u) + 0x7fff8000u);
    }
}

// fp *= 10^n using the power-of-ten tables
void d2game_fp12_scale10(uint8_t* fp, int32_t n, int32_t p3) {   // 1.13c D2Game+0xF7775
    if (n == 0) return;
    uint32_t m = (uint32_t)n;
    const uint8_t* tab = d2game_fp_pow10_pos;
    if (n < 0) { m = (uint32_t)-n; tab = d2game_fp_pow10_neg; }
    if (p3 == 0) w16(fp, 0);
    while (m != 0) {
        uint32_t d = m & 7;
        m = (uint32_t)((int32_t)m >> 3);
        tab += 0x54;
        if (d != 0) {
            const uint8_t* e = tab + d * 12;
            uint8_t tmp[12];
            if (0x7fff < r16(e)) {
                std::memcpy(tmp, e, 12);
                w32(tmp + 2, r32(tmp + 2) - 1);
                e = tmp;
            }
            d2game_fp12_mul(fp, e);
        }
    }
}

// ten-byte-aligned decimal conversion (_fltout core). out: [0]=i16 decpt, [2]=sign, [3]=ndigits, [4..]=digits
uint32_t d2game_fltout_core(uint32_t lo, uint32_t hi, uint16_t se, int32_t ndigits, uint8_t flags, int16_t* out16) {   // 1.13c D2Game+0xF72A3
    uint8_t* o = (uint8_t*)out16;
    uint32_t ex = se & 0x7fff;
    static const uint8_t c01[12] = {0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xfb,0x3f};
    uint8_t b[12];
    uint8_t tmp[12];
    int16_t dp;
    int32_t iVar8;
    uint8_t *p5, *p6, *p1;
    int32_t cnt;
    o[2] = (se & 0x8000) ? '-' : ' ';
    if ((int16_t)ex == 0 && hi == 0 && lo == 0) {
    zero0:
        o[4] = '0';
    zero:
        out16[0] = 0; o[2] = ' '; o[3] = 1; o[5] = 0;
        return 1;
    }
    if ((int16_t)ex == 0x7fff) {
        const char* s;
        int len6 = 0;
        out16[0] = 1;
        if ((hi == 0x80000000u && lo == 0) || (hi & 0x40000000)) {
            if (!(se & 0x8000) || hi != 0xc0000000u) {
                if (hi != 0x80000000u || lo != 0) goto qnan;
                s = "1#INF";
            } else {
                if (lo != 0) { qnan: s = "1#QNAN"; len6 = 1; }
                else s = "1#IND";
            }
            std::strcpy((char*)o + 4, s);
            o[3] = len6 ? 6 : 5;   // QNAN reaches the label shared with SNAN in retail: length 6
        } else {
            std::strcpy((char*)o + 4, "1#SNAN");   // retail copies "1#SNAN" (6 chars)
            o[3] = 6;
        }
        return 0;
    }
    w16(b, 0);
    dp = (int16_t)((((ex >> 8) + (hi >> 24) * 2) * 0x4d + (uint32_t)-0x134312f4 + ex * 0x4d10) >> 16);
    w32(b + 2, lo); w32(b + 6, hi); w16(b + 10, (uint16_t)ex);
    d2game_fp12_scale10(b, -(int32_t)dp, 1);
    if (0x3ffe < r16(b + 10)) { dp++; d2game_fp12_mul(b, c01); }
    out16[0] = dp;
    if ((flags & 1) && (ndigits += dp, ndigits < 1)) goto zero0;
    if (0x15 < ndigits) ndigits = 0x15;
    iVar8 = (int32_t)r16(b + 10) - 0x3ffe;
    w16(b + 10, 0);
    for (int i = 8; i != 0; --i) d2game_shl_12((uint32_t*)b);
    if (iVar8 < 0)
        for (uint32_t u = (uint32_t)-iVar8 & 0xff; u != 0; --u) d2game_shr_12((uint32_t*)b);
    cnt = ndigits + 1;
    p5 = o + 4;
    while (0 < cnt) {
        std::memcpy(tmp, b, 12);
        d2game_shl_12((uint32_t*)b);
        d2game_shl_12((uint32_t*)b);
        d2game_add_12((uint32_t*)b, (uint32_t*)tmp);
        d2game_shl_12((uint32_t*)b);
        *p5++ = (uint8_t)(b[11] + '0');
        b[11] = 0;
        cnt--;
    }
    p6 = p5 - 1;
    p1 = o + 4;
    if ((char)p5[-1] < '5') {
        for (; p1 <= p6 && *p6 == '0'; --p6) {}
        if (p6 < p1) { *p1 = '0'; goto zero; }
    } else {
        for (; p1 <= p6 && *p6 == '9'; --p6) *p6 = '0';
        if (p6 < p1) { ++p6; out16[0] += 1; }
        *p6 += 1;
    }
    o[3] = (uint8_t)((char)(p6 - o) - 3);
    o[(int8_t)o[3] + 4] = 0;
    return 1;
}

// string -> fp12 (strtold core). Returns status flags (1 underflow, 2 overflow, 4 no digits).
uint32_t d2game_strtold_core(uint8_t* out, int32_t* endp, const uint8_t* s, int32_t p4, int32_t p5, int32_t p6, int32_t p7) {   // 1.13c D2Game+0xF6E63
    auto isd = [](uint8_t c) { return c >= '0' && c <= '9'; };
    bool b1 = false, b3 = false, b2 = false;
    int32_t state = 0, sign = 1, ndig = 0, expo = 0, dexp = 0;
    uint32_t status = 0, nd = 0;
    uint16_t sgn = 0;
    char dig[32] = {0};
    char* pd = dig;
    const uint8_t* mark = s;
    const uint8_t* p3 = s;
    const uint8_t* q;
    uint8_t c;
    uint8_t fp[12];
    uint8_t res[12];
    uint32_t uv;
    (void)ndig;
    for (; (c = *p3, c == 0x20 || c == 9 || c == 10 || c == 0xd); ++p3) {}
top:
    c = *p3;
    q = p3 + 1;
    switch (state) {
    case 0:
        if ('0' < (char)c && (char)c < ':') { to3: state = 3; goto re; }
        if (c == d2game_radix_char) { to5: state = 5; p3 = q; goto top; }
        state = 2;
        if (c == '+') { sgn = 0; p3 = q; goto top; }
        if (c == '-') { sgn = 0x8000; p3 = q; goto top; }
        break;
    case 1:
        state = 1; b1 = true;
        if ('0' < (char)c && (char)c < ':') goto to3;
        if (c == d2game_radix_char) goto rad;
        if (c == '+' || c == '-') goto sg;
        p3 = q;
        if (c != '0') goto f5b;
        goto top;
    case 2:
        if ('0' < (char)c && (char)c < ':') goto to3;
        p3 = mark;
        if (c == d2game_radix_char) goto to5;
        break;
    case 3:
        while (isd(c)) {
            if (nd < 0x19) { nd++; *pd++ = (char)(c - 0x30); }
            else dexp++;
            c = *q++;
        }
        if (c != d2game_radix_char) goto l17031;
    rad:
        b1 = true; state = 4; p3 = q; goto top;
    case 4:
        b3 = true;
        if (nd == 0) { while (c == 0x30) { dexp--; c = *q++; } }
        while (isd(c)) {
            if (nd < 0x19) { nd++; *pd++ = (char)(c - 0x30); dexp--; }
            c = *q++;
        }
    l17031:
        if (c == '+' || c == '-') {
        sg: b1 = true; state = 0xb; p3 = q - 1;
        } else {
        f5b:
            b1 = true;
            if ((char)c < 'D' || ((char)c > 'E' && ((char)c < 'd' || (char)c > 'e'))) {
            l170bb: p3 = q - 1; goto l170bf;
            }
            state = 6; p3 = q;
        }
        goto top;
    case 5:
        b3 = true;
        p3 = mark;
        if (isd(c)) { state = 4; goto re; }
        goto l170bf;
    case 6:
        mark = p3 - 1;
        if ((char)c < '1' || '9' < (char)c) {
            if (c == '+') goto l17137;
            if (c == '-') goto l1712b;
            p3 = mark;
            if (c == '0') goto l17099;
            goto l170bf;
        }
    l17100:
        state = 9;
    re: p3 = q - 1; goto top;
    case 7:
        if ('0' < (char)c && (char)c < ':') goto l17100;
        p3 = mark;
        if (c != '0') goto l170bf;
    l17099:
        state = 8; p3 = q; goto top;
    case 8:
        b2 = true;
        while (c == '0') { c = *q++; }
        if ('0' < (char)c && (char)c < ':') goto l17100;
        goto l170bb;
    case 9:
        b2 = true;
        expo = 0;
        goto l1717e;
    default:
        goto caseA;
    case 0xb:
        if (p7 != 0) {
            mark = p3;
            if (c == '+') {
            l17137: state = 7; p3 = q;
            } else {
                if (c != '-') goto l170bf;
            l1712b: sign = -1; state = 7; p3 = q;
            }
            goto top;
        }
        state = 10;
        q = p3;
    caseA:
        p3 = q;
        if (state != 10) goto top;
        goto l170bf;
    }
    if (c != '0') goto l170bf;
    state = 1; p3 = q; goto top;
l1717e:
    if (!isd(c)) goto l171a1;
    expo = (char)c - 0x30 + expo * 10;
    if (0x1450 < expo) { expo = 0x1451; goto l171a1; }
    c = *q++;
    goto l1717e;
l171a1:
    while (isd(c)) c = *q++;
    p3 = q - 1;
l170bf:
    *endp = (int32_t)(intptr_t)p3;
    std::memset(res, 0, 12);
    if (b1) {
        if (0x18 < nd) {
            if (4 < dig[23]) dig[23]++;
            pd--;
            dexp++;
            nd = 0x18;
        }
        if (nd != 0) {
            while (pd--, *pd == 0) { nd--; dexp++; }
            d2game_fp12_from_digits(dig, (int32_t)nd, (uint32_t*)fp);
            if (sign < 0) expo = -expo;
            uv = (uint32_t)(expo + dexp);
            if (!b2) uv += (uint32_t)p5;
            if (!b3) uv -= (uint32_t)p6;
            if (0x1450 < (int32_t)uv) {
                std::memset(res, 0, 12);
                w16(res + 10, 0x7fff); w32(res + 6, 0x80000000u);
                status = 2;
            } else if (-0x1451 < (int32_t)uv) {
                d2game_fp12_scale10(fp, (int32_t)uv, p4);
                std::memcpy(res, fp, 12);
            } else status = 1;
        }
    } else status = 4;
    w16(res + 10, r16(res + 10) | sgn);
    std::memcpy(out, res, 12);
    return status;
}

void d2game_unwind_f78a0(void) {   // 1.13c D2Game+0xF78A0 (Unwind@6fd178a0)
    d2game_DAT_6fd39fa4 &= 0xfffffffeu;
}
}
