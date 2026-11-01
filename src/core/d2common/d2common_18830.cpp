// 1.13c D2Common+0x18830 -- stdcall (info, startPt, buf, &idx, &end, maxLen, majorDir), ret 0x1C.
// Two wall-following walkers (D2PathMovStrc, 210 dwords) leave startPt toward buf[*idx], one turning each way,
// and advance alternately (a walker that is tracking the other, [7] != 0, keeps the turn) until one finishes.
// A walker that steps back onto the main path ahead (majorDir picks the axis) splices its points in with
// 0x18400 and returns its point count. Otherwise, for maxLen <= 0x50, the walker whose last point is nearer the
// target (info[1]) is copied over buf[*idx..] unless the start (info[0]) is nearer still. Points pack x low, y high.
#include "d2common_186d0.h"
#include <cstdlib>
#include <cstring>
#include <emscripten.h>

extern "C" void D2Common_18400_InsertPoints(uint8_t* self, uint8_t* arr, int32_t* pos, int32_t* cnt, int32_t idx);

namespace {
const int32_t kPrev1[16] = { 6, 6, 0, 0, 2, 2, 4, 4, 6, 6, 0, 0, 2, 2, 4, 4 };   // 0x6FDE5158
const int32_t kNext1[16] = { 2, 2, 4, 4, 6, 6, 0, 0, 2, 2, 4, 4, 6, 6, 0, 0 };   // 0x6FDE50D8
const int32_t kPrev2[16] = { 2, 2, 4, 4, 6, 6, 0, 0, 2, 2, 4, 4, 6, 6, 0, 0 };   // 0x6FDE5118
const int32_t kNext2[16] = { 6, 0, 0, 2, 2, 4, 4, 6, 6, 0, 0, 2, 2, 4, 4, 6 };   // 0x6FDE5098
const int32_t kOffToDir[9] = { 7, 0, 1, 6, -1, 2, 5, 4, 3 };                     // 0x6FDE5198, indexed +4
inline uint16_t lo(uint32_t v) { return uint16_t(v); }
inline uint16_t hi(uint32_t v) { return uint16_t(v >> 16); }
inline int16_t s16(uint32_t v) { return int16_t(uint16_t(v)); }
inline int16_t s16h(uint32_t v) { return int16_t(uint16_t(v >> 16)); }
inline uint32_t W32At(uint32_t a, uint32_t o) { return *reinterpret_cast<const uint32_t*>(uintptr_t(a + o)); }
}

extern "C" EMSCRIPTEN_KEEPALIVE int32_t D2Common_18830_FindSubpathWithoutObstacles(uint32_t pInfo, uint32_t startPt,
    uint32_t* buf, int32_t* pIdx, int32_t* pEnd, int32_t maxLen, int32_t majorDir,
    const D2CommonWalkEnv* env) {   // 1.13c D2Common+0x18830
    uint32_t w0[210], w1[210];
    memset(w0, 0, sizeof w0); memset(w1, 0, sizeof w1);
    w0[5] = uint32_t(uintptr_t(kPrev1)); w0[6] = uint32_t(uintptr_t(kNext1));
    w1[5] = uint32_t(uintptr_t(kPrev2)); w1[6] = uint32_t(uintptr_t(kNext2));
    const uint32_t sx = lo(startPt), sy = hi(startPt);
    const uint32_t nxt = buf[*pIdx];
    const int32_t t = kOffToDir[4 + int32_t((hi(nxt) - sy) * 3 - sx + lo(nxt))];
    w0[0] = uint32_t(t + 1); w1[0] = uint32_t(t + 7);
    w0[2] = startPt; w1[2] = startPt;
    uint32_t* cur = w0; uint32_t* oth = w1;
    do {
        if (cur[8] == 0) {
            if (!D2Common_186D0_StepWalker(pInfo, cur, env)) {
                cur[8] = 1;
            } else {
                bool axis = false;
                int32_t d = 0;
                if (int32_t(cur[1]) > 0) {
                    axis = true;
                    switch (majorDir) {
                    case 0: d = int32_t(sy) - int32_t(hi(cur[3])); break;
                    case 1: d = int32_t(lo(cur[3])) - int32_t(sx); break;
                    case 2: d = int32_t(hi(cur[3])) - int32_t(sy); break;
                    case 3: d = int32_t(sx) - int32_t(lo(cur[3])); break;
                    default: axis = false; break;
                    }
                }
                if (axis && d > 0) {
                    const int32_t m = d + *pIdx - 1;
                    if (m < *pEnd && s16(cur[3]) == s16(buf[m]) && s16h(cur[3]) == s16h(buf[m])) {
                        if (int32_t(cur[1]) >= maxLen) abort();   // retail: error 0x1BB, _exit(-1)
                        cur[9 + cur[1]] = cur[3];
                        cur[1]++;
                        reinterpret_cast<uint16_t*>(&cur[9 + cur[1]])[0] = 0;
                        D2Common_18400_InsertPoints(reinterpret_cast<uint8_t*>(cur), reinterpret_cast<uint8_t*>(buf),
                                                    pIdx, pEnd, m);
                        return int32_t(cur[1]);
                    }
                }
                if (int32_t(oth[1]) > 1) {
                    const uint32_t k = cur[7];
                    if (k) {
                        if (s16(cur[3]) == s16(oth[7 + k]) && s16h(cur[3]) == s16h(oth[7 + k])) {
                            reinterpret_cast<uint16_t*>(&buf[*pIdx])[0] = 0;
                            *pEnd = *pIdx;
                            return 0;
                        }
                        cur[7] = 0;
                    }
                    if (s16(cur[3]) == s16(oth[2]) && s16h(cur[3]) == s16h(oth[2])) cur[7] = oth[1];
                }
                cur[9 + cur[1]] = cur[3];
                cur[2] = cur[3];
                cur[0] = uint32_t(reinterpret_cast<const int32_t*>(uintptr_t(cur[5]))[cur[0]]);
                cur[1]++;
                if (int32_t(cur[1]) >= maxLen - *pEnd - 1) cur[8] = 1;
            }
        }
        if (cur[7] == 0) { uint32_t* s = cur; cur = oth; oth = s; }
    } while (w0[8] == 0 && w1[8] == 0);
    if (maxLen > 0x50) {
        reinterpret_cast<uint16_t*>(&buf[*pIdx])[0] = 0;
        *pEnd = *pIdx;
        return 0;
    }
    const uint32_t tgt = W32At(pInfo, 4);
    auto dist = [&](uint32_t p) {
        const int32_t dx = int32_t(lo(p)) - int32_t(lo(tgt)), dy = int32_t(hi(p)) - int32_t(hi(tgt));
        return dy * dy + dx * dx;
    };
    const int32_t d0 = dist(w0[8 + w0[1]]), d1 = dist(w1[8 + w1[1]]), ds = dist(W32At(pInfo, 0));
    const uint32_t* win;
    if (d1 > d0) { if (ds < d0) return 0; win = w0; }
    else         { if (ds < d1) return 0; win = w1; }
    const int32_t n = int32_t(win[1]);
    if (n > 0) memcpy(&buf[*pIdx], &win[9], uint32_t(n) * 4);
    *pIdx += n;
    *pEnd = *pIdx;
    reinterpret_cast<uint16_t*>(&buf[*pIdx])[0] = 0;
    return int32_t(cur[1]);
}
