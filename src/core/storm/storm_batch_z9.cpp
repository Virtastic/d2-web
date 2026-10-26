// src/core/storm/storm_batch_z9.cpp -- 1.13c Storm.dll Stage A batch z9 (implementation + marker, no spec,
// no vector): STRANS.CPP transparency-mask / RLE helpers and two UTF-8 continuation-byte scanners.
// Retail 32-bit field offsets kept (links are uint32_t); hidden register params (EAX/ESI) are explicit,
// in decompiler order. Unmapped callees and Storm globals are seams.
#include <cstdint>
#include <cstring>
#include <Windows.h>

namespace storm_z9 {

// ---- seams ----
uint32_t* Storm_Sub1560(void* list, int32_t a, int32_t b);               // 0x01560
void  Storm_Free(void* p, const char* file, int32_t line);               // 0x0B960
void* Storm_Alloc(uint32_t n, const char* file, int32_t line, uint32_t flags);  // 0x0C580
void  Storm_Sub4D60(void* p);                                            // 0x04D60
extern uint8_t* g_list_458BC;          // 0x6FC458BC (list head, address taken)
extern CRITICAL_SECTION g_cs_458C8;    // 0x6FC458C8
extern uint32_t g_trans_a;             // 0x6FC45364
extern int32_t  g_trans_b;             // 0x6FC45368
extern uint32_t g_trans_c;             // 0x6FC45360 (32-bit ptr to row-offset table)
extern int32_t  g_lastError;           // 0x6FC45388
extern int32_t  g_shX, g_shY;          // 0x6FC415E8, 0x6FC415F0
extern int32_t  g_e0, g_e4, g_ec, g_f4;// 0x6FC415E0, E4, EC, F4
extern const char g_autrans[];         // 0x6FC415D0 "...AUTRANS..." tag

static uint32_t rd32(const void* p, uintptr_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static uint16_t rd16(const void* p) { uint16_t v; std::memcpy(&v, p, 2); return v; }
static void setErr(int32_t e) { g_lastError = e; SetLastError(e); }
static const char* kTrans = "..\\3rdParty\\STORM\\SOURCE\\STRANS.CPP";

// 1.13c Storm+0x35C10: scan forward over UTF-8 continuation bytes; retail has no output (result discarded).
void Storm_Sub35C10(const uint8_t* s, int32_t i) {
    for (;;) {
        i = i + 1;
        if (i < 1) return;
        if (s[i] == 0) break;
        if ((s[i] & 0xc0) != 0x80) return;
    }
}

// 1.13c Storm+0x35C40: scan backward over UTF-8 continuation bytes; retail has no output.
void Storm_Sub35C40(const uint8_t* s, int32_t i) {
    for (; 0 < i && s[i] != 0 && (s[i] & 0xc0) == 0x80; i--) {}
}

// 1.13c Storm+0x365A0 (ordinal 378): store a value in a freshly obtained list node under the lock.
uint32_t Storm_Sub365A0(uint32_t v) {
    EnterCriticalSection(&g_cs_458C8);
    uint32_t* n = Storm_Sub1560(&g_list_458BC, 0, 0);
    n[2] = v;
    LeaveCriticalSection(&g_cs_458C8);
    return 1;
}

// 1.13c Storm+0x36730 (ordinal 441): OR a mask byte into the transparency-mask region of an image.
uint32_t Storm_Sub36730(uint8_t* dst, uint8_t mask, int32_t x, int32_t y, const uint8_t* rec, int32_t flag) {
    if (mask == 0 || dst == nullptr || rec == nullptr) { setErr(0x57); return 0; }
    if (g_trans_c != 0 && (int32_t)rd32(rec, 0x18) > 0 && (int32_t)rd32(rec, 0x1c) > 0 && flag == 0) {
        const uint32_t* tab = (const uint32_t*)(uintptr_t)g_trans_c;
        int32_t h   = (int32_t)rd32(rec, 0x2c);
        int32_t x1  = ((int32_t)rd32(rec, 0x28) + x) >> (g_shX & 0x1f);
        int32_t x0  = ((int32_t)rd32(rec, 0x20) + x) >> (g_shX & 0x1f);
        uint8_t sy  = (uint8_t)g_shY;
        for (int32_t r = ((int32_t)rd32(rec, 0x24) + y) >> (sy & 0x1f); r < ((h + y) >> (sy & 0x1f)); r++) {
            uint8_t* p = dst + tab[r] + x0;
            if (x0 < x1) {
                int32_t n = x1 - x0;
                do { *p |= mask; p++; n--; } while (n != 0);
            }
        }
        return 1;
    }
    return 0;
}

// 1.13c Storm+0x36810 (ordinal 442): test whether pixel (x,y) lies in a transparent run of the RLE image.
uint32_t Storm_Sub36810(const uint8_t* rec, int32_t x, int32_t y) {
    if (rec == nullptr) { setErr(0x57); return 0; }
    if (x > -1 && y > -1 && x < (int32_t)rd32(rec, 0x18) && y < (int32_t)rd32(rec, 0x1c)) {
        const uint8_t* p = (const uint8_t*)(uintptr_t)(rd32(rec, 0x14) + rd32(rec, 8));
        for (; y != 0; y--) {
            int16_t s;
            do { s = (int16_t)rd16(p); p += 2; } while (s != 0);
        }
        for (;;) {
            uint8_t b1 = p[0], b2 = p[1];
            p += 2;
            if (b1 == 0 && b2 == 0) break;
            if (x < (int32_t)b1) return 1;
            int32_t t = x - b1;
            if (t < (int32_t)b2) return 0;
            x = t - b2;
        }
    }
    return 0;
}

// Shared RLE row copy used by 0x368B0 / 0x36960. srcAdv/dstAdv: strides added after each row (0x368B0 skips
// source by param_4; 0x36960 has no source stride). The 32-bit-pointer alignment tests are on the dst address
// (retail test (uint)param_1 & 1/2); they only decide copy width, so byte-wise result is identical.
static void rleCopy(uint8_t* dst, const uint8_t* src, const uint8_t* rec, int32_t dstStride, int32_t srcStride, bool advSrc) {
    const uint8_t* cmd = (const uint8_t*)(uintptr_t)(rd32(rec, 0x14) + rd32(rec, 8));
    for (int32_t rows = (int32_t)rd32(rec, 0x1c); rows != 0; rows--) {
        for (;;) {
            uint32_t c = rd16(cmd); cmd += 2;
            if (c == 0) break;
            uint32_t n = c & 0xff;
            std::memcpy(dst, src, n);  // retail: alignment head + n/4 dwords + word + byte tail == n bytes
            dst += n; src += n;
            dst += c >> 8;
            if (advSrc) src += c >> 8;
        }
        dst += dstStride;
        if (advSrc) src += srcStride;
    }
}
// NOTE rleCopy: the decompile's low-byte count n is consumed as head(<=3 odd/2-aligned bytes) + n/4 dwords +
// (n&2) word + (n&1) byte; that sums to exactly n bytes, which is what memcpy(n) does.

// 1.13c Storm+0x368B0: RLE blit with source skip. EAX = rec.
uint32_t Storm_Sub368B0(const uint8_t* rec /*EAX*/, uint8_t* dst, const uint8_t* src, int32_t dstStride, int32_t srcStride) {
    rleCopy(dst, src, rec, dstStride, srcStride, true);
    return 0;
}

// 1.13c Storm+0x36960: RLE decode in place from the record's own data (source = rec+8 base). EAX = rec.
uint32_t Storm_Sub36960(const uint8_t* rec /*EAX*/, uint8_t* dst, int32_t dstStride) {
    const uint8_t* src = (const uint8_t*)(uintptr_t)rd32(rec, 8);
    rleCopy(dst, src, rec, dstStride, 0, false);
    return 0;
}

// 1.13c Storm+0x36A50: build a bounding rect + RLE (transparent/opaque run pairs) of an 8-bit image against a key colour.
// rect (ECX) = {left,top,right,bottom} or null; EAX = out bounds {minX,minY,maxX,maxY}. Output stream in `out` (may be null).
void Storm_Sub36A50(int32_t* bounds /*EAX*/, const int32_t* rect /*ECX*/, int32_t stride /*EDX*/, const int8_t* img,
                    int32_t h, int8_t key, int32_t skipScan, uint8_t* out, uint32_t* sizeOut, uint32_t* total) {
    uint32_t cnt = 0;
    // retail: base = rect ? rect[1]*stride + rect[0] : 0, added to the image pointer (param_3)
    const int8_t* p7 = img + (rect ? (rect[1] * stride + rect[0]) : 0);
    int32_t w = stride;
    if (rect) { h = rect[3] - rect[1]; w = rect[2] - rect[0]; }
    bounds[0] = 0x7fffffff; bounds[1] = 0x7fffffff; bounds[2] = 0; bounds[3] = 0;
    uint8_t* o = out;
    if (skipScan == 0) {
        const int8_t* p5 = p7;
        for (int32_t y = 0; y < h; y++) {
            bool any = false;
            for (int32_t xx = 0; xx < w; xx++, p5++) {
                if (*p5 != key) {
                    cnt++;
                    if (o) *o++ = (uint8_t)*p5;
                    if (xx < bounds[0]) bounds[0] = xx;
                    if (bounds[2] <= xx) bounds[2] = xx + 1;
                    any = true;
                }
            }
            if (any) { if (y < bounds[1]) bounds[1] = y; if (bounds[3] <= y) bounds[3] = y + 1; }
            p5 += stride - w;
        }
    }
    if (bounds[2] < bounds[0]) bounds[0] = bounds[2];
    if (bounds[3] < bounds[1]) bounds[1] = bounds[3];
    uint32_t m = cnt & 3;
    if (m) { if (o) o += 4 - m; cnt += 4 - m; }
    if (total) *total = cnt;
    for (int32_t rows = h; rows != 0; rows--) {
        int32_t left = w;
        for (;;) {
            uint8_t a = 0, b = 0; bool flag = true;
            if (left == 0) {  // end-of-row marker
                cnt += 2;
                if (o) { o[0] = 0; o[1] = 0; o += 2; }
                break;
            }
            for (;;) {
                int32_t l1 = left - 1;
                const int8_t* nx = p7 + 1;
                bool match = ((*p7 != key) == flag);
                if (match) {
                    if (!flag) b++; else a++;
                } else {
                    if (!flag) { goto emit; }   // run ended: pixel not consumed
                    flag = false;
                    b++;
                }
                p7 = nx; left = l1;
                if (a == 0xfc || b == 0xfc || l1 == 0) goto emit;
            }
        emit:
            cnt += 2;
            if (o) { o[0] = a; o[1] = b; o += 2; }
        }
        p7 += stride - w;
    }
    if (sizeOut) *sizeOut = cnt;
}

// 1.13c Storm+0x36C60 (ordinal 432): RLE blit wrapper; strides reduced by the record width at +0x18.
uint32_t Storm_Sub36C60(uint8_t* dst, const uint8_t* src, int32_t dstStride, int32_t srcStride, const uint8_t* rec) {
    int32_t s = srcStride == 0 ? 0 : srcStride - (int32_t)rd32(rec, 0x18);
    Storm_Sub368B0(rec, dst, src, dstStride - (int32_t)rd32(rec, 0x18), s);
    return 1;
}

// 1.13c Storm+0x36CA0 (ordinal 431): RLE decode wrapper at (x,y) of a pitch-`pitch` surface.
uint32_t Storm_Sub36CA0(uint8_t* surf, int32_t x, int32_t y, int32_t pitch, const uint8_t* rec) {
    Storm_Sub36960(rec, surf + (y * pitch + x) /*retail: param_3*param_4 + param_1 + param_2*/, pitch - (int32_t)rd32(rec, 0x18));
    return 1;
}

// 1.13c Storm+0x36CD0 (ordinal 434): free a transparency record; its buffer (+8/+0xC) is recycled as the cached one.
uint32_t Storm_Sub36CD0(uint32_t* rec) {
    if (rec[2] != 0) {
        if (g_trans_a != 0) Storm_Free((void*)(uintptr_t)g_trans_a, kTrans, 0x2ec);
        g_trans_a = rec[2];
        g_trans_b = (int32_t)rec[3];
        rec[2] = 0;
        rec[3] = 0;
    }
    Storm_Sub4D60(rec);
    Storm_Free(rec, g_autrans, -2);
    return 1;
}

// 1.13c Storm+0x36E00 (ordinal 440): (re)build the power-of-two tile row-offset table.
uint32_t Storm_Sub36E00(int32_t totalW, int32_t totalH, int32_t tileW, int32_t tileH) {
    if (g_trans_c != 0) { Storm_Free((void*)(uintptr_t)g_trans_c, kTrans, 0x508); g_trans_c = 0; }
    int32_t p = 1; g_shX = 0;
    if (1 < tileW) do { p *= 2; g_shX++; } while (p < tileW);
    if (p != tileW) return 0;
    p = 1; g_shY = 0;
    if (1 < tileH) do { p *= 2; g_shY++; } while (p < tileH);
    if (p != tileH) return 0;
    g_e0 = ((1 << (g_shX & 0x1f)) - 1 + totalW) >> (g_shX & 0x1f);
    g_ec = tileW;
    g_f4 = tileH;
    g_e4 = ((1 << (g_shY & 0x1f)) - 1 + totalH) >> (g_shY & 0x1f);
    uint32_t* t = (uint32_t*)Storm_Alloc((uint32_t)g_e4 * 4, kTrans, 0x517, 0);
    int32_t n = g_e4, step = g_e0;
    uint32_t acc = 0;
    g_trans_c = (uint32_t)(uintptr_t)t;
    for (int32_t i = 0; i < n; i++) { t[i] = acc; acc += step; }
    return 1;
}

}  // namespace storm_z9
