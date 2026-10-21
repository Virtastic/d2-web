// src/core/d2game/d2game_34ea0.cpp -- 1.13c D2Game.dll function transcribed from the cached Ghidra
// decompile (build-wasm/ghidra-d2game/decomp/0x34EA0.c). Alpha lane: implementation + marker only.
// Records are opaque host objects addressed by retail byte offsets; register-only arguments
// (this/ECX, unaff_EDI) are explicit. Callees are host seams in retail argument order.
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

struct D2Game34ea0Env {
    uint32_t (*checkMonStatsFlag)(void* self, uint32_t flag);   // 0x15D0
    void*    (*ord10920)();                                     // Fog #10920 (called with no args)
    char     (*getAct)();                                       // 0x49DB0
    int      (*isBadCodePtr)(void* fn);                         // kernel32 IsBadCodePtr
    void     (*sub8a770)();                                     // 0x8A770
};

// 1.13c D2Game+0x34EA0
// Walks the act-callback list at (*(param_1+0x10F4))[0], linked through +0xF4, for the node whose
// act byte (+8) matches the current act, and calls its handler (+0xEC). Every malformed-state
// branch is the Fog #10265/#10024 fatal then _exit(-1).
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_fn_d2game_34ea0(void* self, uint8_t* param_1, const int32_t* param_2,
                            const int32_t* unaffEdi, const D2Game34ea0Env* env)
{
    if (unaffEdi == nullptr || *unaffEdi != 0) _exit(-1);          // 0x7EC
    if (param_2 == nullptr || *param_2 != 1) _exit(-1);            // 0x7ED
    if (env->checkMonStatsFlag(self, 9) == 0) return 0;
    uint8_t* list; std::memcpy(&list, param_1 + 0x10F4, sizeof list);
    uint32_t cnt; std::memcpy(&cnt, list + 8, 4);
    if (cnt == 0) _exit(-1);                                       // 0x7F4
    env->ord10920();
    char act = env->getAct();
    uint8_t* node; std::memcpy(&node, list, sizeof node);
    for (;;) {
        if (node == nullptr) return 0;
        if (act == (char)node[8]) {
            void* fn; std::memcpy(&fn, node + 0xEC, sizeof fn);
            if (fn == nullptr) _exit(-1);                          // 0x813
            if (env->isBadCodePtr(fn)) _exit(-1);                  // 0x802
            if (env->ord10920() == nullptr) _exit(-1);             // 0x803
            uint8_t* tbl = (uint8_t*)env->ord10920();
            uint32_t ent; std::memcpy(&ent, tbl + 0x10 + (uint32_t)param_1[0x6D] * 4, 4);
            if (ent == 0) _exit(-1);                               // 0x805
            if (((char (*)())fn)() != 0) {
                if (*unaffEdi == 0) env->ord10920();
                env->sub8a770();
                return 1;
            }
        }
        std::memcpy(&node, node + 0xF4, sizeof node);
    }
}
