#include <cstdint>

// Callees bound by the host: 6263 = ___dtold (double -> 80-bit), 1A593 = $I10_OUTPUT (digits), 6D90 = strcpy.
void (*g_d2cmp_dtold)(uint32_t* out3, uint32_t* dbl) = nullptr;
int (*g_d2cmp_i10_output)(uint32_t m0, uint32_t m1, uint16_t m2, int ndigits, int flags, void* res) = nullptr;
void (*g_d2cmp_strcpy)(uint32_t* dst, const void* src) = nullptr;

// 1.13c D2CMP+0x19CDB
int* d2cmp_fltout2(uint32_t dlo, uint32_t dhi, int* strflt, uint32_t* buf)
{
    uint32_t dbl[2] = {dlo, dhi};
    uint32_t ld[3];
    struct { int16_t decpt; char sign; char pad; uint32_t digits[6]; } r;
    g_d2cmp_dtold(ld, dbl);
    int flag = g_d2cmp_i10_output(ld[0], ld[1], (uint16_t)ld[2], 0x11, 0, &r);
    strflt[2] = flag;
    strflt[0] = (int)r.sign;
    strflt[1] = (int)r.decpt;
    g_d2cmp_strcpy(buf, r.digits);
    strflt[3] = (int)(intptr_t)buf;
    return strflt;
}
