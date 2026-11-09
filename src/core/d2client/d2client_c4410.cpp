// 1.13c D2Client+0xC4410: EAX = unit, stack s0..s3 (ret 0x10). Reads the unit's two
// position words (players, missiles and items straight from the +0x2C block's +0x10/+0x0C; other types through the
// thunks 0xC2EC/0xC3BE, 0 with no block), builds a spawn request through 0xAFF10 (EDX = unit; stack s0, second word,
// first word, s1, s2; ret 0x14) and, on success, stamps the new unit's +0x2C block with 0xC610(unit)'s byte (0xC3C4),
// clears it with 0xC3E8(.., 0) and with s3 nonzero sets bit 0x10000 of the new unit's +0xC4 flags.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2cC4410Env {
    uint32_t (*c2ec)(uint32_t p);
    uint32_t (*c3be)(uint32_t p);
    uint8_t* (*aff10)(const uint8_t* unit, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4);
    uint32_t (*c610)(const uint8_t* unit);
    void (*c3c4)(uint32_t q, uint32_t v);
    void (*c3e8)(uint32_t q, uint32_t v);
};
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2c_c4410(const D2cC4410Env* e, const uint8_t* unit, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3) {
    int32_t t; uint32_t p;
    memcpy(&t, unit, 4); memcpy(&p, unit + 0x2c, 4);
    bool direct = t == 2 || (t > 3 && t <= 5);
    uint32_t first, second;
    if (direct) memcpy(&first, (const uint8_t*)(uintptr_t)p + 0x10, 4); else first = p ? e->c2ec(p) : 0;
    memcpy(&t, unit, 4);
    direct = t == 2 || (t > 3 && t <= 5);
    if (direct) memcpy(&second, (const uint8_t*)(uintptr_t)p + 0xc, 4); else second = p ? e->c3be(p) : 0;
    uint8_t* n = e->aff10(unit, s0, second, first, s1, s2);
    if (!n) return 0;
    uint8_t b = (uint8_t)e->c610(unit);
    uint32_t q; memcpy(&q, n + 0x2c, 4);
    e->c3c4(q, (s2 & 0xFFFFFF00u) | b);   // retail writes the byte into s2's stack slot and pushes the whole dword
    e->c3e8(q, 0);
    if (s3) { uint32_t f; memcpy(&f, n + 0xc4, 4); f |= 0x10000u; memcpy(n + 0xc4, &f, 4); }
    return 1;
}
