// src/core/d2game/d2game_batch_77d90b.cpp -- 1.13c D2Game.dll rows in 0x77D90-0x8A7E0 (batch lane).
// Implementation + marker only, from the cached decompiles in build-wasm/ghidra-d2game/decomp/.
//
// Register and stack arguments are explicit parameters. Unmapped callees are seams on
// D2Game77Env in retail argument order; the ordinal seams keep the D2Common ordinal number in
// their name because the decompile does not give their prototypes.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)(a + o), 4); return v; }
static inline void wr32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)(uintptr_t)(a + o), &v, 4); }

struct D2Game77Env {
    // 0x79910 gate, Ordinal_10494 with no arguments in the decompile; non-zero passes the gate.
    uint32_t (*ordinal10494)(void);
    // 0x77E70(eaxUnit, thisUnit); non-zero passes.
    uint32_t (*sub77E70)(uint32_t eaxUnit, uint32_t thisUnit);
    // Ordinal_11134 (no arguments); compared with the target's +0xC id.
    uint32_t (*ordinal11134)(void);
    // Ordinal_10973 (stat query); non-zero divisor check in 0x79910.
    uint32_t (*ordinal10973)(void);
    // Ordinal_10887(unit).
    void     (*ordinal10887)(uint32_t unit);
    // 0xDE870(thisUnit).
    void     (*subDE870)(uint32_t unit);
    // 0xD6760(thisUnit, eaxUnit); non-zero passes.
    uint32_t (*subD6760)(uint32_t thisUnit, uint32_t eaxUnit);
    // Ordinal_10157 (no arguments).
    void     (*ordinal10157)(void);
    // Ordinal_10331(a, b, c); returns a value passed to Ordinal_10885.
    uint32_t (*ordinal10331)(uint32_t a, uint32_t b, uint32_t c);
    // Ordinal_10885(value).
    void     (*ordinal10885)(uint32_t value);
    // 0xDFC10(packet): four-byte packet built by the caller.
    void     (*subDFC10)(const uint8_t* packet);
    // Ordinal_10408(unit).
    void     (*ordinal10408)(uint32_t unit);
    // 0xE20A0(thisUnit, unit).
    void     (*subE20A0)(uint32_t thisUnit, uint32_t unit);
    // 0xDFFD0(code, unit).
    void     (*subDFFD0)(uint32_t code, uint32_t unit);
    // Ordinal_10867(unit) and Ordinal_10750(unit).
    void     (*ordinal10867)(uint32_t unit);
    uint32_t (*ordinal10750)(uint32_t unit);
    // 0xEDDB0(skill, unit): returns the created unit or 0.
    uint32_t (*subEDDB0)(uint32_t skill, uint32_t unit);
    // Ordinal_10945(unit, 0xB8, 1).
    void     (*ordinal10945)(uint32_t unit, uint32_t a, uint32_t b);
};

// 1.13c D2Game+0x79910
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_79910(const D2Game77Env* env, uint32_t thisUnit, uint32_t eaxUnit,
                                             uint32_t param1) {
    if (env->ordinal10494() == 0) return;
    if (env->sub77E70(eaxUnit, thisUnit) == 0) return;
    const uint32_t wanted = env->ordinal11134();
    const int32_t id = (eaxUnit == 0) ? -1 : (int32_t)rd32(eaxUnit, 0xC);
    if (id == (int32_t)wanted) {
        if (env->ordinal10973() != 0) {
            env->ordinal10887(param1);
            env->subDE870(thisUnit);
        }
    }
    if (env->subD6760(thisUnit, eaxUnit) != 0) {
        env->ordinal10157();
        const uint32_t value = env->ordinal10331(param1, rd32(param1, 0), rd32(param1, 0xC));
        env->ordinal10885(value);
        // Four-byte packet 0x8E 0x00 followed by the target's +0xC id as a 16-bit value.
        const uint32_t id16 = rd32(eaxUnit, 0xC);
        uint8_t packet[4] = {0x8e, 0x00, (uint8_t)id16, (uint8_t)(id16 >> 8)};
        env->subDFC10(packet);
        env->ordinal10408(param1);
        env->subE20A0(thisUnit, param1);
        env->subDFFD0(0x5d, eaxUnit);
        return;
    }
    env->subDFFD0(0x17, eaxUnit);
}

// 1.13c D2Game+0x7B750
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_7b750(const D2Game77Env* env, uint32_t eaxUnit, uint32_t esiUnit) {
    // The 16-entry table has eight low entries of 8 and four of 0x2D5..0x2DA; the low three bits of
    // the LCG state pick the entry, so the result is always 8 for the eight low slots.
    static const uint32_t table[16] = {8, 8, 8, 8, 8, 8, 8, 8, 0x2d5, 0x2d5, 0x2d6, 0x2d6, 0x2d7, 0x2d8, 0x2d9, 0x2da};

    uint32_t type = rd32(esiUnit, 0);
    if (type != 2 && (type < 4 || type > 5) && rd32(esiUnit, 0x2C) != 0) {
        env->ordinal10867(rd32(esiUnit, 0x2C));
    }
    type = rd32(esiUnit, 0);
    if (type != 2 && (type < 4 || type > 5) && rd32(esiUnit, 0x2C) != 0) {
        env->ordinal10750(rd32(esiUnit, 0x2C));
    }

    const uint64_t state = (uint64_t)rd32(esiUnit, 0x20) * 0x6ac690c5ULL + (uint64_t)rd32(esiUnit, 0x24);
    const uint32_t skill = table[(uint32_t)state & 7];
    wr32(esiUnit, 0x20, (uint32_t)state);
    wr32(esiUnit, 0x24, (uint32_t)(state >> 32));

    const uint32_t owner = (eaxUnit != 0) ? eaxUnit : esiUnit;
    const uint32_t created = env->subEDDB0(skill, owner);
    if (created != 0) {
        wr32(created, 0xC4, rd32(created, 0xC4) | 0x4000000u);
        env->ordinal10945(created, 0xB8, 1);
    }
}
