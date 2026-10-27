// src/core/d2game/d2game_batch_tiny_1019i.cpp -- five UNMAPPED 1.13c D2Game.dll rows plus the one
// UNMAPPED callee they pull in, reimplemented from the cached decompiles
// (build-wasm/ghidra-d2game/decomp/0x<RVA>.c) and the retail disassembly at the same RVAs, in RVA order:
//   0xA018 0xE130 0xE1C0 0xE7E0 0xEE70 0x5D080
// 0xE1C0 is the only callee of 0xE7E0 and was UNMAPPED, so it is taken first as part of that row (the
// lane's "UNMAPPED callees the tool lists are taken first"). Base 0x6FC20000.
//
// Stage A batch lane (OBJECTIVE v9): implementation + marker only, no spec, no vector.
//
// Register-only arguments (EAX / ECX / EDI / EBX / the caller-frame word) are explicit parameters, named
// at each function. D2Common ordinals are 1.13c import numbers; the callees this file does not implement
// are seams on D2GameBatchTiny1019iEnv, in retail argument order. The one callback retail passes as a
// code address (0x7CFF0) is a seam function pointer too.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>

extern "C" void d2game_unlock_3f76(int32_t lockNum);   // 0x3F76 LeaveCriticalSection on the CRT lock table

struct D2GameBatchTiny1019iEnv;

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline uint32_t rd32(uint32_t a, uint32_t o) { return rd32((const void*)(uintptr_t)a, o); }

// Callbacks invoke through retail code addresses, so the seams they need come from the env active for
// the call. The sim is single-threaded and these rows never re-enter themselves.
static const D2GameBatchTiny1019iEnv* g_activeEnv;

struct D2GameBatchTiny1019iEnv {
    // D2Common #10460 first-item getter. The 1.13c thunk takes no pushed argument; the 8F470 row's
    // disassembly shows the inventory in EAX, while these call sites leave EAX = the remove-callback's
    // pArg and ECX = game. Both are forwarded so a probe can pin the real convention.
    uint32_t (*fn10460)(uint32_t eaxArg, uint32_t ecxArg);
    uint32_t (*fn10464)(uint32_t node);                          // D2Common #10464 next-item getter
    uint32_t (*fn11147)(uint32_t node);                          // D2Common #11147 INVENTORY_UnitIsItem
    uint32_t (*fn11017)(uint32_t arg);                           // D2Common #11017 (one stack arg)
    void     (*fn10466)(uint32_t inventory, uint32_t item);      // D2Common #10466 INVENTORY_SetCursorItem
    uint32_t (*fn10646)(uint32_t inventory, uint32_t item);      // D2Common #10646 remove item from inventory
    int32_t  (*fnDFCC0)(uint32_t game, int32_t type, void* arg,   // 0xDFCC0 SUNIT_IterateUnitsOfType
                        int32_t (*cb)(uint32_t, uint32_t, void*));
    int32_t  (*fn7CFF0)(uint32_t game, uint32_t unit, void* arg); // 0x7CFF0 iterate callback of 0x5D080
    void     (*fnDFC10)(uint32_t game, void (*cb)(uint32_t, uint32_t, void*), void* arg); // 0xDFC10 SUNIT_IterateLivingPlayers
    void     (*fnE20A0)(uint32_t game, uint32_t unit);            // 0xE20A0 SUNIT_RemoveUnit
};

// 0xA018: compare ECX against the word at caller-frame -0x1C and, when they differ, release that lock
// number. Retail reads the caller's own local (the function has no prologue), so the word is a
// parameter here. From _raise (0x9EDD) it is called as (ECX = 0, word = "handler was installed").
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_a018(int32_t lockNum, int32_t callerFrameWord) {   // 1.13c D2Game+0xA018
    if (callerFrameWord != lockNum) d2game_unlock_3f76(lockNum);
}

// 0xE130: hyperbolic interpolation. ECX = a (below 0x6F returned unchanged), EDX = b, stack = c.
// Retail multiplies with imul (wrapping) and divides with idiv, so the product wraps and a zero divisor
// traps exactly as the retail instruction does.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2game_e130(int32_t a, int32_t b, int32_t c) {   // 1.13c D2Game+0xE130
    if (a < 0x6F) return a;
    const int32_t d = a - 100;
    const int32_t product = (int32_t)((uint32_t)d * (uint32_t)c);
    return product / (d + b) + 100;
}

// 0xE1C0: purge one unit from an inventory. Walk the item list starting at #10460; when an item node is
// the unit (#11147), remove it from the inventory (#10646) and stop. Otherwise, if #11017(inventory)
// answers the same unit, clear the inventory's cursor item (#10466(inv, 0)). EAX and ECX arrive from the
// caller's callback dispatch; the body reads neither directly.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_e1c0(uint32_t eaxArg, uint32_t ecxGame, uint32_t inventory, uint32_t unit, const D2GameBatchTiny1019iEnv* env) {   // 1.13c D2Game+0xE1C0
    uint32_t node = env->fn10460(eaxArg, ecxGame);
    while (node) {
        if (env->fn11147(node) == unit) {
            env->fn10646(inventory, unit);
            return;
        }
        node = env->fn10464(node);
    }
    if (env->fn11017(inventory) == unit) env->fn10466(inventory, 0);
}

// 0xE7E0: per-player callback of 0xEE70's walk. EDX = player, the pushed argument (read at 0x8(%esp))
// = the unit to purge, ECX = game. EDI = player+0x60 (inventory); the body then runs 0xE1C0 with EAX =
// the pushed argument (the iterate helper sets EAX to pArg before the indirect call).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_e7e0(uint32_t game, uint32_t player, uint32_t unit, const D2GameBatchTiny1019iEnv* env) {   // 1.13c D2Game+0xE7E0
    const uint32_t inventory = rd32(player, 0x60);
    d2_fn_d2game_e1c0(unit, game, inventory, unit, env);
}

static void d2game_tiny_1019i_e7e0_callback(uint32_t game, uint32_t player, void* arg) {
    d2_fn_d2game_e7e0(game, player, (uint32_t)(uintptr_t)arg, g_activeEnv);
}

// 0xEE70: walk the living players purging the unit from each inventory (callback 0xE7E0, argument =
// the unit), then remove the unit itself. EAX = game; ESI = unit and doubles as the pushed walk argument
// (the walk's `ret 4` consumes that slot, which is why this function never pops ESI).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_ee70(uint32_t game, uint32_t unit, const D2GameBatchTiny1019iEnv* env) {   // 1.13c D2Game+0xEE70
    g_activeEnv = env;
    env->fnDFC10(game, d2game_tiny_1019i_e7e0_callback, (void*)(uintptr_t)unit);
    env->fnE20A0(game, unit);
}

// 0x5D080: iterate the player unit list (type 0) with the fixed retail callback 0x7CFF0 and a null
// argument; the iterate helper's result (the first unit the callback accepts, or null) is returned.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_5d080(uint32_t game, const D2GameBatchTiny1019iEnv* env) {   // 1.13c D2Game+0x5D080
    return env->fnDFCC0(game, 0, nullptr, env->fn7CFF0);
}
