// D2Game+0x406D0 -- monster mode "move" do-function, retail convention: ECX = game, EDX = unit, plain ret.
// Order (disassembled): monstats row (unit+4, dt+0xB64/+0xB6C, stride 0x1A4) with a nonzero short at +0x18 and a nonnull unit+0x2C, else return 2.
// x = 0x1100(unit), y = 0x1130(unit). When 0xB28E(unit) < 2: 0xA50E(unit, -(0xAAD8(u2c) - y)), 0xAF5E(unit, 0xAAC6(u2c) - x).
// 0xACC4(u2c) nonzero: a 23-dword request (0xB, 0xE0A20(EAX = unit, stack game), row short +0x18, x, y, 0xB26A, 0xB0FC, row +0x38, then 0xB24C
// and 0xB288) goes through 0x6F930 (EDX = request, ECX = game); the second send negates words 7 and 8. Every path past the row checks ends in the
// tail 0x3F320 (stack game, EBX = unit), whose result is returned.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game406D0Env {
    const uint8_t* dataTables;                                       // *sgptDataTables
    uint32_t (*posX)(uint32_t unit);                                 // 0x1100 (ECX)
    uint32_t (*posY)(uint32_t unit);                                 // 0x1130 (ECX)
    uint32_t (*period)(uint32_t unit);                               // 0xB28E
    uint32_t (*pathX)(uint32_t u2c);                                 // 0xAAC6
    uint32_t (*pathY)(uint32_t u2c);                                 // 0xAAD8
    void     (*setY)(uint32_t unit, uint32_t v);                     // 0xA50E
    void     (*setX)(uint32_t unit, uint32_t v);                     // 0xAF5E
    uint32_t (*check)(uint32_t u2c);                                 // 0xACC4
    uint32_t (*gameId)(uint32_t unit, uint32_t game);                // 0xE0A20 (EAX = unit, stack game)
    uint32_t (*skillId)(uint32_t unit);                              // 0xB26A
    uint32_t (*level)(uint32_t unit);                                // 0xB0FC
    uint32_t (*dirX)(uint32_t unit);                                 // 0xB24C
    uint32_t (*dirY)(uint32_t unit);                                 // 0xB288
    void     (*send)(uint32_t game, uint32_t* req);                  // 0x6F930 (ECX = game, EDX = request)
    uint32_t (*tail)(uint32_t game, uint32_t unit);                  // 0x3F320
};

static inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline uint32_t dt32(const D2Game406D0Env* e, uint32_t o) { return rd32((uint32_t)(uintptr_t)e->dataTables + o); }

// 1.13c D2Game+0x406D0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Game_406D0(uint32_t game, uint32_t unit, const D2Game406D0Env* e)
{
    if (!unit) return 2;
    const int32_t cls = (int32_t)rd32(unit + 4);
    if (cls < 0 || cls >= (int32_t)dt32(e, 0xB6C)) return 2;
    const uint32_t rec = dt32(e, 0xB64) + (uint32_t)cls * 0x1A4;
    if (!rec) return 2;
    int16_t s18; std::memcpy(&s18, (const void*)(uintptr_t)(rec + 0x18), 2);
    if (s18 == 0) return 2;
    const uint32_t u2c = rd32(unit + 0x2C);
    if (!u2c) return 2;
    const uint32_t x = e->posX(unit);
    const uint32_t y = e->posY(unit);
    if ((int32_t)e->period(unit) < 2) {
        const uint32_t px = e->pathX(rd32(unit + 0x2C));
        const uint32_t py = e->pathY(rd32(unit + 0x2C));
        e->setY(unit, 0u - (py - y));
        e->setX(unit, px - x);
    }
    if (e->check(u2c)) {
        uint32_t b[0x17] = {};
        b[0] = 0xB;
        b[1] = e->gameId(unit, game);
        b[5] = x;
        b[6] = y;
        b[11] = e->skillId(unit);
        b[12] = e->level(unit);
        b[13] = rd32(rec + 0x38);
        b[4] = (uint32_t)(int32_t)s18;
        b[7] = e->dirX(unit);
        b[8] = e->dirY(unit);
        e->send(game, b);
        b[7] = 0u - e->dirX(unit);
        b[8] = 0u - e->dirY(unit);
        e->send(game, b);
    }
    return e->tail(game, unit);
}
