// src/core/d2game/d2game_batch_5c47.cpp -- UNMAPPED 1.13c D2Game.dll row 0x05C47 (terminate),
// transcribed from the cached decompile (build-wasm/ghidra-d2game/decomp/0x5C47.c) and the retail
// bytes. Alpha implementation lane (OBJECTIVE v8.3): implementation + marker only, no spec, no vector.
//
// MSVC 2003 (VC7.1) Release CRT terminate linked into the DLL. Host model follows
// src/core/d2game/d2game_batch_crt_b56.cpp: 0x3B93 __getptd is a seam, and 0x96F8 _abort (ours, in
// d2game_batch_crt_tiny_6730.cpp) is called through its extern "C" definition with its environment.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static inline uint32_t rd32(const void* p, uint32_t o) {
    uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v;
}

extern "C" void d2_fn_d2game_96f8(const void* env);   // 0x96F8 _abort

// The callees src/ does not own here arrive as seams in retail order: the thread-data getter, and the
// environment 0x96F8 needs for __NMSG_WRITE (its own D2GameBatchTinyEnv, opaque to this file).
struct D2GameBatch5c47Env {
    void*       (*getptd)(void);                            // 0x3B93 __getptd
    void        (*invokeCallback)(uint32_t retailAddress); // map/call retail callback address
    const void*  abortEnv;                                  // 0x96F8 environment
};

// 1.13c D2Game+0x05C47 (terminate): the thread block's terminator pointer at _ptiddata+0x6C, when
// non-null, is called with no arguments; then _abort(). The block is fetched twice, as the retail code
// does (call reads the field, the second call reloads before the indirect call).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_5C47(const D2GameBatch5c47Env* env) {
    void* ptd = env->getptd();
    if (rd32(ptd, 0x6C) != 0) {
        ptd = env->getptd();
        const uint32_t terminatorAddress = rd32(ptd, 0x6C);
        env->invokeCallback(terminatorAddress);
    }
    d2_fn_d2game_96f8(env->abortEnv);
}
