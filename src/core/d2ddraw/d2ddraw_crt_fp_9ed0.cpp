// D2DDraw statically linked MSVC CRT float->string / string->float helpers (batch j05ddr).
// Externals (___addl 0xA1EA, ___shl_12 0xA269, ___shr_12 0xA297, locale decimal point,
// power-of-ten tables at 0x6f8cebb8 / 0x6f8ced18) come in through the Env.
#include <cstdint>
#include <cstring>

struct d2ddraw_crt_fp_Env {
    int (*addl)(uint32_t a, uint32_t b, uint32_t* out); // 0xA1EA, returns carry
    void (*shl12)(uint32_t* v);                         // 0xA269
    void (*shr12)(uint32_t* v);                         // 0xA297
    uint8_t decimalPoint;                               // DAT_6f8ce7e4
    const uint8_t* posTbl;                              // 0x6f8cebb8
    const uint8_t* negTbl;                              // 0x6f8ced18
};

struct d2ddraw_crt_fp_STRFLT { int32_t sign, decpt, flag; char* man; };

static inline bool fp_isdigit(uint8_t c) { return (uint8_t)(c - '0') < 10; }

// 1.13c D2DDraw+0x9ED0
void d2ddraw_crt_fp___fptostr(char* buf, int count, d2ddraw_crt_fp_STRFLT* pt)
{
    char* dst = buf;
    char* src = pt->man;
    char* str = buf + 1;
    char* p = str;
    *buf = '0';
    if (count > 0) {
        int n = count;
        do {
            char c = *src;
            if (c == '\0') c = '0'; else src++;
            *p++ = c;
        } while (--n != 0);
    }
    *p = '\0';
    if (count >= 0 && *src > '4') {
        while (*--p == '9') *p = '0';
        *p += 1;
    }
    if (*dst == '1') pt->decpt++;
    else memmove(dst, str, strlen(str) + 1);
}

// 1.13c D2DDraw+0x9F47
void d2ddraw_crt_fp____dtold(uint32_t* out, const uint32_t* in)
{
    uint16_t uVar1 = (uint16_t)(in[1] >> 16);
    uint32_t local_8 = 0x80000000u;
    uint16_t uVar4 = (uint16_t)((uVar1 >> 4) & 0x7ff);
    uint32_t uVar2 = in[0];
    if (uVar4 == 0) {
        if ((in[1] & 0xfffff) == 0 && uVar2 == 0) {
            out[1] = 0; out[0] = 0; *(uint16_t*)(out + 2) = 0;
            return;
        }
        uVar4 = (uint16_t)(uVar4 + 0x3c01);
        local_8 = 0;
    } else if (uVar4 == 0x7ff) {
        uVar4 = 0x7fff;
    } else {
        uVar4 = (uint16_t)(uVar4 + 0x3c00);
    }
    out[1] = uVar2 >> 21 | (in[1] & 0xfffff) << 11 | local_8;
    out[0] = uVar2 << 11;
    while (local_8 == 0) {
        uVar2 = out[1];
        uVar4 = (uint16_t)(uVar4 - 1);
        out[1] = uVar2 << 1 | out[0] >> 31;
        out[0] = out[0] * 2;
        local_8 = uVar2 << 1 & 0x80000000u;
    }
    *(uint16_t*)(out + 2) = (uint16_t)((uVar1 & 0x8000) | uVar4);
}

// 1.13c D2DDraw+0xA20B
void d2ddraw_crt_fp____add_12(const d2ddraw_crt_fp_Env* e, uint32_t* a, const uint32_t* b)
{
    if (e->addl(a[0], b[0], a) != 0) {
        if (e->addl(a[1], 1, a + 1) != 0) a[2]++;
    }
    if (e->addl(a[1], b[1], a + 1) != 0) a[2]++;
    e->addl(a[2], b[2], a + 2);
}

// 1.13c D2DDraw+0xA2C4
void d2ddraw_crt_fp_a2c4(const d2ddraw_crt_fp_Env* e, const char* digits, int n, uint32_t* x)
{
    int16_t s = 0x404e;
    uint32_t t[3];
    x[0] = x[1] = x[2] = 0;
    for (; n != 0; n--) {
        t[0] = x[0]; t[1] = x[1]; t[2] = x[2];
        e->shl12(x); e->shl12(x);
        d2ddraw_crt_fp____add_12(e, x, t);
        e->shl12(x);
        t[0] = (uint32_t)(int32_t)*digits++; t[1] = 0; t[2] = 0;
        d2ddraw_crt_fp____add_12(e, x, t);
    }
    if (x[2] == 0) {
        uint32_t u;
        do {
            s -= 0x10;
            u = x[1] >> 16;
            x[1] = x[0] >> 16 | x[1] << 16;
            x[0] <<= 16;
        } while (u == 0);
        x[2] = u;
    }
    while ((x[2] & 0x8000) == 0) { e->shl12(x); s--; }
    *(int16_t*)((uint8_t*)x + 10) = s;
}

// 1.13c D2DDraw+0xAA88  (80-bit extended multiply: a *= b)
void d2ddraw_crt_fp_aa88(const d2ddraw_crt_fp_Env* e, uint32_t* a32, const uint32_t* b32)
{
    uint8_t* a = (uint8_t*)a32;
    const uint8_t* b = (const uint8_t*)b32;
    uint32_t prodw[3] = {0, 0, 0};
    uint8_t* prod = (uint8_t*)prodw;
    auto rd16 = [](const uint8_t* p) { uint16_t v; memcpy(&v, p, 2); return v; };
    auto rd32 = [](const uint8_t* p) { uint32_t v; memcpy(&v, p, 4); return v; };
    auto wr16 = [](uint8_t* p, uint16_t v) { memcpy(p, &v, 2); };
    auto wr32 = [](uint8_t* p, uint32_t v) { memcpy(p, &v, 4); };
    uint16_t uVar6 = rd16(b + 10) & 0x7fff;
    uint16_t uVar4 = rd16(a + 10) & 0x7fff;
    uint16_t uVar8 = (rd16(b + 10) ^ rd16(a + 10)) & 0x8000;
    uint16_t uVar3 = (uint16_t)(uVar6 + uVar4);
    uint16_t exp = 0;
    int sticky = 0;
    bool skipMul = false;
    if (!(uVar4 < 0x7fff && uVar6 < 0x7fff && uVar3 < 0xbffe)) goto overflow;
    if (uVar3 < 0x3fc0) goto zero;
    if (uVar4 == 0) {
        uVar3++;
        exp = 0;
        if (!((a32[2] & 0x7fffffffu) != 0 || a32[1] != 0 || a32[0] != 0)) skipMul = true;
    }
    if (!skipMul) {
        if (uVar6 == 0) {
            uVar3++;
            if ((b32[2] & 0x7fffffffu) == 0 && b32[1] == 0 && b32[0] == 0) goto zero;
        }
        int outer = 0, cnt = 5;
        uint8_t* acc = prod;                      // local_20 - 2 words
        while (cnt > 0) {
            const uint8_t* pa = a + outer * 2;
            const uint8_t* pb = b + 8;
            for (int k = cnt; k > 0; k--) {
                uint32_t cur = rd32(acc), res;
                int c = e->addl(cur, (uint32_t)rd16(pb) * (uint32_t)rd16(pa), &res);
                wr32(acc, res);
                if (c) wr16(acc + 4, rd16(acc + 4) + 1);
                pa += 2; pb -= 2;
            }
            acc += 2; outer++; cnt--;
        }
        exp = (uint16_t)(uVar3 + 0xc002);
        bool shiftRight = (int16_t)exp < 1;
        if (!shiftRight) {
            do {
                if (prod[11] & 0x80) break;
                e->shl12(prodw);
                exp--;
            } while (0 < (int16_t)exp);
            shiftRight = (int16_t)exp < 1;
        }
        if (shiftRight) {
            exp--;
            if ((int16_t)exp < 0) {
                uint32_t n = (uint16_t)-(int16_t)exp;
                exp = 0;
                do {
                    if (prod[0] & 1) sticky++;
                    e->shr12(prodw);
                } while (--n != 0);
                if (sticky) prod[0] |= 1;
            }
        }
        uint32_t lo = rd32(prod);
        if (rd16(prod) > 0x8000 || (lo & 0x1ffff) == 0x18000) {
            uint32_t m0 = rd32(prod + 2), m1 = rd32(prod + 6);
            uint16_t m2 = rd16(prod + 10);
            if (m0 == 0xffffffffu) {
                m0 = 0;
                if (m1 == 0xffffffffu) {
                    m1 = 0;
                    if (m2 == 0xffff) { exp++; m2 = 0x8000; } else m2++;
                } else m1++;
            } else m0++;
            wr32(prod + 2, m0); wr32(prod + 6, m1); wr16(prod + 10, m2);
        }
        if (exp > 0x7ffe) goto overflow;
        memcpy(a, prod + 2, 10);
        exp |= uVar8;
    }
    wr16(a + 10, exp);
    return;
zero:
    a32[2] = 0; a32[1] = 0; a32[0] = 0;
    return;
overflow:
    a32[1] = 0; a32[0] = 0;
    a32[2] = (uVar8 ? 0x80000000u : 0u) + 0x7fff8000u;
}

// 1.13c D2DDraw+0xACBE
void d2ddraw_crt_fp_acbe(const d2ddraw_crt_fp_Env* e, int32_t* out, int32_t p2, int p3)
{
    const uint8_t* tbl = e->posTbl;
    if (p2 == 0) return;
    if (p2 < 0) { p2 = -p2; tbl = e->negTbl; }
    if (p3 == 0) *(uint16_t*)out = 0;
    while (p2 != 0) {
        int32_t next = p2 >> 3;
        uint32_t r = (uint32_t)p2 & 7;
        tbl += 0x54;
        p2 = next;
        if (r != 0) {
            const uint8_t* ent = tbl + r * 0xc;
            uint32_t tmp[3];
            uint16_t w0; memcpy(&w0, ent, 2);
            if (w0 > 0x7fff) {
                uint8_t* t = (uint8_t*)tmp;
                memcpy(t, ent, 12);
                uint32_t v; memcpy(&v, t + 2, 4); v -= 1; memcpy(t + 2, &v, 4);
                ent = t;
            }
            d2ddraw_crt_fp_aa88(e, (uint32_t*)out, (const uint32_t*)ent);
        }
    }
}

// 1.13c D2DDraw+0xA7EC  (80-bit -> decimal digits; out: +0 decpt, +2 sign, +3 ndigits, +4 digits)
int d2ddraw_crt_fp_a7ec(const d2ddraw_crt_fp_Env* e, uint32_t mlo, uint32_t mhi, uint16_t sexp,
                        int ndig, uint8_t flags, uint8_t* out)
{
    uint32_t uVar4 = sexp & 0x7fff;
    static const uint8_t k[12] = {0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xfb,0x3f};
    uint32_t cw[3]; memcpy(cw, k, 12);
    out[2] = (sexp & 0x8000) ? '-' : ' ';
    auto zero = [&]() { out[4] = '0'; *(int16_t*)out = 0; out[2] = ' '; out[3] = 1; out[5] = 0; };
    if (uVar4 == 0 && mhi == 0 && mlo == 0) { zero(); return 1; }
    if (uVar4 == 0x7fff) {
        const char* s;
        *(int16_t*)out = 1;
        if ((mhi == 0x80000000u && mlo == 0) || (mhi & 0x40000000u)) {
            if (!(sexp & 0x8000) || mhi != 0xc0000000u) {
                s = (mhi != 0x80000000u || mlo != 0) ? "1#QNAN" : "1#INF";
            } else {
                s = (mlo != 0) ? "1#QNAN" : "1#IND";
            }
            out[3] = (s[2] == 'Q') ? 6 : 5;
        } else {
            s = "1#SNAN";
            out[3] = 6;
        }
        strcpy((char*)out + 4, s);
        return 0;
    }
    uint32_t mw[3];
    uint8_t* m = (uint8_t*)mw;
    memset(m, 0, 12);
    int16_t sVar7 = (int16_t)((((uVar4 >> 8) + (mhi >> 24) * 2) * 0x4d + 0xecbced0cu + uVar4 * 0x4d10) >> 16);
    m[10] = (uint8_t)uVar4; m[11] = (uint8_t)(uVar4 >> 8);
    memcpy(m + 2, &mlo, 4); memcpy(m + 6, &mhi, 4);
    d2ddraw_crt_fp_acbe(e, (int32_t*)mw, -(int)sVar7, 1);
    uint16_t ex; memcpy(&ex, m + 10, 2);
    if (ex > 0x3ffe) { sVar7++; d2ddraw_crt_fp_aa88(e, mw, cw); }
    *(int16_t*)out = sVar7;
    if ((flags & 1) && (ndig += sVar7, ndig < 1)) { zero(); return 1; }
    if (ndig > 0x15) ndig = 0x15;
    memcpy(&ex, m + 10, 2);
    int iVar8 = ex - 0x3ffe;
    m[10] = 0; m[11] = 0;
    for (int i = 8; i != 0; i--) e->shl12(mw);
    if (iVar8 < 0) for (uint32_t n = (uint32_t)-iVar8 & 0xff; n != 0; n--) e->shr12(mw);
    uint8_t* first = out + 4;
    uint8_t* p = first;
    for (int n = ndig + 1; n > 0; n--) {
        uint32_t cp[3]; memcpy(cp, mw, 12);
        e->shl12(mw); e->shl12(mw);
        d2ddraw_crt_fp____add_12(e, mw, cp);
        e->shl12(mw);
        *p++ = (uint8_t)(m[11] + '0');
        m[11] = 0;
    }
    uint8_t* q = p - 1;
    if (*(p - 1) < '5') {
        for (; first <= q && *q == '0'; q--) {}
        if (q < first) { zero(); return 1; }
    } else {
        for (; first <= q && *q == '9'; q--) *q = '0';
        if (q < first) { q++; (*(int16_t*)out)++; }
        *q += 1;
    }
    int8_t c = (int8_t)((q - out) - 3);
    out[3] = (uint8_t)c;
    out[c + 4] = 0;
    return 1;
}

// 1.13c D2DDraw+0xA001
void d2ddraw_crt_fp_a001(const d2ddraw_crt_fp_Env* e, double v, d2ddraw_crt_fp_STRFLT* pt, char* buf)
{
    uint32_t dv[2]; memcpy(dv, &v, 8);
    uint32_t ld[3];
    uint8_t r[28];                       // +0 decpt, +2 sign, +3 n, +4 digits
    d2ddraw_crt_fp____dtold(ld, dv);
    uint16_t sx; memcpy(&sx, (uint8_t*)ld + 8, 2);
    int rc = d2ddraw_crt_fp_a7ec(e, ld[0], ld[1], sx, 0x11, 0, r);
    int16_t decpt; memcpy(&decpt, r, 2);
    pt->flag = rc;
    pt->sign = (int8_t)r[2];
    pt->decpt = decpt;
    strcpy(buf, (const char*)r + 4);
    pt->man = buf;
}

// 1.13c D2DDraw+0xA3AC  (string -> 80-bit long double; returns status bits 1=underflow 2=overflow 4=no digits)
uint32_t d2ddraw_crt_fp_a3ac(const d2ddraw_crt_fp_Env* e, uint8_t* out12, const uint8_t** endp,
                             const uint8_t* s, int p4, int p5, int p6, int p7)
{
    bool bVar1 = false, bVar2 = false, bVar3 = false;
    uint8_t bVar7;
    const uint8_t* pbVar10;
    const uint8_t* param_3 = s;
    int iVar6 = 0;
    uint16_t uVar4 = 0;
    int local_60 = 1, local_50 = 0, local_40 = 0;
    uint32_t local_3c = 0, local_4c = 0, uVar5;
    const uint8_t* local_44 = s;
    char buf[32] = {0};
    char* pcVar8 = buf;
    char& local_d = buf[23];
    uint32_t xw[3] = {0, 0, 0};
    uint8_t* x = (uint8_t*)xw;
    uint32_t uVar9;
    uint16_t local_2a;

    while ((bVar7 = *param_3, bVar7 == 0x20 || bVar7 == 9 || bVar7 == 10 || bVar7 == 0xd)) param_3++;
LAB_40f:
    bVar7 = *param_3;
    pbVar10 = param_3 + 1;
    switch (iVar6) {
    case 0:
        if ('0' < (char)bVar7 && (char)bVar7 < ':') {
LAB_42c:
            iVar6 = 3;
            goto LAB_64b;
        }
        if (bVar7 == e->decimalPoint) {
LAB_43b:
            iVar6 = 5;
            param_3 = pbVar10;
            goto LAB_40f;
        }
        iVar6 = 2;
        if (bVar7 == 0x2b) { uVar4 = 0; param_3 = pbVar10; goto LAB_40f; }
        if (bVar7 == 0x2d) { uVar4 = 0x8000; param_3 = pbVar10; goto LAB_40f; }
        break;
    case 1:
        iVar6 = 1;
        bVar1 = true;
        if ('0' < (char)bVar7 && (char)bVar7 < ':') goto LAB_42c;
        if (bVar7 == e->decimalPoint) goto LAB_48a;
        if (bVar7 == 0x2b || bVar7 == 0x2d) goto LAB_4cb;
        param_3 = pbVar10;
        if (bVar7 != 0x30) goto LAB_4a4;
        goto LAB_40f;
    case 2:
        if ('0' < (char)bVar7 && (char)bVar7 < ':') goto LAB_42c;
        param_3 = local_44;
        if (bVar7 == e->decimalPoint) goto LAB_43b;
        break;
    case 3:
        while (fp_isdigit(bVar7)) {
            if (local_3c < 0x19) { local_3c++; *pcVar8++ = (char)(bVar7 - 0x30); }
            else local_40++;
            bVar7 = *pbVar10++;
        }
        if (bVar7 != e->decimalPoint) goto LAB_57a;
LAB_48a:
        bVar1 = true;
        iVar6 = 4;
        param_3 = pbVar10;
        goto LAB_40f;
    case 4:
        bVar3 = true;
        if (local_3c == 0) {
            while (bVar7 == 0x30) { local_40--; bVar7 = *pbVar10++; }
        }
        while (fp_isdigit(bVar7)) {
            if (local_3c < 0x19) { local_3c++; *pcVar8++ = (char)(bVar7 - 0x30); local_40--; }
            bVar7 = *pbVar10++;
        }
LAB_57a:
        if (bVar7 == 0x2b || bVar7 == 0x2d) {
LAB_4cb:
            bVar1 = true;
            iVar6 = 0xb;
            param_3 = pbVar10 - 1;
        } else {
LAB_4a4:
            bVar1 = true;
            if ((char)bVar7 < 'D' || ((char)bVar7 > 'E' && ((char)bVar7 < 'd' || (char)bVar7 > 'e'))) {
LAB_604:
                param_3 = pbVar10 - 1;
                goto LAB_608;
            }
            iVar6 = 6;
            param_3 = pbVar10;
        }
        goto LAB_40f;
    case 5:
        bVar3 = true;
        param_3 = local_44;
        if (fp_isdigit(bVar7)) { iVar6 = 4; goto LAB_64b; }
        goto LAB_608;
    case 6:
        local_44 = param_3 - 1;
        if ((char)bVar7 < '1' || '9' < (char)bVar7) {
            if (bVar7 == 0x2b) goto LAB_680;
            if (bVar7 == 0x2d) goto LAB_674;
            param_3 = local_44;
            if (bVar7 == 0x30) goto LAB_5e2;
            goto LAB_608;
        }
LAB_649:
        iVar6 = 9;
LAB_64b:
        param_3 = pbVar10 - 1;
        goto LAB_40f;
    case 7:
        if ('0' < (char)bVar7 && (char)bVar7 < ':') goto LAB_649;
        param_3 = local_44;
        if (bVar7 != 0x30) goto LAB_608;
LAB_5e2:
        iVar6 = 8;
        param_3 = pbVar10;
        goto LAB_40f;
    case 8:
        bVar2 = true;
        while (bVar7 == 0x30) bVar7 = *pbVar10++;
        if ('0' < (char)bVar7 && (char)bVar7 < ':') goto LAB_649;
        goto LAB_604;
    case 9:
        bVar2 = true;
        local_50 = 0;
        goto LAB_6c7;
    default:
        goto caseD_a;
    case 0xb:
        if (p7 != 0) {
            local_44 = param_3;
            if (bVar7 == 0x2b) {
LAB_680:
                iVar6 = 7;
                param_3 = pbVar10;
            } else {
                if (bVar7 != 0x2d) goto LAB_608;
LAB_674:
                local_60 = -1;
                iVar6 = 7;
                param_3 = pbVar10;
            }
            goto LAB_40f;
        }
        iVar6 = 10;
        pbVar10 = param_3;
caseD_a:
        param_3 = pbVar10;
        if (iVar6 != 10) goto LAB_40f;
        goto LAB_608;
    }
    if (bVar7 != 0x30) goto LAB_608;
    iVar6 = 1;
    param_3 = pbVar10;
    goto LAB_40f;
LAB_6c7:
    if (fp_isdigit(bVar7)) {
        local_50 = (char)bVar7 - 0x30 + local_50 * 10;
        if (0x1450 < local_50) { local_50 = 0x1451; goto LAB_6ea; }
        bVar7 = *pbVar10++;
        goto LAB_6c7;
    }
LAB_6ea:
    while (fp_isdigit(bVar7)) bVar7 = *pbVar10++;
    param_3 = pbVar10 - 1;
LAB_608:
    *endp = param_3;
    if (bVar1) {
        if (0x18 < local_3c) {
            if (local_d > 4) local_d++;
            pcVar8--;
            local_40++;
            local_3c = 0x18;
        }
        if (local_3c != 0) {
            while (*--pcVar8 == '\0') { local_3c--; local_40++; }
            d2ddraw_crt_fp_a2c4(e, buf, (int)local_3c, xw);
            if (local_60 < 0) local_50 = -local_50;
            uVar5 = (uint32_t)(local_50 + local_40);
            if (!bVar2) uVar5 += p5;
            if (!bVar3) uVar5 -= p6;
            if (0x1450 < (int32_t)uVar5) {
                uVar9 = 0;
                local_2a = 0x7fff;
                memset(x, 0, 12);
                uint32_t v = 0x80000000u;
                memcpy(x + 6, &v, 4);
                local_4c = 2;
                memcpy(x + 10, &local_2a, 2);
                goto LAB_797;
            }
            if (-0x1451 < (int32_t)uVar5) {
                d2ddraw_crt_fp_acbe(e, (int32_t*)xw, (int32_t)uVar5, p4);
                memcpy(&uVar9, x + 2, 4);
                goto LAB_797;
            }
            local_4c = 1;
        }
    } else {
        local_4c = 4;
    }
    memset(x, 0, 12);
    uVar9 = 0;
LAB_797:
    memcpy(out12, x, 2);
    memcpy(out12 + 2, &uVar9, 4);
    memcpy(&local_2a, x + 10, 2);
    local_2a |= uVar4;
    memcpy(out12 + 10, &local_2a, 2);
    memcpy(out12 + 6, x + 6, 4);
    return local_4c;
}
