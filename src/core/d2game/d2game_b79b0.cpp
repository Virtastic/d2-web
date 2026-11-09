// 1.13c D2Game+0xB79B0, from the disassembly.
// EAX = target (0 = no target), ECX = unit; stack (stdcall, ret 0x14): game, aiControl, nMax, nCount, aiCmd.
// 1.13c splits the 1.10f monolith: melee goes to 0xB7750 (ESI = unit; game, target, notColliding), the open path to 0xB7380 (EAX = unit),
// the colliding case to 0xB75D0 (EAX = target). The callees are seams; the capture hooks every one of them.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2GB79B0Seams {
    void*    (*at)(uint32_t guest);
    void     (*fatal)(uint32_t line);
    uint32_t (*posY)(uint32_t path);                                  // 0xA3B8, 1 word
    uint32_t (*posX)(uint32_t path);                                  // 0xA454, 1 word
    uint32_t (*room)(uint32_t unit);                                  // 0xA478, 1 word
    uint32_t (*collide)(uint32_t room, uint32_t x, uint32_t y, uint32_t size, uint32_t mask); // 0xAC6A, 5 words
    uint32_t (*gameOf)(uint32_t unit);                                // 0xDF780, EAX = unit
    uint32_t (*uidOf)(uint32_t target);                               // 0x37440, EAX = target
    uint32_t (*level)(uint32_t room);                                 // 0xA556, 1 word
    uint32_t (*objDist)(uint32_t obj, uint32_t x, uint32_t y);        // 0xAB80, 3 words
    uint32_t (*tgtDist)(uint32_t target, uint32_t x, uint32_t y);     // 0xB0980, EBX = target, 2 words
    uint32_t (*melee)(uint32_t unit, uint32_t target, uint32_t z);    // 0xA754, 3 words
    uint32_t (*wall)(uint32_t unit, uint32_t target, uint32_t mask);  // 0xAD78, 3 words
    uint32_t (*b7750)(uint32_t esiUnit, uint32_t game, uint32_t target, uint32_t notColliding);
    uint32_t (*b75d0)(uint32_t eaxTarget, uint32_t game, uint32_t unit, uint32_t count, uint32_t medium, uint32_t far);
    uint32_t (*b7380)(uint32_t eaxUnit, uint32_t game, uint32_t ai, uint32_t target, uint32_t count, uint32_t medium, uint32_t far, uint32_t close, uint32_t nMax);
};

extern "C" uint32_t d2game_b79b0_port(uint32_t T, uint32_t U, uint32_t game, uint32_t ai, uint32_t nMax, uint32_t nCount,
                                      uint32_t cmd, const D2GB79B0Seams* s)
{
    auto rd = [&](uint32_t a) { uint32_t v; memcpy(&v, s->at(a), 4); return v; };
    const uint32_t p0 = rd(ai + 0x14);
    if (p0) return p0;
    if (!T) {
        const int32_t type = (int32_t)rd(U);
        const bool direct = type == 2 || (type > 3 && type <= 5);
        uint32_t path = rd(U + 0x2c);
        const uint32_t y = direct ? rd(rd(U + 0x2c) + 0x10) : (path ? s->posY(path) : 0);
        path = rd(U + 0x2c);
        const uint32_t x = direct ? rd(rd(U + 0x2c) + 0xc) : (path ? s->posX(path) : 0);
        if (s->collide(s->room(U), x, y, 2, 0x40)) return 4;
        const uint64_t v = (uint64_t)rd(U + 0x20) * 0x6ac690c5u + rd(U + 0x24);
        const uint32_t lo = (uint32_t)v, hi = (uint32_t)(v >> 32);
        memcpy(s->at(U + 0x24), &hi, 4); memcpy(s->at(U + 0x20), &lo, 4);
        return ((int32_t)(lo % 100) < 8) * 8 + 1;
    }
    if (!game) game = s->gameOf(U);
    uint32_t close = 0;
    if (rd(T) == 0) {
        const uint32_t id = s->uidOf(T);
        if (!game) { s->fatal(0x2d3); return 0; }
        uint32_t obj = rd(game + (id & 0x7f) * 4 + 0x1520);
        while (obj && rd(obj + 0xc) != id) obj = rd(obj + 0xe4);
        if (obj && s->level(s->room(obj)) == 0x84) {
            if (!cmd || s->objDist(obj, rd(cmd + 0xc), rd(cmd + 0x10)) < 0x4b) close = 1;
        }
    }
    uint32_t medium = 0, far = 0;
    if (cmd) {
        const int32_t d = (int32_t)s->tgtDist(T, rd(cmd + 0xc), rd(cmd + 0x10));
        if (d > 0x4b) medium = 1;
        if (d > 0x64) far = 1;
    }
    const uint32_t melee = s->melee(U, T, 0);
    const uint32_t notColliding = s->wall(U, T, 4) == 0;
    if (melee) return s->b7750(U, game, T, notColliding);
    if (notColliding) return s->b7380(U, game, ai, T, nCount, medium, far, close, nMax);
    return s->b75d0(T, game, U, nCount, medium, far);
}
