// src/core/d2game/d2game_crt_unlock_thunks.cpp -- six 1.13c D2Game.dll CRT lock thunks from their cached
// decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Alpha batch lane (OBJECTIVE v8.3):
// implementation + marker only, no spec, no vector.
//
// 0x60DE, 0x5E11, 0x5EC8 and 0x5F43 are `unlock(4)`; 0x7DAC is `unlock(0xD)`. They tail into the mapped
// 0x3F76 __unlock (src/core/d2game/d2game_batch_3e70.cpp), so they call it directly. 0x8BB9 is the
// __unlock_fhandle wrapper: the file handle's ioinfo critical section is released; 0x897B (UNMAPPED) is a
// seam. Retail argument registers (EBX for 0x8BB9) are explicit parameters.
#include <cstdint>
#include <emscripten.h>

// 0x3F76 __unlock(lockNum) -- LeaveCriticalSection on CRT lock table slot lockNum.
extern "C" void d2game_unlock_3f76(int32_t lockNum);

struct D2GameCrtUnlockThunksEnv {
    void (*unlockFhandle)(int32_t fileHandle);          // 0x897B __unlock_fhandle
};

// 1.13c D2Game+0x060DE
extern "C"
void d2game_thunk_60de(void) { d2game_unlock_3f76(4); }

// 1.13c D2Game+0x05E11
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_5e11(void) { d2game_unlock_3f76(4); }

// 1.13c D2Game+0x05EC8
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_5ec8(void) { d2game_unlock_3f76(4); }

// 1.13c D2Game+0x05F43
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_5f43(void) { d2game_unlock_3f76(4); }

// 1.13c D2Game+0x07DAC
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_7dac(void) { d2game_unlock_3f76(0xD); }

// 1.13c D2Game+0x08BB9
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_8bb9(int32_t fileHandle, const D2GameCrtUnlockThunksEnv* env) {
    env->unlockFhandle(fileHandle);
}

// 1.13c D2Game+0x08A76
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_8a76(void) { d2game_unlock_3f76(10); }

// 1.13c D2Game+0x08B10 (unlock(0xB), the finally of __alloc_osfhnd 0x899D)
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_8b10(void) { d2game_unlock_3f76(0xB); }
