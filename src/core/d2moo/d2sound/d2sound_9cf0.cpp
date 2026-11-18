// 1.13c D2sound+0x9CF0 -- CRT math dispatcher (args in ST1/ST0). If the SSE2 flag (DAT_6f9c6600) is set and
// MXCSR&0x1f80==0x1f80 and FPU CW&0x7f==0x7f (all exceptions masked) it calls the SSE2 body 0xB2D0, else the
// x87 body 0x9D4D. Both bodies are unported (0x9D4D no decompile, 0xB2D0 a thunk to 0xB2E9); they are host seams.
#include <cstdint>

double D2sound_9CF0(bool sse2Flag, uint32_t mxcsr, uint16_t fpuCw, double st1, double st0,
                    double (*sse2Body)(double, double), double (*x87Body)(double, double)) {
    if (sse2Flag && (mxcsr & 0x1f80) == 0x1f80 && (fpuCw & 0x7f) == 0x7f)
        return sse2Body(st1, st0);
    return x87Body(st1, st0);
}
