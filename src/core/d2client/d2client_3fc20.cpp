// 1.13c D2Client+0x3FC20 -- automap debug overlay for one room. stdcall(room), ret 4.
// Pass 1: the room's 0x30-byte preset slots (D2Common room query thunk 0xCB0E: array + count out) are mapped from tile to
// screen space; a slot with a unit, kind != 0xF and (flags & 0x1C000) == 0x4000 gets a marker (0x3F590) and its label.
// Pass 2 (only when the room is the player's): each node of the room's rectangle list (thunk 0xCE8C, next at +0x2C) is
// mapped corner by corner and outlined; the player's own room also gets four tick marks. Every import is an env seam.
#include <emscripten.h>
#include <cstdint>
#include <cstring>

struct D2Client3fc20Env {
    uint32_t (*roomSlots)(uint32_t room, uint32_t* count);          // thunk 0xCB0E (stdcall 2): slot array, count out
    void (*fatal)(uint32_t line);                                   // 0xBF92 / 0xBF80 / 0x3664 (assert, exit)
    void (*roomRect)(uint32_t room, int32_t* rect);                 // thunk 0xC5B0 (stdcall 2): 6+ dword rect, [4] [5] = tile origin
    void (*tileToScreen)(int32_t* x, int32_t* y);                   // thunk 0xC2F8 (stdcall 2)
    uint32_t (*presetFlags)(uint32_t id);                           // thunk 0xC17E (stdcall 1)
    void (*marker)(int32_t x, int32_t y, int32_t color);            // 0x3F590 (EAX = x, stack y, color; ret 8)
    void (*label)(int32_t x, int32_t y, uint32_t fmt, uint32_t v);  // thunk 0xD174 (cdecl 4)
    uint32_t (*unitRoom)(uint32_t unit);                            // thunk 0xC1F0 (stdcall 1)
    uint32_t (*roomRects)(uint32_t room);                           // thunk 0xCE8C (stdcall 1): first rect node
    void (*worldToScreen)(int32_t* x, int32_t* y);                  // thunk 0xCC58 (stdcall 2)
    void (*line)(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t color, int32_t alpha);   // thunk 0xD102 (stdcall 6)
    uint32_t fmt;              // the label format string (D2Client 0xD057C)
    const int32_t* scrollY;    // DAT 0x11995C
    const int32_t* scrollX;    // DAT 0x119960
    const uint32_t* player;    // DAT 0x11BBFC
};

namespace {
inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
}

// 1.13c D2Client+0x3FC20
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_3fc20(uint32_t room, const D2Client3fc20Env* env) {
    uint32_t count = 0;
    uint32_t slots = env->roomSlots(room, &count);
    if (!slots && count) env->fatal(0x2D5);
    int32_t rect[8] = {};
    env->roomRect(room, rect);
    for (uint32_t i = 0; i < count; ++i) {
        const uint32_t s = slots + i * 0x30;
        if (!rd32(s + 0x10)) continue;
        int32_t x = ((int32_t)rd32(s + 0x08) + rect[4]) * 5 + 3;
        int32_t y = ((int32_t)rd32(s + 0x0C) + rect[5]) * 5 + 3;
        env->tileToScreen(&x, &y);
        if (rd32(s + 0x1C) == 0xF) continue;
        if ((rd32(s + 0x14) & 0x1C000) != 0x4000) continue;
        if (env->presetFlags(rd32(s + 0x18)) & 4) env->marker(x + 0x20, y, 0x78);
        else if (rd32(s + 0x14) & 0x800) env->marker(x + 0x10, y + 8, 0x78);
        else env->marker(x, y, 0xFF);
        x += -7 - *env->scrollX;
        y -= *env->scrollY;
        env->label(x, y, env->fmt, rd32(rd32(s + 0x10) + 0x28));
    }
    const uint32_t pr = *env->player ? env->unitRoom(*env->player) : 0;
    if (room != pr) return;
    for (uint32_t n = env->roomRects(room); n; n = rd32(n + 0x2C)) {
        int32_t xs[4], ys[4];
        xs[0] = xs[1] = (int32_t)rd32(n + 0x0);
        xs[2] = xs[3] = (int32_t)rd32(n + 0x8);
        ys[0] = ys[2] = (int32_t)rd32(n + 0x4);
        ys[1] = ys[3] = (int32_t)rd32(n + 0xC);
        for (int k = 0; k < 4; ++k) {
            env->worldToScreen(&xs[k], &ys[k]);
            xs[k] -= *env->scrollX;
            ys[k] -= *env->scrollY;
        }
        const uint32_t pr2 = *env->player ? env->unitRoom(*env->player) : 0;
        if (room == pr2) {
            env->line(xs[0], ys[0], xs[0], ys[0] + 0x10, 0x78, 0xFF);
            env->line(xs[1], ys[1], xs[1] + 0x10, ys[1], 0x78, 0xFF);
            env->line(xs[2], ys[2], xs[2] - 0x10, ys[2], 0x78, 0xFF);
            env->line(xs[3], ys[3], xs[3], ys[3] - 0x10, 0x78, 0xFF);
        }
        env->line(xs[0], ys[0], xs[1], ys[1], 0x78, 0xFF);
        env->line(xs[0], ys[0], xs[2], ys[2], 0x78, 0xFF);
        env->line(xs[1], ys[1], xs[3], ys[3], 0x78, 0xFF);
        env->line(xs[2], ys[2], xs[3], ys[3], 0x78, 0xFF);
    }
}
