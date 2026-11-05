// src/core/d2game/d2game_batch_a0830.cpp -- five 1.13c D2Game.dll functions transcribed from the cached Ghidra
// decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Base 0x6FC20000. Written from the decompile, not from
// the spec alone; the spec deltas are listed on each function. No test runner run.
//
// Layout: the build is 32-bit wasm, so pointer fields are read as int-width dwords (same as d2game_batch_*.cpp).
// Register inputs the decompiler could not attribute (unaff_ESI, in_EAX) are explicit parameters marked REG?.
// Callees not in src/ are weak env imports (EXT) with variadic signatures, retail argument order.
// Skipped in this batch (see the hand-back): A0830, A2100, A2F90, A3E80, A4A10, A4FD0, A59B0, A5EB0, A62D0, A6E60,
// A7410, A7A10, A7C20, A7DC0, A80C0, A8430, A8E80, A9E80, AA1D0, AA4D0, AA710, AB070, AB360, AC410, AE6D0.
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <Windows.h>

#define EXT(n) extern "C" __attribute__((weak)) intptr_t n(...)
// EXTA: an env import other TUs declare with a fixed signature; a namespaced symbol keeps this file's
// variadic calls without a wasm-ld signature clash on the shared C symbol (pattern of d2game_batch_68d70.cpp).
#define EXTA(n) namespace b10_env { __attribute__((weak, import_module("env"), import_name(#n))) intptr_t n(...); } using namespace b10_env
EXT(Ordinal_10265);
EXT(Ordinal_10024);
EXT(Ordinal_10867);
EXT(Ordinal_10750);
EXT(FUN_6fcc00b0);   // 0xA00B0 completion path of MonstatLevelScaledValue
EXT(FUN_6fcc0c70);   // 0xA0C70 skill-use core, called by SkillUseTrampoline
EXT(FUN_6fcc10b0);   // 0xA10B0 record dispatch, called by SkillRecordCopyAndDispatch
EXTA(FUN_6fd003a0);   // 0xE03A0 owner record from (kind, id)
EXT(FUN_6fcffa90);   // 0xDFA90 unit flag test
EXT(FUN_6fcfeee0);   // 0xDEEE0 state commit
EXT(FUN_6fc21180);   // 0x1180 unit level
EXT(FUN_6fd122c0);   // 0xF22C0 exit with a code
EXT(FUN_6fd12220);   // 0xF2220 attack-slot helper
EXT(FUN_6fd12380);   // 0xF2380 state commit
extern "C" char* sgptDataTables_exref;

typedef uint8_t u8;
template <class T> static inline T& at(const void* p, int off) { return *(T*)((u8*)p + off); }
static inline u8* ptr(intptr_t v) { return (u8*)v; }

// SkillTable row base for id, or null when out of range (+0xB98 base, +0xBA0 count, stride 0x23C).
static inline u8* skillRow(int id) {
    u8* t = (u8*)(intptr_t)*(int*)sgptDataTables_exref;
    int count = at<int>(t, 0xba0);
    if (id < 0 || count <= id) return nullptr;
    return ptr(id * 0x23c + at<int>(t, 0xb98));
}

// 0x6AC690C5 LCG step on the 64-bit seed at seed+0 (lo dword, hi dword).
static inline void seedStep(void* seed) {
    uint64_t s = (uint64_t)at<uint32_t>(seed, 0) * 0x6ac690c5ull + at<uint32_t>(seed, 4);
    at<uint64_t>(seed, 0) = s;
}

// MonstatLevelScaledValue (eax=level, edx=row id, esi=callback; stack p3..p6 go to the fatal log).
// MISMATCH vs spec: the decompile calls 0xA00B0 unconditionally after the callback gate, not only when the callback
// is null or readable; the fatal branch exits before it. Out-of-range row: the decompile reads the row fields at
// address 0x14C/0x150/0x148 (row base 0); the port reads the same addresses, so the result matches retail's read.
// 1.13c D2Game+0xA09F0
void D2GAME_A09F0(int level, int rowId, void* cb, int p3, int p4, int p5, int p6)
{
    u8* row = skillRow(rowId);
    int scaled = at<int>(row, 0x14c) * level;               // REG? level in eax
    int lin = (level - 1) * at<int>(row, 0x150) + at<int>(row, 0x148);
    if (cb && IsBadCodePtr((FARPROC)cb)) {                  // REG? callback in esi
        intptr_t code = Ordinal_10265(0x52b, p3, p4, rowId, level, scaled, p5, lin, p6);
        Ordinal_10024("D2Game.dll", code);                  // retail string DAT_6fd19ff9 is not in our image
        _exit(-1);
    }
    FUN_6fcc00b0();
}

// 1.13c D2Game+0xA0FC0  (ecx=p1, edx=p2, stack p3..p6, eax=flag word base)
// REG? eax is the decompiler's in_EAX; the call ORs 0xA783 into it. Returns 0 as retail.
int D2GAME_A0FC0(intptr_t p1, int p2, intptr_t p3, intptr_t* p4, void* p5, intptr_t p6, uint32_t eax)
{
    (void)p1; (void)p3;
    FUN_6fcc0c70(p4, p5, p6, p2, eax | 0xa783, 0x6fcbe600);
    return 0;
}

// 1.13c D2Game+0xA1210  (stack param_1 = record pointer, param_2 = second argument)
// The decompile copies 28 dwords from record+0xC onto an unused local before the call. The copy has no observable
// effect (the local is never read), so the port makes the call directly; the source read is side-effect free.
int D2GAME_A1210(intptr_t* param1, int param2)
{
    FUN_6fcc10b0((intptr_t)param1[0], param2);
    return 1;
}

// The A4D10 cooldown step: d2game_a4d10.cpp is its own file.

// 1.13c D2Game+0xA5350  (ecx=caster unit, edx=rec (the player-type unit), stack ai record)
// Spec deltas: none. The decompile's two arms of the power-of-two test are the same LCG step, so one step is taken
// per arm position. A null owner (decompiler deref of 0) is skipped in the charge calls here; retail would fault.
void D2GAME_A5350(int unit, u8* rec, u8* ai)
{
    u8* state = ptr(at<int>(ai, 0));
    u8* row = ptr(at<int>(ai, 0x1c));
    uint32_t idx = (uint32_t)at<u8>((void*)(intptr_t)unit, 0x6d) * 2;
    u8* owner = nullptr;
    if (rec && at<int>(rec, 0) == 1) {
        int it = at<int>(rec, 0x14);
        if (it) {
            int o = at<int>(ptr(it), 0x28);
            if (o && at<int>(ptr(o), 0x28))
                owner = (u8*)FUN_6fd003a0(at<int>(ptr(o), 0x28), at<int>(ptr(o), 0x2c));
        }
    }
    int cost = (int)at<int16_t>(row, 0x56 + idx);
    int mode = owner ? at<int>(owner, 0x10) : 0;
    if (mode == 3) cost = cost / 2;
    if (at<int>((void*)(intptr_t)unit, 0xa8) <= at<int>(state, 0x14) && mode != 3) {
        bool ok = !rec || at<int>(rec, 0) != 1 || at<int>(rec, 0x14) == 0;
        if (!ok) {
            int m = at<int>(ptr(at<int>(rec, 0x14)), 0x54);
            ok = m != 3 && m != 0x13;
        }
        if (ok) {
            FUN_6fd122c0((int)at<int16_t>(row, 0x5c + idx));
            return;
        }
    }
    uint32_t u = (uint32_t)(cost * 2);
    if (owner && at<int>(owner, 0) != 2 && (at<int>(owner, 0) < 4 || at<int>(owner, 0) > 5) && at<int>(owner, 0x2c))
        Ordinal_10867(at<int>(owner, 0x2c));
    if ((int)u > 0) seedStep(rec + 0x20);
    if (owner && at<int>(owner, 0) != 2 && (at<int>(owner, 0) < 4 || at<int>(owner, 0) > 5) && at<int>(owner, 0x2c))
        Ordinal_10750(at<int>(owner, 0x2c));
    if ((int)u > 0) seedStep(rec + 0x20);
    FUN_6fd12380(rec);
    at<int>(state, 0x14) = (int)at<int16_t>(row, 0x5c + idx) + at<int>((void*)(intptr_t)unit, 0xa8);
}
