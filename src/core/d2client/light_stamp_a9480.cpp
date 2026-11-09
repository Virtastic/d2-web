// 1.13c D2Client+0xA9480 (992 B): light-node stamp. ESI = light node (+0x10 x, +0x14 y, +0x18 radius, +0x24 intensity, +0x25/26/27 r g b), no stack args.
// Radius 0 < r < 0x100. Aligned box x0 = (x & ~7) - r, y0 = (y & ~7) - r, M = (2r)>>3; returns when the box misses the window {minx 0x104DF0, miny 0x104DF4,
// maxx 0x104DF8, maxy 0x104DFC}. Clears the 0x400-dword grid (D2Client+0xFCDE8), calls 0xA8950 (EDX = node), runs the ring sweep (same shape as 0xA9100: 8 pairs of
// 0xA8470(EAX = node, EDX, stack) + 0xA8310(EAX, ECX) per step) when r>>3 >= 2, then stamps (M+1)^2 cells: where grid < 0x10 the weight
// (8 - grid/2) * (((r - dist) * ((inten << 16) / r)) >> 16) >> 3, dist = (max * 0x3D7 + min * 0x197) >> 10 of |dx|, |dy|, calls 0xA8A40(EAX = x, ECX = y, stack v r g b).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static uint32_t R32(uintptr_t p) { uint32_t v; std::memcpy(&v, (const void*)p, 4); return v; }
static void W32(uintptr_t p, uint32_t v) { std::memcpy((void*)p, &v, 4); }

extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a9480(uintptr_t rec, uintptr_t grid, uintptr_t win, void (*a8950)(uintptr_t rec), void (*a8470)(uintptr_t rec, int32_t edx, int32_t stk),
                                                     void (*a8310)(int32_t eax, int32_t ecx), void (*a8a40)(int32_t x, int32_t y, int32_t v, uint32_t r, uint32_t g, uint32_t b)) {
    int32_t r = (int32_t)R32(rec + 0x18);
    if (r <= 0 || r >= 0x100) return;
    int32_t rx = (int32_t)R32(rec + 0x10), ry = (int32_t)R32(rec + 0x14);
    int32_t x0 = (int32_t)((uint32_t)(rx & ~7) - (uint32_t)r), y0 = (int32_t)((uint32_t)(ry & ~7) - (uint32_t)r), M = (2 * r) >> 3;
    int32_t minx = (int32_t)R32(win), miny = (int32_t)R32(win + 4), maxx = (int32_t)R32(win + 8), maxy = (int32_t)R32(win + 12);
    if ((x0 >> 3) > maxx) return;
    if ((x0 >> 3) + M + 1 < minx) return;
    if ((y0 >> 3) > maxy) return;
    if ((y0 >> 3) + M + 1 < miny) return;
    for (uint32_t i = 0; i < 0x400; i++) W32(grid + i * 4, 0);
    int32_t lim = r >> 3, X = rx >> 3, Y = ry >> 3;
    a8950(rec);
    if (lim >= 2) {
        int32_t a24 = Y * 8 + 0x14, a20 = X * 8 + 0x14, b2c = Y * 8 - 0xc, b28 = X * 8 - 0xc, v6 = 0x1e, ebp = 0x22;
        do {
            int32_t c14 = Y * 8 + 4, c1c = c14, c10 = X * 8 + 4, c18 = c10, ebx = 0x20, edi = 0x20;
            for (int32_t n = ebp - 0x1f; n != 0; n--) {
                a8470(rec, c18, b2c); a8310(v6, edi);
                a8470(rec, c10, b2c); a8310(v6, ebx);
                a8470(rec, c18, a24); a8310(ebp, edi);
                a8470(rec, c10, a24); a8310(ebp, ebx);
                a8470(rec, a20, c1c); a8310(edi, ebp);
                a8470(rec, a20, c14); a8310(ebx, ebp);
                a8470(rec, b28, c1c); a8310(edi, v6);
                a8470(rec, b28, c14); a8310(ebx, v6);
                c10 += 8; c14 += 8; c18 -= 8; c1c -= 8; edi--; ebx++;
            }
            a20 += 8; a24 += 8; b28 -= 8; b2c -= 8; v6--; ebp++;
        } while (ebp - 0x20 <= lim);
    }
    int32_t q = (int32_t)(((uint32_t)(R32(rec + 0x24) & 0xff)) << 16) / r;
    if (M < 0) return;
    uintptr_t row = grid + (uint32_t)(0x20 - (r >> 3)) * 0x104;
    int32_t y = y0;
    for (int32_t k = M + 1; k != 0; k--, row += 0x100, y += 8) {
        int32_t ay = (int32_t)R32(rec + 0x14) - y; if (ay < 0) ay = -ay;
        int32_t x = x0;
        uintptr_t g = row;
        for (int32_t c = M + 1; c != 0; c--, g += 4, x += 8) {
            int32_t gv = (int32_t)R32(g);
            if (gv >= 0x10) continue;
            int32_t ax = (int32_t)R32(rec + 0x10) - x; if (ax < 0) ax = -ax;
            uint32_t dist = ax < ay ? ((uint32_t)ax * 0x197 + (uint32_t)ay * 0x3d7) >> 10 : ((uint32_t)ax * 0x3d7 + (uint32_t)ay * 0x197) >> 10;
            int32_t t = (int32_t)((uint32_t)(r - (int32_t)dist) * (uint32_t)q) >> 16;
            int32_t v = (int32_t)((uint32_t)(8 - (gv >> 1)) * (uint32_t)t) >> 3;
            if (v <= 0) continue;
            a8a40(x, y, v, R32(rec + 0x25) & 0xff, R32(rec + 0x26) & 0xff, R32(rec + 0x27) & 0xff);
        }
    }
}
