// src/core/d2sound/d2sound_crt_n40.cpp -- D2Sound.dll MSVC CRT float<->string helpers. Unported callees are passed as seams.
#include <cstdint>
#include <cstring>

struct D2sStrflt { int32_t sign; int32_t decpt; int32_t flag; char* mantissa; };  // STRFLT: sign char at +0 (0x2d == '-')
typedef void (*D2sFltOut)(uint32_t lo, uint32_t hi, D2sStrflt* out, uint32_t* scratch);  // 0xCA9B
typedef void (*D2sCftof2)(char* buf, int32_t prec, int32_t g);   // __cftof2
typedef void (*D2sCftoe2)(char* buf, int32_t prec, int32_t caps, int32_t g);  // __cftoe2

// 1.13c D2sound+0xC96A
char* d2s_fptostr(char* buf, int32_t nd, D2sStrflt* flt) {
    const char* src = flt->mantissa;
    char* dst = buf; char* p = buf + 1;
    *buf = '0';
    if (nd > 0) {
        for (int32_t n = nd; n; n--) {
            char c = *src;
            if (c == 0) c = '0'; else src++;
            *p++ = c;
        }
    }
    *p = 0;
    if (nd >= 0 && *src > '4') {   // round
        while (*--p == '9') *p = '0';
        (*p)++;
    }
    if (*dst == '1') { flt->decpt++; return p; }
    size_t n = strlen(buf + 1);
    memmove(dst, buf + 1, n + 1);
    return dst;
}

// 1.13c D2sound+0xAFF7
char* d2s_cftof(const uint32_t* dbl, char* buf, int32_t prec, D2sFltOut fltout, D2sCftof2 cftof2) {
    D2sStrflt f; uint32_t scratch[6];
    fltout(dbl[0], dbl[1], &f, scratch);
    d2s_fptostr(buf + (f.sign == 0x2d), f.decpt + prec, &f);
    cftof2(buf, prec, 0);
    return buf;
}

// 1.13c D2sound+0xB05E
void d2s_cftog(const uint32_t* dbl, char* buf, int32_t prec, int32_t caps, D2sFltOut fltout, D2sCftof2 cftof2, D2sCftoe2 cftoe2) {
    D2sStrflt f; uint32_t scratch[6];
    fltout(dbl[0], dbl[1], &f, scratch);
    int32_t d0 = f.decpt - 1;
    d2s_fptostr(buf + (f.sign == 0x2d), prec, &f);
    f.decpt -= 1;
    if (f.decpt < -4 || prec <= f.decpt) { cftoe2(buf, prec, caps, 1); return; }
    char* p = buf + (f.sign == 0x2d);
    if (d0 < f.decpt) { while (*p++) {} p[-2] = 0; }
    cftof2(buf, prec, 1);
}

// 1.13c D2sound+0xB1C9
int32_t d2s_b1c9() { return 1; }

// 1.13c D2sound+0xB1FE  CPU feature probe; cpuid results and EFLAGS.ID-toggle result are host args
int32_t d2s_b1fe(int32_t cpuidSupported, const char vendor[12], uint32_t verEax, uint32_t verEdx, uint32_t* sse2Flag, uint32_t* fastFlag) {
    uint32_t edx = 0, eax = 0;
    if (cpuidSupported) { eax = verEax; edx = verEdx; }
    *fastFlag = 0; *sse2Flag = 0;
    if ((edx & 0x4000000) && d2s_b1c9()) {
        *fastFlag = 1;
        uint32_t fam = eax >> 8 & 0xf;
        if (memcmp("AuthenticAMD", vendor, 12) != 0 || ((fam != 0xf ? 0u : 0xffu) & (eax >> 20)) + fam != 0xf)
            *sse2Flag = 1;
    }
    return 0;
}

// 1.13c D2sound+0xC6AD
void d2s_CopyMan(uint32_t* dst, const uint32_t* src) { for (int i = 0; i < 3; i++) dst[i] = src[i]; }

// 1.13c D2sound+0xC6C8
int32_t d2s_IsZeroMan(const uint32_t* m) { for (int i = 0; i < 3; i++) if (m[i]) return 0; return 1; }

typedef int32_t (*D2sRoundMan)(uint32_t* m, int32_t bits);  // __RoundMan
typedef void (*D2sShrMan)(uint32_t* m, int32_t n);          // __ShrMan

// 1.13c D2sound+0xC75C
int32_t d2s_ld12cvt(const uint16_t* in, uint32_t* out, const int32_t* t, D2sRoundMan roundMan, D2sShrMan shrMan) {
    uint32_t m[3]; uint32_t save[3];
    uint16_t se = in[5];
    memcpy(&m[0], in + 3, 4); memcpy(&m[1], in + 1, 4); m[2] = (uint32_t)in[0] << 16;
    uint32_t e = se & 0x7fff; int32_t ex = (int32_t)e - 0x3fff; int32_t ret;
    if (ex == -0x3fff) {
        ex = 0;
        if (d2s_IsZeroMan(m)) { ret = 0; goto fin; }
        m[0] = m[1] = 0;
    } else {
        d2s_CopyMan(save, m);
        if (roundMan(m, t[2])) ex = (int32_t)e - 0x3ffe;
        if (ex < t[1] - t[2]) { m[0] = m[1] = 0; }
        else if (t[1] < ex) {
            if (t[0] <= ex) { m[1] = 0; m[2] = 0; m[0] = 0x80000000; shrMan(m, t[3]); ex = t[5] + t[0]; ret = 1; goto fin; }
            m[0] &= 0x7fffffff; ex = t[5] + ex; shrMan(m, t[3]); ret = 0; goto fin;
        } else {
            d2s_CopyMan(m, save); shrMan(m, t[1] - ex); roundMan(m, t[2]); shrMan(m, t[3] + 1);
        }
    }
    ex = 0; ret = 2;
fin:
    m[0] = ((uint32_t)ex << (0x1f - t[3] & 0x1f)) | ((se & 0x8000) ? 0x80000000u : 0) | m[0];
    if (t[4] == 0x40) { out[1] = m[0]; out[0] = m[1]; }
    else if (t[4] == 0x20) out[0] = m[0];
    return ret;
}

// 1.13c D2sound+0xC8B4  (table DAT_6f9c5bd0 passed in)
int32_t d2s_ld12tod_c8b4(const uint16_t* in, uint32_t* d, const int32_t* tbl, D2sRoundMan r, D2sShrMan s) { return d2s_ld12cvt(in, d, tbl, r, s); }

// 1.13c D2sound+0xC8CA  (table DAT_6f9c5be8 passed in)
int32_t d2s_ld12tod_c8ca(const uint16_t* in, uint32_t* d, const int32_t* tbl, D2sRoundMan r, D2sShrMan s) { return d2s_ld12cvt(in, d, tbl, r, s); }

typedef void (*D2sStrToLd)(uint16_t* ld12, int32_t* consumed, const uint8_t* s);  // 0xD3A7 with trailing zero args

// 1.13c D2sound+0xC8E0
void d2s_atodbl_c8e0(uint32_t* d, const uint8_t* s, D2sStrToLd strtold, const int32_t* tbl, D2sRoundMan r, D2sShrMan sh) {
    uint16_t ld[6]; int32_t used;
    strtold(ld, &used, s);
    d2s_ld12cvt(ld, d, tbl, r, sh);
}

// 1.13c D2sound+0xC925
void d2s_atodbl_c925(uint32_t* d, const uint8_t* s, D2sStrToLd strtold, const int32_t* tbl, D2sRoundMan r, D2sShrMan sh) {
    uint16_t ld[6]; int32_t used;
    strtold(ld, &used, s);
    d2s_ld12cvt(ld, d, tbl, r, sh);
}
