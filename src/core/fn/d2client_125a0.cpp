// 1.13c D2Client+0x125A0 probe. r[0]=st (0x11C4FC..+88) r[1]=ps (0x1236D0 y x frame) r[2]=ms (mouseY mouseX)
// r[3]=W r[5]=H r[6]=thumbY r[7]=ui (0xF8B98..+84) r[8]=gt (unused, *0xF8B94) r[9]=tb (row table) r[10]=tt (key table 0x11A2E0)
// r[11]=hv (callee script: [0] D35A width, [1..64] 0x398C0 results) r[12]=lg (hook log). Every callee is a HookEnv recorder.
// D1EC results 0x5100+k, 391E0 results 0x6100+k, D3A8 first result = old font (hv[65]); GetTickCount := 0xC0DE1000.
#include <emscripten.h>
#include <cstdint>
#include "hookenv.h"
namespace {
typedef int32_t I; typedef uint32_t U;
struct G { I mouseY, mouseX, W, H, thumbY; const void* p; void* last; const uint8_t* ui; const uint8_t* rows; const uint8_t* keys; };
struct Cb { void (*pre)(); void (*mouse)(I, I); U (*font)(U); U (*lang)(U); void (*text)(U, I, I, I, I); I (*width)(U); void (*fill)(I, I, I, I, I, I);
            void (*cel)(const U*, I, I, I, I, I); void (*rect)(const I*); U (*label)(U, U); I (*known)(U, U); U (*tick)(); };
HookEnv* he;
void pre() { he->h(0x11410); }
void mouse(I x, I y) { he->h(0x11a80, 0, 0, 0, 0, 0, (U)x, (U)y); }
U font(U id) { return he->h(0xd3a8, 0, id); }
U lang(U c) { return he->h(0xd1ec, 0, c); }
void text(U s, I x, I y, I a, I b) { he->h(0xd37e, 0, s, (U)x, 0, 0, (U)y, (U)a, (U)b); }
I width(U s) { return (I)he->h(0xd35a, 0, s); }
void fill(I x0, I y0, I x1, I y1, I c, I f) { he->h(0xd0ba, (U)c, (U)f, 0, 0, 0, (U)x0, (U)y0, (U)x1, (U)y1); }
void cel(const U* blk, I x, I y, I m1, I mode, I last) { he->h(0xd096, (U)mode, (U)last, 0, 0, 0, 0xB3000000u, (U)x, (U)y, (U)m1); he->blk(0xd096, blk, 18); }
void rect(const I* r) { he->h(0xbe4c0, 0xB3000000u); he->blk(0xbe4c0, (const U*)r, 4); }
U label(U id, U f) { return he->h(0x391e0, 0, 0, f, 0, id); }
I known(U id, U f) { return (I)he->h(0x398c0, 0, 0, id, 0, f); }
U tick() { return 0xC0DE1000u; }
}
extern "C" void d2_client_125a0(const G*, const Cb*);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_125a0(uint32_t* r) {
    const uint32_t* hv = (const uint32_t*)(uintptr_t)r[11];
    static uint32_t sc[2 * (40 + 40 + 64 + 2 + 1) + 2];
    int n = 0;
    for (U k = 0; k < 40; k++) { sc[n++] = 0xd1ec; sc[n++] = 0x5100 + k; }
    for (U k = 0; k < 40; k++) { sc[n++] = 0x391e0; sc[n++] = 0x6100 + k; }
    for (U k = 0; k < 64; k++) { sc[n++] = 0x398c0; sc[n++] = hv[1 + k]; }
    sc[n++] = 0xd35a; sc[n++] = hv[0];
    sc[n++] = 0xd3a8; sc[n++] = hv[65]; sc[n++] = 0xd3a8; sc[n++] = 0;
    sc[n++] = 0; sc[n++] = 0;
    HookEnv h((uint32_t*)(uintptr_t)r[12], sc); he = &h;
    const uint32_t* ms = (const uint32_t*)(uintptr_t)r[2];
    G g = { (I)ms[0], (I)ms[1], (I)*(const uint32_t*)(uintptr_t)r[3], (I)*(const uint32_t*)(uintptr_t)r[5], (I)*(const uint32_t*)(uintptr_t)r[6],
            (const void*)(uintptr_t)r[0], (void*)(uintptr_t)r[1], (const uint8_t*)(uintptr_t)r[7], (const uint8_t*)(uintptr_t)r[9], (const uint8_t*)(uintptr_t)r[10] };
    Cb cb = { pre, mouse, font, lang, text, width, fill, cel, rect, label, known, tick };
    d2_client_125a0(&g, &cb);
    r[4] = 0;
}
