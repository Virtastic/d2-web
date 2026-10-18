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

// 1.13c D2Game+0x072B0 (strcpy): byte-copy until the source is word aligned, then word-copy while the
// 0x7EFEFEFF trick says no zero byte is inside; once one is, store the 1/2/3/4 bytes it covers and stop.
// 1.13c D2Game+0x072B0
extern "C"
EMSCRIPTEN_KEEPALIVE uint8_t* d2_fn_d2game_72b0(uint8_t* dst, const uint8_t* src) {
    uint8_t* d = dst;
    while (((uintptr_t)src & 3) != 0) {
        uint8_t b = *src++;
        if (b == 0) { *d = b; return dst; }
        *d++ = b;
    }
    for (;;) {
        uint32_t v = *(const uint32_t*)src;
        src += 4;
        if (((v ^ 0xFFFFFFFFu ^ (v + 0x7EFEFEFFu)) & 0x81010100u) != 0) {
            if ((uint8_t)v == 0) { *d = (uint8_t)v; return dst; }
            if ((uint8_t)(v >> 8) == 0) { *(uint16_t*)d = (uint16_t)v; return dst; }
            if ((v & 0x00FF0000u) == 0) { *(uint16_t*)d = (uint16_t)v; d[2] = 0; return dst; }
            if ((v & 0xFF000000u) == 0) { *(uint32_t*)d = v; return dst; }
        }
        *(uint32_t*)d = v;
        d += 4;
    }
}

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
