// D2DDraw utility primitives over the 8-bit surface: point and diamond markers, built on the
// FillRect8 and DrawLine8 helpers (same arithmetic as the retail wrappers).
#include <cstdint>

static void d2ddraw_fatal_default(uint32_t) {}
extern "C" { void (*d2ddraw_fatal)(uint32_t code) = d2ddraw_fatal_default; }   // the host sets the fatal-error handler; the default returns
extern "C" void d2ddraw_fill_rect(int32_t* r, uint8_t color);
extern "C" void d2ddraw_draw_line(const int32_t* p1, const int32_t* p0, uint8_t color);

extern "C" void d2ddraw_util_point_63c0(void (*)(int32_t*, int32_t, int32_t, int32_t, int32_t), void (*)(int32_t*, uint8_t),
                                        const int32_t*, uint8_t, uint8_t);
extern "C" void d2ddraw_util_diamond_64e0(void (*)(const int32_t*, const int32_t*, uint8_t), void (*)(uint32_t),
                                          const int32_t*, uint8_t);
static void set_rect(int32_t* r, int32_t l, int32_t t, int32_t rt, int32_t b) { r[0] = l; r[1] = t; r[2] = rt; r[3] = b; }
static void fatal_thunk(uint32_t code) { d2ddraw_fatal(code); }

// Engine entries over the 1.13c ports in d2ddraw_63c0.cpp and d2ddraw_64e0.cpp.
extern "C" void d2ddraw_util_point(const int32_t* pt, uint8_t size, uint8_t color) {
    d2ddraw_util_point_63c0(set_rect, d2ddraw_fill_rect, pt, size, color);
}
extern "C" void d2ddraw_util_diamond(const int32_t* rc, uint8_t color) {
    d2ddraw_util_diamond_64e0(d2ddraw_draw_line, fatal_thunk, rc, color);
}
