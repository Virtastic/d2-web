#include <cstdint>
#include <cstring>

// Callees not yet mapped, bound by the host: 1AC0C = scale by 10^n (num, n, 1), 1A9D6 = 96-bit multiply step (num, tmp12).
void (*g_d2cmp_i10_scale)(uint32_t* num12, int n, int one) = nullptr;
void (*g_d2cmp_i10_mulstep)(uint32_t* num12, uint32_t* tmp12) = nullptr;

static void shl12(uint32_t* p) { uint32_t a = p[0], b = p[1]; p[0] = a << 1; p[1] = b << 1 | a >> 31; p[2] = p[2] << 1 | b >> 31; }  // 1.13c D2CMP+0x1A010
static void shr12(uint32_t* p) { uint32_t b = p[1]; p[1] = b >> 1 | p[2] << 31; p[2] >>= 1; p[0] = p[0] >> 1 | b << 31; }  // 1.13c D2CMP+0x1A03E
static void add12(uint32_t* d, const uint32_t* s)  // 1.13c D2CMP+0x19FB2
{
    uint64_t t = (uint64_t)d[0] + s[0]; d[0] = (uint32_t)t;
    t = (uint64_t)d[1] + s[1] + (t >> 32); d[1] = (uint32_t)t;
    d[2] = d[2] + s[2] + (uint32_t)(t >> 32);
}

// 1.13c D2CMP+0x1A593  ($I10_OUTPUT: 80-bit long double (m0,m1,se) -> decimal digits). res: i16 decpt, +2 sign, +3 count, +4 digits.
int d2cmp_i10_output_1a593(uint32_t m0, uint32_t m1, uint16_t se, int ndigits, uint8_t flags, uint8_t* res)
{
    uint32_t e = se & 0x7fff;
    int16_t decpt;
    res[2] = (se & 0x8000) ? '-' : ' ';
    auto zero = [&] { res[4] = '0'; decpt = 0; res[2] = ' '; res[3] = 1; res[5] = 0; memcpy(res, &decpt, 2); return 1; };
    if (e == 0 && m1 == 0 && m0 == 0) return zero();
    if (e == 0x7fff) {
        decpt = 1; memcpy(res, &decpt, 2);
        const char* s;
        if ((m1 == 0x80000000 && m0 == 0) || (m1 & 0x40000000)) {
            if (!((se & 0x8000) && m1 == 0xc0000000)) { if (m1 != 0x80000000 || m0) { s = "1#QNAN"; } else { s = "1#INF"; } }
            else if (m0) { s = "1#QNAN"; } else { s = "1#IND"; }
            if (s[2] == 'Q') res[3] = 5;
        } else { s = "1#SNAN"; res[3] = 6; }
        strcpy((char*)res + 4, s);
        return 0;
    }
    uint32_t w[3] = {m0 << 16, m0 >> 16 | m1 << 16, m1 >> 16 | e << 16};  // 12 bytes: 0, m0, m1, exp(2)
    uint32_t cc[3] = {0xcccccccc, 0xcccccccc, 0x3ffbcccc};                 // local_20 constant
    int16_t d = (int16_t)((((e >> 8) + (m1 >> 24) * 2) * 0x4d - 0x134312f4 + e * 0x4d10) >> 16);
    g_d2cmp_i10_scale(w, -(int)d, 1);
    if ((w[2] >> 16) > 0x3ffe) { d++; g_d2cmp_i10_mulstep(w, cc); }
    decpt = d; memcpy(res, &decpt, 2);
    int nd = ndigits;
    if ((flags & 1) && (nd += d) < 1) return zero();
    if (nd > 0x15) nd = 0x15;
    int sh = (int)(w[2] >> 16) - 0x3ffe;
    w[2] &= 0xffff;
    for (int i = 0; i < 8; i++) shl12(w);
    if (sh < 0) for (uint32_t k = -sh & 0xff; k; k--) shr12(w);
    uint8_t* p = res + 4;
    for (int n = nd + 1; n > 0; n--) {
        uint32_t c[3] = {w[0], w[1], w[2]};
        shl12(w); shl12(w); add12(w, c); shl12(w);
        *p++ = (uint8_t)(w[2] >> 24) + '0';
        w[2] &= 0xffffff;
    }
    uint8_t *q = p - 1, *b = res + 4;
    if (*q < '5') {
        while (b <= q && *q == '0') q--;
        if (q < b) { *b = '0'; return zero(); }
    } else {
        while (b <= q && *q == '9') *q-- = '0';
        if (q < b) { q++; decpt = ++d; memcpy(res, &decpt, 2); }
        (*q)++;
    }
    int8_t c = (int8_t)(q - res) - 3;
    res[3] = c; res[c + 4] = 0;
    return 1;
}
