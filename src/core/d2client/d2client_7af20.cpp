// 1.13c D2Client+0x7AF20 (stdcall (RECT* r, n), ret 8; returns a BOOL in AL): place a 400x280 panel rectangle among the n already placed.
// Globals: the placed rectangles at 0x113960 (16-byte stride), the step multipliers at 0xE13DC with their count at 0xE13FC,
// screen width/height at 0xDBC48/0xDBC4C. Overlap is user32 IntersectRect (non-empty intersection, empty rectangles never overlap).
//   n >= 16: AL = 0 (EAX's upper bytes are the entry EAX).
//   n == 0: rects[0] = *r; EAX = r with AL = 1.
//   r overlaps none of rects[0..n-1] (or n < 0): rects[n] = *r; EAX = r->bottom with AL = 1.
//   else for x = 50..350 step 50, y = 35..245 step 35, k = 0..count-1: c = (r->l + m*x, r->t + m*y, +400, +280), m = mult[k];
//     c must lie on screen (l >= 0, l + 400 <= W, t >= 0, t + 280 <= H) and overlap none of rects[0..n-1]: then rects[n] = c, *r = c,
//     EAX = c.left with AL = 1. No candidate: EAX = 0x100 (the x counter 0x190 with AL cleared).
#include <cstdint>
#include <emscripten.h>

struct D2c7af20Rect { int32_t l, t, r, b; };

static bool d2c_7af20_isect(const D2c7af20Rect& a, const D2c7af20Rect& b) {   // user32 IntersectRect's result
    if (a.r <= a.l || a.b <= a.t || b.r <= b.l || b.b <= b.t) return false;
    return !(a.l >= b.r || b.l >= a.r || a.t >= b.b || b.t >= a.b);
}

extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_7af20(D2c7af20Rect* rects, const int32_t* mult, const int32_t* count, int32_t scrW, int32_t scrH,
                         D2c7af20Rect* r, int32_t n, uint32_t eax_in) {
    if (n >= 0x10) return eax_in & ~0xffu;
    if (n == 0) { rects[0] = *r; return ((uint32_t)(uintptr_t)r & ~0xffu) | 1u; }
    int32_t i = 0;
    for (; i < n; ++i) if (d2c_7af20_isect(*r, rects[i])) break;
    if (i >= n) {   // n < 0 also lands here; retail then writes below the table (not reachable from a valid caller)
        if (n >= 0) rects[n] = *r;
        return ((uint32_t)r->b & ~0xffu) | 1u;
    }
    for (int32_t x = 0x32; x < 0x190; x += 0x32)
        for (int32_t y = 0x23; y < 0x118; y += 0x23)
            for (int32_t k = 0; k < *count; ++k) {
                const int32_t m = mult[k];
                D2c7af20Rect c;
                c.l = (int32_t)((uint32_t)r->l + (uint32_t)m * (uint32_t)x);
                c.t = (int32_t)((uint32_t)r->t + (uint32_t)m * (uint32_t)y);
                c.r = (int32_t)((uint32_t)c.l + 0x190u);
                c.b = (int32_t)((uint32_t)c.t + 0x118u);
                if (c.l < 0 || c.r > scrW || c.t < 0 || c.b > scrH) continue;
                bool hit = false;
                for (int32_t j = 0; j < n; ++j) if (d2c_7af20_isect(rects[j], c)) hit = true;
                if (hit) continue;
                rects[n] = c; *r = c;
                return ((uint32_t)c.l & ~0xffu) | 1u;
            }
    return 0x100u;
}
