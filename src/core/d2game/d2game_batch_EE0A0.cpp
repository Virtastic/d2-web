// src/core/d2game/d2game_batch_EE0A0.cpp -- six 1.13c D2Game.dll functions transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Base 0x6FC20000.
//
// Stage A of the alpha lane: implementations only, no spec and no vector yet. Register-only arguments
// Ghidra reports as in_EAX / unaff_ESI / unaff_EBP / unaff_retaddr are explicit parameters named after
// the register, and the caller stack words the decompiles read (iStack_*) follow them. Unmapped callees
// and every D2Common ordinal are seams on D2GameBatchEE0A0Env in the decompile's argument order; the two
// flat data tables and the assert string 0x15240 reads are env pointers because retail content never
// enters git.
//
// 0xEE0A0  missile/object grid spawn: RNG advances in unit-info+8, 6x3 offset table at RVA 0xFC298.
// 0x13DE0  per-act monster spawn follow-up; calls 0x135E0 (10718 == 0) or 0x13320.
// 0x144D0  weighted candidate scan over a 0x90-stride record array and its drop pick.
// 0x148A0  same shape as 0x144D0 for the other array pair, plus an explicit index (param_2).
// 0x14F20  multi-pass drop driver (three passes of 0x144D0/0x148A0), cleanup, then 0x13DE0.
// 0x15240  same driver against the caller's two candidate lists at iStack_1c+0x68/+0x74.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>
#include <unistd.h>

static inline uint8_t  rd8(const void* p, uint32_t o) { uint8_t v; std::memcpy(&v, (const uint8_t*)p + o, 1); return v; }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline void     wr64(void* p, uint32_t o, uint64_t v) { std::memcpy((uint8_t*)p + o, &v, 8); }

// The shared LCG all six run: (seedLow * 0x6ac690c5 + seedHigh) stored back as the next 64-bit seed.
static inline uint64_t rng_next(void* state)
{
    const uint64_t v = (uint64_t)rd32(state, 0) * 0x6ac690c5ull + (uint64_t)rd32(state, 4);
    wr64(state, 0, v);
    return v;
}

// 0xEE0A0 hands 0xEEF10 the address of this caller-frame block (iStack_2c .. uStack_c in the decompile).
struct D2GameSpawnCtx {
    int32_t  out;   // iStack_2c: written by 0xEEF10, unread here
    uint32_t s28;   // uStack_28 = #10331 result
    uint32_t s24;   // uStack_24 = 0
    uint32_t s20;   // piStack_20 = the unit-info record (32-bit retail pointer)
    uint32_t s1c;   // uStack_1c = param_3
    uint32_t s18;   // uStack_18 = 0
    int32_t  s14;   // iStack_14 = x
    int32_t  s10;   // iStack_10 = y
    uint32_t sc;    // uStack_c = -1
};

// Host seams for the callees src/ does not implement and for every imported D2Common ordinal.
struct D2GameBatchEE0A0Env {
    uint32_t (*ordinal10331)(const void* unitInfo);
    int32_t  (*ordinal10867)(int32_t handle);
    int32_t  (*ordinal10750)(int32_t handle);
    int32_t  (*ordinal10957)();
    int32_t  (*ordinal11120)(int32_t act);
    uint32_t (*ordinal10044)(int32_t act, int32_t which);
    int16_t  (*ordinal10718)(const void* unit);
    void*    (*ordinal10492)();
    void*    (*ordinal10411)(const void* unit);      // RNG state, 8 bytes
    int32_t  (*ordinal10395)(const void* unit);
    int32_t  (*ordinal10086)(const void* thing);
    intptr_t (*ordinal10695)(uint32_t record);
    int32_t  (*ordinal10106)(const void* unit);
    int32_t  (*ordinal10461)(const void* unit, const void* record);
    int32_t  (*ordinal10822)(const void* unit);
    void     (*ordinal10996)(int32_t a, int32_t b, int32_t c, const void* record, int32_t e, int32_t f);
    void     (*ordinal10611)();                      // register-only args, not recovered
    void     (*ordinal10883)();                      // register-only args, not recovered
    int32_t  (*ordinal11088)();
    int16_t  (*ordinal11028)();
    intptr_t (*ordinal10248)(int16_t unitId);
    int16_t  (*ordinal10257)();
    void     (*ordinal10525)();
    void     (*ordinal10312)();
    void     (*ordinal10977)();
    uint32_t (*ordinal10265)(uint32_t code);
    void     (*ordinal10024)(const char* msg, uint32_t arg);
    // D2Game callees (0xEEF10, 0x40CF0, 0x135E0, 0x13320, 0x21480, 0x131E0, 0x14210, 0x14370).
    int32_t* (*subEEF10)(D2GameSpawnCtx* ctx);
    void     (*sub40CF0)();
    void     (*sub135E0)();
    void     (*sub13320)(uint32_t a, uint32_t b, int32_t c);
    uint32_t (*sub21480)(void* rng);
    int32_t  (*sub131E0)();                          // register-only args, not recovered
    uint32_t (*sub14210)(const void* unit, int32_t flag);
    uint32_t (*sub14370)(const void* unit, int32_t flag);
    // 0x144D0 / 0x148A0 as their call sites see them: the register/stack tail (count, unit, stack word)
    // is not recovered at these sites, so the seam supplies it.
    uint32_t (*sub144D0)(uint32_t unit, int32_t a, int32_t b, int32_t c, int32_t d, uint32_t e, int32_t f);
    uint32_t (*sub148A0)(uint32_t unit, int32_t a, int32_t b, int32_t c, int32_t d);
    // Retail data these functions read (never vendored).
    const int32_t* offsFC298;   // 0xEE0A0: 18 x (x,y) offsets, 8 bytes each, indexed by direction*6 + slot
    const int32_t* tableFD058;  // 0x15240: eight drop counts
    const char*    msgF9FF9;    // 0x15240: the assert message
};

// Grid spawn: reads the unit-info word, clamps the direction (EAX) to 0..2, derives x/y from the
// unit-info's owner fields, advances the RNG once for the starting slot and then spawns param_4 units at
// the six slots walking backwards. Each spawned record (when 0xEEF10 answers a type-1 record) gets the
// unit-info's +0xc dword into +0x2c, mode 1 into +0x30 and the caller's retaddr word into +0x28.
// Returns 1 once at least one spawn was built. The decompile leaves the answer word uninitialised when
// the loop never spawns; 0 is used here.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_EE0A0(uint32_t param1, int32_t* param2, uint32_t param3, int32_t param4, int32_t in_EAX, uint32_t retaddr, const D2GameBatchEE0A0Env* env) { // 1.13c D2Game+0xEE0A0
    if (param2 == nullptr) return 0;
    if (in_EAX < 0 || in_EAX > 2) in_EAX = 0;
    const uint32_t uVar3 = env->ordinal10331(param2);
    const int32_t iVar1 = param2[3];
    int32_t iVar4 = param2[0];
    if (iVar4 == 2 || (iVar4 > 3 && iVar4 < 6)) param1 = rd32((const void*)(uintptr_t)param2[0xb], 0xc);
    else if (param2[0xb] == 0) param1 = 0;
    else param1 = (uint32_t)env->ordinal10867(param2[0xb]);
    iVar4 = param2[0];
    if (iVar4 == 2 || (iVar4 > 3 && iVar4 < 6)) iVar4 = (int32_t)rd32((const void*)(uintptr_t)param2[0xb], 0x10);
    else if (param2[0xb] == 0) iVar4 = 0;
    else iVar4 = env->ordinal10750(param2[0xb]);
    const uint64_t uVar2 = rng_next(param2 + 8);
    int32_t iVar7 = (int32_t)((uint32_t)uVar2 % 6);
    uint32_t uStack_3c = 0;
    while (param4 > 0) {
        const int32_t iVar5 = (in_EAX * 6 + iVar7) * 8;
        const int32_t* t = (const int32_t*)((const uint8_t*)env->offsFC298 + iVar5);
        D2GameSpawnCtx ctx = {};
        ctx.s14 = t[0] + (int32_t)param1;
        ctx.s10 = t[1] + iVar4;
        iVar7 = (iVar7 + 5) % 6;
        ctx.s18 = 0;
        ctx.s1c = param3;
        ctx.sc = 0xffffffff;
        ctx.s24 = 0;
        ctx.s20 = (uint32_t)(uintptr_t)param2;
        ctx.s28 = uVar3;
        int32_t* piVar6 = env->subEEF10(&ctx);
        if (piVar6 != nullptr) {
            uStack_3c = 1;
            if (*piVar6 == 1) {
                const int32_t iVar5b = (piVar6[5] == 0) ? 0 : (int32_t)rd32((const void*)(uintptr_t)piVar6[5], 0x28);
                wr32((void*)(uintptr_t)iVar5b, 0x2c, (uint32_t)iVar1);
                wr32((void*)(uintptr_t)iVar5b, 0x30, 1);
                wr32((void*)(uintptr_t)iVar5b, 0x28, retaddr);
            }
            env->sub40CF0();
        }
        param4 = param4 - 1;
    }
    return uStack_3c;
}

// Act spawn follow-up: act in 0..6 with a positive #11120 count, #10044 fetches the act record, +0xc its
// owner word. #10718 == 0 runs 0x135E0; otherwise 0x13320(owner, record, #10718 == 0 ? 0 : owner) where
// bit 0x20 of the record's byte +0x80 selects the owner word instead of zero.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_13DE0(int32_t param1, const D2GameBatchEE0A0Env* env) { // 1.13c D2Game+0x13DE0
    const int32_t iVar3 = env->ordinal10957();
    if (iVar3 < 0 || iVar3 >= 7) return;
    const int32_t iVar4 = env->ordinal11120(iVar3);
    if (iVar4 <= 0) return;
    const uint32_t uVar5 = env->ordinal10044(iVar3, 0);
    const uint32_t uVar1 = rd32((const void*)(uintptr_t)param1, 0xc);
    const int16_t sVar2 = env->ordinal10718((const void*)(uintptr_t)param1);
    if (sVar2 == 0) {
        env->sub135E0();
        return;
    }
    if ((rd8((const void*)(uintptr_t)param1, 0x80) & 0x20) != 0) {
        env->sub13320(uVar1, uVar5, (int32_t)rd32((const void*)(uintptr_t)param1, 0xc));
        return;
    }
    env->sub13320(uVar1, uVar5, 0);
}

// Weighted drop scan. The unit-info (0x10492) gives the record-array bounds for the requested band
// (param_7 -> array 3/4, else param_5 -> 2/3 or 3/4). The RNG advances once; a zero unit with an even
// roll returns immediately. #10086/#10395/#10695 produce the level and the record; the record's byte
// +0x140 picks the flat or the doubled score curve, clamped to 1..99. Then each 0x90-stride entry that
// passes the +0x22 / +0x54 / +0x58 / +0x60 filters and #10461 (plus the +0x66 skill test) becomes a
// candidate at its running index. param_1 - 1 selects the forced entry. A #21480 roll is the budget;
// walking the candidates and subtracting byte +0x82 (times +0x58 when +0x140 is set) picks the last
// affordable one. #10996(0,0,iStack_10,item,0,0) is the tail (the decompile's iStack_4 guard is a
// __chkstk artifact and always taken). Returns the picked index + 1, else a masked zero.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_144D0(int32_t param1, int32_t param2, uint32_t param3, uint32_t param4, int32_t param5, uint32_t param6, int32_t param7, int32_t unaff_EBP, uint32_t unaff_retaddr, int32_t iStack_10, int32_t iStack_8, int32_t iStack_c, const D2GameBatchEE0A0Env* env) { // 1.13c D2Game+0x144D0
    int32_t* piVar3 = (int32_t*)(uintptr_t)env->ordinal10492();
    if (piVar3 == nullptr) return 0;
    int32_t iVar15;
    int32_t iVar12;
    if (param7 == 0) {
        if (param5 == 0) { iVar15 = piVar3[2]; iVar12 = piVar3[3]; }
        else { iVar15 = piVar3[3]; iVar12 = piVar3[4]; }
    } else {
        iVar15 = piVar3[4];
        iVar12 = *piVar3 * 0x90 + piVar3[1];
    }
    uint32_t uVar13 = (uint32_t)((iVar12 - iVar15) / 0x90);
    if (iVar15 == 0) return uVar13 & 0xffff0000u;
    // The decompile writes aiStack_1014[2] here and overwrites it with #10718 below before any read.
    iVar12 = -1;
    const uint64_t uVar1 = rng_next(env->ordinal10411((const void*)(uintptr_t)param1));
    int32_t aiStack_1014[1025];
    aiStack_1014[2] = env->ordinal10718((const void*)(uintptr_t)unaff_retaddr);
    int32_t iVar16 = 0;
    uint32_t uVar5 = (uint32_t)env->ordinal10395((const void*)(uintptr_t)unaff_retaddr) & 0xffu;
    if (((uVar1 & 1) == 0) && (unaff_retaddr == 0)) return (uint32_t)(iVar12 + 1);
    uint32_t uVar6 = (uint32_t)env->ordinal10086((const void*)(uintptr_t)iStack_8);
    const uint32_t uVar7 = (iStack_8 == 0) ? 0xffffffffu : rd32((const void*)(uintptr_t)iStack_8, 4);
    iVar12 = (int32_t)env->ordinal10695(uVar7);
    piVar3 = nullptr;
    if (iVar12 != 0) {
        if ((int32_t)uVar6 <= (int32_t)uVar5) uVar6 = uVar5;
        const uint32_t uVar8 = rd8((const void*)(uintptr_t)iVar12, 0x140);
        int32_t iVar14;
        if (uVar8 == 0) {
            uVar5 = uVar5 / 2;
            if ((int32_t)uVar6 < (int32_t)(99 - uVar5)) iVar14 = (int32_t)uVar6 - (int32_t)uVar5;
            else iVar14 = ((int32_t)uVar6 * 2 - (int32_t)uVar5) - (99 - (int32_t)uVar5);
        } else {
            iVar14 = (int32_t)uVar6 + (int32_t)uVar8;
        }
        if (iVar14 < 2) iVar14 = 1;
        else if (iVar14 > 0x62) iVar14 = 99;
        iVar12 = 0;
        piVar3 = (int32_t*)(uintptr_t)unaff_EBP;   // the masked fall-through value
        if (unaff_EBP > 0) {
            const uint8_t* p = (const uint8_t*)(uintptr_t)iVar15 + 0x60;
            iVar15 = iStack_8;
            auto finish = [&](const void* item) -> uint32_t {
                env->ordinal10996(0, 0, iStack_10, item, 0, 0);
                return (uint32_t)(iVar12 + 1);
            };
            do {
                const uint8_t* piVar10 = p - 0x60;
                bool bVar2 = false;
                bool skip = false;
                if (((iStack_c == 0) || (rd8(p - 0xc, 0) != 0)) && (rd16(p - 0x3e, 0) < 100)) {
                    if ((param1 < 1) || (param1 - 1 != iVar12)) {
                        const int32_t c58 = (int32_t)rd32(p - 8, 0);
                        const uint32_t c60 = rd32(p, 0);
                        if ((iVar14 < c58) || ((c60 != 0) && ((int32_t)c60 < iVar14))) skip = true;
                    } else {
                        bVar2 = true;
                    }
                    if (!skip) {
                        bool take = (rd8(p, 4) != 0);
                        if (!take) {
                            const int32_t m = env->ordinal10106((const void*)(uintptr_t)iVar15);
                            take = !(m == 6 || (m >= 8 && m <= 9));
                        }
                        if (take) {
                            int32_t iVar9 = env->ordinal10461((const void*)(uintptr_t)iVar15, piVar10);
                            bool ok = (iVar9 != 0);
                            if (ok) ok = ((param2 == 0) || (param2 == (int32_t)rd32(p - 4, 0)));
                            if (ok) ok = (rd8(p, 0x22) != 0);
                            if (ok) {
                                const uint8_t c66 = rd8(p, 6);
                                bool skillOk = (c66 == 0xff);
                                if (!skillOk) { iVar9 = env->ordinal10822((const void*)(uintptr_t)iVar15); skillOk = (iVar9 == 7); }
                                if (!skillOk) { uVar5 = (uint32_t)env->ordinal10822((const void*)(uintptr_t)iVar15); skillOk = (c66 == (uint8_t)uVar5); }
                                if (skillOk) { iVar9 = env->sub131E0(); iVar15 = iStack_10; skillOk = (iVar9 == 0); }
                                ok = skillOk;
                            }
                            if (ok) {
                                if (bVar2) {
                                    iVar12 = iVar12 + (int32_t)uVar13;
                                    if (iVar12 >= 0) return finish(piVar10);
                                    break;
                                }
                                if (iVar16 < 0x1ff) {
                                    aiStack_1014[iVar16 * 2 + 1] = (int32_t)(uintptr_t)piVar10;
                                    aiStack_1014[iVar16 * 2] = iVar12 + (int32_t)uVar13;
                                    iVar16 = iVar16 + 1;
                                }
                            }
                        }
                    }
                }
                iVar12 = iVar12 + 1;
                p = p + 0x90;
            } while (iVar12 < unaff_EBP);
            if (0 < iVar16) {
                int32_t left = (int32_t)env->sub21480(env->ordinal10411((const void*)(uintptr_t)iVar15));
                int32_t last = 0;
                for (int32_t k = 0; k < iVar16; ++k) {
                    last = k;
                    const uint32_t item = (uint32_t)aiStack_1014[k * 2 + 1];
                    uint32_t cost;
                    if (uVar8 == 0) cost = rd8((const void*)(uintptr_t)item, 0x82);
                    else cost = (uint32_t)rd8((const void*)(uintptr_t)item, 0x82) * rd32((const void*)(uintptr_t)item, 0x58);
                    left = left - (int32_t)cost;
                    if (left < 0) break;
                }
                iVar12 = aiStack_1014[last * 2];
                return finish((const void*)(uintptr_t)aiStack_1014[last * 2 + 1]);
            }
        }
    }
    return (uint32_t)(uintptr_t)piVar3 & 0xffff0000u;
}

// The second weighted scan: records come from the unit-info's second array pair (param_5 chooses), and
// the candidate test is the +0x22 ushort count versus the paired count unless the pair is over 99, the
// +0x32 byte flag, and int +0x36 <= #10086 + 2 clamped to 1..99. #10461(unit, entry) qualifies. A #21480
// roll indexes the candidate pairs; param_2 > 0 forces the pair whose stored index equals the base
// index + param_2 - 1, otherwise the roll stands. #10996(0,0,uStack_8,item,0,0) is the tail and the
// answer is that pair's index + 1. The decompile's iStack_4 guard is a __chkstk artifact.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_148A0(int32_t param1, int32_t param2, uint32_t param3, uint32_t param4, int32_t param5, uint32_t unaff_EDI, uint32_t unaff_retaddr, int32_t uStack_8, int32_t iStack_4, const D2GameBatchEE0A0Env* env) { // 1.13c D2Game+0x148A0
    const int32_t iVar3 = (int32_t)(uintptr_t)env->ordinal10492();
    if (iVar3 == 0) return 0;
    int32_t iVar6;
    int32_t iVar5;
    if (param5 == 0) { iVar6 = (int32_t)rd32((const void*)(uintptr_t)iVar3, 8); iVar5 = (int32_t)rd32((const void*)(uintptr_t)iVar3, 0xc); }
    else { iVar6 = (int32_t)rd32((const void*)(uintptr_t)iVar3, 0xc); iVar5 = (int32_t)rd32((const void*)(uintptr_t)iVar3, 0x10); }
    uint32_t auStack_1010[1026];
    auStack_1010[0] = (uint32_t)((iVar5 - iVar6) / 0x90);
    if (iVar6 == 0) return auStack_1010[0] & 0xffff0000u;
    int32_t baseIndex = (iVar6 - (int32_t)rd32((const void*)(uintptr_t)iVar3, 4)) / 0x90;
    auStack_1010[2] = 0;
    const uint64_t uVar2 = rng_next(env->ordinal10411((const void*)(uintptr_t)param1));
    auStack_1010[1] = (uint32_t)env->ordinal10718((const void*)(uintptr_t)unaff_retaddr);
    if (((uVar2 & 1) == 0) && (param1 == 0)) return unaff_EDI + 1;
    iVar5 = env->ordinal10086((const void*)(uintptr_t)unaff_retaddr) + 2;
    if (iVar5 < 2) iVar5 = 1;
    else if (iVar5 > 0x62) iVar5 = 99;
    uint32_t uVar7 = unaff_EDI;
    if ((int32_t)unaff_EDI <= 0) return uVar7 & 0xffff0000u;
    int32_t iVar10 = -1;
    const uint8_t* p = (const uint8_t*)(uintptr_t)iVar6 + 0x22;
    uVar7 = 0;
    do {
        if ((rd8(p, 0x32) != 0) && ((rd16(p, 0) < 100) || (auStack_1010[0] > 99))
            && ((int32_t)rd32(p, 0x36) <= iVar5)) {
            const int32_t r = env->ordinal10461((const void*)(uintptr_t)uStack_8, p - 0x22);
            if ((r != 0) && (iVar10 < 0x1ff)) {
                auStack_1010[iVar10 * 2 + 2] = (uint32_t)(uintptr_t)(p - 0x22);
                auStack_1010[iVar10 * 2 + 1] = uVar7 + (uint32_t)baseIndex;
                iVar10 = iVar10 + 1;
            }
        }
        uVar7 = uVar7 + 1;
        p = p + 0x90;
    } while ((int32_t)uVar7 < (int32_t)unaff_EDI);
    if (iVar10 > 0) {
        uVar7 = env->sub21480(env->ordinal10411((const void*)(uintptr_t)uStack_8));
        if ((int32_t)uVar7 < iVar10) {
            uint32_t* puVar1 = auStack_1010 + uVar7 * 2;
            uint32_t edi = *puVar1;
            uint32_t* chosen = puVar1;
            if (param2 > 0) {
                const int32_t wanted = param2 + baseIndex;
                if (wanted > 0) {
                    bool found = false;
                    for (int32_t k = 0; k < iVar10; ++k) {
                        uint32_t* pair = auStack_1010 + k * 2;
                        if (*pair == (uint32_t)(wanted - 1)) { edi = *pair; chosen = pair; found = true; break; }
                    }
                    if (!found) { edi = 0xffffffffu; chosen = puVar1; }
                }
            }
            if (unaff_retaddr != 0) env->ordinal10996(0, 0, uStack_8, (const void*)(uintptr_t)chosen[1], 0, 0);
            return edi + 1;
        }
    }
    return uVar7 & 0xffff0000u;
}

// Three-pass drop driver over the expansion flag (#10718): pass 1 and 2 take 0x14370 when the flag is
// zero, 0x14210 otherwise, with param 1 then 0; both must answer non-zero. The creature type (#11088)
// picks 3 + roll&1 for type 0x3a, else 4..6. Each pass rolls again and runs either the 0x148A0/0x144D0
// pool (type 3 or odd roll when the first answer is not 3) or the same pair with the flag set, up to
// three attempts and 0xfb drops per pass, cross-checking #11028/#10257 against #10248's +0x5c. A
// non-zero pass result runs #10977, frees the three per-pass pairs through #10996 and then 0x13DE0(0)
// and answers 1. Note the decompile reuses iVar10 for both the unit word and the second pool's counter.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_14F20(uint32_t unaff_ESI, const D2GameBatchEE0A0Env* env) { // 1.13c D2Game+0x14F20
    int16_t sVar2 = env->ordinal10718((const void*)(uintptr_t)unaff_ESI);
    uint32_t uVar4 = (sVar2 == 0) ? env->sub14370((const void*)(uintptr_t)unaff_ESI, 1)
                                  : env->sub14210((const void*)(uintptr_t)unaff_ESI, 1);
    sVar2 = env->ordinal10718((const void*)(uintptr_t)unaff_ESI);
    uint32_t uVar5 = (sVar2 == 0) ? env->sub14370((const void*)(uintptr_t)unaff_ESI, 0)
                                  : env->sub14210((const void*)(uintptr_t)unaff_ESI, 0);
    sVar2 = (int16_t)uVar5;
    if (((int16_t)uVar4 == 0) || (sVar2 == 0)) return 0;
    int32_t iVar10 = (int32_t)unaff_ESI;
    env->ordinal10611();
    env->ordinal10883();
    void* puVar6 = env->ordinal10411((const void*)(uintptr_t)unaff_ESI);
    int32_t iVar7 = env->ordinal11088();
    uint64_t uVar1;
    if (iVar7 == 0x3a) {
        uVar1 = rng_next(puVar6);
        iVar7 = (int32_t)((uint32_t)uVar1 & 1) + 3;
    } else {
        uVar1 = rng_next(puVar6);
        iVar7 = (int32_t)((uint32_t)uVar1 % 3) + 4;
    }
    if (iVar7 != 0) {
        do {
            uVar1 = rng_next(puVar6);
            if ((iVar10 == 3) || ((uVar4 != 3) && ((uVar1 & 1) != 0))) {
                int32_t iVar11 = 0;
                int32_t iVar12 = 0;
                do {
                    sVar2 = env->ordinal10718((const void*)(uintptr_t)unaff_ESI);
                    iVar12 = iVar11;
                    uVar5 = (sVar2 == 0) ? env->sub148A0(unaff_ESI, 1, 1, 0, 0)
                                         : env->sub144D0(unaff_ESI, 1, 1, 0, 0, 0, 0);
                    if ((int16_t)uVar5 == 0) goto loopEnd;
                    iVar11 = 0;
                    while (true) {
                        sVar2 = env->ordinal11028();
                        const intptr_t iVar8 = env->ordinal10248(sVar2);
                        if (iVar8 != 0) {
                            const intptr_t iVar9 = env->ordinal10248((int16_t)(uVar5 & 0xffff));
                            if ((sVar2 == (int16_t)uVar5) || (rd32((const void*)iVar9, 0x5c) == rd32((const void*)iVar8, 0x5c))) break;
                        }
                        iVar11 = iVar11 + 1;
                        if (iVar11 > 2) {
                            env->ordinal10525();
                            uVar4 = uVar4 + 1;
                            goto loopEnd;
                        }
                    }
                    iVar11 = iVar12 + 1;
                } while (iVar12 < 0xfb);
                env->ordinal10525();
            } else {
                int32_t iVar11 = 0;
                int32_t iVar12 = 0;
                do {
                    sVar2 = env->ordinal10718((const void*)(uintptr_t)unaff_ESI);
                    iVar12 = iVar11;
                    uVar5 = (sVar2 == 0) ? env->sub148A0(unaff_ESI, 1, 1, 0, 1)
                                         : env->sub144D0(unaff_ESI, 1, 1, 0, 1, 0, 0);
                    if ((int16_t)uVar5 == 0) goto loopEnd;
                    iVar11 = 0;
                    while (true) {
                        sVar2 = env->ordinal10257();
                        const intptr_t iVar8 = env->ordinal10248(sVar2);
                        if (iVar8 != 0) {
                            const intptr_t iVar9 = env->ordinal10248((int16_t)(uVar5 & 0xffff));
                            if ((sVar2 == (int16_t)uVar5) || (rd32((const void*)iVar9, 0x5c) == rd32((const void*)iVar8, 0x5c))) break;
                        }
                        iVar11 = iVar11 + 1;
                        if (iVar11 > 2) {
                            env->ordinal10312();
                            iVar10 = iVar10 + 1;
                            goto loopEnd;
                        }
                    }
                    iVar11 = iVar12 + 1;
                } while (iVar12 < 0xfb);
                env->ordinal10312();
            }
loopEnd:
            iVar7 = iVar7 - 1;
            // the decompile reloads the RNG pointer from EDI here: puVar6 = unaff_EDI, i.e. itself.
        } while (iVar7 != 0);
        if ((iVar10 != 0) || (uVar4 != 0)) {
            env->ordinal10977();
            for (int32_t k = 0; k < 3; ++k) {
                // the decompile passes only (0,0) here; the seam's remaining words are the unrecovered registers.
                if (env->ordinal10248(env->ordinal10257()) != 0) env->ordinal10996(0, 0, 0, nullptr, 0, 0);
                if (env->ordinal10248(env->ordinal11028()) != 0) env->ordinal10996(0, 0, 0, nullptr, 0, 0);
            }
            d2_fn_d2game_13DE0(0, env);
            return 1;
        }
    }
    return 0;
}

// The caller-list variant of the driver: same preamble and creature-type roll, but the drop count comes
// from the eight-entry table at RVA 0xFD058 unless the type is 0x3a (3 + roll&1). The two candidate
// lists sit at iStack_1c+0x68 (unconstrained) and iStack_1c+0x74 (constrained); each pass alternates by
// the ESI flag and the expansion flag, up to three attempts per side, and the same fatal assert at
// 0x31d (unconstrained) / 0x32e (constrained) once a side is exhausted. The failed-attempt paths
// decrement and then re-increment the pass counter, so a failure retries; success advances it.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_15240(uint32_t unaff_ESI, uint32_t iStack_1c, const D2GameBatchEE0A0Env* env) { // 1.13c D2Game+0x15240
    int16_t sVar2 = env->ordinal10718((const void*)(uintptr_t)unaff_ESI);
    uint32_t uVar4 = (sVar2 == 0) ? env->sub14370((const void*)(uintptr_t)unaff_ESI, 1)
                                  : env->sub14210((const void*)(uintptr_t)unaff_ESI, 1);
    sVar2 = env->ordinal10718((const void*)(uintptr_t)unaff_ESI);
    uint32_t uVar5 = (sVar2 == 0) ? env->sub14370((const void*)(uintptr_t)unaff_ESI, 0)
                                  : env->sub14210((const void*)(uintptr_t)unaff_ESI, 0);
    sVar2 = (int16_t)uVar5;
    if (((int16_t)uVar4 == 0) || (sVar2 == 0)) return 0;
    int32_t iVar10 = (int32_t)unaff_ESI;
    env->ordinal10611();
    env->ordinal10883();
    int32_t iVar9 = 0;
    void* puVar6 = env->ordinal10411((const void*)(uintptr_t)unaff_ESI);
    int32_t iVar7 = env->ordinal11088();
    uint64_t uVar1;
    if (iVar7 == 0x3a) {
        uVar1 = rng_next(puVar6);
        iVar7 = (int32_t)((uint32_t)uVar1 & 1) + 3;
    } else {
        uVar1 = rng_next(puVar6);
        iVar7 = env->tableFD058[(uint32_t)uVar1 & 7];
    }
    auto fatal = [&](uint32_t code) -> void {
        env->ordinal10024(env->msgF9FF9, env->ordinal10265(code));
        _exit(-1);
    };
    int32_t iStack_40 = 0;
    if (iVar7 > 0) {
        uint32_t* puVar14 = (uint32_t*)(uintptr_t)(iStack_1c + 0x74);
        uint32_t* puVar8 = (uint32_t*)(uintptr_t)(iStack_1c + 0x68);
        int32_t iVar12 = iVar10;
        int32_t iVar13 = (int32_t)unaff_ESI;
        do {
            bool constrained = (iVar13 != 0);
            if (iVar13 != 0) {
                if (uVar4 != 0) break;
            } else if (uVar4 == 0) {
                uVar1 = rng_next(puVar6);
                iVar10 = iVar12;
                if ((uVar1 & 1) != 0) constrained = true;
            }
            if (!constrained) {
                if (iVar9 > 2) fatal(0x31d);
                const uint32_t w = env->sub144D0(unaff_ESI, 1, 1, 0, 1, *puVar8, 0);
                if ((int16_t)w == 0) {
                    iVar13 = 1;
                    iStack_40 = iStack_40 - 1;
                } else {
                    env->ordinal10312();
                    iVar9 = iVar9 + 1;
                    puVar8 = puVar8 + 1;
                    if (iVar9 > 2) iVar13 = 1;
                    iStack_40 = iStack_40 + 1;
                }
            } else {
                if (iVar10 > 2) fatal(0x32e);
                const uint32_t w = env->sub144D0(unaff_ESI, 1, 1, 0, 0, *puVar14, 0);
                if ((int16_t)w == 0) {
                    uVar4 = 1;
                    iStack_40 = iStack_40 - 1;
                } else {
                    env->ordinal10525();
                    iVar10 = iVar10 + 1;
                    puVar14 = puVar14 + 1;
                    iVar12 = iVar10;
                    if (iVar10 > 2) uVar4 = 1;
                    iStack_40 = iStack_40 + 1;
                }
            }
            // the label's common tail: the failed paths above already did the -1 + 1 pair, the successes the +1
        } while (iStack_40 < iVar7);
        if ((iVar9 != 0) || (iVar10 != 0)) {
            env->ordinal10977();
            for (int32_t k = 0; k < 3; ++k) {
                if (env->ordinal10248(env->ordinal10257()) != 0) env->ordinal10996(0, 0, 0, nullptr, 0, 0);
                if (env->ordinal10248(env->ordinal11028()) != 0) env->ordinal10996(0, 0, 0, nullptr, 0, 0);
            }
            d2_fn_d2game_13DE0(iVar7, env);
            return 1;
        }
    }
    return 0;
}
