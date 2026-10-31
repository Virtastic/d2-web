// src/core/d2game/d2game_batch_77d90d.cpp -- 1.13c D2Game.dll AI think rows in 0x7E640-0x89C30 (batch lane).
// Implementation + marker only, from the cached decompiles checked line by line against the retail
// disassembly (the decompiles drop register arguments and the stack command record).
//
// Every row is __fastcall(ecx = pGame, edx = pUnit, stack = pAiTickParam). Task fields (D2AiTickParamStrc):
// +0x00 pAiControl, +0x08 pTarget, +0x14 target distance, +0x18 bCombat, +0x1C monstats row; the unit's
// +0x20 is its 64-bit seed, game+0x6D the difficulty that indexes the row's 16-bit param tables.
// Unmapped callees are seams on D2Game77dEnv, named by D2Game RVA, with the register arguments the
// disassembly shows spelled out (eax/ecx/edx/ebx first, then the stack arguments in call order).
// The command record handed to 0x75E10 is the 8-dword local the callers build on their stack:
//   +0x00 kind (4 or 5), +0x04 unit, +0x08 pTarget, +0x15 byte 0x64 or 0x65.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)(a + o), 4); return v; }
static inline int32_t  rdS16(uint32_t a, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)(a + o), 2); return v; }
static inline uint32_t rd8(uint32_t a, uint32_t o) { return *(const uint8_t*)(uintptr_t)(a + o); }
static inline void wr32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)(uintptr_t)(a + o), &v, 4); }
static inline void wr8(uint32_t a, uint32_t o, uint8_t v) { *(uint8_t*)(uintptr_t)(a + o) = v; }

struct D2Game77dEnv {
    // Ordinal_11103(unit, 0) (stdcall).
    void     (*ordinal11103)(uint32_t unit, uint32_t zero);
    // 0x1320(eax = unit class id): monstats row or 0; byte +0x104 is masked with the 0xF8BE0 global.
    uint32_t (*sub1320)(uint32_t classId);
    // The global pointer at D2Game+0xF8BE0; dwords +0x10 and +0x14 are the masks.
    uint32_t (*globalF8BE0)(void);
    // 0x75E10(game, cmd, 1) (stdcall): post a command record.
    void     (*sub75E10)(uint32_t game, const uint8_t* cmd, uint32_t one);
    // 0x20EC0(eax = unit, stack game): free the current command.
    void     (*sub20EC0)(uint32_t unit, uint32_t game);
    // 0xE03A0(eax = type, edx = id, ecx = game): the unit found or 0 (SUNIT_GetServerUnit).
    uint32_t (*subE03A0)(uint32_t type, uint32_t id, uint32_t game);
    // 0xF1B10(al, game, unit) (stdcall): walk close to the unit.
    void     (*subF1B10)(uint32_t al, uint32_t game, uint32_t unit);
    // 0xF2220(eax = mode, edi = unit, game, pTarget): change mode and target.
    void     (*subF2220)(uint32_t mode, uint32_t unit, uint32_t game, uint32_t pTarget);
    // 0xF22C0(eax = game, stack value).
    void     (*subF22C0)(uint32_t game, uint32_t value);
    // 0xF2380(al, ecx = unit, ebx = game, skill, pTarget, 0, 0): use a skill.
    void     (*subF2380)(uint32_t al, uint32_t unit, uint32_t game, uint32_t skill, uint32_t pTarget, uint32_t z0, uint32_t z1);
    // 0xF2540(ecx = game, edx = unit, pTarget, 2, 0, 0, 1, 7).
    uint32_t (*subF2540)(uint32_t game, uint32_t unit, uint32_t pTarget, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);
    // 0xF2D80(eax = pTarget, game, unit, byte, 1): non-zero when it acted.
    uint32_t (*subF2D80)(uint32_t pTarget, uint32_t game, uint32_t unit, uint32_t byteArg, uint32_t one);
    // Ordinal_10507(unit); 0x9F9A0(eax = skill, value, 1); 0xA2100(ecx = unit, edx = 0, skill, -1).
    uint32_t (*ordinal10507)(uint32_t unit);
    void     (*sub9F9A0)(uint32_t skill, uint32_t value, uint32_t one);
    void     (*subA2100)(uint32_t unit, uint32_t zero, uint32_t skill, uint32_t minusOne);
    // Import thunks, named by their D2Game address (all stdcall): 0x2A718(unit) = 0..99 style roll R,
    // 0x2A478(unit) -> a, 0x2A556(a) -> b, 0x2A56E(b) -> byte, 0x2A4C0(player) -> table (dword +0x10+diff*4),
    // 0x2A568(v, 0x15, 0) -> flag, 0x2A3B8(sub)/0x2A454(sub) -> dword, 0x2AFE8(a, b, c, 0x40) -> word.
    uint32_t (*th2A718)(uint32_t unit);
    uint32_t (*th2A478)(uint32_t unit);
    uint32_t (*th2A556)(uint32_t a);
    uint32_t (*th2A56E)(uint32_t a);
    uint32_t (*th2A4C0)(uint32_t player);
    uint32_t (*th2A568)(uint32_t v, uint32_t k, uint32_t z);
    uint32_t (*th2A3B8)(uint32_t sub);
    uint32_t (*th2A454)(uint32_t sub);
    uint32_t (*th2AFE8)(uint32_t a, uint32_t b, uint32_t c, uint32_t d);
    // 0xF2C10(eax = pTarget, ebx = unit, game, 8, 1): non-zero when it acted.
    uint32_t (*subF2C10)(uint32_t pTarget, uint32_t unit, uint32_t game, uint32_t eight, uint32_t one);
    // 0xF26B0(eax = 0, ecx = unit, game, pTarget, 4).
    void     (*subF26B0)(uint32_t zero, uint32_t unit, uint32_t game, uint32_t pTarget, uint32_t four);
    // 0x1900(eax = unit): distance-like integer to the unit's current target.
    int32_t  (*sub1900)(uint32_t unit);
    // 0xB17B0(eax = 1, ecx = buf, edx = game, unit, callback): the per-unit search; the callback rva
    // (0x7BB00 or 0x7CF10) selects the filter. The buffer layout is the callback's.
    void     (*subB17B0)(uint32_t buf, uint32_t game, uint32_t unit, uint32_t callbackRva);
    // Import thunks (stdcall): 0x2A3D0(ecx = unit, edx = skill, -1) -> skill record or 0;
    // 0x2A400(unit, rec, 1) -> int; 0x2A68E(unit, state) -> flag; 0x2A3CA(unit, state, 0).
    uint32_t (*th2A3D0)(uint32_t unit, uint32_t skill, uint32_t minusOne);
    int32_t  (*th2A400)(uint32_t unit, uint32_t rec, uint32_t one);
    uint32_t (*th2A68E)(uint32_t unit, uint32_t state);
    void     (*th2A3CA)(uint32_t unit, uint32_t state, uint32_t zero);
    // 0x412F0(eax = unit, game, cmd): set the unit's AI command; cmd has kind at +8, id +0xC, type +0x10.
    void     (*sub412F0)(uint32_t unit, uint32_t game, uint32_t cmd);
    // 0xF3E30(eax = y, ecx = target unit or 0, game, skill, x): non-zero when the skill may be used.
    uint32_t (*subF3E30)(uint32_t y, uint32_t target, uint32_t game, uint32_t skill, uint32_t x);
    // 0xF2870(al, ecx = target, game, unit); 0xF2120(edi = skill, game, target, 0, 0).
    void     (*subF2870)(uint32_t al, uint32_t target, uint32_t game, uint32_t unit);
    void     (*subF2120)(uint32_t skill, uint32_t game, uint32_t target, uint32_t z0, uint32_t z1);
    // 0x1100(ecx = unit) / 0x1130(ecx = unit): the unit's x / y; 0x2AD78(unit, pTarget, 4) (stdcall).
    int32_t  (*sub1100)(uint32_t unit);
    int32_t  (*sub1130)(uint32_t unit);
    uint32_t (*th2AD78)(uint32_t unit, uint32_t pTarget, uint32_t four);
    // 0xB17B0 with its eax mode argument (1 or 9) in front; 0xB1BC0(eax = unit, ecx = game, &outDist)
    // returns a target unit and writes its distance.
    void     (*subB17B0m)(uint32_t mode, uint32_t buf, uint32_t game, uint32_t unit, uint32_t callbackRva);
    uint32_t (*subB1BC0)(uint32_t unit, uint32_t game, uint32_t outDist);
    // 0x89730(eax = unit, stack game); 0x7BE10(eax = game, esi = unit).
    uint32_t (*sub89730)(uint32_t unit, uint32_t game);
    void     (*sub7BE10)(uint32_t game, uint32_t unit);
    // 0x1450(esi = seed, stack n) and 0x11D0(ecx = seed, esi = n): seeded rolls below n.
    uint32_t (*sub1450)(uint32_t seed, uint32_t n);
    uint32_t (*sub11D0)(uint32_t seed, uint32_t n);
    // 0xF1D90(eax = mode, ecx, game, unit, a, b); 0xF20C0(eax = game, ebx = unit, value);
    // 0xF1FF0(eax = 8, ecx = unit, game, x, y); 0xB07B0(eax = unit, x, y) -> distance.
    void     (*subF1D90)(uint32_t mode, uint32_t ecx, uint32_t game, uint32_t unit, uint32_t a, uint32_t b);
    void     (*subF20C0)(uint32_t game, uint32_t unit, uint32_t value);
    void     (*subF1FF0)(uint32_t mode, uint32_t unit, uint32_t game, uint32_t x, uint32_t y);
    uint32_t (*subB07B0)(uint32_t unit, uint32_t x, uint32_t y);
    // 0x2A71E(unit, 0xFFFF, 1) -> mask; 0x2A694(room, ptr, mask) -> non-zero when blocked.
    uint32_t (*th2A71E)(uint32_t unit, uint32_t mask, uint32_t one);
    uint32_t (*th2A694)(uint32_t room, uint32_t ptr, uint32_t mask);
    // 0xE03A0 with the registers exactly as 0x89C30 loads them: eax = ai+0x30, ecx = ai+0x28, edx = ai+0x2C.
    uint32_t (*subE03A0raw)(uint32_t eax, uint32_t ecx, uint32_t edx);
    // The structure behind the pointer-to-pointer at D2Game+0x1829C: +0xB98 skill record table (stride 0x23C),
    // +0xBA0 record count.
    uint32_t (*globalSkillData)(void);
};

// 0x1180: eax = limit, ecx = 64-bit state. The roll step is state = low32 * 0x6AC690C5 + high32.
static inline uint32_t roll77d(uint32_t state, uint32_t n) {
    if ((int32_t)n < 1) return 0;
    const uint64_t s = (uint64_t)rd32(state, 0) * 0x6AC690C5ull + (uint64_t)rd32(state, 4);
    wr32(state, 0, (uint32_t)s);
    wr32(state, 4, (uint32_t)(s >> 32));
    if ((n & (n - 1)) != 0) return (uint32_t)s % n;
    return (n - 1) & (uint32_t)s;
}

// The 0x20-byte command record built on the callers' stack, then posted through 0x75E10.
static void postTargetCmd(const D2Game77dEnv* env, uint32_t game, uint32_t unit, uint32_t kind,
                          uint32_t pTarget, uint32_t maskOff) {
    uint8_t cmd[0x20];
    std::memset(cmd, 0, sizeof cmd);
    std::memcpy(cmd + 0x00, &kind, 4);
    std::memcpy(cmd + 0x04, &unit, 4);
    uint32_t bl = 0x64;
    if (unit != 0) {
        const uint32_t rec = env->sub1320(rd32(unit, 4));
        if (rec != 0 && (rd8(rec, 0x104) & rd32(env->globalF8BE0(), maskOff)) != 0) bl = 0x65;
    }
    cmd[0x15] = (uint8_t)bl;
    std::memcpy(cmd + 0x08, &pTarget, 4);
    env->sub75E10(game, cmd, 1);
}

// 1.13c D2Game+0x826B0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_826b0(const D2Game77dEnv* env, uint32_t game, uint32_t unit,
                                             uint32_t task) {
    const uint32_t flag = rd32(task, 0x18);
    const uint32_t row = rd32(task, 0x1C);
    const uint32_t pTarget = rd32(task, 0x8);
    const int32_t dist = (int32_t)rd32(task, 0x14);

    if (unit != 0 && rd32(unit, 0) == 1) {
        const uint32_t p = rd32(unit, 0x14);
        const uint32_t o = p ? rd32(p, 0x28) : 0;
        const uint32_t tbl = o ? rd32(o, 0x20) : 0;
        if (tbl != 0) {
            if (env->subE03A0(rd32(tbl, 0xC), rd32(tbl, 0x10), game) != 0) {
                env->ordinal11103(unit, 0);
                postTargetCmd(env, game, unit, 5, pTarget, 0x14);
                env->sub20EC0(unit, game);
                return;
            }
            env->sub20EC0(unit, game);
        }
    }
    if (flag != 0) {
        env->ordinal11103(unit, 0);
        postTargetCmd(env, game, unit, 4, pTarget, 0x10);
        return;
    }
    if (unit != 0 && rd32(unit, 0) == 1 && rd32(unit, 0x14) != 0) {
        const uint32_t mode = rd32(rd32(unit, 0x14), 0x54);
        if (mode == 3 || mode == 0x13) {
            env->ordinal11103(unit, 0);
            postTargetCmd(env, game, unit, 5, pTarget, 0x14);
            return;
        }
    }
    const uint32_t idx = rd8(game, 0x6D);
    if (dist < rdS16(row, 0x56 + idx * 2)) {
        const uint32_t r = roll77d(unit + 0x20, 100);
        if ((int32_t)r < rdS16(row, 0x5C + idx * 2)) {
            env->subF2220(5, unit, game, pTarget);
            return;
        }
        if (env->subF2D80(pTarget, game, unit, rd8(row, 0x68 + idx * 2), 1) != 0) return;
        if (dist < 4) {
            env->subF2220(5, unit, game, pTarget);
            return;
        }
    }
    const int32_t w = rdS16(row, 0x68 + idx * 2);
    env->subF1B10(w < 3 ? 3 : rd8(row, 0x68 + idx * 2), game, unit);
}

// 1.13c D2Game+0x7F780 (the function starts a few bytes earlier, at 0x7F760)
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_7f780(const D2Game77dEnv* env, uint32_t game, uint32_t unit,
                                             uint32_t task) {
    const uint32_t flag = rd32(task, 0x18);
    const uint32_t pTarget = rd32(task, 0x8);
    const uint32_t M = rd32(task, 0x0);
    const uint32_t row = rd32(task, 0x1C);
    const uint32_t idx = rd8(game, 0x6D);

    if (flag == 0) {
        wr32(M, 0x14, 0);
        if ((int32_t)roll77d(unit + 0x20, 100) < rdS16(row, 0x62 + idx * 2)) {
            env->subF2540(game, unit, pTarget, 2, 0, 0, 1, 7);
            return;
        }
        env->subF22C0(game, (uint32_t)rdS16(row, 0x68 + idx * 2));
        return;
    }
    if (rd32(M, 0x14) != 0) {
        if (rdS16(row, 0x56 + idx * 2) <= (int32_t)roll77d(unit + 0x20, 100)) {
            env->subF22C0(game, (uint32_t)rdS16(row, 0x5C + idx * 2));
            return;
        }
    }
    wr32(M, 0x14, 1);
    if (rdS16(row, 0x170) >= 0 &&
        (int32_t)roll77d(unit + 0x20, 100) < rdS16(row, 0x6E + idx * 2)) {
        env->subF2380(rd8(row, 0x180), unit, game, (uint32_t)rdS16(row, 0x170), pTarget, 0, 0);
        return;
    }
    if (rdS16(row, 0x172) >= 0 &&
        (int32_t)roll77d(unit + 0x20, 100) < rdS16(row, 0x74 + idx * 2)) {
        env->subF2380(rd8(row, 0x181), unit, game, (uint32_t)rdS16(row, 0x172), pTarget, 0, 0);
        return;
    }
    env->ordinal11103(unit, 0);
    postTargetCmd(env, game, unit, 4, pTarget, 0x10);
}

// 1.13c D2Game+0x89430
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_89430(const D2Game77dEnv* env, uint32_t game, uint32_t unit,
                                             uint32_t task) {
    const uint32_t flag = rd32(task, 0x18);
    const uint32_t row = rd32(task, 0x1C);
    const uint32_t pTarget = rd32(task, 0x8);
    const uint32_t idx = rd8(game, 0x6D);

    if (rdS16(row, 0x176) >= 0 && env->ordinal10507(unit) == 0) {
        env->sub9F9A0((uint32_t)rdS16(row, 0x176), (uint32_t)rdS16(row, 0x56 + idx * 2), 1);
        env->subA2100(unit, 0, (uint32_t)rdS16(row, 0x176), 0xFFFFFFFFu);
    }
    if (flag == 0) {
        if (rdS16(row, 0x170) >= 0 &&
            (int32_t)roll77d(unit + 0x20, 100) < rdS16(row, 0x6E + idx * 2)) {
            env->subF2380(rd8(row, 0x180), unit, game, (uint32_t)rdS16(row, 0x170), pTarget, 0, 0);
            return;
        }
        uint32_t t = 0;
        if (unit != 0 && rd32(unit, 0) == 1 && rd32(unit, 0x14) != 0) t = rd32(rd32(unit, 0x14), 0x2C);
        wr32(t, 0x18, 0xD);
        env->subF2540(game, unit, pTarget, 2, 0, 0, 1, 7);
        return;
    }
    if (rdS16(row, 0x174) >= 0 &&
        (int32_t)roll77d(unit + 0x20, 100) < rdS16(row, 0x5C + idx * 2)) {
        env->subF2380(rd8(row, 0x182), unit, game, (uint32_t)rdS16(row, 0x174), pTarget, 0, 0);
        return;
    }
    if (rdS16(row, 0x172) >= 0 &&
        (int32_t)roll77d(unit + 0x20, 100) < rdS16(row, 0x62 + idx * 2)) {
        env->subF2380(rd8(row, 0x181), unit, game, (uint32_t)rdS16(row, 0x172), pTarget, 0, 0);
        return;
    }
    const bool low = (int32_t)roll77d(unit + 0x20, 100) < rdS16(row, 0x68 + idx * 2);
    env->ordinal11103(unit, 0);
    postTargetCmd(env, game, unit, low ? 5 : 4, pTarget, low ? 0x14 : 0x10);
}

// The unit's sub-record at +0x2C of its monster data (0 for non-monsters); callers write through it.
static inline uint32_t monSub(uint32_t unit) {
    if (unit != 0 && rd32(unit, 0) == 1 && rd32(unit, 0x14) != 0) return rd32(rd32(unit, 0x14), 0x2C);
    return 0;
}

// The retail range assert (-126..126, else the logged _exit(-1)); the port stops via a trap.
static inline int32_t checkRange126(int32_t v) {
    if (v < -126 || v > 126) __builtin_trap();
    return v;
}

// 1.13c D2Game+0x7ED90 (Zakarum-zealot style think; L is the speed-up percentage from row 0x32/0x34)
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_7ed90(const D2Game77dEnv* env, uint32_t game, uint32_t unit,
                                             uint32_t task) {
    const uint32_t M = rd32(task, 0);
    const uint32_t row = rd32(task, 0x1C);
    const uint32_t flag = rd32(task, 0x18);
    const uint32_t pTarget0 = rd32(task, 0x8);
    const uint32_t d = rd8(game, 0x6D);
    const uint32_t seed = unit + 0x20;

    int32_t L = 0;
    const int32_t den = rdS16(row, 0x32);
    if (den > 0) {
        int32_t v = rdS16(row, 0x34) * 100 / den - 100;
        L = v <= 0 ? 0 : (v < 0x78 ? v : 0x78);
    }
    const int32_t R = (int32_t)env->th2A718(unit);
    const int32_t rowD62 = rdS16(row, 0x62 + d * 2);

    // Redirect a monster target to its owner (a player), when the owner is found.
    uint32_t t = pTarget0;
    if (t != 0) {
        if (rd32(t, 0) == 1) {
            const uint32_t md = rd32(t, 0x14);
            const uint32_t ai = md ? rd32(md, 0x28) : 0;
            uint32_t guid = 0xFFFFFFFFu, type = 0;  // retail leaves `type` unset when the owner slot is empty
            if (rd32(ai, 0x28) != 0) { guid = rd32(ai, 0x2C); type = rd32(ai, 0x30); }
            if (type == 0) t = env->subE03A0(0, guid, game);
        }
        if (t != 0 && rd32(t, 0) == 0) {
            const uint32_t a = env->th2A478(unit);
            const uint32_t b = env->th2A556(a);
            if ((env->th2A56E(b) & 0xFF) == 2) {
                const uint32_t tbl = env->th2A4C0(t);
                const uint32_t val = rd32(tbl, 0x10 + d * 4);
                if (env->th2A568(val, 0x15, 0) != 0) {
                    checkRange126(L);
                    const uint32_t sub = monSub(unit);
                    if (L != 0 && sub != 0) wr32(sub, 0x1C, (uint32_t)L);
                    if (env->subF2C10(pTarget0, unit, game, 8, 1) != 0) return;
                    env->subF1B10(6, game, unit);
                    return;
                }
            }
        }
    }

    const uint32_t mode = (unit != 0 && rd32(unit, 0) == 1 && rd32(unit, 0x14) != 0) ? rd32(rd32(unit, 0x14), 0x54) : 0xFFFFFFFFu;
    const bool inWalk = (mode == 3 || mode == 0x13);
    bool done = false;
    if (inWalk) {
        if (rd32(M, 0x18) == 0 && R < rowD62) {
            wr32(M, 0x14, 0);
            checkRange126(L);
            const uint32_t sub = monSub(unit);
            if (L != 0 && sub != 0) wr32(sub, 0x1C, (uint32_t)L);
            if (env->subF2C10(pTarget0, unit, game, 8, 1) != 0) return;
            wr32(M, 0x18, 5);
        }
        const uint32_t ty = rd32(unit, 0);
        uint32_t b, a;
        const uint32_t s2 = rd32(unit, 0x2C);
        if (ty == 2 || (ty > 3 && ty <= 5)) { b = rd32(s2, 0x10); a = rd32(s2, 0xC); }
        else { b = s2 ? env->th2A3B8(s2) : 0; a = s2 ? env->th2A454(s2) : 0; }
        const uint32_t u = env->th2A478(unit);
        if ((env->th2AFE8(u, a, b, 0x40) & 0xFFFF) != 0) {
            if (flag == 0) {
                env->subF2540(game, unit, pTarget0, 2, 0, 0, 1, 7);
                return;
            }
            const uint32_t r = roll77d(seed, 0x100);  // low byte of the advanced seed, no modulo
            uint32_t m = r < 0x80 ? 5 : 6;
            const uint32_t sub = monSub(unit);
            if (sub != 0) { wr32(sub, 0x18, m); wr8(sub, 0x20, 4); }
            env->subF2540(game, unit, pTarget0, 2, 0, 0, 1, 0);
            return;
        }
    }
    (void)done;

    if (rd32(M, 0x18) != 0) wr32(M, 0x18, rd32(M, 0x18) - 1);
    if (flag != 0) {
        if (rd32(M, 0x14) != 0) {
            const int32_t r = (int32_t)roll77d(seed, 100);
            if (r >= rdS16(row, 0x56 + d * 2)) {
                wr32(M, 0x14, 0);
                const uint32_t v = roll77d(seed, 100);
                if ((int32_t)v < 0x50) env->subF22C0(game, 10);
                else env->subF26B0(0, unit, game, pTarget0, 4);
                return;
            }
        }
        wr32(M, 0x14, 1);
        const bool hit = (int32_t)roll77d(seed, 100) < rdS16(row, 0x5C + d * 2);
        env->ordinal11103(unit, 0);
        postTargetCmd(env, game, unit, hit ? 5 : 4, pTarget0, hit ? 0x14 : 0x10);
        return;
    }
    wr32(M, 0x14, 0);
    if ((int32_t)roll77d(seed, 100) < rdS16(row, 0x68 + d * 2)) {
        checkRange126(L);
        const uint32_t sub = monSub(unit);
        if (L != 0 && sub != 0) wr32(sub, 0x1C, (uint32_t)L);
        env->subF2540(game, unit, pTarget0, 0xF, 0, 0, 1, 0);
        return;
    }
    env->subF2540(game, unit, pTarget0, 2, 0, 0, 1, 7);
}

// monSub write guard: the retail code stores through the pointer unchecked; the port skips a null one.
static inline void subSet(uint32_t unit, uint32_t off, uint32_t v) {
    const uint32_t s = monSub(unit);
    if (s != 0) wr32(s, off, v);
}

// 1.13c D2Game+0x800C0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_800c0(const D2Game77dEnv* env, uint32_t game, uint32_t unit,
                                             uint32_t task) {
    const uint32_t M = rd32(task, 0);
    const uint32_t row = rd32(task, 0x1C);
    const uint32_t flag = rd32(task, 0x18);
    const int32_t dist = (int32_t)rd32(task, 0x14);
    const uint32_t pTarget = rd32(task, 0x8);
    const uint32_t d = rd8(game, 0x6D);
    const uint32_t seed = unit + 0x20;

    if (unit != 0 && rd32(unit, 0) == 1 && rd32(unit, 0x14) != 0) {
        const uint32_t ai = rd32(rd32(unit, 0x14), 0x28);
        const uint32_t cmd = ai ? rd32(ai, 0x20) : 0;
        if (cmd != 0) {
            uint32_t found = 0;
            if (rd32(cmd, 8) == 1) found = env->subE03A0(rd32(cmd, 0x10), rd32(cmd, 0xC), game);
            if (found == 0 && rd32(cmd, 8) == 0xE) found = env->subE03A0(rd32(cmd, 0x10), rd32(cmd, 0xC), game);
            if (found != 0) {
                wr32(M, 0x14, 0);
                wr32(M, 0x18, 0);
                subSet(unit, 0x18, 0xD);
                subSet(unit, 0x1C, 0x32);
                env->subF2540(game, unit, found, 2, 0, 0, 1, 0);
                env->sub20EC0(unit, game);
                return;
            }
            env->sub20EC0(unit, game);
        }
    }
    const int32_t R = (int32_t)env->th2A718(pTarget);
    const uint32_t state = rd32(M, 0x14);
    if (state == 0) {
        if (flag == 0) {
            subSet(unit, 0x18, 0xD);
            subSet(unit, 0x1C, 0x32);
            env->subF2540(game, unit, pTarget, 2, 0, 0, 1, 7);
            return;
        }
        wr32(M, 0x18, 0);
        wr32(M, 0x14, 1);
        if ((int32_t)roll77d(seed, 100) < rdS16(row, 0x56 + d * 2)) env->subF2220(4, unit, game, pTarget);
        else env->subF22C0(game, (uint32_t)rdS16(row, 0x5C + d * 2));
        return;
    }
    if (state == 1) {
        const int32_t c = (int32_t)rd32(M, 0x18) + 1;
        wr32(M, 0x18, (uint32_t)c);
        if (c > rdS16(row, 0x62 + d * 2) && R > rdS16(row, 0x68 + d * 2)) {
            wr32(M, 0x14, 2);
            wr32(M, 0x18, 0);
            subSet(unit, 0x18, 2);
            subSet(unit, 0x1C, 0x32);
            env->subF2D80(pTarget, game, unit, 0xE, 1);
            return;
        }
        if (flag != 0) {
            if ((int32_t)roll77d(seed, 100) < rdS16(row, 0x56 + d * 2)) env->subF2220(4, unit, game, pTarget);
            else env->subF22C0(game, (uint32_t)rdS16(row, 0x5C + d * 2));
            return;
        }
        subSet(unit, 0x18, 0xD);
        subSet(unit, 0x1C, 0x32);
        env->subF2540(game, unit, pTarget, 2, 0, 0, 1, 7);
        return;
    }
    if (state == 2) {
        if (dist > 0xC) {
            const int32_t c = (int32_t)rd32(M, 0x18) + 1;
            wr32(M, 0x18, (uint32_t)c);
            if (c > 1) { wr32(M, 0x14, 0); wr32(M, 0x18, 0); }
            if ((int32_t)roll77d(seed, 100) < 0x14) {
                env->subF26B0(0, unit, game, pTarget, 4);
                return;
            }
        } else {
            subSet(unit, 0x18, 2);
            subSet(unit, 0x1C, 0x32);
            if (env->subF2D80(pTarget, game, unit, 0xE, 1) != 0) return;
            wr32(M, 0x14, 0);
            wr32(M, 0x18, 0);
        }
    }
    env->subF22C0(game, 10);
}

// 1.13c D2Game+0x81C80
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_81c80(const D2Game77dEnv* env, uint32_t game, uint32_t unit,
                                             uint32_t task) {
    const uint32_t M = rd32(task, 0);
    const uint32_t row = rd32(task, 0x1C);
    const uint32_t flag = rd32(task, 0x18);
    const uint32_t pTarget = rd32(task, 0x8);
    const uint32_t d = rd8(game, 0x6D);
    const uint32_t seed = unit + 0x20;

    if (flag != 0) {
        const int32_t r = (int32_t)roll77d(seed, 100);
        if (r >= rdS16(row, 0x62 + d * 2) && rd32(M, 0x18) != 0) {
            if ((int32_t)roll77d(seed, 100) > 0x14) {
                env->subF22C0(game, 0xF);
                return;
            }
            wr32(M, 0x14, 2);
            wr32(M, 0x18, 0);
            // falls into the state dispatch below
        } else {
            wr32(M, 0x18, rd32(M, 0x18) + 1);
            if (rdS16(row, 0x170) >= 0 && (int32_t)roll77d(seed, 100) < rdS16(row, 0x68 + d * 2)) {
                env->subF2380(rd8(row, 0x180), unit, game, (uint32_t)rdS16(row, 0x170), pTarget, 0, 0);
                return;
            }
            env->ordinal11103(unit, 0);
            postTargetCmd(env, game, unit, 4, pTarget, 0x10);
            return;
        }
    }
    const uint32_t state = rd32(M, 0x14);
    if (state == 0) {
        subSet(unit, 0x18, 0xD);
        subSet(unit, 0x1C, 0x64);
        env->subF2540(game, unit, pTarget, 2, 0, 0, 1, 0);
        return;
    }
    if (state == 1) {
        subSet(unit, 0x1C, 0x32);
        env->subF1B10(4, game, unit);
        const int32_t c = (int32_t)rd32(M, 0x18) + 1;
        wr32(M, 0x18, (uint32_t)c);
        if (c > rdS16(row, 0x6E + d * 2)) { wr32(M, 0x14, 0); wr32(M, 0x18, 0); }
        return;
    }
    if (state == 2) {
        subSet(unit, 0x18, 2);
        subSet(unit, 0x1C, 0x64);
        env->subF2D80(pTarget, game, unit, 0xA, 1);
        wr32(M, 0x18, 0);
        wr32(M, 0x14, 1);
        return;
    }
    wr32(M, 0x14, 0);
    wr32(M, 0x18, 0);
    env->subF22C0(game, 0xA);
}

// Hash lookup in the game's first unit table (game+0x1320, 128 chains linked through +0xE4, guid at +0xC).
static inline uint32_t gameUnitByGuid0(uint32_t game, uint32_t guid) {
    if (game == 0) __builtin_trap();  // the logged assert path in the original
    uint32_t u = rd32(game, 0x1320 + (guid & 0x7F) * 4);
    while (u != 0) {
        if (rd32(u, 0xC) == guid) return u;
        u = rd32(u, 0xE4);
    }
    return 0;
}

// 1.13c D2Game+0x83FE0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_83fe0(const D2Game77dEnv* env, uint32_t game, uint32_t unit,
                                             uint32_t task) {
    const uint32_t M = rd32(task, 0);
    const uint32_t row = rd32(task, 0x1C);
    const uint32_t flag = rd32(task, 0x18);
    const uint32_t pTarget = rd32(task, 0x8);
    const uint32_t d = rd8(game, 0x6D);
    const uint32_t seed = unit + 0x20;
    const uint32_t state = rd32(M, 0x14);

    auto reset8 = [&]() {
        wr32(M, 0x14, 0); wr32(M, 0x18, 0); wr32(M, 0x1C, 0);
        env->subF1B10(8, game, unit);
    };

    if (state == 5) {
        env->subF2D80(pTarget, game, unit, 0x10, 1);
        wr32(M, 0x14, 0);
        return;
    }
    if (state == 2) {
        const uint32_t f = gameUnitByGuid0(game, rd32(M, 0x18));
        if (f == 0 || rd32(f, 0x10) != 0xC) { reset8(); return; }
        if (env->sub1900(unit) > 4) {
            if ((int32_t)rd32(M, 0x1C) >= 6) { reset8(); return; }
            env->subF2540(game, unit, f, 2, 0, 0, 1, 0);
            wr32(M, 0x1C, rd32(M, 0x1C) + 1);
        }
        wr32(M, 0x14, 3);
        env->subF22C0(game, 8);
        return;
    }
    if (state == 3) {
        const uint32_t f = gameUnitByGuid0(game, rd32(M, 0x18));
        const int32_t skill = rdS16(row, 0x170);
        if (skill >= 0 && f != 0 && rd32(f, 0x10) == 0xC) {
            env->subF2380(rd8(row, 0x180), unit, game, (uint32_t)skill, f, 0, 0);
            wr32(M, 0x14, 4);
            return;
        }
        reset8();
        return;
    }
    if (state == 4) {
        if (flag == 0) {
            env->subF2220(5, unit, game, pTarget);
            wr32(M, 0x14, 5);
        } else {
            env->subF2D80(pTarget, game, unit, 8, 1);
        }
        return;
    }

    uint32_t buf[2] = {0, 0};
    const int32_t rad = rdS16(row, 0x74 + d * 2);
    buf[1] = (uint32_t)(rad * rad);
    env->subB17B0((uint32_t)(uintptr_t)buf, game, unit, 0x7BB00);
    const uint32_t res = buf[0];

    bool chase = false;  // jump to the 0x43AB tail
    if (state == 1) {
        if (res == 0) {
            if ((int32_t)roll77d(seed, 100) < 0x14) { wr32(M, 0x14, 0); wr32(M, 0x18, 0); wr32(M, 0x1C, 0); return; }
            env->subF1B10(8, game, unit);
            return;
        }
        wr32(M, 0x18, rd32(res, 0xC));
        if (env->sub1900(unit) > 2) {
            env->subF2540(game, unit, res, 2, 0, 0, 1, 0);
            wr32(M, 0x14, 2);
            wr32(M, 0x1C, 0);
        } else {
            env->subF22C0(game, 8);
            wr32(M, 0x14, 3);
            wr32(M, 0x1C, 0);
        }
        return;
    }
    if (res != 0) {
        bool first = true;
        if (env->sub1900(unit) < 9) {
            if ((int32_t)roll77d(seed, 100) < rdS16(row, 0x5C + d * 2)) { chase = true; first = false; }
        }
        if (first && (int32_t)roll77d(seed, 100) < rdS16(row, 0x6E + d * 2)) chase = true;
    }
    if (!chase) {
        if (flag != 0) {
            if ((int32_t)roll77d(seed, 100) < rdS16(row, 0x56 + d * 2)) env->subF2220(4, unit, game, pTarget);
            else env->subF22C0(game, 0xF);
            return;
        }
        if ((int32_t)roll77d(seed, 100) < rdS16(row, 0x62 + d * 2)) {
            env->subF2540(game, unit, pTarget, 2, 0, 0, 1, 0);
            return;
        }
        if (res == 0 || (int32_t)roll77d(seed, 100) >= rdS16(row, 0x68 + d * 2)) {
            wr32(M, 0x14, 3);
            env->subF22C0(game, 8);
            return;
        }
    }
    env->subF2540(game, unit, res, 2, 0, 0, 1, 0);
    wr32(M, 0x14, 2);
    wr32(M, 0x18, rd32(res, 0xC));
    wr32(M, 0x1C, 0);
}

// 1.13c D2Game+0x84720 -- helper, not a think entry: edi = game, esi = unit, ebx = monstats row, one stack
// argument (the value returned when nothing qualifies). Search buffer: dword 3 and dword 5 are the two
// candidate results, dwords 1, 4 and 6 start at 0x7FFFFFFF.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_84720(const D2Game77dEnv* env, uint32_t game, uint32_t unit,
                                                 uint32_t row, uint32_t deflt) {
    uint32_t buf[10] = {0};
    buf[1] = 0x7FFFFFFFu;
    buf[4] = 0x7FFFFFFFu;
    buf[6] = 0x7FFFFFFFu;
    env->subB17B0((uint32_t)(uintptr_t)buf, game, unit, 0x7CF10);
    const uint32_t d = rd8(game, 0x6D);
    const uint32_t a = buf[3];
    if (a != 0 && (int32_t)roll77d(unit + 0x20, 100) < rdS16(row, 0x5C + d * 2)) return a;
    if (buf[5] != 0 && (int32_t)roll77d(unit + 0x20, 100) < rdS16(row, 0x62 + d * 2)) return a;
    return deflt;
}

// 1.13c D2Game+0x87BB0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_87bb0(const D2Game77dEnv* env, uint32_t game, uint32_t unit,
                                             uint32_t task) {
    const uint32_t row = rd32(task, 0x1C);
    const uint32_t pTarget = rd32(task, 0x8);
    const int32_t dist = (int32_t)rd32(task, 0x14);
    const uint32_t d = rd8(game, 0x6D);
    const uint32_t seed = unit + 0x20;

    int32_t range = 1;
    if (rdS16(row, 0x170) >= 0) {
        const uint32_t rec = env->th2A3D0(unit, (uint32_t)rdS16(row, 0x170), 0xFFFFFFFFu);
        if (rec != 0) {
            const int32_t v = env->th2A400(unit, rec, 1);
            if (v > 0) range = v;
        }
    }
    if (rdS16(row, 0x170) >= 0 && dist < range && env->th2A68E(unit, 0xC) == 0) {
        uint32_t cmd[8] = {0};  // retail leaves dwords 0, 1 and 5.. unset
        cmd[2] = 1;
        cmd[3] = pTarget ? rd32(pTarget, 0xC) : 0xFFFFFFFFu;
        cmd[4] = pTarget ? rd32(pTarget, 0) : 6;
        env->sub412F0(unit, game, (uint32_t)(uintptr_t)cmd);
        env->subF2380(rd8(row, 0x180), unit, game, (uint32_t)rdS16(row, 0x170), pTarget, 0, 0);
        return;
    }
    if (env->th2A68E(unit, 0xC) != 0) env->th2A3CA(unit, 0xC, 0);

    uint32_t buf[7] = {0};
    buf[1] = 0x7FFFFFFFu;
    const int32_t rad = rdS16(row, 0x6E + d * 2);
    buf[2] = (uint32_t)(rad * rad);
    buf[4] = 0x7FFFFFFFu;
    buf[6] = (uint32_t)rdS16(row, 0x5C + d * 2);
    env->subB17B0((uint32_t)(uintptr_t)buf, game, unit, 0x7BB90);
    const uint32_t res = buf[0];
    if (res != 0 && (int32_t)roll77d(seed, 100) < rdS16(row, 0x56 + d * 2) &&
        env->subF3E30(0, res, game, (uint32_t)rdS16(row, 0x174), 0) != 0) {
        uint32_t cmd[8] = {0};
        cmd[2] = 0xE;
        cmd[3] = rd32(res, 0xC);
        cmd[4] = 1;
        env->sub412F0(unit, game, (uint32_t)(uintptr_t)cmd);
        if ((int32_t)buf[1] > rdS16(row, 0x62 + d * 2)) env->subF2870(0xA, res, game, unit);
        else env->subF2120((uint32_t)rdS16(row, 0x174), game, res, 0, 0);
        return;
    }
    if ((int32_t)roll77d(seed, 100) < rdS16(row, 0x68 + d * 2)) {
        const uint32_t m = roll77d(seed, 0x100) < 0x80 ? 5 : 6;
        const uint32_t sub = monSub(unit);
        if (sub != 0) { wr32(sub, 0x18, m); wr8(sub, 0x20, 4); }
        env->subF2540(game, unit, pTarget, 2, 0, 0, 1, 0);
        return;
    }
    env->subF22C0(game, 0xA);
}

// 1.13c D2Game+0x87E60
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_87e60(const D2Game77dEnv* env, uint32_t game, uint32_t unit,
                                             uint32_t task) {
    const uint32_t M = rd32(task, 0);
    const uint32_t row = rd32(task, 0x1C);
    const uint32_t flag = rd32(task, 0x18);
    const uint32_t pTarget = rd32(task, 0x8);
    const uint32_t d = rd8(game, 0x6D);
    const uint32_t seed = unit + 0x20;
    const int32_t now = (int32_t)rd32(game, 0xA8);
    const int32_t R = (int32_t)env->th2A718(unit);

    bool main = flag != 0;
    if (!main && unit != 0 && rd32(unit, 0) == 1 && rd32(unit, 0x14) != 0) {
        const uint32_t mode = rd32(rd32(unit, 0x14), 0x54);
        main = (mode == 3 || mode == 0x13);
    }
    if (main) {
        if (rdS16(row, 0x174) >= 0 && R < 0x21 && now > (int32_t)rd32(M, 0x14)) {
            wr32(M, 0x14, (uint32_t)(now + rdS16(row, 0x6E + d * 2) * 4));
            const int32_t ux = env->sub1100(unit), uy = env->sub1130(unit);
            const int32_t tx0 = env->sub1100(pTarget), ty0 = env->sub1130(pTarget);
            const uint32_t sh = flag != 0 ? 2 : 0;
            const int32_t tx = tx0 + ((tx0 - ux) << sh);
            const int32_t ty = ty0 + ((ty0 - uy) << sh);
            if (env->subF3E30((uint32_t)ty, 0, game, (uint32_t)rdS16(row, 0x174), (uint32_t)tx) != 0) {
                env->subF2380(rd8(row, 0x182), unit, game, (uint32_t)rdS16(row, 0x174), 0, (uint32_t)tx, (uint32_t)ty);
                return;
            }
        }
        if (flag != 0 && (int32_t)roll77d(seed, 100) < rdS16(row, 0x56 + d * 2)) {
            env->ordinal11103(unit, 0);
            postTargetCmd(env, game, unit, 4, pTarget, 0x10);
            return;
        }
    }

    uint32_t buf[4] = {0, 0, 0x7FFFFFFFu, 0};
    const int32_t rad = rdS16(row, 0x74 + d * 2);
    buf[3] = (uint32_t)(rad * rad);
    env->subB17B0((uint32_t)(uintptr_t)buf, game, unit, 0x7BC70);
    if (rdS16(row, 0x170) >= 0 && buf[0] != 0 && (int32_t)roll77d(seed, 100) < 0x19) {
        env->subF2380(rd8(row, 0x180), unit, game, (uint32_t)rdS16(row, 0x170), buf[0], 0, 0);
        return;
    }

    const bool near = env->th2AD78(unit, pTarget, 4) == 0;
    bool special = false;  // the 0x8282 tail (skill at +0x176)
    if (near) {
        if ((int32_t)roll77d(seed, 100) < rdS16(row, 0x68 + d * 2)) {
            if (rdS16(row, 0x176) >= 0 && now > (int32_t)rd32(M, 0x18) &&
                (int32_t)roll77d(seed, 100) < rdS16(row, 0x5C + d * 2)) {
                special = true;
            } else if (rdS16(row, 0x172) >= 0 && now > (int32_t)rd32(M, 0x1C) &&
                       (int32_t)roll77d(seed, 100) < rdS16(row, 0x62 + d * 2)) {
                env->subF2380(rd8(row, 0x181), unit, game, (uint32_t)rdS16(row, 0x172), pTarget, 0, 0);
                wr32(M, 0x1C, (uint32_t)(rd32(game, 0xA8) + 0x14));
                return;
            }
        }
    } else if ((int32_t)roll77d(seed, 100) < rdS16(row, 0x56 + d * 2) && rdS16(row, 0x176) >= 0 &&
               now > (int32_t)rd32(M, 0x18) && (int32_t)roll77d(seed, 100) < rdS16(row, 0x5C + d * 2)) {
        special = true;
    }
    if (special) {
        env->subF2380(rd8(row, 0x183), unit, game, (uint32_t)rdS16(row, 0x176), pTarget, 0, 0);
        wr32(M, 0x18, (uint32_t)(rdS16(row, 0x6E + d * 2) + (int32_t)rd32(game, 0xA8)));
        return;
    }

    if ((int32_t)roll77d(seed, 100) < 0x1E) {
        const uint32_t m = roll77d(seed, 0x100) < 0x80 ? 5 : 6;
        const uint32_t sub = monSub(unit);
        if (sub != 0) { wr32(sub, 0x18, m); wr8(sub, 0x20, 4); }
        env->subF2540(game, unit, pTarget, 2, 0, 0, 1, 0);
        return;
    }
    env->subF22C0(game, 0x14);
}

// 1.13c D2Game+0x890A0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_890a0(const D2Game77dEnv* env, uint32_t game, uint32_t unit,
                                             uint32_t task) {
    int32_t dist = (int32_t)rd32(task, 0x14);
    const uint32_t flag = rd32(task, 0x18);
    const uint32_t row = rd32(task, 0x1C);
    const uint32_t pTarget = rd32(task, 0x8);
    const uint32_t d = rd8(game, 0x6D);
    const uint32_t seed = unit + 0x20;

    if (flag != 0 && (int32_t)roll77d(seed, 100) < rdS16(row, 0x62 + d * 2)) {
        env->ordinal11103(unit, 0);
        postTargetCmd(env, game, unit, 4, pTarget, 0x10);
        return;
    }

    uint32_t buf[4] = {0, 0, 0, 0};  // retail leaves dword 2 unset
    const int32_t rad = rdS16(row, 0x68 + d * 2);
    buf[3] = (uint32_t)(rad * rad);
    uint32_t mode = 9, cb = 0;
    if (unit != 0 && rd32(unit, 0) == 1 && rd32(unit, 0x14) != 0) {
        const uint32_t fl = rd8(rd32(unit, 0x14), 0x16);
        if ((fl & 8) != 0 && (fl & 4) == 0) { mode = 1; cb = 0x7BD50; }
    }
    env->subB17B0m(mode, (uint32_t)(uintptr_t)buf, game, unit, cb);

    if ((int32_t)roll77d(seed, 100) < rdS16(row, 0x56 + d * 2)) {
        uint32_t cmd[8] = {0};  // retail leaves dwords 0, 1, 3, 4 and 5.. unset
        cmd[2] = 1;
        env->sub412F0(unit, game, (uint32_t)(uintptr_t)cmd);
    }
    uint32_t res = buf[0];
    if (res != 0 && buf[1] != 0 && (int32_t)roll77d(seed, 100) < rdS16(row, 0x56 + d * 2) &&
        env->subF3E30(0, res, game, (uint32_t)rdS16(row, 0x170), 0) != 0) {
        env->subF2120((uint32_t)rdS16(row, 0x170), game, res, 0, 0);
        return;
    }

    uint32_t tgt = 0;
    if (pTarget != 0 && dist < rdS16(row, 0x6E + d * 2) &&
        (int32_t)roll77d(seed, 100) < rdS16(row, 0x5C + d * 2)) {
        tgt = pTarget;
    } else {
        res = env->subB1BC0(unit, game, (uint32_t)(uintptr_t)&dist);
        if (res != 0 && dist < rdS16(row, 0x6E + d * 2) &&
            (int32_t)roll77d(seed, 100) < rdS16(row, 0x5C + d * 2)) {
            tgt = res;
        }
    }
    if (tgt != 0) {
        env->subF2380(rd8(row, 0x181), unit, game, (uint32_t)rdS16(row, 0x172), tgt, 0, 0);
        return;
    }

    if ((int32_t)roll77d(seed, 100) < rdS16(row, 0x62 + d * 2)) {
        const uint32_t m = roll77d(seed, 0x100) < 0x80 ? 5 : 6;
        const uint32_t sub = monSub(unit);
        if (sub != 0) { wr32(sub, 0x18, m); wr8(sub, 0x20, 3); }
        env->subF2540(game, unit, pTarget, 2, 0, 0, 1, 0);
        return;
    }
    env->subF22C0(game, 0xA);
}

static uint32_t unitCoord(const D2Game77dEnv* env, uint32_t u, bool y) {
    const uint32_t t = rd32(u, 0);
    if (t == 2 || (t > 3 && t <= 5)) return rd32(rd32(u, 0x2C), y ? 0x10 : 0xC);
    const uint32_t sub = rd32(u, 0x2C);
    if (sub == 0) return 0;
    return y ? env->th2A3B8(sub) : env->th2A454(sub);
}

// 1.13c D2Game+0x89C30
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_89c30(const D2Game77dEnv* env, uint32_t game, uint32_t unit,
                                             uint32_t task) {
    const uint32_t M = rd32(task, 0);
    const uint32_t row = rd32(task, 0x1C);
    const uint32_t flag = rd32(task, 0x18);
    const uint32_t pTarget = rd32(task, 0x8);
    int32_t dist = (int32_t)rd32(task, 0x14);
    int32_t state = (int32_t)rd32(M, 0x14);
    const uint32_t d = rd8(game, 0x6D);
    const uint32_t seed = unit + 0x20;

    auto endTail = [&] { env->subF20C0(game, unit, 0xC); };
    auto fleeTail = [&](uint32_t ecx) {
        env->subF1D90(2, ecx, game, unit, 9, 3);
        endTail();
        wr32(M, 0x14, 0xFFFFFFFFu);
    };

    if (pTarget == 0) {
        if (state < 1) { endTail(); return; }
        if (env->sub89730(unit, game) != 0) { fleeTail(unit); return; }
    }
    const bool far = env->sub1900(unit) > 0x90;
    if (pTarget != 0) {
        const uint32_t ra = env->th2A478(pTarget);
        const uint32_t rb = env->th2A478(unit);
        if (ra != rb && far) {
            if (state >= 1) {
                if (env->sub89730(unit, game) != 0) { fleeTail(unit); return; }
            } else {
                env->subF1D90(9, pTarget, game, unit, 2, 0);
                return;
            }
        }
    }

    if (state == 0) {
        bool skip = false;
        if (unit != 0 && rd32(unit, 0) == 1 && rd32(unit, 0x14) != 0) {
            const uint32_t ai = rd32(rd32(unit, 0x14), 0x28);
            if (ai != 0 && rd32(ai, 0x28) != 0 &&
                env->subE03A0raw(rd32(ai, 0x30), rd32(ai, 0x28), rd32(ai, 0x2C)) != 0) skip = true;
        }
        if (!skip && pTarget != 0 && far && (int32_t)roll77d(seed, 100) < 0x3C) {
            env->sub7BE10(game, unit);
            const uint32_t r = env->sub1450(seed, 8);
            wr32(M, 0x14, r + 0x18);
            env->subF1D90(8, pTarget, game, unit, 8, 8);
            endTail();
            return;
        }
    }

    uint32_t buf[4] = {0, 0x79, 0, (uint32_t)rdS16(row, 0x62 + d * 2)};  // dword 2 unset in retail
    env->subB17B0m(1, (uint32_t)(uintptr_t)buf, game, unit, 0x7B860);

    bool hold = false;  // the 0xAA0EA tail
    if (state > 1) {
        if (buf[0] != 0 || (dist < 6 && (int32_t)roll77d(seed, 100) < 0xF)) {
            wr32(M, 0x14, 1);
            state = 1;
            hold = true;
        } else {
            env->sub7BE10(game, unit);
            const uint32_t ux = unitCoord(env, unit, false), uy = unitCoord(env, unit, true);
            const uint32_t hx = rd32(M, 0x18), hy = rd32(M, 0x1C);
            if (hx != 0 && hy != 0 && env->subB07B0(unit, hx, hy) > 1) {
                bool blocked = false;
                const uint32_t ptr = (uint32_t)(uintptr_t)buf;
                if (pTarget != 0) {
                    const uint32_t mask = env->th2A71E(unit, 0xFFFF, 1);
                    blocked = env->th2A694(env->th2A478(pTarget), ptr, mask) != 0;
                }
                if (!blocked) {
                    const uint32_t mask = env->th2A71E(unit, 0xFFFF, 1);
                    blocked = env->th2A694(env->th2A478(unit), ptr, mask) != 0;
                }
                if (blocked) { wr32(M, 0x18, hx); wr32(M, 0x1C, hy); }
                wr32(M, 0x14, rd32(M, 0x14) - 1);
                const uint32_t sub = monSub(unit);
                if (sub != 0) wr32(sub, 0x18, 7);
                env->subF1FF0(8, unit, game, rd32(M, 0x18), rd32(M, 0x1C));
                endTail();
                return;
            }
            const int32_t span = state + 8;
            const uint32_t n = (uint32_t)(span * 2);
            const uint32_t tx = unitCoord(env, pTarget, false);
            int32_t bx = (int32_t)(env->sub11D0(seed, n) + (tx - (uint32_t)span));
            const uint32_t ty = unitCoord(env, pTarget, true);
            int32_t by = (int32_t)(env->sub11D0(seed, n) + (ty - (uint32_t)span));
            int32_t lim = span * 2;
            if (lim < 0xC) lim = 0xC; else if (lim > 0x24) lim = 0x24;
            while ((int32_t)env->subB07B0(unit, (uint32_t)bx, (uint32_t)by) < lim) {
                if (bx < (int32_t)ux) --bx; else if (bx > (int32_t)ux) ++bx;
                if (by < (int32_t)uy) --by; else if (by > (int32_t)uy) ++by;
                if (bx == (int32_t)ux && by == (int32_t)uy) { ++bx; ++by; }
            }
            wr32(M, 0x18, (uint32_t)bx);
            wr32(M, 0x1C, (uint32_t)by);
            const uint32_t sub = monSub(unit);
            if (sub != 0) wr32(sub, 0x18, 7);
            env->subF1FF0(8, unit, game, (uint32_t)bx, (uint32_t)by);
            endTail();
            wr32(M, 0x14, rd32(M, 0x14) - 1);
            return;
        }
    } else if (state == 1) {
        hold = true;
    }

    if (hold) {
        if (env->sub89730(unit, game) != 0) { fleeTail(pTarget); return; }
        wr32(M, 0x14, 8);
        endTail();
    }

    if (flag != 0) {
        if ((int32_t)roll77d(seed, 100) < rdS16(row, 0x56 + d * 2)) {
            env->subF2220(4, unit, game, pTarget);
        } else {
            env->subF20C0(game, unit, (uint32_t)rdS16(row, 0x5C + d * 2));
        }
        return;
    }
    if (state != -1 && (int32_t)roll77d(seed, 100) >= rdS16(row, 0x6E + d * 2)) {
        env->subF20C0(game, unit, (uint32_t)rdS16(row, 0x5C + d * 2));
        return;
    }
    if ((int32_t)roll77d(seed, 100) < rdS16(row, 0x68 + d * 2)) env->subF26B0(0, unit, game, pTarget, 6);
    else env->subF1D90(9, pTarget, game, unit, 2, 0);
    wr32(M, 0x14, 0);
    endTail();
}

// 1.13c D2Game+0x7E640
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_7e640(const D2Game77dEnv* env, uint32_t game, uint32_t unit,
                                             uint32_t task) {
    // Retail reads the target distance from its first local (task+0x14); it is not a caller stack argument.
    const uint32_t M = rd32(task, 0);
    const int32_t dist = (int32_t)rd32(task, 0x14);
    const uint32_t flag = rd32(task, 0x18);
    const uint32_t row = rd32(task, 0x1C);
    const uint32_t pTarget = rd32(task, 0x8);
    const uint32_t d = rd8(game, 0x6D);
    const uint32_t seed = unit + 0x20;
    const uint32_t tx = unitCoord(env, pTarget, false);
    const uint32_t ty = unitCoord(env, pTarget, true);

    const int32_t sk172 = rdS16(row, 0x172);
    if (sk172 >= 0) {
        const uint32_t S = env->globalSkillData();
        if (sk172 < (int32_t)rd32(S, 0xBA0)) {
            const uint32_t rec = rd32(S, 0xB98) + (uint32_t)sk172 * 0x23C;
            if (rec != 0 && rdS16(rec, 0x80) >= 0 && env->th2A68E(unit, (uint32_t)rdS16(rec, 0x80)) == 0) {
                env->subF2380(rd8(row, 0x181), unit, game, (uint32_t)sk172, 0, 0, 0);
                return;
            }
        }
    }
    if (env->th2AD78(unit, pTarget, 6) != 0) {
        env->subF2380(rd8(row, 0x182), unit, game, (uint32_t)rdS16(row, 0x174), 0, tx, ty);
        return;
    }
    if (rd32(M, 0x18) != 0) {
        env->subF22C0(game, rd32(M, 0x18));
        wr32(M, 0x18, 0);
        return;
    }

    if (flag == 0) {
        bool melee = false;  // the F2540 (arg 7) tail
        if (rdS16(row, 0x170) >= 0 && dist < 10) {
            if ((int32_t)rd32(M, 0x1C) > 0) {
                melee = true;
            } else if ((int32_t)roll77d(seed, 100) < rdS16(row, 0x62 + d * 2)) {
                env->subF2380(rd8(row, 0x180), unit, game, (uint32_t)rdS16(row, 0x170), pTarget, tx, ty);
                wr32(M, 0x18, (uint32_t)rdS16(row, 0x6E + d * 2));
                wr32(M, 0x1C, (uint32_t)rdS16(row, 0x74 + d * 2));
                return;
            }
        }
        if (!melee) {
            if ((int32_t)rd32(M, 0x1C) > 0) {
                melee = true;
            } else if ((int32_t)roll77d(seed, 100) < rdS16(row, 0x5C + d * 2)) {
                melee = true;
            } else if (dist > 10) {
                env->subF1D90(6, pTarget, game, unit, 2, 9);
                return;
            } else {
                env->subF22C0(game, rd8(row, 0x4F + d));
                return;
            }
        }
        env->subF2540(game, unit, pTarget, 2, 0, 0, 1, 7);
        return;
    }

    if (!((int32_t)rd32(M, 0x1C) > 0 || (int32_t)roll77d(seed, 100) < rdS16(row, 0x56 + d * 2))) {
        wr32(M, 0x1C, 0);
        if (rdS16(row, 0x170) >= 0 && (int32_t)roll77d(seed, 100) < rdS16(row, 0x68 + d * 2)) {
            env->subF2380(rd8(row, 0x180), unit, game, (uint32_t)rdS16(row, 0x170), pTarget, tx, ty);
            wr32(M, 0x18, (uint32_t)rdS16(row, 0x6E + d * 2));
            wr32(M, 0x1C, (uint32_t)rdS16(row, 0x74 + d * 2));
            return;
        }
        env->subF22C0(game, rd8(row, 0x4F + d));
        return;
    }
    if ((int32_t)rd32(M, 0x1C) > 0) wr32(M, 0x1C, rd32(M, 0x1C) - 1);
    env->ordinal11103(unit, 0);
    postTargetCmd(env, game, unit, 4, pTarget, 0x10);
}
