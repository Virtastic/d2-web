// src/core/d2game/d2game_batch_F6782.cpp -- twelve 1.13c D2Game.dll CRT float-conversion functions
// (VS2003 __ld12cvt / __fptostr / ___dtold / __control87 family) transcribed from the cached Ghidra
// decompiles, RVA order. Batch lane: implementation + marker only, no spec, no vector, no test runner.
// Callees outside the batch (__IsZeroMan 0xF66EE, __CopyMan 0xF66D3, __RoundMan 0xF6661, __ShrMan
// 0xF6707, FUN_6fd16e63 0xF6E63, FUN_6fd172a3 0xF72A3, FUN_6fc272b0, strlen, memmove) are seams on
// D2GameBatchF6782Env. Register-only args (BX / EBX control word) are explicit parameters.
#include <cstdint>
#include <cstring>

struct D2GameBatchF6782Env {
    int      (*IsZeroMan)(uint32_t* man);
    void     (*CopyMan)(uint32_t* dst, const uint32_t* src);
    int      (*RoundMan)(uint32_t* man, int nbit);
    void     (*ShrMan)(uint32_t* man, int n);
    void     (*fltin_ld12)(uint16_t* ld12, int* consumed, const uint8_t* str); // FUN_6fd16e63(.., 0,0,0,0)
    uint32_t (*fp_digits)(uint32_t a, uint32_t b, uint16_t c, int ndig, int, int16_t* strflt); // FUN_6fd172a3
    void     (*copy_digits)(uint32_t* dst, const uint32_t* src);                              // FUN_6fc272b0
    const int* tbl_double; // DAT_6fd2fa20
    const int* tbl_float;  // DAT_6fd2fa38 (the 0xF68F0 caller passes this table)
};

// 1.13c D2Game+0xF6782
uint32_t d2game_ld12cvt(const D2GameBatchF6782Env& E, const uint16_t* p1, uint32_t* p2, const int* p3)
{
    uint16_t uVar1 = p1[5];
    uint32_t local_10, local_c, local_1c[3];
    int32_t local_8;
    std::memcpy(&local_10, p1 + 3, 4);
    std::memcpy(&local_c, p1 + 1, 4);
    local_8 = (int32_t)((uint32_t)p1[0] << 16);
    uint32_t uVar4 = uVar1 & 0x7fff;
    int iVar5 = (int)uVar4 - 0x3fff;
    uint32_t uVar3;
    bool done = false;
    if (iVar5 == -0x3fff) {
        iVar5 = 0;
        if (E.IsZeroMan(&local_10) != 0) { uVar3 = 0; done = true; }
        else { local_10 = 0; local_c = 0; }
    } else {
        E.CopyMan(local_1c, &local_10);
        if (E.RoundMan(&local_10, p3[2]) != 0) iVar5 = (int)uVar4 - 0x3ffe;
        int iVar2 = p3[1];
        if (iVar5 < iVar2 - p3[2]) { local_10 = 0; local_c = 0; }
        else if (iVar2 < iVar5) {
            done = true;
            if (p3[0] <= iVar5) {
                local_c = 0; local_8 = 0; local_10 = 0x80000000;
                E.ShrMan(&local_10, p3[3]);
                iVar5 = p3[5] + p3[0];
                uVar3 = 1;
            } else {
                local_10 &= 0x7fffffff;
                iVar5 = p3[5] + iVar5;
                E.ShrMan(&local_10, p3[3]);
                uVar3 = 0;
            }
        } else {
            E.CopyMan(&local_10, local_1c);
            E.ShrMan(&local_10, iVar2 - iVar5);
            E.RoundMan(&local_10, p3[2]);
            E.ShrMan(&local_10, p3[3] + 1);
        }
    }
    if (!done) { iVar5 = 0; uVar3 = 2; }
    local_10 = ((uint32_t)iVar5 << ((0x1fu - (uint8_t)p3[3]) & 0x1f))
             | ((uVar1 & 0x8000) ? 0x80000000u : 0u) | local_10;
    if (p3[4] == 0x40) { p2[1] = local_10; p2[0] = local_c; }
    else if (p3[4] == 0x20) p2[0] = local_10;
    (void)local_8;
    return uVar3;
}

// 1.13c D2Game+0xF68DA
uint32_t d2game_ld12tod_F68DA(const D2GameBatchF6782Env& E, const uint16_t* ifp, uint32_t* d)
{ return d2game_ld12cvt(E, ifp, d, E.tbl_double); }

// 1.13c D2Game+0xF68F0
uint32_t d2game_ld12tod_F68F0(const D2GameBatchF6782Env& E, const uint16_t* ifp, uint32_t* d)
{ return d2game_ld12cvt(E, ifp, d, E.tbl_float); }

// 1.13c D2Game+0xF6906
void d2game_atodbl_F6906(const D2GameBatchF6782Env& E, uint32_t* out, const uint8_t* str)
{
    int consumed; uint16_t ld[6];
    E.fltin_ld12(ld, &consumed, str);
    d2game_ld12tod_F68DA(E, ld, out);
}

// 1.13c D2Game+0xF694B
void d2game_atodbl_F694B(const D2GameBatchF6782Env& E, uint32_t* out, const uint8_t* str)
{
    int consumed; uint16_t ld[6];
    E.fltin_ld12(ld, &consumed, str);
    d2game_ld12tod_F68F0(E, ld, out);
}

// 1.13c D2Game+0xF6990  (digits = STRFLT: +4 decpt, +0xc mantissa string)
void d2game_fptostr(char* buf, int ndigits, uint8_t* strflt)
{
    char* mant = *(char**)(strflt + 0xc);
    char* str = buf + 1;
    *buf = '0';
    char* p = str;
    int n = ndigits;
    if (0 < ndigits) {
        n = 0;
        for (int i = ndigits; i; --i) {
            char c = *mant;
            if (c == 0) c = '0'; else ++mant;
            *p++ = c;
        }
    }
    *p = 0;
    if (-1 < n && '4' < *mant) {
        while (*--p == '9') *p = '0';
        *p += 1;
    }
    if (*buf == '1') ++*(int*)(strflt + 4);
    else std::memmove(buf, str, std::strlen(str) + 1);
}

// 1.13c D2Game+0xF6A07
void d2game_dtold(uint32_t* out, const uint32_t* in)
{
    uint16_t hi = (uint16_t)(in[1] >> 16);
    uint32_t local_8 = 0x80000000;
    uint16_t e = (hi >> 4) & 0x7ff;
    uint32_t lo = in[0];
    if (e == 0) {
        if ((in[1] & 0xfffff) == 0 && lo == 0) { out[1] = 0; out[0] = 0; *(uint16_t*)(out + 2) = 0; return; }
        e += 0x3c01; local_8 = 0;
    } else if (e == 0x7ff) e = 0x7fff;
    else e += 0x3c00;
    out[1] = (lo >> 0x15) | ((in[1] & 0xfffff) << 0xb) | local_8;
    out[0] = lo << 0xb;
    while (local_8 == 0) {
        uint32_t t = out[1];
        --e;
        out[1] = (t << 1) | (out[0] >> 0x1f);
        out[0] *= 2;
        local_8 = (t << 1) & 0x80000000;
    }
    *(uint16_t*)(out + 2) = (hi & 0x8000) | e;
}

// 1.13c D2Game+0xF6AC1  (param_1/2 = the double, stack-passed)
int* d2game_fltout2(const D2GameBatchF6782Env& E, uint32_t dlo, uint32_t dhi, int* p3, uint32_t* p4)
{
    uint32_t d[2] = {dlo, dhi}, ld[3] = {0, 0, 0};
    int16_t sf[2 + 1 + 24]; uint32_t digits[6];
    d2game_dtold(ld, d);
    int32_t r = (int32_t)E.fp_digits(ld[0], ld[1], (uint16_t)ld[2], 0x11, 0, sf);
    p3[2] = r;
    p3[0] = (int)(int8_t)((char*)sf)[2];
    p3[1] = (int)sf[0];
    E.copy_digits(p4, digits);
    p3[3] = (int)(intptr_t)p4;
    return p3;
}

// 1.13c D2Game+0xF6B39  (cw = BX on entry)
uint32_t d2game_abstract_cw(uint16_t cw)
{
    uint32_t r = 0;
    if (cw & 1) r = 0x10;
    if (cw & 4) r |= 8;
    if (cw & 8) r |= 4;
    if (cw & 0x10) r |= 2;
    if (cw & 0x20) r |= 1;
    if (cw & 2) r |= 0x80000;
    uint16_t rc = cw & 0xc00;
    if (rc == 0x400) r |= 0x100; else if (rc == 0x800) r |= 0x200; else if (rc == 0xc00) r |= 0x300;
    if ((cw & 0x300) == 0) r |= 0x20000; else if ((cw & 0x300) == 0x200) r |= 0x10000;
    if (cw & 0x1000) r |= 0x40000;
    return r;
}

// 1.13c D2Game+0xF6BCB  (v = EBX on entry)
uint32_t d2game_hw_cw(uint32_t v)
{
    uint32_t r = (v & 0x10) != 0;
    if (v & 8) r |= 4;
    if (v & 4) r |= 8;
    if (v & 2) r |= 0x10;
    if (v & 1) r |= 0x20;
    if (v & 0x80000) r |= 2;
    uint32_t pc = v & 0x300;
    if (pc == 0x100) r |= 0x400; else if (pc == 0x200) r |= 0x800; else if (pc == 0x300) r |= 0xc00;
    if ((v & 0x30000) == 0) r |= 0x300; else if ((v & 0x30000) == 0x10000) r |= 0x200;
    if (v & 0x40000) r |= 0x1000;
    return r;
}

// 1.13c D2Game+0xF6C59  (hwcw = fnstcw result; __hw_cw's result is applied via fldcw, no x87 here)
uint32_t d2game_control87(uint16_t hwcw, uint32_t newv, uint32_t mask)
{
    uint32_t cur = d2game_abstract_cw(hwcw);
    return (cur & ~mask) | (newv & mask);
}

// 1.13c D2Game+0xF6C8B
uint32_t d2game_controlfp(uint16_t hwcw, uint32_t newv, uint32_t mask)
{ return d2game_control87(hwcw, newv, mask & 0xfff7ffff); }
