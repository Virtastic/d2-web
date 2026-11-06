// src/core/d2ddraw/d2ddraw_batch_b51.cpp -- UNMAPPED 1.13c D2DDraw.dll rows reimplemented from the cached
// decompiles (alpha lane, no spec/vector yet). Globals are named at their retail RVA (base 0x6F8C0000).
// Skipped as not faithfully portable: 0x51D0 (LCMapStringA CRT body, stack-shuffled), 0x560C (__SEH
// security-failure box), 0x6E20 (register-passed ordinal clip), 0x6FF0 (Bink), 0x82F0/0x85A0 (COM vtable
// calls on DirectDraw), 0x9644 (CRT fn-pointer table of code labels).
#include <cstdint>

static int32_t  g_width;    // +0x101C8 DAT_6f8d01c8: surface width / row pitch
static int32_t  g_height;   // +0x101D8 DAT_6f8d01d8
static uint8_t* g_surface;  // +0x0F8A8 DAT_6f8cf8a8: 8-bit render buffer

// 1.13c D2DDraw+0x06010 (FillRect8): rect {l,t,r,b}; clip to the surface, fill width r-l, height b-t
// (exclusive, as retail) with colour. Bad rect order is fatal in retail (caller seam).
extern "C" void (*d2ddraw_fatal)(uint32_t code);
extern "C"
void d2ddraw_fill_rect(int32_t* r, uint8_t color) {
    int32_t l = r[0], t = r[1], rt = r[2], b = r[3];
    if (rt < l) { d2ddraw_fatal(0x165); return; }
    if (t > b) { d2ddraw_fatal(0x166); return; }
    if (rt < 0 || l >= g_width || b < 0 || t >= g_height) return;
    if (l < 0) r[0] = 0;
    if (rt >= g_width) r[2] = g_width - 1;
    if (t < 0) r[1] = 0;
    if (b >= g_height) r[3] = g_height - 1;
    uint32_t w = (uint32_t)(r[2] - r[0]);
    int32_t h = r[3] - r[1];
    if (!w || !h) return;
    uint8_t* row = g_surface + r[1] * g_width + r[0];
    do { for (uint32_t i = 0; i < w; ++i) row[i] = color; row += g_width; } while (--h);
}

// 1.13c D2DDraw+0x06130 (DrawLine8): 16.16 stepping along p1 -> p0 from p0's start point; steps =
// ceil(sqrt(dx^2+dy^2)); off-surface start points are skipped until on-surface.
extern "C"
void d2ddraw_draw_line(const int32_t* p1, const int32_t* p0, uint8_t color) {
    int32_t y = p0[1], x = p0[0];
    int32_t dx = p1[0] - x, dy = p1[1] - y;
    if (!dx && !dy) return;
    int32_t d2 = dy * dy + dx * dx, n = 0;
    if (d2 > 0) do { ++n; } while (n * n < d2);
    int32_t sx = (dx * 0x10000) / n, sy = (dy * 0x10000) / n;
    int32_t fx = 0, fy = 0, i = 0;
    while (y < 0 || y >= g_height || x < 0 || x >= g_width) {
        fx += sx;
        if (fx > 0xffff || fx < -0xffff) { int32_t s = (fx + ((fx >> 31) & 0xffff)) >> 16; x += s; fx -= s * 0x10000; }
        fy += sy;
        if (fy > 0xffff || fy < -0xffff) { int32_t s = (fy + ((fy >> 31) & 0xffff)) >> 16; y += s; fy -= s * 0x10000; }
        if (n <= i) return;
        ++i;
    }
    uint8_t* p = g_surface + g_width * y + x;
    *p = color;
    if (++i >= n) return;
    for (;;) {
        fx += sx;
        if (fx > 0xffff || fx < -0xffff) { int32_t s = (fx + ((fx >> 31) & 0xffff)) >> 16; x += s; p += s; fx -= s * 0x10000; }
        if (x < 0 || x >= g_width) return;
        fy += sy;
        if (fy > 0xffff || fy < -0xffff) { int32_t s = (fy + ((fy >> 31) & 0xffff)) >> 16; y += s; p += g_width * s; fy -= s * 0x10000; }
        if (y < 0 || y >= g_height) return;
        *p = color;
        if (n <= ++i) return;
    }
}

// (old reading of 0x5C5E, superseded by d2ddraw_5c5e.cpp) ___isctype_mt: ctype lookup. pctype = locale +0x48 table, codepage +4,
// lcid +0x14. Chars -1..255 index the table; others go through __crtGetStringTypeA (0x4F88) via seam.
struct D2DDrawLocale { uint8_t pad0[4]; uint32_t cp; uint8_t pad1[0x0c]; uint32_t lcid; uint8_t pad2[0x30]; const uint16_t* pctype; };
extern "C" int32_t (*d2ddraw_gst)(uint32_t info, const char* s, int32_t cch, uint16_t* out, uint32_t cp, uint32_t lcid);
extern "C"
uint32_t d2ddraw_isctype_mt(const D2DDrawLocale* loc, int32_t c, uint32_t mask) {
    uint16_t t;
    if ((uint32_t)(c + 1) < 0x101) t = loc->pctype[c];
    else {
        const uint8_t* hi = reinterpret_cast<const uint8_t*>(loc->pctype);
        char buf[3]; int32_t n;
        if (hi[1 + ((c >> 8) & 0xff) * 2] & 0x80) { buf[0] = (char)(c >> 8); buf[1] = (char)c; n = 2; }
        else { buf[0] = (char)c; buf[1] = 0; n = 1; }
        buf[2] = 0;
        if (!d2ddraw_gst(1, buf, n, &t, loc->cp, loc->lcid)) return 0;
    }
    return t & mask;
}

// DDraw error logger: the engine entry over the 1.13c port d2ddraw_error_log_7690 (d2ddraw_7690.cpp), logging through
// the Fog ordinal 10029 seam.
extern "C" void (*d2ddraw_log)(const char* fmt, const char* what, const char* name, uint32_t hr);
struct D2DDrawErrLog { void (*fogLog)(const char* fmt, const char* what, const char* name, uint32_t hr); };
extern "C" void d2ddraw_error_log_7690(const D2DDrawErrLog* env, uint32_t hr, const char* what);
static void d2ddraw_log_thunk(const char* fmt, const char* what, const char* name, uint32_t hr) { d2ddraw_log(fmt, what, name, hr); }
extern "C"
void d2ddraw_log_error(uint32_t hr, const char* what) {
    static const D2DDrawErrLog env{ d2ddraw_log_thunk };
    d2ddraw_error_log_7690(&env, hr, what);
}

// Test seam: sets the surface width, height and buffer the fill family reads.
extern "C" void d2ddraw_surface_state(int32_t w, int32_t h, uint8_t* s) { g_width = w; g_height = h; g_surface = s; }
