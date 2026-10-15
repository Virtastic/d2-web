#include <cstdint>
#include <emscripten.h>

// Room1 fields read by D2Client+0x1400: +0x00 ppNear, +0x24 nNear (Common #10383 outputs), +0x4c x0, +0x50 y0, +0x54 w, +0x58 h.
// The pinned table stands in for 4 Room1 structs; near[] holds table indices, -1 = null pointer.
struct Room1400 { int32_t x0, y0, w, h, n, near[3]; };
namespace { Room1400 g_rooms1400[4]; }

extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_1400_pin(int32_t i, int32_t x0, int32_t y0, int32_t w, int32_t h, int32_t n, int32_t a, int32_t b, int32_t c) {
    g_rooms1400[i & 3] = { x0, y0, w, h, n, { a, b, c } };
}

static bool in1400(const Room1400& r, int32_t x, int32_t y) {
    return x >= r.x0 && x < r.w + r.x0 && y >= r.y0 && y < r.h + r.y0;
}

// 1.13c D2Client+0x1400: EAX=room, EBX=x, EDI=y. Returns the room itself if (x,y) lies in it, else the first
// non-null near room containing it, else null. Here: room index -1 = null; returns index, or -1.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_1400(int32_t room, int32_t x, int32_t y) {
    if (room < 0) return -1;
    const Room1400& r = g_rooms1400[room & 3];
    if (in1400(r, x, y)) return room;
    for (int32_t i = 0; i < r.n; i++) {
        const int32_t k = i < 3 ? r.near[i] : -1;   // table holds <=3 near rooms; n is capped to 3 by the capture
        if (k >= 0 && in1400(g_rooms1400[k & 3], x, y)) return k;
    }
    return -1;
}
