// 1.13c D2Game+0x674A0 probe: the port over the HookEnv recorder.
// Descriptor `args 2` (stdcall, no regs): r[0] game, r[1] unit, r[4] result; blobs 4 (hook log) and 5 (hook values) at r[22], r[24].
#include <emscripten.h>
#include <cstdint>
#include "hookenv.h"

struct D2Game674A0Env {
    void     (*fatal)(uint32_t line);
    uint32_t (*flag)(uint32_t, uint32_t, uint32_t); uint32_t (*playerData)(uint32_t); uint32_t (*roomOf)(uint32_t); uint32_t (*roomKey)(uint32_t);
    void     (*getPos)(uint32_t, uint32_t*); void (*findRoom)(uint32_t, uint32_t*, uint32_t*); void (*release)(uint32_t);
    uint32_t (*kind)(uint32_t); uint32_t (*move)(uint32_t, uint32_t*); uint32_t (*roomAt)(uint32_t, uint32_t, uint32_t);
    uint32_t (*create)(uint32_t, uint32_t, uint32_t, uint32_t); void (*setMode)(uint32_t, uint32_t);
    uint32_t (*place)(uint32_t, uint32_t, uint32_t, uint32_t); uint32_t (*flags)(uint32_t); void (*setFlags)(uint32_t, uint32_t);
    void     (*link)(uint32_t, uint32_t); void (*mark)(uint32_t, uint32_t, uint32_t); void (*unitDone)(uint32_t);
};
uint32_t D2Game_674A0(uint32_t game, uint32_t unit, const D2Game674A0Env& e);

namespace {
HookEnv* H;
inline uint32_t P(const void* p) { return (uint32_t)(uintptr_t)p; }
void fatal(uint32_t) { abort(); }
uint32_t flag(uint32_t q, uint32_t a, uint32_t b) { return H->h(0xA568, 0, 0, 0, 0, 0, q, a, b); }
uint32_t gPd;   // blob 1 (the player-data blob the retail hook returns)
uint32_t playerData(uint32_t u) { return H->h(0xA4C0, 0, 0, 0, 0, 0, u) ? gPd : 0; }
uint32_t roomOf(uint32_t u) { return H->h(0xA478, 0, 0, 0, 0, 0, u); }
uint32_t roomKey(uint32_t r) { return H->h(0xA556, 0, 0, 0, 0, 0, r); }
void getPos(uint32_t u, uint32_t* pos) { H->h(0xA544, 0, 0, 0, 0, 0, u, P(pos)); }
void findRoom(uint32_t room, uint32_t* pos, uint32_t* out) { H->h(0x34130, 0, 0, 0, 0, 0, room, P(pos), 3, 0x400, P(out), 0x64); H->blk(0x34130, pos, 2); }
void release(uint32_t f) { H->h(0xA808, 0, 0, 0, 0, 0, f); }
uint32_t kind(uint32_t v) { return H->h(0xA56E, 0, 0, 0, 0, 0, v); }
uint32_t move(uint32_t room, uint32_t* pos) { uint32_t v = H->h(0xAE98, 0, 0, 0, 0, 0, room, P(pos), P(pos), 3); return v; }
uint32_t roomAt(uint32_t r, uint32_t x, uint32_t y) { H->ed(y); uint32_t v = H->h(0x1280, r, 0, 0, x); H->edi(0x1280, y); return v; }
uint32_t create(uint32_t x, uint32_t y, uint32_t g, uint32_t r) { return H->h(0xE1D90, 0, 2, 0x3c, 0, 0, x, y, g, r); }
void setMode(uint32_t u, uint32_t m) { H->h(0xA4A2, 0, 0, 0, 0, 0, u, m); }
uint32_t place(uint32_t g, uint32_t u, uint32_t w, uint32_t v) { return H->h(0xA22E0, 0, 0, 0, 0, 0, g, u, w, v); }
uint32_t flags(uint32_t u) { return H->h(0xAEE0, 0, 0, 0, 0, 0, u); }
void setFlags(uint32_t u, uint32_t v) { H->h(0xAEC8, 0, 0, 0, 0, 0, u, v); }
void link(uint32_t a, uint32_t b) { H->h(0xA51A, 0, 0, 0, 0, 0, a, b); }
void mark(uint32_t g, uint32_t a, uint32_t b) { H->h(0x33920, g, 0, 0, 0, 0, a, b); }
void unitDone(uint32_t u) { H->h(0xA4DE, 0, 0, 0, 0, 0, u); }
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_674a0(uint32_t* r) {
    HookEnv e((uint32_t*)(uintptr_t)r[22], (const uint32_t*)(uintptr_t)r[24]);
    H = &e; gPd = r[16];
    const D2Game674A0Env env = { fatal, flag, playerData, roomOf, roomKey, getPos, findRoom, release, kind, move, roomAt, create, setMode, place, flags, setFlags, link, mark, unitDone };
    r[4] = D2Game_674A0(r[0], r[1], env);
}
