// src/core/d2cmp/d2cmp_crt_cftof_18cd1.cpp -- MSVC CRT _cftof (%f float-to-string), D2CMP statically linked copy.
#include <cstdint>
#include <cstddef>

struct D2CmpEnv18CD1 {
    void (*fltout2)(uint32_t lo, uint32_t hi, int32_t* strflt, uint32_t* buf24); // 0x19CDB
    void (*fptostr)(char* buf, int32_t ndigit, int32_t* strflt);                 // 0x19BAA
    void (*cftof2)(char* buf, size_t ndigit, char flag);                         // 0x18C35
};

// 1.13c D2CMP+0x18CD1
char* d2cmp_cftof_18cd1(const D2CmpEnv18CD1* e, const uint32_t* dbl, char* buf, size_t ndigit) {
    int32_t st[3] = {0, 0, 0};          // STRFLT: sign, decpt, ... (local_30, local_2c)
    uint32_t tmp[6];
    e->fltout2(dbl[0], dbl[1], st, tmp);
    e->fptostr(buf + (st[0] == '-'), st[1] + (int32_t)ndigit, st);
    e->cftof2(buf, ndigit, 0);
    return buf;
}
