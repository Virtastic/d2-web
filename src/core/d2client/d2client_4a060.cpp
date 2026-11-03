// 1.13c D2Client+0x4A060 -- EAX = menu index, stack dword (ret 4) = insert flag. Gate: BF8C() (D2Common thunk, host seam) non-zero and [119854] non-zero, else 0.
// Per index a 0x27-byte record at F3B90+idx*0x27: count dword at +0x14 (F3BA4), words at +0x18 (F3BA8), handler dwords at +0x22 (F3BB2).
// flag != 0: nothing when handler 46D10 is already listed; else the entry for 49440 (if listed) shifts up and 46D10 / word 0x1507 is inserted at its place,
// otherwise it goes in front of the last entry (the last one shifts up once). flag == 0: the 46D10 entry (first match) is removed by shifting the rest down. Returns 1.
// g points at the data address F3B90 (the reads below can reach one entry before the handler array, as the retail loop does).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_hcall.h"
namespace {
inline uint32_t rd4(const uint8_t* p) { uint32_t v; std::memcpy(&v, p, 4); return v; }
inline void wr4(uint8_t* p, uint32_t v) { std::memcpy(p, &v, 4); }
inline void cp2(uint8_t* d, const uint8_t* s) { std::memcpy(d, s, 2); }
}
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2c_4a060(D2cHost h, int32_t idx, uint32_t flag, uint8_t* g, uint32_t g119854, uint32_t fn46d10, uint32_t fn49440) {
    if (!d2cCall(h, 0xBF8C) || !g119854) return 0;
    uint8_t* rec = g + (int32_t)(idx * 0x27);
    uint8_t* cnt = rec + 0x14; uint8_t* wd = rec + 0x18; uint8_t* hd = rec + 0x22;
    int32_t n = (int32_t)rd4(cnt);
    if (flag) {
        for (int32_t i = 0; i < n; ++i) if (rd4(hd + 4 * i) == fn46d10) return 1;
        int32_t at = -1;
        for (int32_t i = 0; i < n; ++i) if (rd4(hd + 4 * i) == fn49440) { at = i; break; }
        if (at >= 0) {
            for (int32_t k = n - 1; k >= at; --k) { wr4(hd + 4 * (k + 1), rd4(hd + 4 * k)); cp2(wd + 2 * (k + 1), wd + 2 * k); }
            wr4(cnt, rd4(cnt) + 1); wr4(hd + 4 * at, fn46d10); uint16_t w = 0x1507; std::memcpy(wd + 2 * at, &w, 2);
            return 1;
        }
        int32_t e = n - 1;
        wr4(hd + 4 * (e + 1), rd4(hd + 4 * e)); cp2(wd + 2 * (e + 1), wd + 2 * e);
        wr4(cnt, rd4(cnt) + 1); wr4(hd + 4 * e, fn46d10); uint16_t w = 0x1507; std::memcpy(wd + 2 * e, &w, 2);
        return 1;
    }
    for (int32_t i = 0; i < n; ++i) {
        if (rd4(hd + 4 * i) != fn46d10) continue;
        for (int32_t k = i; k < (int32_t)rd4(cnt) - 1; ++k) { wr4(hd + 4 * k, rd4(hd + 4 * (k + 1))); cp2(wd + 2 * k, wd + 2 * (k + 1)); }
        wr4(cnt, rd4(cnt) - 1);
        return 1;
    }
    return 1;
}
