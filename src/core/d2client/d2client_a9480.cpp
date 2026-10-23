// 1.13c D2Client+0xA9480 (992 B, caller 0xA9B50): light-record stamp. ESI = light record
// (+0x10 x, +0x14 y, +0x18 radius, +0x24 intensity, +0x25/26/27 r,g,b). Mapping lane: globals are
// parameters, callees are host callbacks. UNVERIFIED: the EAX/ECX register args of the a8470/a8310
// calls are lost in the decompile; the 8 sweeps below pass the printed args (x, y, y2) / (n) as is.
#include <cstdint>
#include <emscripten.h>

static inline uint32_t R32(uintptr_t p) { return *(const uint32_t*)p; }
typedef void (*Fn470)(uintptr_t rec, int32_t x, int32_t y);   // d2_client_a8470 (edx=x, stack=y)
typedef void (*Fn310)(int32_t n);                              // d2_client_a8310
typedef void (*Fn950)(uintptr_t rec);                          // d2_client_a8950 (clear+fill grid)
typedef void (*FnA40)(int32_t x, int32_t y, int32_t a, uint8_t r, uint8_t g, uint8_t b);

// tbl = DAT_6fbacde8 (int32[0x400], row stride 0x41 after the 0x20-row offset), min/max = DAT_6fbb4df0/4df4/4df8/4dfc.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_a9480(uintptr_t rec, int32_t* tbl, int32_t minx, int32_t miny, int32_t maxx, int32_t maxy,
                     Fn950 f950, Fn470 f470, Fn310 f310, FnA40 fa40) {
    int32_t r = (int32_t)R32(rec + 0x18);
    if (r <= 0 || r >= 0x100) return;
    int32_t x0 = (int32_t)((R32(rec + 0x10) & ~7u) - (uint32_t)r);
    int32_t y0 = (int32_t)((R32(rec + 0x14) & ~7u) - (uint32_t)r);
    int32_t n = (r * 2) >> 3;
    if (!(x0 >> 3 <= maxx && minx <= (x0 >> 3) + 1 + n && (y0 >> 3) <= maxy && miny <= (y0 >> 3) + 1 + n)) return;
    for (int i = 0; i < 0x400; i++) tbl[i] = 0;
    int32_t cy = (int32_t)R32(rec + 0x14) >> 3, cx = (int32_t)R32(rec + 0x10) >> 3, rr = (int32_t)R32(rec + 0x18) >> 3;
    f950(rec);
    if (rr > 1) {
        int32_t a = cy * 8 + 0x14, b = cy * 8 - 0xc, c = cx * 8 + 0x14, d = cx * 8 - 0xc, e = 0x1e, k = 0x22;
        for (;;) {
            if (k - 0x20 >= 0) {
                int32_t py = cy * 8 + 4, px = cx * 8 + 4, i10 = 0x20, i16 = 0x20, p38 = py, p34 = px;
                for (int32_t m = k - 0x1f; m != 0; m--) {
                    f470(rec, py, b);       f310(i16);
                    f470(rec, px, b);       f310(i10);
                    f470(rec, p34, a);      f310(i16);
                    f470(rec, px, a);       f310(i10);
                    f470(rec, c, py);       f310(k);
                    f470(rec, c, p38);      f310(k);
                    f470(rec, d, py);       f310(e);
                    f470(rec, d, p38);      f310(e);
                    px += 8; p38 += 8; p34 -= 8; py -= 8; i16--; i10++;
                }
            }
            c += 8; a += 8; d -= 8; b -= 8; e--;
            int32_t t = k - 0x1f; k++;
            if (!(t <= rr)) break;
        }
    }
    uint8_t inten = *(const uint8_t*)(rec + 0x24);
    if (n < 0) return;
    int32_t* row = tbl + (0x20 - (r >> 3)) * 0x41;
    int32_t y = y0;
    for (int32_t j = 0; j <= n; j++, row += 0x40, y += 8) {
        uint32_t dy = (uint32_t)(int32_t)R32(rec + 0x14) - (uint32_t)y;
        int32_t ay = (int32_t)dy < 0 ? -(int32_t)dy : (int32_t)dy;
        int32_t x = x0;
        for (int32_t i = 0; i <= n; i++, x += 8) {
            if (row[i] >= 0x10) continue;
            int32_t dx = (int32_t)R32(rec + 0x10) - x, ax = dx < 0 ? -dx : dx;
            uint32_t dist = ax < ay ? (uint32_t)ay * 0x3d7 + (uint32_t)ax * 0x197 : (uint32_t)ax * 0x3d7 + (uint32_t)ay * 0x197;
            int32_t q = (int32_t)(((int64_t)inten << 16) / r);
            int32_t v = (8 - (row[i] >> 1)) * (int32_t)(((int32_t)(R32(rec + 0x18) - (dist >> 10)) * q) >> 16) >> 3;
            if (v > 0) fa40(x, y, v, *(const uint8_t*)(rec + 0x25), *(const uint8_t*)(rec + 0x26), *(const uint8_t*)(rec + 0x27));
        }
    }
}
