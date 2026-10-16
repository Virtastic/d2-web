#include <cstdint>
#include <cstddef>
#include <emscripten.h>

// Layouts as the 1.13c code sees them (32-bit fields; pointers are the low 32 bits in wasm).
struct B22Bounds { uint8_t p0[0x28]; int32_t on, minX, maxX, minY, maxY; };       // eax of 0x13560
struct B22Flags  { uint8_t p0[0x14]; uint32_t flags; };                          // edx of 0x13560
struct B22Unit   { int32_t kind; uint8_t p0[0x10]; uint32_t sub; };              // eax of 0x16FE0/0x1FCA0
static_assert(offsetof(B22Bounds, on) == 0x28 && offsetof(B22Bounds, maxY) == 0x38, "bounds");
static_assert(offsetof(B22Flags, flags) == 0x14, "flags");
static_assert(offsetof(B22Unit, sub) == 0x14, "unit");

// 1.13c D2Client+0x7BC0: strcpy, returns dst.
extern "C" EMSCRIPTEN_KEEPALIVE char* d2_client_7bc0(char* d, const char* s) { char* o = d; while ((*d++ = *s++)) {} return o; }

// 1.13c D2Client+0xD440: strrchr; ch compared as a byte, ch 0 finds the terminator.
extern "C" EMSCRIPTEN_KEEPALIVE char* d2_client_d440(char* s, int32_t ch) {
    char* r = nullptr; char c = (char)ch;
    for (;; s++) { if (*s == c) r = s; if (!*s) break; }
    return r;
}

// 1.13c D2Client+0x13560: set bit 0x20000 in b->flags; if a->on, widen the x/y bounds to include (x,y).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_13560(B22Bounds* a, int32_t x, B22Flags* b, int32_t y) {
    b->flags |= 0x20000;
    if (!a->on) return;
    if (x < a->minX) a->minX = x;
    if (a->maxX < x) a->maxX = x;
    if (y < a->minY) a->minY = y;
    if (a->maxY < y) a->maxY = y;
}

static uint8_t* b22_sub(B22Unit* u) { return (u && u->kind == 1 && u->sub) ? (uint8_t*)(uintptr_t)u->sub : nullptr; }

// 1.13c D2Client+0x16FE0: sub+0x30 := min(v, 200) when u is a kind-1 unit with a sub block.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_16fe0(B22Unit* u, int32_t v) {
    if (v > 199) v = 200;
    if (uint8_t* p = b22_sub(u)) *(int32_t*)(p + 0x30) = v;
}

// 1.13c D2Client+0x1FCA0: sub+0x17 bit 0 := (f != 0).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_1fca0(B22Unit* u, int32_t f) {
    if (uint8_t* p = b22_sub(u)) { if (f) p[0x17] |= 1; else p[0x17] &= 0xfe; }
}
