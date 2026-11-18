// 1.13c Fog+0xB7F8: void __cdecl(double* value, char* out, size_t n, int caps) -- the CRT %g formatter (_cftog). 0xBF1C(lo, hi, &st, &buf) splits the
// double (st[0] = sign character 0x2D for a negative value, st[1] = decimal point position); the digits go to out + (st[0] == 0x2D) through
// 0xBDEB(digits, n, &st). With e = st[1] - 1: e below -4 or at least n (signed) formats with 0xB5D7 (EAX = &st, stack n, caps, 1); otherwise, when the digit
// string got shorter than before (st[1] - 1 > the first estimate), the last digit before the NUL is cleared first, and 0xB6F5 (EAX = &st, stack out, n, 1)
// formats. All four callees are cdecl; st and buf are the caller's locals.
#include "fog_env.h"
extern "C" void fog_b7f8(const FogEnv* e, uint32_t value, uint32_t out, uint32_t n, uint32_t caps) {
    uint32_t st[4] = {0, 0, 0, 0}, buf[6] = {0, 0, 0, 0, 0, 0};
    uint32_t pst = (uint32_t)(uintptr_t)st;
    fe_call(e, 0xBF1C, 0, 0, 0, 0, 0, fe_r32(value), fe_r32(value + 4), pst, (uint32_t)(uintptr_t)buf);
    int32_t before = (int32_t)st[1] - 1;
    uint32_t p = out + (st[0] == 0x2d ? 1 : 0);
    fe_call(e, 0xBDEB, 0, 0, 0, 0, 0, p, n, pst);
    int32_t ex = (int32_t)st[1] - 1;
    if (ex < -4 || ex >= (int32_t)n) {
        fe_call(e, 0xB5D7, pst, 0, 0, 0, 0, n, caps, 1);
    } else {
        if (before < ex) {
            uint8_t c;
            do { c = *fe_p(p); p++; } while (c != 0);
            *fe_p(p - 2) = 0;
        }
        fe_call(e, 0xB6F5, pst, 0, 0, 0, 0, out, n, 1);
    }
}
