// src/core/d2game/d2game_batch_b55gama.cpp -- 1.13c D2Game.dll batch b55-gam-a, transcribed from the cached
// Ghidra decompile. Stage A batch lane: implementation + marker only, no spec, no vector.
//
// Of the 12 rows claimed, only 0x7FE60 is a plain __fastcall(ECX, EDX, stack) function with no hidden
// register inputs. The others take unmodelled register arguments (unaff_EDI/ESI/EBX/EBP/in_EAX) or call
// helpers with unrecovered conventions, and are left UNMAPPED rather than written as placeholders.
// Structures use the 1.13c 32-bit image offsets; callees the file does not own are Env seams in retail
// argument order, as the sibling batch files do.
#include <cstdint>
#include <emscripten.h>

struct D2GameB55gamaEnv {
    void* (*resolve)(uint32_t addr32);                       // 32-bit image pointer -> host pointer
    void (*fn_F2380)(void* pUnit);                           // FUN_6fd12380 (ECX)
    void (*fn_F22C0)(int32_t mode);                          // FUN_6fd122c0
    void (*fn_F2540)(void* pGame, void* pUnit, uint32_t a, int32_t b, int32_t c, int32_t d, int32_t e);
    void (*fn_75E10)(void* pGame, void* pEvent);             // FUN_6fc95e10
    void* (*fn_1320)(void);                                  // FUN_6fc21320 (unit-> record)
    uint32_t (*fn_1180)(void* pSeed);                        // FUN_6fc21180
    void (*fn_A496)(void* pUnit, int32_t flag);              // Ordinal_11103
    const uint8_t* gdwBitMasks;                              // gdwBitMasks_exref
    uint32_t stat4_unused;
};

static inline int16_t b55gama_s16(const uint8_t* p, uint32_t o) { return *(const int16_t*)(p + o); }

// 1.13c D2Game+0x7FE60
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_7fe60(uint8_t* param_1, uint8_t* param_2, uint8_t* param_3,
                                             const D2GameB55gamaEnv* env)
{
    uint32_t uVar2 = *(uint32_t*)(param_3 + 8);
    uint8_t* rec = (uint8_t*)env->resolve(*(uint32_t*)(param_3 + 0x1c));
    uint32_t* seed = (uint32_t*)(param_2 + 0x20);
    uint32_t diff = param_1[0x6d];

    if (*(int32_t*)(param_3 + 0x18) != 0) {
        if (b55gama_s16(rec, 0x170) >= 0) {
            uint64_t v = (uint64_t)seed[0] * 0x6ac690c5ull + seed[1];
            *(uint64_t*)seed = v;
            if ((int32_t)((uint32_t)v % 100) < b55gama_s16(rec, 0x56 + diff * 2)) {
                env->fn_F2380(param_2);
                return;
            }
        }
        env->fn_A496(param_2, 0);
        uint32_t ev[8] = {0};
        ev[0] = 4;
        uint8_t pct = 100;
        ev[1] = (uint32_t)(uintptr_t)param_2;  // ponytail: host-pointer truncation; event layout is env-owned
        if (param_2) {
            uint8_t* r = (uint8_t*)env->fn_1320();
            if (r && (r[0x104] & *(const uint32_t*)(env->gdwBitMasks + 0x10)))
                pct = 0x65;
        }
        ((uint8_t*)&ev[5])[1] = pct;
        env->fn_75E10(param_1, ev);
        return;
    }

    uint64_t v = (uint64_t)seed[0] * 0x6ac690c5ull + seed[1];
    uint32_t hi = (uint32_t)(v >> 32);
    *(uint64_t*)seed = v;
    if ((int32_t)((uint32_t)v % 100) < b55gama_s16(rec, 0x5c + diff * 2)) {
        env->fn_F22C0(5);
        return;
    }
    v = (uint64_t)(uint32_t)v * 0x6ac690c5ull + hi;
    *(uint64_t*)seed = v;
    if ((int32_t)((uint32_t)v % 100) < b55gama_s16(rec, 0x62 + diff * 2)) {
        if (b55gama_s16(rec, 0x170) >= 0) {
            uint32_t u = env->fn_1180(seed);
            if ((int32_t)u < b55gama_s16(rec, 0x68 + diff * 2)) {
                env->fn_F2380(param_2);
                return;
            }
        }
        if (b55gama_s16(rec, 0x172) >= 0) {
            env->fn_F2380(param_2);
            return;
        }
    }
    uint8_t* sub = nullptr;
    if (param_2 && *(int32_t*)param_2 == 1 && *(uint32_t*)(param_2 + 0x14))
        sub = (uint8_t*)env->resolve(*(uint32_t*)(param_2 + 0x14));
    uint8_t* tgt = sub ? (uint8_t*)env->resolve(*(uint32_t*)(sub + 0x2c)) : nullptr;
    *(uint32_t*)(tgt + 0x18) = 7;
    env->fn_F2540(param_1, param_2, uVar2, 2, 0, 0, 1);
}
