// src/core/d2game/d2game_inconsistency_5c7c.cpp -- 1.13c D2Game+0x05C7C (_inconsistency).
// Host-side seams model the process-global CRT terminate handler and the following CRT terminate
// path. This is noreturn in retail; FUNCTION-mode capture needs an isolated process to record it.
#include <cstdint>
#include <cstdlib>
#include <emscripten.h>

struct D2GameBatch5c47Env;
extern "C" void d2_fn_d2game_5C47(const D2GameBatch5c47Env* env);

struct D2GameInconsistency5c7cEnv {
    uint32_t globalTerminateHandler;               // value of retail PTR_terminate_6fd1f720
    void (*invokeCallback)(uint32_t callback);     // invokes a retail-address callback with no args
    const D2GameBatch5c47Env* terminateEnv;        // environment for D2Game+0x05C47 (terminate)
};

// 1.13c D2Game+0x05C7C (_inconsistency): call the optional process-global handler, then CRT
// terminate (+0x05C47), which performs the per-thread handler path and _abort. Neither call returns.
extern "C" EMSCRIPTEN_KEEPALIVE [[noreturn]] void d2_fn_d2game_5C7C(const D2GameInconsistency5c7cEnv* env) {
    if (env->globalTerminateHandler != 0) {
        env->invokeCallback(env->globalTerminateHandler);
    }
    d2_fn_d2game_5C47(env->terminateEnv);
    std::abort(); // Preserve retail's non-returning contract if a host seam unexpectedly returns.
}
