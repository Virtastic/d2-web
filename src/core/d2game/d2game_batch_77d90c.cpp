// src/core/d2game/d2game_batch_77d90c.cpp -- 1.13c D2Game.dll AI think rows (batch lane).
// Implementation + marker only, from the cached decompiles in build-wasm/ghidra-d2game/decomp/.
//
// Unmapped callees are seams on D2Game820Env, named by their D2Game RVA (the decompile header
// lists them by RVA). Addresses are 32-bit guest offsets into linear memory.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)(a + o), 4); return v; }
static inline int32_t  rdS16(uint32_t a, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)(a + o), 2); return v; }
static inline void wr32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)(uintptr_t)(a + o), &v, 4); }
static inline void wr8(uint32_t a, uint32_t o, uint8_t v) { *(uint8_t*)(uintptr_t)(a + o) = v; }

struct D2Game820Env {
    // 0x20EC0 (no arguments); return value is passed through.
    uint32_t (*sub20EC0)(void);
    // 0x212F0(unit, buf) and 0x21290(target, buf): fill a 16-byte local record.
    void     (*sub212F0)(uint32_t unit, uint32_t buf);
    void     (*sub21290)(uint32_t target, uint32_t buf);
    // 0xE03A0(a, b): returns the unit pointer compared with the target.
    uint32_t (*subE03A0)(uint32_t a, uint32_t b);
    // 0xF2380(target).
    uint32_t (*subF2380)(uint32_t target);
    // 0x1180(stateAddr): reads the 64-bit roll state; returned value is compared with a table short.
    uint32_t (*sub1180)(uint32_t stateAddr);
    // 0xF2220(unit, a).
    uint32_t (*subF2220)(uint32_t unit, uint32_t a);
    // 0xF22C0(value).
    uint32_t (*subF22C0)(uint32_t value);
    // 0xF26B0(target, unit, a, b).
    uint32_t (*subF26B0)(uint32_t target, uint32_t unit, uint32_t a, uint32_t b);
    // 0xF2540(unit, target, a, 2, 0, 0, 1).
    uint32_t (*subF2540)(uint32_t unit, uint32_t target, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);
};

// Roll step on the target's 64-bit state at +0x8: state = low32(state) * 0x6AC690C5 + target[+0x24].
static inline uint64_t rngStep820(uint32_t target) {
    const uint64_t s = (uint64_t)rd32(target, 0x8) * 0x6AC690C5ull + (uint64_t)rd32(target, 0x24);
    wr32(target, 0x8, (uint32_t)s);
    wr32(target, 0xC, (uint32_t)(s >> 32));
    return s;
}

// 1.13c D2Game+0x820D0
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_820d0(const D2Game820Env* env, uint32_t unit, uint32_t target,
                                                 uint32_t rec) {
    const uint32_t tbl = rd32(rec, 0x1C);          // param_3[7]: per-monster table base
    const uint32_t flag = rd32(rec, 0x18);         // param_3[6]
    const uint32_t a3 = rd32(rec, 0x8);            // param_3[2]
    const uint32_t mode = rd32(rec, 0x0);          // *param_3: mode record
    const uint32_t mo = (uint32_t)*(uint8_t*)(uintptr_t)(unit + 0x6D) * 2;  // table index by mode byte
    const uint32_t stateAddr = target + 8;

    // The roll-gated branch: a target that is valid and whose owner resolves to it.
    uint32_t local = 0;                            // local_1c record address when used
    uint8_t localBuf[16] = {0};
    const uint32_t localAddr = (uint32_t)(uintptr_t)localBuf;
    uint32_t rec8 = 0;                             // puVar8
    bool toLab = false;

    const bool tgtOk = target != 0 && rd32(target, 0) == 1 && rd32(target, 0x14) != 0;
    bool condA = !tgtOk;
    if (!condA) {
        local = rd32(rd32(target, 0x14), 0x28);    // iVar5
        if (local == 0) condA = true;
        else {
            rec8 = rd32(local, 0x20);
            if (rec8 == 0) condA = true;
        }
    }
    if (condA) {
        if ((int32_t)rd32(rec, 0x14) > 0x13) {
            toLab = true;
        } else {
            uint32_t piVar7 = 0;
            if (target != 0 && rd32(target, 0) == 1 && rd32(target, 0x14) != 0) {
                const uint32_t i5 = rd32(target, 0x14);
                const uint32_t i5b = rd32(i5, 0x28);
                if (i5b != 0 && rd32(i5b, 0x28) != 0)
                    piVar7 = env->subE03A0(rd32(i5b, 0x28), rd32(i5b, 0x2C));
            }
            if (piVar7 != target) {
                toLab = true;
            } else {
                const uint64_t s = rngStep820(target);
                if (rdS16(tbl, 0x6E + mo) <= (int32_t)((uint32_t)s % 100)) toLab = true;
            }
            if (!toLab) {
                wr32(localAddr, 8, 1);             // local_14 = 1
                env->sub212F0(unit, localAddr);
                env->sub21290(target, localAddr);
                rec8 = localAddr;
            }
        }
    }

    if (!toLab && rd32(rec8, 8) == 1) {
        if (flag != 0 && rdS16(tbl, 0x170) >= 0) {
            env->sub20EC0();
            return env->subF2380(target);
        }
        const uint32_t t9 = (target == 0 || rd32(target, 0) != 1 || rd32(target, 0x14) == 0)
                                ? 0 : rd32(rd32(target, 0x14), 0x2C);
        wr32(t9, 0x18, 2);
        wr32(t9, 0x1C, 100);
        const uint32_t r = env->subF2540(unit, target, a3, 2, 0, 0, 1);
        if (r != 0) return r;
        return env->sub20EC0();
    }

    // LAB: no roll-gated state applied.
    if (flag == 0) {
        if (rd32(mode, 0x14) != 0) {
            const uint32_t t = (target == 0 || rd32(target, 0) != 1 || rd32(target, 0x14) == 0)
                                   ? 0 : rd32(rd32(target, 0x14), 0x2C);
            wr32(t, 0x18, 2);
            wr8(t, 0x20, 4);
            env->subF2540(unit, target, a3, 2, 0, 0, 1);
            const uint64_t s = rngStep820(target);
            const uint32_t v = (uint32_t)s;
            if (10 < v % 100) wr32(mode, 0x14, 0);
            return v / 100;
        }
        const uint32_t r = env->subF26B0(target, unit, a3, 0);
        wr32(mode, 0x14, 1);
        return r;
    }

    {
        const uint64_t s = rngStep820(target);
        if (rdS16(tbl, 0x56 + mo) <= (int32_t)((uint32_t)s % 100))
            return env->subF22C0((uint32_t)rdS16(tbl, 0x62 + mo));
    }
    if (rdS16(tbl, 0x170) >= 0 && (int32_t)env->sub1180(stateAddr) < rdS16(tbl, 0x68 + mo))
        return env->subF2380(target);
    rngStep820(target);
    // Both outcomes of the last roll return the same call.
    return env->subF2220(unit, a3);
}
