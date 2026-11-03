// D2Game+0x36B90 (QUESTS_SetObjectSelection): EAX = request S {+0 game, +4 unit, +0x10 object id}, no stack arguments, plain ret.
// Order (disassembled): a unit with [unit+0x10] != 0 returns 0 untouched; else Ordinal 10141(unit, &local8) (thunk 0xA544); rec = Ordinal 10688(S[4])
// (thunk 0xA520); [rec+0x140] == 0: Ordinal 11090(unit, 2) (thunk 0xA4A2); else Ordinal 11090(unit, 1) then 0x8EA80 (ecx = game, edx = unit,
// stack 1, (rec[0xDC] >> 8) + game[0xA8], 0, 0, 0). A unit still set then clears bit 1 of [unit+0xC4]. Returns 1.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game36B90Env {
    void     (*unitInfo)(uint32_t unit, void* out8);                       // Ordinal 10141 via 0xA544 (stdcall, 2 words)
    uint32_t (*getRec)(uint32_t id);                                       // Ordinal 10688 via 0xA520
    void     (*setMode)(uint32_t unit, uint32_t mode);                     // Ordinal 11090 via 0xA4A2
    void     (*spawn)(uint32_t game, uint32_t unit, uint32_t one, uint32_t arg, uint32_t z0, uint32_t z1, uint32_t z2); // 0x8EA80
};

static inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }

// 1.13c D2Game+0x36B90
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Game_36B90(uint32_t s, const D2Game36B90Env* e)
{
    uint32_t unit = rd32(s + 4);
    if (unit && rd32(unit + 0x10)) return 0;
    uint8_t local[8];
    e->unitInfo(unit, local);
    const uint32_t rec = e->getRec(rd32(s + 0x10));
    if (*(const uint8_t*)(uintptr_t)(rec + 0x140) == 0) {
        e->setMode(rd32(s + 4), 2);
    } else {
        e->setMode(rd32(s + 4), 1);
        const uint32_t game = rd32(s);
        e->spawn(game, rd32(s + 4), 1, (rd32(rec + 0xdc) >> 8) + rd32(game + 0xa8), 0, 0, 0);
    }
    unit = rd32(s + 4);
    if (unit) {
        uint32_t f = rd32(unit + 0xc4) & 0xfffffffdu;
        std::memcpy((void*)(uintptr_t)(unit + 0xc4), &f, 4);
    }
    return 1;
}
