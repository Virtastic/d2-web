// src/core/d2game/d2game_b52a0.cpp -- 1.13c D2Game.dll ApplySkillEffectTable (sub_B52A0), transcribed from the cached decompile and
// its disassembly (the decompile's local_5c/uStack_7c overlap is resolved from the disassembly: one 0x5C-byte event block).
// Callees go through the D2GameB52A0Env seams (ord = D2Common ordinal, sub = D2Game RVA), as the launchers in d2game_batch_b4520.cpp.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

typedef uint32_t U;
typedef int32_t S;
#define R32(b, o) (*(uint32_t*)((uintptr_t)(b) + (o)))

struct D2GameB52A0Env {   // the seams of d2game_batch_b4520.cpp plus this port's tbl468
    S (*ord)(U n, U a, U b, U c, U d);
    S (*sub)(U rva, U a, U b, U c, U d, U e, U f, U g);
    const uint8_t* dt;        // *sgptDataTables_exref
    const uint8_t* tbl1B510;
    const uint8_t* tbl1B518;
    const uint8_t* tbl468;    // DAT_6fd1b468: two 8-dword tables (dwords read at byte offsets +0, +4, +0x40, +0x44, ...)
};
static inline U tbl(const D2GameB52A0Env* e, U byteoff) { return R32(e->tbl468, byteoff); }

// 1.13c D2Game+0xB52A0 -- ECX = game, EDX = unit, stack = (skill id into the 0x23C-byte table at +0xB98, param_4). Returns 1, or 0 when
// the skill id, its +0x48 class index or the class record is out of range. Fires one 0x1F event per table entry through 0x6F930 (ECX = game, EDX = event).
extern "C" EMSCRIPTEN_KEEPALIVE U d2_fn_d2game_B52A0(void* game, void* unit, S skill, U p4, const D2GameB52A0Env* env)
{
    const uintptr_t dt = (uintptr_t)env->dt;
    if (skill < 0 || skill >= (S)R32(dt, 0xba0)) return 0;
    const uintptr_t row = (uintptr_t)((U)skill * 0x23c) + R32(dt, 0xb98);
    if (!row) return 0;
    const S cls = (int16_t)*(uint16_t*)(row + 0x48);
    if (cls < 0 || cls >= (S)R32(dt, 0xb6c)) return 0;
    const uintptr_t crec = (uintptr_t)((U)cls * 0x1a4) + R32(dt, 0xb64);
    if (!crec) return 0;
    const U un = (U)(uintptr_t)unit;
    if (unit) R32(unit, 0xc4) |= 0x40;
    U ev[23];
    std::memset(ev, 0, sizeof ev);
    ev[0] = 0x1f;
    ev[1] = un; ev[2] = un;
    ev[4] = (U)cls;
    ev[5] = (U)env->sub(0x1100, un, 0, 0, 0, 0, 0, 0);
    ev[6] = (U)env->sub(0x1130, un, 0, 0, 0, 0, 0, 0);
    ev[12] = p4;
    ev[13] = (U)env->ord(10786, un, R32(row, 0x138), (U)skill, p4);
    ev[10] = R32(crec, 0x38) << 6;
    S n = env->ord(10786, un, R32(row, 0x13c), (U)skill, p4);
    if (n < 1) n = 1;
    const U g = (U)(uintptr_t)game, evp = (U)(uintptr_t)ev;
    for (U i = 0; i < 0x40; i += 8) {
        ev[7] = tbl(env, i + 0x40);
        ev[8] = tbl(env, i);
        env->sub(0x6F930, g, evp, 0, 0, 0, 0, 0);
    }
    ev[10] = R32(crec, 0x3c) << 6;
    if (n > 1) {
        const U step = (U)n * 4;
        for (U i = 0; i < 0x3c; i += step) {
            ev[8] = tbl(env, i + 4);
            ev[7] = tbl(env, i + 0x44);
            env->sub(0x6F930, g, evp, 0, 0, 0, 0, 0);
        }
    }
    return 1;
}
