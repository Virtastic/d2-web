// src/core/d2game/d2game_batch_edb50.cpp -- 1.13c D2Game.dll 0xEDB50 (player-save section reader driver)
// transcribed from the cached Ghidra decompile. Implementation + marker only. Base 0x6FC20000.
//
// Register-only arguments (in_EAX, extraout_ECX) are explicit parameters. The three stack slots
// local_10 / iStack_c / uStack_8 are adjacent in retail, so they are one uint32_t[3] here and the
// callees receive pointers into it. Callees are seams on D2GameEDB50Env in retail argument order.
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

struct D2GameEDB50Env {
    uint32_t (*ordinal10265)(uint32_t id);
    void     (*ordinal10024)(const char* module, uint32_t id);              // tail is _exit(-1)
    int32_t  (*subED250)(void* p1, uint32_t* cur, uint32_t* out);           // 0xED250
    int32_t  (*subEB560)(void* unit, uint32_t* cur, uint32_t end);          // 0xEB560
    int32_t  (*subEB4E0)(void* unit, uint32_t* cur, uint32_t end);          // 0xEB4E0
    int32_t  (*subEB440)(void* unit);                                       // 0xEB440
    int32_t  (*subEBDA0)(uint32_t end, void* p1, void* unit, uint32_t* cur, uint32_t b29);  // 0xEBDA0
    uint32_t (*ordinal10973)(void* unit, uint32_t a, uint32_t b);
    int32_t  (*subECA60)(uint32_t* cur, uint32_t end, uint32_t* cur2);      // 0xECA60
    int32_t  (*subECB50)(uint32_t ecx, uint32_t v, void* p1, uint32_t* out);  // 0xECB50
    int32_t  (*subECF30)(void* p1, void* unit, uint32_t* cur, uint32_t end, uint32_t v, uint32_t* out);  // 0xECF30
    int32_t* (*subED960)(void* p1);                                         // 0xED960
    int32_t  (*subECBD0)(int32_t* ctx, uint32_t* cur, uint32_t v, uint32_t* out);  // 0xECBD0
    void     (*sub48950)(void* p1);                                         // 0x48950
    int32_t  (*subED880)(uint32_t* cur);                                    // 0xED880
    void     (*subECC90)();                                                 // 0xECC90
    uint32_t (*subED540)(uint32_t* p2);                                     // 0xED540
    int32_t  (*ordinal10331)(void* unit);
    uint32_t (*ordinal10331_3)(void* unit, uint32_t a, uint32_t b);
    void     (*ordinal10885)(uint32_t v);
    void     (*subE20A0)(void* p1, void* unit);                             // 0xE20A0
    const uint32_t* errTable;                                               // 0x6FD19F68
};

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_EDB50(int32_t in_eax, void* param_1, uint32_t* param_2, uint8_t* param_3, uint32_t* param_4, uint32_t extraout_ecx, const D2GameEDB50Env* env) {  // 1.13c D2Game+0xEDB50
    static const char kModule[] = "D2Game.dll";        // 0x6FD19FF9
    uint32_t loc[3] = { (uint32_t)(uintptr_t)param_3, 0, 0 };  // local_10, iStack_c, uStack_8
    const uint32_t end = (uint32_t)(uintptr_t)param_3 + (uint32_t)in_eax;
    void* unit = nullptr;
    *param_4 = 0;
    if (param_2 == nullptr) {
        env->ordinal10024(kModule, env->ordinal10265(0x114));
        _exit(-1);
    }
    int32_t r = env->subED250(param_1, loc, param_4);
    if (r == 0) {
        unit = (void*)(uintptr_t)*param_4;
        uint32_t u1; std::memcpy(&u1, param_3 + 4, 4);
        r = env->subEB560(unit, loc, end);
        if (r == 0 && (r = env->subEB4E0(unit, loc, end)) == 0 && (r = env->subEB440(unit)) == 0
            && (r = env->subEBDA0(end, param_1, unit, loc, param_3[0x29])) == 0) {
            loc[0] = env->ordinal10973(unit, 6, 0);
            loc[2] = env->ordinal10973(unit, 8, 0);
            if ((r = env->subECA60(loc, end, loc)) == 0
                && (r = env->subECB50(extraout_ecx, u1, param_1, &loc[1])) == 0
                && (r = env->subECF30(param_1, unit, loc, end, u1, &loc[1])) == 0) {
                int32_t* ctx = env->subED960(param_1);
                if ((r = env->subECBD0(ctx, loc, u1, &loc[1])) == 0) {
                    if (ctx != nullptr) env->sub48950(param_1);
                    r = env->subED880(loc);
                    if (r == 0) { env->subECC90(); return 0; }
                }
            }
        }
    } else if (r == 2) {
        if (loc[0] == end) return env->subED540(param_2);
        param_2[0x5d] = 0;
        return env->errTable[r];
    }
    if (r < 0 || r > 0x1a) r = 1;
    if (unit != nullptr) {
        if (env->ordinal10331(unit) != 0) {
            int32_t* u = (int32_t*)unit;
            env->ordinal10885(env->ordinal10331_3(unit, (uint32_t)u[0], (uint32_t)u[3]));
        }
        env->subE20A0(param_1, unit);
    }
    param_2[0x5d] = 0;
    return env->errTable[r];
}
