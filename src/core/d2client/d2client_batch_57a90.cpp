// 1.13c D2Client+0x58DE0: showitems label placement. __thiscall(ecx=minX); stack (pL,pT,pR,pB,mode,maxX). Clamps the label
// rect to the screen then slides it off any earlier label rect. rects = DAT_6fbcfef8 stride 0x48 words {l,t,r,b}; n = DAT_6fbc7800;
// scrH = DAT_6fbc4c4c-style screen height (DAT_6fb8bc4c). Returns 1 = placed clear, 0 = moved. n>0x1f is a fatal in retail (not modelled).
#include <stdint.h>
#include <emscripten.h>
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_58de0(int32_t minX, int32_t* pL, int32_t* pT, int32_t* pR, int32_t* pB,
                                                        int32_t mode, int32_t maxX, const int32_t* rects, int32_t n, int32_t scrH) {
    int32_t lim = scrH - 0x30;
    if (*pL < minX) { *pR = minX + (*pR - *pL); *pL = 6; }          // retail stores 6, not minX (as decompiled)
    if (*pT < 6) { *pB += 6 - *pT; *pT = 6; }
    if (maxX <= *pR) { int32_t d = *pR - maxX + 1; *pR -= d; *pL -= d; }
    if (lim <= *pB) { int32_t d = *pB - lim; *pB -= d; *pT -= d; }
    for (int32_t i = 0; i < n; i++) {
        const int32_t* r = rects + i * 0x48;
        if (r[2] < *pL || *pR < r[0] || r[3] < *pT || *pB < r[1]) continue;
        if (mode < 7) {
            int32_t d;
            if ((*pB - *pT) / 2 != 0 && *pT <= lim) d = (r[1] - *pB) - 3; else d = (r[3] - *pT) + 5;
            *pT += d; *pB += d; return 0;
        }
        int32_t d;
        if ((*pR - *pL) / 2 != 0 && maxX <= *pL) d = (r[0] - *pR) - 1; else d = (r[2] - *pL) + 1;
        *pL += d; *pR += d; return 0;
    }
    return 1;
}
