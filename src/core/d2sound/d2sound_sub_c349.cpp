// 1.13c D2sound+0x0C349 -- CRT libm error dispatch after an x87 op (caller: 0x9D4D).
// The decompile loses the FPU inputs; they are explicit parameters here. Callees are host seams.
#include <cstdint>
#include <cstring>

struct D2soundC349Env {
    double (*fscale)(double x, double k);   // x87 FSCALE
    double kDenorm;                         // _DAT_6f9c437c
    double kInf;                            // _DAT_6f9c4374
    void (*c200)(int code);                 // FUN_6f9bc200 (code 0x1d)
    void (*oneArgErr)(int code);            // __startOneArgErrorHandling
};

void D2sound_sub_C349(double st0, uint16_t cw, uint16_t sw, int code, const D2soundC349Env* e) {
    uint64_t b; memcpy(&b, &st0, 8);
    if ((b & 0x7ff0000000000000ull) == 0) e->fscale(st0, e->kDenorm);
    else if (((b >> 32) & 0x7ff00000) == 0x7ff00000) e->fscale(st0, e->kInf);
    else if (cw == 0x27f || (cw & 0x20) || !(sw & 0x20)) return;
    if (code == 0x1d) e->c200(code); else e->oneArgErr(code);
}
