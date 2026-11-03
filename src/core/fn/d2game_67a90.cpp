// 1.13c D2Game+0x67A90 probe: the port over the HookEnv recorder.
// Descriptor `regs ecx edx`, args 4: r[0] game, r[1] quest record, r[2] hook log, r[3] hook values, r[4] result.
#include <emscripten.h>
#include <cstdint>
#include "hookenv.h"

struct D2Game67A90Env {
    void     (*fatal)(uint32_t line);
    uint32_t (*extentA)(uint32_t); uint32_t (*extentB)(uint32_t); uint32_t (*roomOf)(uint32_t); uint32_t (*kind)(uint32_t);
    uint32_t (*roomKey)(uint32_t); uint32_t (*roomAt)(uint32_t, uint32_t, uint32_t);
    uint32_t (*create)(uint32_t, uint32_t, uint32_t, uint32_t);
    void     (*setMode)(uint32_t, uint32_t); uint32_t (*place)(uint32_t, uint32_t, uint32_t, uint32_t);
    uint32_t (*flags)(uint32_t); void (*setFlags)(uint32_t, uint32_t); void (*link)(uint32_t, uint32_t);
};
uint32_t D2Game_67A90(uint32_t game, uint32_t rec, const D2Game67A90Env& e);

namespace {
HookEnv* H;
void fatal(uint32_t) { abort(); }
uint32_t extentA(uint32_t p) { return H->h(0xA3B8, 0, 0, 0, 0, 0, p); }
uint32_t extentB(uint32_t p) { return H->h(0xA454, 0, 0, 0, 0, 0, p); }
uint32_t roomOf(uint32_t it) { return H->h(0xA478, 0, 0, 0, 0, 0, it); }
uint32_t kind(uint32_t v) { return H->h(0xA56E, 0, 0, 0, 0, 0, v); }
uint32_t roomKey(uint32_t r) { return H->h(0xA556, 0, 0, 0, 0, 0, r); }
uint32_t roomAt(uint32_t r, uint32_t x, uint32_t y) { H->ed(y); uint32_t v = H->h(0x1280, r, 0, 0, x); H->edi(0x1280, y); return v; }
uint32_t create(uint32_t x, uint32_t y, uint32_t g, uint32_t r) { return H->h(0xE1D90, 0, 2, 0x3c, 0, 0, x, y, g, r); }
void setMode(uint32_t u, uint32_t m) { H->h(0xA4A2, 0, 0, 0, 0, 0, u, m); }
uint32_t place(uint32_t g, uint32_t u, uint32_t w, uint32_t v) { return H->h(0xA22E0, 0, 0, 0, 0, 0, g, u, w, v); }
uint32_t flags(uint32_t u) { return H->h(0xAEE0, 0, 0, 0, 0, 0, u); }
void setFlags(uint32_t u, uint32_t v) { H->h(0xAEC8, 0, 0, 0, 0, 0, u, v); }
void link(uint32_t a, uint32_t b) { H->h(0xA51A, 0, 0, 0, 0, 0, a, b); }
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_67a90(uint32_t* r) {
    HookEnv e((uint32_t*)(uintptr_t)r[2], (const uint32_t*)(uintptr_t)r[3]);
    H = &e;
    const D2Game67A90Env env = { fatal, extentA, extentB, roomOf, kind, roomKey, roomAt, create, setMode, place, flags, setFlags, link };
    r[4] = D2Game_67A90(r[0], r[1], env);
}
