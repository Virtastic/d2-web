// src/core/d2game/d2game_batch_72b0.cpp -- five UNMAPPED 1.13c D2Game.dll rows transcribed from the
// cached decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c): strcpy and the four 8-byte
// __unlock_fhandle thunks __read/__write/__close/__lseek use to release their handle on the way out.
// Alpha batch lane (OBJECTIVE v8.3): implementation + marker only, no spec, no vector.
//
// Host model: the thunks receive the file handle in EBX from the enclosing CRT function (Ghidra shows it
// as unaff_EBX); it is spelled out here as the retail argument. 0x897B __unlock_fhandle is still UNMAPPED,
// so it is a seam in the env struct, after the retail argument.
#include <cstdint>
#include <emscripten.h>

struct D2GameBatch72B0Env {
    void (*unlockFHandle)(int32_t);   // 0x897B __unlock_fhandle, handle in EBX at the retail call site
};

// 1.13c D2Game+0x072B0 (strcpy) lives in src/core/game/D2Game/CRT/Strcpy72B0.cpp (the game tree calls it).

// 1.13c D2Game+0x05351 (__read's unlock thunk): __unlock_fhandle(_FileHandle), then return.
// 1.13c D2Game+0x05351
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_5351(int32_t _FileHandle, const D2GameBatch72B0Env* env) {
    env->unlockFHandle(_FileHandle);
}

// 1.13c D2Game+0x05900 (__write's unlock thunk): __unlock_fhandle(_FileHandle), then return.
// 1.13c D2Game+0x05900
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_5900(int32_t _FileHandle, const D2GameBatch72B0Env* env) {
    env->unlockFHandle(_FileHandle);
}

// 1.13c D2Game+0x05BF8 (__close's unlock thunk): __unlock_fhandle(_FileHandle), then return.
// 1.13c D2Game+0x05BF8
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_5bf8(int32_t _FileHandle, const D2GameBatch72B0Env* env) {
    env->unlockFHandle(_FileHandle);
}

// 1.13c D2Game+0x08192 (__lseek's unlock thunk): __unlock_fhandle(_FileHandle), then return.
// 1.13c D2Game+0x08192
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_8192(int32_t _FileHandle, const D2GameBatch72B0Env* env) {
    env->unlockFHandle(_FileHandle);
}
