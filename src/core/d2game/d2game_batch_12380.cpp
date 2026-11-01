// src/core/d2game/d2game_batch_12380.cpp -- D2Game.dll functions transcribed from cached decompiles, one probe
// per function. Implementation and per-function markers only.
//
// Host model: retail module state is read by its retail byte offsets (rd8/rd16/rd32/rd64, wr*). Every callee is
// one seam, D2GameBatch12380Env::call(ctx, target, args, n): target = retail D2Game RVA, or 0x80000000 | ordinal
// for an Ordinal_NNNN import. A probe passes the env pointer in r[5]; arguments are documented per function;
// the result is returned in r[4]. Out-parameters are offsets into env->scratch.
#include <cstdint>
#include <initializer_list>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

static inline uint8_t rd8(const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline uint64_t rd64(const void* p, uint32_t o) { uint64_t v; std::memcpy(&v, (const uint8_t*)p + o, 8); return v; }
static inline void wr8(void* p, uint32_t o, uint8_t v) { ((uint8_t*)p)[o] = v; }
static inline void wr16(void* p, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, 2); }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline void wr64(void* p, uint32_t o, uint64_t v) { std::memcpy((uint8_t*)p + o, &v, 8); }
static inline void* rdptr(const void* p, uint32_t o) { return (void*)(uintptr_t)rd32(p, o); }   // retail structs hold 32-bit pointers

// Out-parameters (retail &local) are passed as offsets into env->scratch, a host block the seam owns.
struct D2GameBatch12380Env {
    void* ctx;
    uint8_t* scratch;
    // gdwBitMasks_exref (D2Common table): uint32 masks read at +0x10 and +0x14.
    const uint8_t* bitMasks;
    uint32_t (*call)(void* ctx, uint32_t target, const uint32_t* a, uint32_t n);
};

static inline uint32_t callOrd(const D2GameBatch12380Env* e, uint32_t ord, std::initializer_list<uint32_t> a) {
    uint32_t buf[8] = {0}; uint32_t n = 0;
    for (uint32_t v : a) buf[n++] = v;
    return e->call(e->ctx, 0x80000000u | ord, buf, n);
}
static inline uint32_t callRva(const D2GameBatch12380Env* e, uint32_t rva, std::initializer_list<uint32_t> a) {
    uint32_t buf[8] = {0}; uint32_t n = 0;
    for (uint32_t v : a) buf[n++] = v;
    return e->call(e->ctx, rva, buf, n);
}

static inline const D2GameBatch12380Env* d2game12380_env(const uint32_t* r) { return (const D2GameBatch12380Env*)(uintptr_t)r[5]; }

[[noreturn]] static void d2game_fatal12380(uint32_t) { _exit(-1); }

// 1.13c D2Game+0x1EB90 -- SendItemSlotLookup_1EB90 (62 bytes). Ordinal_10561 (no args), Ordinal_10826 and
// Ordinal_10459 level lookup, then FUN_6fc3e960 (RVA 0x1E960) with the implicit stack/ESI values, passed
// as r[0]=stack_c, r[1]=stack_8, r[2]=ESI (unit), r[3]=unused.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_1EB90(uint32_t* r) {
    const D2GameBatch12380Env* e = d2game12380_env(r);
    callOrd(e, 10561, {});
    uint32_t lvl = callOrd(e, 10826, {});
    uint32_t act = callOrd(e, 10459, {lvl});
    callRva(e, 0x1E960, {r[0], act & 0xff, r[2], r[1]});
    r[4] = 0;
}

// 1.13c D2Game+0x1EC40 -- SUNITINACTIVE_RemoveById_1EC40 (154 bytes). __fastcall: r[0]=ECX (param_1, unit id),
// r[1]=EAX (type index, low byte), r[2]=EBX (game base). Walks the inactive list at game+0xD8 + type*4.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_1EC40(uint32_t* r) {
    const D2GameBatch12380Env* e = d2game12380_env(r);
    uint8_t* game = (uint8_t*)(uintptr_t)r[2];
    uint32_t param1 = r[0];
    uint32_t iVar4 = rd32(game, 0xd8 + (r[1] & 0xff) * 4);
    r[4] = 0;
    if (iVar4 == 0) return;
    do {
        uint32_t iVar3 = rd32((void*)(uintptr_t)iVar4, 0xc);
        uint32_t iVar2 = 0;
        uint32_t iVar1;
        while ((iVar1 = iVar3) != 0) {
            if (rd32((void*)(uintptr_t)iVar1, 8) == (param1 & 0xffff)) {
                if (iVar1 == rd32((void*)(uintptr_t)iVar4, 0xc)) {
                    wr32((void*)(uintptr_t)iVar4, 0xc, rd32((void*)(uintptr_t)iVar1, 0x58));
                } else {
                    wr32((void*)(uintptr_t)iVar2, 0x58, rd32((void*)(uintptr_t)iVar1, 0x58));
                }
                iVar4 = rd32((void*)(uintptr_t)iVar1, 0x20);
                while (iVar4 != 0) {
                    iVar4 = rd32((void*)(uintptr_t)iVar4, 4);
                    callOrd(e, 10046, {0});   // "..\\Source\\D2Game\\Ai\\AiGeneral.cpp", 0x246
                }
                if (rd32((void*)(uintptr_t)iVar1, 0x24) != 0) callOrd(e, 10850, {});
                callOrd(e, 10046, {0});       // "..\\Source\\D2Game\\UNIT\\SUnitInactive.cpp", 0x8D
                return;
            }
            iVar2 = iVar1;
            iVar3 = rd32((void*)(uintptr_t)iVar1, 0x58);
        }
        iVar4 = rd32((void*)(uintptr_t)iVar4, 0x14);
    } while (iVar4 != 0);
}

// 1.13c D2Game+0x19662 -- AiAcquireTargetAndAct_19662 (208 bytes). r[0]=ECX (unused), r[1]=EDX (param_2, a
// uint32 pointer), r[2]=EDI (unit-owner game arg), r[3]=ESI (unit). Unit seed at unit+8 (uint64).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_19662(uint32_t* r) {
    const D2GameBatch12380Env* e = d2game12380_env(r);
    uint32_t unit = r[3];
    uint32_t* param2 = (uint32_t*)(uintptr_t)r[1];
    uint32_t game = r[2];
    uint32_t target = callOrd(e, 10331, {unit});
    uint32_t iVar4 = callOrd(e, 10057, {target});
    r[4] = 0;
    if (iVar4 == 0) {
        uint32_t piVar5 = callRva(e, 0xB1C90, {unit, param2 ? *param2 : 0});
        if (piVar5 != 0) {
            if (param2 == nullptr) {
                uint32_t uVar6 = callRva(e, 0x1180, {unit + 0x20});
                if ((int32_t)uVar6 < 0x1e) {
                    callRva(e, 0xF2540, {game, unit, piVar5, 2, 0, 0, 1});
                    return;
                }
            } else {
                uint32_t uVar6 = callRva(e, 0x1180, {unit + 0x20});
                if ((int32_t)uVar6 < 0x50) {
                    callRva(e, 0xF2220, {game, piVar5});
                    return;
                }
            }
            goto LAB_fallthrough;
        }
    }
    {
        uint64_t seed = (uint64_t)rd32((void*)(uintptr_t)unit, 0x20) * 0x6ac690c5ull + (uint64_t)rd32((void*)(uintptr_t)unit, 0x24);
        wr64((void*)(uintptr_t)unit, 0x20, seed);
        if ((uint32_t)((uint32_t)seed % 100) < 0x14) {
            callRva(e, 0xF1B10, {game, unit});
            return;
        }
    }
LAB_fallthrough:
    callRva(e, 0xF22C0, {10});
    (void)d2game_fatal12380;
}

// 1.13c D2Game+0x23440 -- SUNIT_SendStateUpdate_23440 (290 bytes). __fastcall: r[0]=ECX (game), r[1]=EDX
// (unit pointer). Returns with no output; sends the state update through the seams.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_23440(uint32_t* r) {
    const D2GameBatch12380Env* e = d2game12380_env(r);
    uint32_t game = r[0];
    uint32_t unit = r[1];
    r[4] = 0;
    if (unit == 0) return;
    if (rd8((void*)(uintptr_t)unit, 0xc6) & 1) return;
    uint32_t type = rd32((void*)(uintptr_t)unit, 0);
    uint32_t mode = rd32((void*)(uintptr_t)unit, 0x10);
    if (type == 0) {
        if (mode == 0 || mode == 0x11) return;
    } else {
        if (type != 1) return;
        if (mode == 0 || mode == 0xc) return;
    }
    uint32_t iVar1 = callOrd(e, 10494, {unit});
    if (iVar1 != 0) {   // iStack_30 = 0x36 is written before the call; the seam takes it as r-arg-less
        callRva(e, 0x8EA80, {game, unit, 7, rd32((void*)(uintptr_t)game, 0xa8) + 3, 0, 0, 0});
        return;
    }
    uint32_t sub = rd32((void*)(uintptr_t)unit, 0x14);
    if (type == 1 && sub != 0) {
        uint32_t iVar1b = rd32((void*)(uintptr_t)sub, 0x28);
        if (iVar1b != 0 && rd32((void*)(uintptr_t)iVar1b, 0x28) != 0) {
            uint32_t piVar2 = callRva(e, 0xE03A0, {rd32((void*)(uintptr_t)iVar1b, 0x28), rd32((void*)(uintptr_t)iVar1b, 0x2c)});
            if (piVar2 != 0 && rd32((void*)(uintptr_t)piVar2, 0) == 0) {
                callRva(e, 0x97B50, {game, piVar2, rd32((void*)(uintptr_t)unit, 0x0c)});
                return;
            }
        }
    }
    callOrd(e, 11103, {unit, 0});
    // iStack_30 = 0 is a host-stack pointer in retail; passed as 0 (MISMATCH note: pointer-in-out).
    callRva(e, 0x75E10, {game, 0});
}

// 1.13c D2Game+0x173D0 -- AI_WanderStep_173D0 (726 bytes). __fastcall: r[0]=ECX (param_1, tick/game arg),
// r[1]=EDX (param_2, unit), r[2]=stack param_3 (game-state pointer). Control flow mirrors the decompile's
// labels; the locals at stack -0x24/-0x1C are scratch offsets S_LOCAL24/S_LOCAL1C. The message record at
// S_LOCAL1C is {+0: pad, +8: type 2 or 8, +0xC: x delta, +0x10: y delta}.
#define S_LOCAL24 0x00u
#define S_LOCAL1C 0x10u
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_173D0(uint32_t* r) {
    const D2GameBatch12380Env* e = d2game12380_env(r);
    uint32_t param1 = r[0], p2 = r[1];
    uint32_t iVar1 = rd32((void*)(uintptr_t)r[2], 0);      // *param_3, the game
    uint32_t pp3 = 0, iVar6 = 0, iVar7 = 0, iVar8 = 0, uVar5 = 0, piVar4 = 0, local_2c = 0, local_24 = 0;
    uint8_t* scr = e->scratch;
    r[4] = 0;

    if (p2 == 0 || rd32((void*)(uintptr_t)p2, 0) != 1 || rd32((void*)(uintptr_t)p2, 0x14) == 0 ||
        (iVar6 = rd32((void*)(uintptr_t)rd32((void*)(uintptr_t)p2, 0x14), 0x28)) == 0) {
        pp3 = 0;
        goto LAB_406;
    }
    pp3 = rd32((void*)(uintptr_t)iVar6, 0x20);
    if (pp3 == 0 || rd32((void*)(uintptr_t)pp3, 8) != 8) goto LAB_406;
    callRva(e, 0x216A0, {});
    goto LAB_bd230;

LAB_406:
    local_24 = 0;
    wr32(scr, S_LOCAL24, 0);
    callRva(e, 0x21490, {S_LOCAL24, 0});
    local_24 = rd32(scr, S_LOCAL24);
    if ((p2 == 0 || (rd32((void*)(uintptr_t)p2, 0) != 1 || rd32((void*)(uintptr_t)p2, 0x14) == 0 ||
         ((iVar6 = rd32((void*)(uintptr_t)rd32((void*)(uintptr_t)p2, 0x14), 0x54)) != 3 && iVar6 != 0x13))) &&
        local_24 == 0) {
        if (p2 == 0 || rd32((void*)(uintptr_t)p2, 0) != 1) {
            piVar4 = 0;
        } else if (rd32((void*)(uintptr_t)p2, 0x14) == 0 ||
                   (iVar6 = rd32((void*)(uintptr_t)rd32((void*)(uintptr_t)p2, 0x14), 0x28)) == 0) {
            piVar4 = 0;
        } else if (rd32((void*)(uintptr_t)iVar6, 0x28) == 0) {
            piVar4 = 0;
        } else {
            piVar4 = callRva(e, 0xE03A0, {rd32((void*)(uintptr_t)iVar6, 0x28), rd32((void*)(uintptr_t)iVar6, 0x2c)});
        }
        if (piVar4 != p2) {
            if (pp3 != 0) {
                uint32_t a3 = rd32((void*)(uintptr_t)pp3, 0xc);
                uint32_t a4 = rd32((void*)(uintptr_t)iVar1, 0x14);
                uint32_t u2 = rd32((void*)(uintptr_t)iVar1, 0x18);
                uint32_t a6 = rd32((void*)(uintptr_t)pp3, 0x10);
                callRva(e, 0x20EC0, {});
                callRva(e, 0xF2540, {param1, p2, 0, 2, a3 + a4, (uint8_t)((uint8_t)a6 + (uint8_t)u2), 1});
                return;
            }
            callRva(e, 0xF22C0, {0xF});
            return;
        }
        uVar5 = callRva(e, 0x1180, {p2 + 0x20});
        if ((int32_t)uVar5 < 0x1e) goto LAB_691;
        local_2c = rd32((void*)(uintptr_t)iVar1, 0x1c) + 1;
        if ((int32_t)local_2c > 99) local_2c = 0;
        iVar6 = callRva(e, 0x1100, {p2});
        iVar7 = callRva(e, 0x1130, {p2});
        pp3 = callRva(e, 0x1180, {p2 + 0x20});
        if ((int32_t)local_2c < 0x19) {
            uVar5 = callRva(e, 0x1450, {2});
            if (uVar5 != 0) pp3 = (uint32_t)(-(int32_t)pp3);
            iVar6 = iVar6 + pp3;
            piVar4 = 0xfffffffbu;
            goto LAB_551;
        }
        if ((int32_t)local_2c < 0x32) {
            uVar5 = callRva(e, 0x1450, {2});
            if (uVar5 != 0) pp3 = (uint32_t)(-(int32_t)pp3);
            iVar6 = iVar6 + 5;
            piVar4 = pp3;
            goto LAB_551;
        }
        goto LAB_55c;
LAB_551:
        iVar7 = iVar7 + piVar4;
        if ((int32_t)local_2c > 0x31) goto LAB_55c;
LAB_580:
        if ((int32_t)local_2c < 0x4b) goto LAB_5a0;
        goto LAB_tail;
LAB_55c:
        if ((int32_t)local_2c < 0x4b) {
            uVar5 = callRva(e, 0x1450, {2});
            if (uVar5 != 0) pp3 = (uint32_t)(-(int32_t)pp3);
            iVar6 = iVar6 + pp3;
            iVar7 = iVar7 + 5;
            goto LAB_580;
        }
LAB_tail:
        uVar5 = callRva(e, 0x1450, {2});
        if (uVar5 != 0) pp3 = (uint32_t)(-(int32_t)pp3);
        iVar6 = iVar6 - 5;
        iVar7 = iVar7 + pp3;
LAB_5a0:
        iVar8 = callRva(e, 0xF2540, {param1, p2, 0, 2, iVar6, (uint8_t)iVar7, 1});
        if (iVar8 == 0) {
            wr32((void*)(uintptr_t)iVar1, 0x1c, local_2c + 0x19);
            return;
        }
        wr32(scr, S_LOCAL1C + 8, 2);
        wr32(scr, S_LOCAL1C + 0xC, iVar6);
        wr32(scr, S_LOCAL1C + 0x10, iVar7);
        callRva(e, 0x212F0, {param1, S_LOCAL1C});
        wr32((void*)(uintptr_t)iVar1, 0x1c, local_2c);
        return;
    }
    wr32(scr, S_LOCAL1C + 8, 8);
    callRva(e, 0x212F0, {param1, S_LOCAL1C});
    callRva(e, 0x216A0, {});
LAB_bd230:
    callRva(e, 0x9D230, {param1, p2});
LAB_691:
    callRva(e, 0xF22C0, {10});
}

// 1.13c D2Game+0x18480 -- AI_RollActionByTable_18480 (674 bytes). __fastcall: r[0]=ECX (game), r[1]=EDX
// (unit), r[2]=stack param_3 (the monster-action record). The 0x2C-byte message record at S_MSG is
// {+0 type, +4 target, +0x14 low word = the hit percent}. The out-of-band stack words are zeroed (MISMATCH:
// retail stack layout inferred from the decompile, not verified).
#define S_MSG 0x40u
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_18480(uint32_t* r) {
    const D2GameBatch12380Env* e = d2game12380_env(r);
    uint32_t param1 = r[0], p2 = r[1], param3 = r[2];
    uint8_t* scr = e->scratch;
    uint32_t iVar5 = rd32((void*)(uintptr_t)param3, 0x1c);
    uint32_t iVar1 = rd32((void*)(uintptr_t)param3, 8);
    uint32_t local_24 = rd32((void*)(uintptr_t)param3, 0x18);
    uint32_t local_28 = 0, iVar3 = 0, iVar4 = 0, uVar2 = 0;
    uint32_t mode = 0;
    uint8_t uVar6 = 0;
    r[4] = 0;
    param3 = iVar1;

    if (p2 != 0 && rd32((void*)(uintptr_t)p2, 0) == 1 && rd32((void*)(uintptr_t)p2, 0x14) != 0 &&
        (iVar3 = rd32((void*)(uintptr_t)rd32((void*)(uintptr_t)p2, 0x14), 0x28)) != 0 &&
        (iVar3 = rd32((void*)(uintptr_t)iVar3, 0x20)) != 0) {
        if (rd32((void*)(uintptr_t)iVar3, 8) == 1 && rd32((void*)(uintptr_t)param1, 0xa8) < rd32((void*)(uintptr_t)iVar3, 0x14)) {
            iVar3 = callRva(e, 0xE03A0, {rd32((void*)(uintptr_t)iVar3, 0x10)});
            if (iVar3 != 0) {
                iVar4 = callRva(e, 0xDFA90, {});
                if (iVar4 == 0) {
                    local_28 = 1;
                    if (iVar1 != iVar3) {
                        callOrd(e, 10522, {p2, iVar3});
                        local_24 = callOrd(e, 11138, {p2, iVar3, 0});
                        param3 = iVar3;
                    }
                    goto LAB_523;
                }
            }
        }
        callRva(e, 0x20EC0, {});
    }
LAB_523:
    mode = rd8((void*)(uintptr_t)param1, 0x6d);
    if (local_24 == 0) {
        if (local_28 == 0) {
            uVar2 = (uint64_t)rd32((void*)(uintptr_t)p2, 0x20) * 0x6ac690c5ull + rd32((void*)(uintptr_t)p2, 0x24);
            wr64((void*)(uintptr_t)p2, 0x20, uVar2);
            if ((int16_t)rd16((void*)(uintptr_t)iVar5, 0x62 + mode * 2) <= (int32_t)((uint32_t)uVar2 % 100)) {
                callRva(e, 0xF22C0, {(uint32_t)(int16_t)rd16((void*)(uintptr_t)iVar5, 0x68 + mode * 2)});
                return;
            }
        }
        callRva(e, 0xF2540, {param1, p2, param3, 2, 0, 0, 1});
        return;
    }
    if (local_28 == 0) {
        uVar2 = (uint64_t)rd32((void*)(uintptr_t)p2, 0x20) * 0x6ac690c5ull + rd32((void*)(uintptr_t)p2, 0x24);
        wr64((void*)(uintptr_t)p2, 0x20, uVar2);
        if ((int16_t)rd16((void*)(uintptr_t)iVar5, 0x56 + mode * 2) <= (int32_t)((uint32_t)uVar2 % 100)) {
            callRva(e, 0xF22C0, {(uint32_t)(int16_t)rd16((void*)(uintptr_t)iVar5, 0x5c + mode * 2)});
            return;
        }
    }
    uVar2 = (uint64_t)rd32((void*)(uintptr_t)p2, 0x20) * 0x6ac690c5ull + rd32((void*)(uintptr_t)p2, 0x24);
    wr64((void*)(uintptr_t)p2, 0x20, uVar2);
    if ((int32_t)((uint32_t)uVar2 % 100) < (int16_t)rd16((void*)(uintptr_t)iVar5, 0x5c + mode * 2)) {
        callOrd(e, 11103, {p2, 0});
        std::memset(scr + S_MSG, 0, 0x20);
        wr32(scr, S_MSG, 5);
        uVar6 = 100;
        local_24 = p2;
        if (p2 != 0) {
            iVar5 = callRva(e, 0x1320, {});
            if (iVar5 != 0 && (rd8((void*)(uintptr_t)iVar5, 0x104) & rd32(e->bitMasks, 0x14)) != 0) uVar6 = 0x65;
        }
        wr8(scr, S_MSG + 0x14, uVar6);
        callRva(e, 0x75E10, {param1, S_MSG});
        return;
    }
    callOrd(e, 11103, {p2, 0});
    std::memset(scr + S_MSG, 0, 0x20);
    wr32(scr, S_MSG, 4);
    uVar6 = 100;
    local_24 = p2;
    if (p2 != 0) {
        iVar5 = callRva(e, 0x1320, {});
        if (iVar5 != 0 && (rd8((void*)(uintptr_t)iVar5, 0x104) & rd32(e->bitMasks, 0x10)) != 0) uVar6 = 0x65;
    }
    wr8(scr, S_MSG + 0x14, uVar6);
    callRva(e, 0x75E10, {param1, S_MSG});
}

// 1.13c D2Game+0x19C20 -- AI_TryAttackStep_19C20 (591 bytes). Stack-args: r[1]=param_1 (unit-id pointer),
// r[2]=param_2 (a pointer to the step record); r[0]=EAX (in_EAX, the monster unit). Returns the low byte
// of the retail result (1 on the handled paths); the upper 24 bits are not used by any caller.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_19C20(uint32_t* r) {
    const D2GameBatch12380Env* e = d2game12380_env(r);
    uint32_t inEax = r[0], param1 = r[1], param2 = r[2];
    uint32_t thisp = callRva(e, 0xF30C0, {param1});
    uint32_t uVar5 = callRva(e, 0xB0A20, {thisp});
    uint32_t uVar9 = uVar5;
    uint32_t bVar3 = 0, bVar4 = 0, iVar6 = 0, iVar7 = 0, puVar1 = 0, piVar2 = 0;
    uint32_t uVar8 = 0;
    r[4] = 0;
    if (inEax != 0 && (uVar9 = rd32((void*)(uintptr_t)inEax, 0)) == 1 &&
        (uVar9 = rd32((void*)(uintptr_t)inEax, 0x14)) != 0 &&
        (puVar1 = rd32((void*)(uintptr_t)uVar9, 0x30)) != 0) {
        for (piVar2 = rd32((void*)(uintptr_t)puVar1, 0); piVar2 != 0; piVar2 = rd32((void*)(uintptr_t)piVar2, 8)) {
            if (rd32((void*)(uintptr_t)piVar2, 0) == thisp) { bVar4 = 1; goto LAB_9c84; }
        }
        bVar4 = 0;
LAB_9c84:
        if (thisp != 0 && rd32((void*)(uintptr_t)thisp, 0) == 0 &&
            (rd8((void*)(uintptr_t)thisp, 0x6c) != 0 ||
             ((iVar6 = callOrd(e, 11017, {rd32((void*)(uintptr_t)thisp, 0x60)})) != 0 ||
              (iVar6 = callOrd(e, 10920, {thisp}), rd32((void*)(uintptr_t)iVar6, 0x4c) == 1)))) {
            bVar4 = 1;
        }
        for (piVar2 = rd32((void*)(uintptr_t)puVar1, 0); piVar2 != 0; piVar2 = rd32((void*)(uintptr_t)piVar2, 8)) {
            if (rd32((void*)(uintptr_t)piVar2, 0) != 0) { bVar3 = 1; goto LAB_9cce; }
        }
        bVar3 = 0;
LAB_9cce:
        iVar6 = rd32((void*)(uintptr_t)param2, 0);
        if (bVar3 || bVar4 || (uVar9 = rd32((void*)(uintptr_t)iVar6, 0x14), (int32_t)uVar9 > 0)) {
            uVar9 = callRva(e, 0xB07B0, {});
            if (uVar9 < 3 || (int32_t)rd32((void*)(uintptr_t)iVar6, 0x14) < 0x25) {
                if ((int32_t)rd32((void*)(uintptr_t)iVar6, 0x14) > 0) {
                    if (rd32((void*)(uintptr_t)inEax, 0x2c) != 0) callOrd(e, 10159, {rd32((void*)(uintptr_t)inEax, 0x2c)});
                    callRva(e, 0xF22C0, {8});
                }
            } else {
                callRva(e, 0xF2540, {param1, inEax, 0, 2, rd32((void*)(uintptr_t)iVar6, 0x18), rd8((void*)(uintptr_t)iVar6, 0x1c), 1});
            }
            iVar7 = rd32((void*)(uintptr_t)iVar6, 0x14);
            if ((int32_t)iVar7 < 0) {
                wr32((void*)(uintptr_t)iVar6, 0x14, 0);
                r[4] = 1;
                return;
            }
            iVar7 = iVar7 - 1;
            wr32((void*)(uintptr_t)iVar6, 0x14, iVar7);
        } else {
            if (thisp == 0 || thisp == inEax) goto LAB_9de9;
            if ((int32_t)uVar5 < 3 || (int32_t)uVar5 > 0x17) {
                callOrd(e, 10159, {rd32((void*)(uintptr_t)inEax, 0x2c)});
                if (rd32((void*)(uintptr_t)iVar6, 0x18) == 0) {
                    wr32((void*)(uintptr_t)iVar6, 0x18, 0x3c);
                    if (rd32((void*)(uintptr_t)thisp, 0) == 0) {
                        callRva(e, 0xDFFD0, {0x12, thisp});
                        callRva(e, 0xF22C0, {0x14});
                        r[4] = 1;
                        return;
                    }
                } else {
                    if ((int32_t)rd32((void*)(uintptr_t)iVar6, 0x18) < 0) wr32((void*)(uintptr_t)iVar6, 0x18, 0);
                    if ((int32_t)rd32((void*)(uintptr_t)iVar6, 0x18) > 0)
                        wr32((void*)(uintptr_t)iVar6, 0x18, rd32((void*)(uintptr_t)iVar6, 0x18) - 1);
                }
                callRva(e, 0xF22C0, {0x14});
                r[4] = 1;
                return;
            }
            iVar7 = callRva(e, 0x16610, {param1, param2, 0x10});
            if ((iVar7 & 0xff) == 0) {
                iVar6 = (rd32((void*)(uintptr_t)inEax, 0) == 1 && rd32((void*)(uintptr_t)inEax, 0x14) != 0)
                            ? rd32((void*)(uintptr_t)rd32((void*)(uintptr_t)inEax, 0x14), 0x2c) : 0;
                wr32((void*)(uintptr_t)iVar6, 0x18, 7);
                uVar8 = callRva(e, 0xF1D90, {thisp, param1, inEax, 2, 2});
                r[4] = 1;
                return;
            }
        }
        r[4] = 1;
        return;
    }
LAB_9de9:
    r[4] = uVar9 & 0xffffff00u;
    (void)bVar3; (void)uVar8;
}

// Calls our 0x19C20 probe as a function: the caller's EAX (in_EAX) is passed as r[0].
static uint32_t d2game12380_call19C20(const uint32_t* outer, uint32_t eax, uint32_t p1, uint32_t p2) {
    uint32_t rr[8] = {eax, p1, p2, 0, 0, outer[5]};
    d2_fn_d2game_19C20(rr);
    return rr[4];
}

// 1.13c D2Game+0x1B950 -- AI_SkillUseStep_1B950 (609 bytes). __fastcall: r[0]=ECX (param_1, unit), r[1]=EDX
// (param_2, monster), r[2]=stack param_3. The 0x19C20 call takes an in_EAX the decompile does not show; here
// it is passed as the unit (MISMATCH: implicit register input, not recoverable from the decompile).
#define S_L8 0x00u
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_1B950(uint32_t* r) {
    const D2GameBatch12380Env* e = d2game12380_env(r);
    uint32_t param1 = r[0], p2 = r[1], p3 = r[2];
    uint8_t* scr = e->scratch;
    uint32_t uVar2 = callRva(e, 0x15D10, {});
    uint32_t piVar5 = 0, piVar7 = 0, iVar6 = 0, iVar3 = 0, uVar4 = 0;
    uint8_t bVar1 = 0;
    r[4] = 0;
    if ((uint8_t)uVar2 != 0) return;
    if (p2 != 0 && rd32((void*)(uintptr_t)p2, 0) == 1 && rd32((void*)(uintptr_t)p2, 0x14) != 0) {
        iVar6 = rd32((void*)(uintptr_t)rd32((void*)(uintptr_t)p2, 0x14), 0x28);
        if (iVar6 != 0) {
            piVar5 = rd32((void*)(uintptr_t)iVar6, 0x20);
            if (piVar5 != 0) {
                piVar7 = rd32((void*)(uintptr_t)piVar5, 0);
                iVar6 = rd32((void*)(uintptr_t)piVar7, 8);
                while (iVar6 != 10 && piVar7 != piVar5) {
                    piVar7 = rd32((void*)(uintptr_t)piVar7, 0);
                    iVar6 = rd32((void*)(uintptr_t)piVar7, 8);
                }
                if (rd32((void*)(uintptr_t)piVar7, 8) == 10) goto LAB_9a8;
            }
        }
    }
    piVar7 = 0;
LAB_9a8:
    bVar1 = (uint8_t)callRva(e, 0xC1F40, {});
    if (bVar1 != 0) {
        if (piVar7 != 0) {
            piVar5 = callRva(e, 0xF30C0, {param1});
            iVar6 = callRva(e, 0xB0A20, {piVar5});
            iVar3 = callRva(e, 0xC2040, {});
            if (iVar3 != 0 && (uVar4 = callRva(e, 0xB07B0, {})) > 2) {
                callRva(e, 0xF2540, {param1, p2, 0, 2, rd32((void*)(uintptr_t)piVar7, 0xc), (uint8_t)(rd32((void*)(uintptr_t)piVar7, 0x10) - 3), 1});
                return;
            }
            uVar4 = callRva(e, 0xB07B0, {});
            if (uVar4 > 7) {
                callRva(e, 0xF2540, {param1, p2, 0, 2, rd32((void*)(uintptr_t)piVar7, 0xc), (uint8_t)(rd32((void*)(uintptr_t)piVar7, 0x10) - 3), 1});
                return;
            }
            uVar2 = d2game12380_call19C20(r, param1, param1, p3);
            if ((uint8_t)uVar2 == 0 && piVar5 != 0 && piVar5 != p2) {
                if (rd32((void*)(uintptr_t)piVar5, 0) == 0) {
                    if ((int32_t)iVar6 < 0xc) {
                        uVar4 = callRva(e, 0x1180, {p2 + 0x20});
                        if ((int32_t)uVar4 < 100) callRva(e, 0xF2870, {piVar5, param1, p2});
                        callRva(e, 0xF22C0, {200});
                        return;
                    }
                    callRva(e, 0xF22C0, {0x14});
                }
                uVar4 = callRva(e, 0x1180, {p2 + 0x20});
                if ((int32_t)uVar4 < 0x32) callRva(e, 0xF1B10, {param1, p2});
            }
        }
        callRva(e, 0xF22C0, {0x78});
        return;
    }
    wr32(scr, S_L8, rd32((void*)(uintptr_t)piVar7, 0xc));
    uint32_t local_4 = rd32((void*)(uintptr_t)piVar7, 0x10);
    iVar3 = callRva(e, 0xC1EE0, {S_L8});
    iVar6 = local_4;
    uint32_t pa8 = rd32((void*)(uintptr_t)param1, 0xa8);
    uint32_t p18 = rd32((void*)(uintptr_t)piVar7, 0x18);
    if ((int32_t)pa8 <= (int32_t)p18 || (pa8 - p18) > 199u) {
        uVar4 = callRva(e, 0xB07B0, {});
        if (uVar4 > 1) {
            callRva(e, 0xF2540, {param1, p2, 0, 2, rd32(scr, S_L8), (uint8_t)iVar6, 1});
            wr32((void*)(uintptr_t)piVar7, 0x18, rd32((void*)(uintptr_t)param1, 0xa8));
            return;
        }
        if (iVar3 == 0) {
            uVar2 = d2game12380_call19C20(r, param1, param1, p3);
            if ((uint8_t)uVar2 != 0) {
                wr32((void*)(uintptr_t)piVar7, 0x18, rd32((void*)(uintptr_t)param1, 0xa8));
                return;
            }
        }
    }
    callRva(e, 0xF22C0, {0x14});
}

// 1.13c D2Game+0x1DB40 -- AI_CommandDispatch_1DB40 (511 bytes). __fastcall: r[0]=ECX (param_1, unit),
// r[1]=EDX (param_2, command record: +4 is the command id), r[2]=stack param_3. The out-params &local_8,
// &local_c are scratch offsets S_L8/S_LC; local_4 (a byte the decompile reads but never sets on the reached
// paths) is passed as 0 (MISMATCH: uninitialized in retail).
#define S_L8D 0x00u
#define S_LCD 0x04u
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_1DB40(uint32_t* r) {
    const D2GameBatch12380Env* e = d2game12380_env(r);
    uint32_t param1 = r[0], p2 = r[1], p3 = r[2];
    uint8_t* scr = e->scratch;
    uint32_t uVar1 = callRva(e, 0x15D10, {});
    int32_t iVar2 = 0, iVar3 = 0;
    uint32_t uVar4 = 0;
    uint8_t local_4 = 0;
    r[4] = 0;
    if ((uint8_t)uVar1 != 0) return;
    if (p2 == 0) {
        iVar2 = -1;
    } else {
        iVar2 = (int32_t)rd32((void*)(uintptr_t)p2, 4);
        if (iVar2 > 0xff) {
            if (iVar2 == 0x109) {
                iVar2 = (int32_t)callRva(e, 0x604C0, {S_L8D});
                if (iVar2 != 0) {
                    uVar4 = callRva(e, 0xB07B0, {});
                    if (uVar4 > 2) goto LAB_dce1;
                    callRva(e, 0x603D0, {});
                }
            } else if (iVar2 == 0x200) {
                callRva(e, 0x5D1D0, {param1});
            }
            goto LAB_dd03;
        }
        if (iVar2 == 0xff) {
            iVar2 = (int32_t)callRva(e, 0xE2C40, {S_L8D});
            if (iVar2 != 0) {
                uVar4 = callRva(e, 0xB07B0, {});
                if (uVar4 < 4) {
                    callRva(e, 0xF21E0, {param1, 0, 0});
                    callRva(e, 0xE5F60, {});
                    return;
                }
LAB_dce1:
                callRva(e, 0xF2540, {param1, p2, 0, 2, rd32(scr, S_L8D), local_4, 1});
                return;
            }
            goto LAB_dd03;
        }
    }
    if (iVar2 == 0xc9) {
        uVar1 = callRva(e, 0xC3260, {});
        if ((uint8_t)uVar1 != 1) {
            callRva(e, 0xF22C0, {0x28});
            return;
        }
        callRva(e, 0xC8990, {param1, p2, S_L8D, S_LCD});
        if (rd32(scr, S_LCD) != 0) callRva(e, 0xF22C0, {0x14});
        if (rd32(scr, S_L8D) == 0) return;
        iVar2 = (int32_t)callRva(e, 0xC1F90, {});
        if (iVar2 != 0) {
            iVar2 = (int32_t)callRva(e, 0x217D0, {});
            if (iVar2 != 0) {
                iVar3 = (int32_t)callRva(e, 0x1100, {p2});
                wr32((void*)(uintptr_t)iVar2, 0xc, iVar3 + 9);
                uVar1 = callRva(e, 0x1130, {p2});
                wr32((void*)(uintptr_t)iVar2, 0x10, uVar1);
                callRva(e, 0xF22C0, {0x32});
                return;
            }
        }
    } else if (iVar2 == 0xfe && (iVar2 = (int32_t)callRva(e, 0xE2B40, {}), iVar2 != 0)) {
        callRva(e, 0xF21E0, {param1, 0, 0});
        callRva(e, 0xE2B10, {});
        return;
    }
LAB_dd03:
    uVar1 = d2game12380_call19C20(r, param1, param1, p3);
    if ((uint8_t)uVar1 == 0) {
        uVar1 = callRva(e, 0x16A30, {param1});
        if ((uint8_t)uVar1 == 0) {
            uVar4 = callRva(e, 0x1BBC0, {param1, p2, p3});
            if ((uint8_t)uVar4 == 0) callRva(e, 0xF22C0, {8});
        }
    }
}
