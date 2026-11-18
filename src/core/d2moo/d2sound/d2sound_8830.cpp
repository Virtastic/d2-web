// 1.13c D2sound+0x8830 -- distance attenuation curve (stdcall, three floats, result in ST0).
// Near bound + bias at or above the distance gives the floor (-10000); far bound - bias at or below it gives 0;
// otherwise -2000 * log10((near + far) / divisor), the divisor being the distance (the far bound when the distance is 0).
// Constants are the image's rdata words at 0x10970..0x1098F; the FDIV-check divide path (DAT_65e8) computes the same quotient.
#include <cmath>
#include <cstdint>

float D2Sound_8830(float a, float b, float c) {
    const double bias = (double)1e-4f;                       // DAT_6f9c097c
    if (!((double)a + bias < (double)c)) return -10000.0f;   // DAT_6f9c0970
    if (!((double)b - bias > (double)c)) return 0.0f;        // DAT_6f9c0978 (also on unordered)
    const double div = (c == 0.0f) ? (double)b : (double)c;  // fucompp against the qword 0.0 at 0x10988
    return (float)(std::log10(((double)a + (double)b) / div) * -2000.0);   // fldlg2/fyl2x, fmul qword -2000.0
}
