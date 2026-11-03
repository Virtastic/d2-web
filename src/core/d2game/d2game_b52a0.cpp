// src/core/d2game/d2game_b52a0.cpp -- 1.13c D2Game.dll ApplySkillEffectTable (sub_B52A0), transcribed from the cached decompile and
// its disassembly (the decompile's local_5c/uStack_7c overlap is resolved from the disassembly: one 0x5C-byte event block).
// Convention as d2game_batch_aea40.cpp: raw retail records, register inputs explicit, callees weak externs.
#include <cstdint>
#include <cstring>

typedef intptr_t I;
typedef uint32_t U;
#define O(n) extern "C" __attribute__((weak)) I Ord##n(...)
#define F(n) extern "C" __attribute__((weak)) I D2GAME_##n(...)
#define R32(b, o) (*(uint32_t*)((uintptr_t)(b) + (o)))

extern "C" __attribute__((weak)) char sgptDataTables_exref[];
extern "C" __attribute__((weak)) int32_t D2GAME_DAT_F1B468[32];   // DAT_6fd1b468: two 8-dword tables, dwords read at byte offsets +0, +4, +0x40, +0x44 (+esi)
static inline uintptr_t DT() { return (uintptr_t)R32(sgptDataTables_exref, 0); }
static inline U tbl(U byteoff) { return (U)D2GAME_DAT_F1B468[byteoff / 4]; }
O(10786); F(21100); F(21130); F(8F930);

// 1.13c D2Game+0xB52A0 -- ECX = game, EDX = unit, stack = (skill id into the 0x23C-byte table at +0xB98, param_4). Returns 1, or 0 when
// the skill id, its +0x48 class index or the class record is out of range. Fires one 0x1F event per table entry through 0x8F930.
extern "C" U d2_fn_d2game_B52A0(I game, I unit, I skill, U p4)
{
    const int32_t sk = (int32_t)skill;
    const uintptr_t dt = DT();
    if (sk < 0 || sk >= (int32_t)R32(dt, 0xba0)) return 0;
    const uintptr_t row = (uintptr_t)(sk * 0x23c) + R32(dt, 0xb98);
    if (!row) return 0;
    const int32_t cls = (int16_t)*(uint16_t*)(row + 0x48);
    if (cls < 0 || cls >= (int32_t)R32(dt, 0xb6c)) return 0;
    const uintptr_t crec = (uintptr_t)(cls * 0x1a4) + R32(dt, 0xb64);
    if (!crec) return 0;
    if (unit) R32(unit, 0xc4) |= 0x40;
    U ev[23];
    std::memset(ev, 0, sizeof ev);
    ev[0] = 0x1f;
    ev[1] = (U)unit; ev[2] = (U)unit;
    ev[4] = (U)cls;
    ev[5] = (U)D2GAME_21100(unit);
    ev[6] = (U)D2GAME_21130(unit);
    ev[12] = p4;
    ev[13] = (U)Ord10786(unit, R32(row, 0x138), skill, p4);
    ev[10] = R32(crec, 0x38) << 6;
    I n = Ord10786(unit, R32(row, 0x13c), skill, p4);
    if (n < 1) n = 1;
    for (U i = 0; i < 0x40; i += 8) {
        ev[7] = tbl(i + 0x40);
        ev[8] = tbl(i);
        D2GAME_8F930(game, ev);
    }
    ev[10] = R32(crec, 0x3c) << 6;
    if (n > 1) {
        const U step = (U)n * 4;
        for (U i = 0; i < 0x3c; i += step) {
            ev[8] = tbl(i + 4);
            ev[7] = tbl(i + 0x44);
            D2GAME_8F930(game, ev);
        }
    }
    return 1;
}
