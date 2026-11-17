// The browser's belt draw: D2Client+0x28DC0 (d2client_28dc0.cpp) run against the shell's belt state, its callees answered
// from that state, its draw calls returned as an op list the platform layer rasterises. Nothing here decides what the
// belt shows; the port does (which frames, which slots, which hotkey labels, how they are trimmed).
//
// in  (int32): [0] w [1] h [2] belt record (0xF5384 seed) [3] popup open [4] box count n [5] item count m
//              [6 ..] n boxes (left, right, top, bottom), then m item belt slots, then 4 keys (len or -1 unbound, 15 UTF-16 units)
// ops (int32): 1 x y             background frame (D2Win #10041 via 0xD096)
//              2 item x y        item cel (0x6B5A0) of the item'th input item at the box's left/top
//              3 x y color len c0..  text (D2Win #10150)
// returns the ints written, or -1 (a callee the port needed that this answers nothing for, or the ops overflowed).
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include "d2client_callee.h"
#if __has_include(<emscripten.h>)
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

struct D2c28dc0G {
    int32_t w, h;
    uint32_t state, adfc, ada4, cell, g99c;
    uint32_t en, g46c, g470, g474;
    uint32_t hot[4];
    uint8_t col[4];
};
extern "C" void d2c_28dc0(const D2cSvc& v, D2c28dc0G* g, uint32_t player, uint32_t item, const uint8_t* tbl);
extern "C" int32_t d2_win_text_width(const uint8_t* adv, const uint16_t* text, int32_t n);

namespace {
constexpr int MAXI = 16, MAXB = 16;
struct Ctx {
    const int32_t* boxes; int32_t nb, ni, record;
    const int32_t* slot;
    int32_t* ops; int32_t cap, used; bool bad;
    const uint8_t* adv;
    uint32_t units[MAXI][12], aux[MAXI][4];
    uint16_t keys[4][16];
    const uint8_t* tbl;
};
Ctx g_c;
uint8_t g_tbl[114 * 10];
uint32_t g_player[32];

int slen(const uint16_t* p) { int n = 0; while (p[n]) ++n; return n; }
void put(Ctx* x, std::initializer_list<int32_t> v) {
    if (x->used + (int)v.size() > x->cap) { x->bad = true; return; }
    for (int32_t a : v) x->ops[x->used++] = a;
}
int itemOf(Ctx* x, uint32_t h) {
    for (int i = 0; i < x->ni; ++i) if (h == (uint32_t)(uintptr_t)x->units[i]) return i;
    return -1;
}
uint32_t cb(void* c, uint32_t rva, const uint32_t* r, const uint32_t* s) {
    Ctx* x = (Ctx*)c;
    switch (rva) {
    case 0xd3a8: case 0x265e0: case 0x277e0: case 0xd0ba: case 0xd390: return 0;
    case 0xc262: return x->record == 2 ? 0 : 0x1000;       // the belt in inventory slot 8
    case 0xc220: return 0x13;
    case 0xcf1c: return (uint32_t)x->record;
    case 0xcf22: ((uint8_t*)(uintptr_t)s[2])[4] = (uint8_t)x->nb; return 0;
    case 0xcf28:
        for (int i = 0; i < x->ni; ++i) if (x->slot[i] == (int32_t)s[1]) return (uint32_t)(uintptr_t)x->units[i];
        return 0;
    case 0xc3be: return 0;
    case 0xc1a8: case 0xca12: case 0xc1fc: return 0;
    case 0xc814: return 1;
    case 0xcf46: {                                          // box of belt slot s[3]
        uint32_t* o = (uint32_t*)(uintptr_t)s[2];
        const int32_t* b = x->boxes + 4 * (s[3] < (uint32_t)x->nb ? s[3] : 0);
        o[0] = (uint32_t)b[0]; o[1] = (uint32_t)b[1]; o[2] = (uint32_t)b[2]; o[3] = (uint32_t)b[3];
        return 0;
    }
    case 0x6b5a0: { const int i = itemOf(x, r[3]); if (i < 0) x->bad = true; else put(x, { 2, i, (int32_t)s[0], (int32_t)s[1] }); return 0; }
    case 0x39f30: {
        const uint32_t k = (uint32_t)(((uintptr_t)r[1] - (uintptr_t)x->tbl) / 10);
        if (k > 3) { x->bad = true; return 0; }
        return (uint32_t)(uintptr_t)x->keys[k];
    }
    case 0xceb00: *(uint16_t*)(uintptr_t)r[1] = 0; return 0;
    case 0xceab8: { const uint16_t* a = (const uint16_t*)(uintptr_t)r[2]; uint16_t* d = (uint16_t*)(uintptr_t)r[1]; do *d++ = *a; while (*a++); return 0; }
    case 0xceab0: return (uint32_t)slen((const uint16_t*)(uintptr_t)r[1]);
    case 0xd35a: { const uint16_t* t = (const uint16_t*)(uintptr_t)r[1]; return x->adv ? (uint32_t)d2_win_text_width(x->adv, t, slen(t)) : 0; }
    case 0xd096: put(x, { 1, (int32_t)s[1], (int32_t)s[2] }); return 0;
    case 0xd37e: {
        const uint16_t* t = (const uint16_t*)(uintptr_t)r[1]; const int n = slen(t);
        if (x->used + 5 + n > x->cap) { x->bad = true; return 0; }
        put(x, { 3, (int32_t)r[2], (int32_t)s[0], (int32_t)s[1], n });
        for (int i = 0; i < n; ++i) x->ops[x->used++] = t[i];
        return 0;
    }
    default: x->bad = true; return 0;
    }
}
}

extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_belt_draw(const int32_t* in, const uint8_t* adv, int32_t* ops, int32_t cap) {
    Ctx* x = &g_c;
    std::memset(x, 0, sizeof *x);
    x->nb = in[4]; x->ni = in[5]; x->record = in[2];
    if (x->nb < 0 || x->nb > MAXB || x->ni < 0 || x->ni > MAXI) return -1;
    x->boxes = in + 6; x->slot = in + 6 + 4 * x->nb;
    const int32_t* keys = x->slot + x->ni;
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 15 && j < keys[16 * i]; ++j) x->keys[i][j] = (uint16_t)keys[16 * i + 1 + j];
    x->ops = ops; x->cap = cap; x->adv = adv; x->tbl = g_tbl;
    for (int i = 0; i < x->ni; ++i) {
        x->aux[i][3] = (uint32_t)x->slot[i];
        x->units[i][0] = 2; x->units[i][11] = (uint32_t)(uintptr_t)x->aux[i];   // type 2: belt slot at aux+0xC
    }
    std::memset(g_tbl, 0xff, sizeof g_tbl);
    for (int i = 0; i < 4; ++i) {                            // the key binding table: id, key (0xFFFF unbound), state
        const uint32_t id = (uint32_t)i + 1, st = 1;
        const uint16_t key = keys[16 * i] > 0 ? 1 : 0xFFFF;
        std::memcpy(g_tbl + 10 * i, &id, 4); std::memcpy(g_tbl + 10 * i + 4, &key, 2); std::memcpy(g_tbl + 10 * i + 6, &st, 4);
    }
    std::memset(g_player, 0, sizeof g_player);
    g_player[0x60 / 4] = 0x2000;                             // inventory handle
    D2c28dc0G g = {};
    g.w = in[0]; g.h = in[1]; g.state = (uint32_t)in[2];
    g.cell = 1; g.adfc = (uint32_t)in[3]; g.g46c = (uint32_t)in[3];
    for (int i = 0; i < 4; ++i) g.hot[i] = (uint32_t)i + 1;
    if ((uintptr_t)g_player > 0xffffffffu || (uintptr_t)x->units[0] > 0xffffffffu) return -1;
    d2c_28dc0({ x, cb }, &g, (uint32_t)(uintptr_t)g_player, 0, g_tbl);
    return x->bad ? -1 : x->used;
}
