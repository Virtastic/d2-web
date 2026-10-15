#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x11360 (fastcall, ECX = n): step the scroll position P [0x6FBCC534] by sign(n), |n| times,
// then rebuild the thumb y [0x6FBD33F4]. M = d2_client_11240(g) (g-15, or 0). D = [0x6FBCC500], H = [0x6FBA9E18].
// n==0 only recomputes. M==0 divides by zero in retail; callers must not pass it (not modelled).
static int32_t m11240(int32_t g) { for (int i = 0; i < 15; i++) { g = (int32_t)((uint32_t)g - 1); if (g <= 0) return 0; } return g; }
static int32_t step(int32_t dir, int32_t M, int32_t p) {
    int32_t q = p;
    if (dir < 0) { if (0 < p) q = p - 1; if (q < 0) return p; }
    else { if (p < M) q = p + 1; if (M < q) return p; }
    return q;
}
static int32_t newp(int32_t n, int32_t g, int32_t p) {
    int32_t M = m11240(g);
    if (n < 0) for (int32_t k = n; k != 0; k++) p = step(-1, M, p);
    else for (int32_t k = n; k > 0; k--) p = step(1, M, p);
    return p;
}
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_11360_p(int32_t n, int32_t g, int32_t d, int32_t p, int32_t h) {
    (void)d; (void)h; return newp(n, g, p);
}
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_11360_y(int32_t n, int32_t g, int32_t d, int32_t p, int32_t h) {
    int32_t M = m11240(g); p = newp(n, g, p);
    return (int32_t)((uint32_t)((int32_t)((uint32_t)(0xF6 - 12 * d) * (uint32_t)p) / M) + 0x2F + (uint32_t)((h - 0x1A4) / 2));
}
