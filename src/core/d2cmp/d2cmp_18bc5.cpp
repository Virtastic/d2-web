#include <cstdint>

// Callees of the MSVC CRT _cftoe wrapper; bound by the host (19CDB = _fltout2, 19BAA = __fptostr, 18B17 = __cftoe2).
int (*g_d2cmp_fltout2)(uint32_t lo, uint32_t hi, int* strflt, uint32_t* buf) = nullptr;
void (*g_d2cmp_fptostr)(char* out, int digits, int* strflt) = nullptr;
void (*g_d2cmp_cftoe2)(int ndigits, int caps, char zero) = nullptr;

// 1.13c D2CMP+0x18BC5
int d2cmp_cftoe(uint32_t* val, int buf, int ndigits, int caps)
{
    int strflt[4];
    uint32_t tmp[6];
    g_d2cmp_fltout2(val[0], val[1], strflt, tmp);
    g_d2cmp_fptostr((char*)(intptr_t)(buf + (ndigits > 0) + (strflt[0] == 0x2d)), ndigits + 1, strflt);
    g_d2cmp_cftoe2(ndigits, caps, 0);
    return buf;
}
