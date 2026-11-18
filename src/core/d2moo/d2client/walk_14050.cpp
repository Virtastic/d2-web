// 1.13c D2Client+0x14050: per-frame room/cell walk setup. Fills the walk context
// (EBX), submits every floor cell of the player's rooms to 0x13CE0 and nudges the frame bounds (0x13FD0) near the edges.
#include <cstddef>
#include <cstdint>
#include <emscripten.h>

struct WalkCtx {   // EBX of the retail call
    int32_t a0, a1, spawnA, spawnB, f4, t5, t6; uint32_t unit20; int32_t mode; uint32_t view; int32_t flag;
    int32_t minx, maxx, miny, maxy, ox, oy;
};
static_assert(offsetof(WalkCtx, unit20) == 0x1C && offsetof(WalkCtx, flag) == 0x28 && offsetof(WalkCtx, oy) == 0x40, "ctx");
struct WUnit { int32_t type; uint8_t p0[0x28]; uint32_t path; };   // +0 type, +0x2C path (x +0xC, y +0x10)
static_assert(offsetof(WUnit, path) == 0x2C, "unit");
struct WCell { int32_t x, y, w, h, p0, flags, p1[6]; };            // stride 0x30; flags +0x14 (bits 14..16 = pass)
static_assert(sizeof(WCell) == 0x30 && offsetof(WCell, flags) == 0x14, "cell");

// Callees are D2Common/D2Gfx ordinals and D2Client functions; `call` is their one door: rva, EAX/EBX in, the stack words
// in push order (first argument first), their count. Returns EAX.
typedef uint32_t (*WalkCallee)(uint32_t rva, uint32_t eax, const uint32_t* st, uint32_t n);
static WalkCallee g_walk_callee;
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_14050_set_callee(WalkCallee f) { g_walk_callee = f; }
static uint32_t cl(uint32_t rva, uint32_t eax = 0, uint32_t a = 0, uint32_t b = 0, uint32_t c = 0, uint32_t d = 0, uint32_t e = 0, uint32_t n = 0, uint32_t f = 0) {
    const uint32_t st[6] = { a, b, c, d, e, f }; return g_walk_callee(rva, eax, st, n);
}
static uint32_t P(const void* p) { return (uint32_t)(uintptr_t)p; }

// 1.13c D2Client+0x14050; `view` = the stack arg (+0x24/+0x28 scroll origin), tick = global +0x1197F8, mode = +0x11C414,
// unitp = the player unit pointer (global +0x11BBFC; null is the retail fatal path, not vectored)
extern "C" void d2_client_14050(WalkCtx* c, const int32_t* view, uint32_t tick, int32_t mode, WUnit* unit) {
    c->a0 = (int32_t)cl(0xD0C6);
    c->a1 = (int32_t)cl(0xD07E);
    c->spawnA = c->spawnB = 0;
    c->f4 = (int32_t)cl(0xCACC4);
    c->mode = mode; c->view = P(view);
    uint32_t flag = 0;
    if (uint32_t u = cl(0xC1F0, 0, P(unit), 0, 0, 0, 0, 1)) {
        uint32_t r = cl(0xC214, 0, cl(0xC1DE, 0, u, 0, 0, 0, 0, 1), 0, 0, 0, 0, 1);
        if (*(const uint8_t*)(uintptr_t)(r + 9) && mode == 0 && (int32_t)cl(0xD0AE) == 2) flag = 1;
    }
    c->flag = (int32_t)flag;
    c->maxx = c->maxy = (int32_t)0x80000000; c->minx = c->miny = 0x7FFFFFFF;
    c->ox = (int32_t)(0x50 - (uint32_t)view[9]); c->oy = (int32_t)(-0x50 - (uint32_t)view[10]);
    if ((uint32_t)c->t5 + 3 < tick) { c->t5 = (int32_t)tick; if (c->f4) c->spawnA = 1; }
    uint32_t u2 = cl(0xC1DE, 0, cl(0xC1F0, 0, P(unit), 0, 0, 0, 0, 1), 0, 0, 0, 0, 1);
    cl(0xCAB4, 0, u2, 0, 0, 0, 0, 1);
    if ((cl(0xCAAE, 0, u2, 0, 0, 0, 0, 1) & 0xFF) && (uint32_t)c->t6 + 0x19 < tick) { c->t6 = (int32_t)tick; c->spawnB = 1; }
    c->unit20 = P(unit) + 0x20;
    uint32_t room = cl(0xC1F0, 0, P(unit), 0, 0, 0, 0, 1);
    if (!room) return;
    uint32_t list = 0, count = 0;
    { const uint32_t st[3] = { room, P(&list), P(&count) }; g_walk_callee(0xC3DC, 0, st, 3); }
    uint32_t rec = cl(0xC214, 0, cl(0xC1DE, 0, room, 0, 0, 0, 0, 1), 0, 0, 0, 0, 1);   // null is the retail fatal path (message 0x130)
    uint32_t nofloorOK = *(const uint32_t*)(uintptr_t)(rec + 0x214) != 0;
    for (int32_t i = 0; i < (int32_t)count; ++i) {
        uint32_t rm = ((const uint32_t*)(uintptr_t)list)[i];
        int32_t org[2];
        { const uint32_t st[2] = { rm, P(org) }; g_walk_callee(0xC5B0, 0, st, 2); }
        for (uint32_t pass = 1; pass < 3; ++pass) {
            uint32_t nofloor = nofloorOK && pass == 1;
            uint32_t n = 0; const WCell* k;
            { const uint32_t st[2] = { rm, P(&n) }; k = (const WCell*)(uintptr_t)g_walk_callee(0xCC34, 0, st, 2); }
            for (; n; --n, ++k)
                if (((uint32_t)k->flags >> 14 & 7) == pass)
                    cl(0x13CE0, P(c), P(k), nofloor, (uint32_t)(c->ox + k->x), (uint32_t)(k->y + c->oy), (uint32_t)(org[0] + k->w * 5), 6, (uint32_t)(org[1] + k->h * 5));
        }
    }
    if (!flag) return;
    int32_t mA = (c->a1 ? 3 : 0) + 30, mB = (c->a1 == 0) * 2 + 23;
    int32_t px = 0, py = 0;
    const WUnit* un = unit;
    bool pathy = un->type == 2 || (un->type > 3 && un->type < 6);
    if (pathy) { px = *(const int32_t*)(uintptr_t)(un->path + 0xC); py = *(const int32_t*)(uintptr_t)(un->path + 0x10); }
    else if (un->path) { px = (int32_t)cl(0xC3BE, 0, un->path, 0, 0, 0, 0, 1); py = (int32_t)cl(0xC2EC, 0, un->path, 0, 0, 0, 0, 1); }
    if (px - c->minx < mA) cl(0x13FD0, 0, P(c), (uint32_t)(c->minx - 5), (uint32_t)py, 0, (uint32_t)-5, 5);
    if (c->maxx - px < mB) cl(0x13FD0, 0, P(c), (uint32_t)(c->maxx + 5), (uint32_t)py, 0, 5, 5);
    if (py - c->miny < mA) cl(0x13FD0, 0, P(c), (uint32_t)px, (uint32_t)(c->miny - 5), (uint32_t)-5, 0, 5);
    if (c->maxy - py < mB) cl(0x13FD0, 0, P(c), (uint32_t)px, (uint32_t)(c->maxy + 5), 5, 0, 5);
}
