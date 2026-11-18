// Party colour quad refresh. Retail: no arguments, plain ret. Globals: 0x11C3B8 (the arg of ordinals 10189/10731), 0x11BBFC (the player record, may be 0),
// the four bytes 0x10B234..0x10B237. 10189(g, &B237, &B235, &B234) fills the defaults, 10731(g) gives B236; with a player, 10331(player) returns v and,
// when v is non-zero, 10636(v, &L[3], &L[0], &L[2], &L[1]) fills four local bytes: if L[0], L[1] or L[2] is non-zero the quad := {B234 = L[1], B235 = L[2],
// B236 = L[3], B237 = L[0]} (L[3] alone does not trigger it). All four callees are stdcall.
// 1.13c D2Client+0x9EDD0
#include <cstdint>
#include <emscripten.h>

struct D2c9edd0Env {
    uint32_t arg;                                                                  // *(D2Client+0x11C3B8)
    uintptr_t player;                                                              // *(D2Client+0x11BBFC)
    uint8_t* quad;                                                                 // D2Client+0x10B234 (4 bytes)
    void (*c10189)(uint32_t arg, uint8_t* p237, uint8_t* p235, uint8_t* p234);     // 0xC5B6
    uint32_t (*c10731)(uint32_t arg);                                              // 0xC5A4 (AL used)
    uint32_t (*c10331)(uintptr_t player);                                          // 0xC1F0
    void (*c10636)(uint32_t v, uint8_t* a3, uint8_t* a0, uint8_t* a2, uint8_t* a1);// 0xC59E
};

extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_9edd0(const D2c9edd0Env* e) {
    uint8_t* q = e->quad;
    e->c10189(e->arg, q + 3, q + 1, q + 0);
    q[2] = (uint8_t)e->c10731(e->arg);
    if (!e->player) return;
    const uint32_t v = e->c10331(e->player);
    if (!v) return;
    uint8_t L[4] = {};
    e->c10636(v, L + 3, L + 0, L + 2, L + 1);
    if (L[0] || L[2] || L[1]) { q[2] = L[3]; q[3] = L[0]; q[1] = L[2]; q[0] = L[1]; }
}
