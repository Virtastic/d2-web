// 1.13c D2Client+0x8C8B0 -- the twelve fixed captions of a panel: no inputs; g = pinned globals (+0 x shift [0x11B9A0], +4 y shift
// [0x11B9A4]). Font 6 swapped in (0xD3A8). Per row of the constant table {x0, x1, y, string id}: a 100-char local zeroed, the string
// (0xD1EC, id in CX) copied with strncpy(99) and capped at char 99; x = x0 + dx, xr = x1 + dx, y = y - dy. A '\n' splits the text in
// two lines drawn at y - 4 and y + 4, each centred in [x, xr] when narrower than xr - x + 1; without one the line is centred the same
// way only when xr != 0. Draw = 0xD37E(text at x; y, 0, 0), width = 0xD35A(text). The old font is restored.
#include "cl_env.h"
namespace {
struct Cap { int32_t x0, x1, y; uint16_t id; };
constexpr Cap kCaps[12] = {
    { 15, 0, 236, 0xfda }, { 145, 0, 236, 0xfd9 }, { 200, 0, 236, 0xfdb }, { 15, 0, 281, 0xfdc },
    { 15, 0, 305, 0xfde }, { 15, 0, 329, 0xfdd }, { 15, 0, 353, 0xfe0 }, { 180, 245, 281, 0xfe7 },
    { 180, 245, 305, 0xfe8 }, { 180, 245, 329, 0xfe9 }, { 180, 245, 353, 0xfea }, { 180, 0, 214, 0xfe4 },
};
inline uint32_t A(const volatile char16_t* p) { return (uint32_t)(uintptr_t)p; }
int32_t centred(const cl::Env& e, const volatile char16_t* s, int32_t x, int32_t span) {
    int32_t w = (int32_t)e.H(0xd35a, 0, 0, 0, 0, 0, 0, A(s));
    return w < span ? ((span - w) >> 1) + x : x;
}
}
extern "C" void d2c_8c8b0(const cl::Env& e, uint32_t g) {
    uint32_t old = e.H(0xd3a8, 0, 0, 0, 0, 0, 0, 6);
    for (const Cap& c : kCaps) {
        volatile char16_t buf[100];
        for (int i = 0; i < 100; i++) buf[i] = 0;
        const char16_t* s = (const char16_t*)(uintptr_t)e.H(0xd1ec, 0, 0, 0, 0, 0, 0, c.id);
        int n = 0;
        for (; n < 99 && s[n]; n++) buf[n] = s[n];
        for (; n < 99; n++) buf[n] = 0;
        buf[99] = 0;
        int32_t dx = (int32_t)e.R(g), dy = (int32_t)e.R(g + 4);
        int32_t x = c.x0 + dx, xr = c.x1 + dx, y = c.y - dy;
        int32_t len = 0;
        while (buf[len]) len++;
        int32_t i = 0;
        while (buf[i] && buf[i] != u'\n') i++;
        const volatile char16_t* last = buf;
        if (buf[i] == u'\n') {
            buf[i] = 0;
            if (i < len) {
                int32_t span = xr - x + 1;
                e.H(0xd37e, (uint32_t)(y - 4), 0, 0, 0, 0, 0, A(buf), (uint32_t)centred(e, buf, x, span));
                last = buf + i + 1;
                x = centred(e, last, x, span);
                y += 4;
            } else if (xr) x = centred(e, buf, x, xr - x + 1);
        } else if (xr) x = centred(e, buf, x, xr - x + 1);
        e.H(0xd37e, (uint32_t)y, 0, 0, 0, 0, 0, A(last), (uint32_t)x);
    }
    e.H(0xd3a8, 0, 0, 0, 0, 0, 0, old);
}
