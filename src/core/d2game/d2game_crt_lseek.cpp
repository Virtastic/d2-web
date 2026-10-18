// src/core/d2game/d2game_crt_lseek.cpp -- 1.13c D2Game.dll row 0x0810B __lseek, from the cached decompile
// (build-wasm/ghidra-d2game/decomp/0x810B.c). Alpha batch lane (OBJECTIVE v8.3): implementation + marker
// only, no spec, no vector.
//
// Host model follows src/core/d2game/d2game_batch_3e70.cpp: retail's module globals are the statics below
// at their retail RVA; callees whose own rows are still UNMAPPED are seams on a caller-supplied struct in
// retail argument order. The two __SEH_prolog/__SEH_epilog callees (0x40E4 / 0x411F) bracket the body
// only so the finally unlocks the handle; the model calls the unlock directly on both paths out.
#include <cstdint>
#include <emscripten.h>

// The callees this row's decompile calls and whose rows are not mapped yet (RVA = retail address).
struct D2GameCrtLseekEnv {
    void      (*lockFHandle)(int32_t);                    // 0x88DB __lock_fhandle
    void      (*unlockFHandle)(int32_t);                  // 0x8192 __unlock_fhandle wrapper (calls 0x897B)
    int32_t   (*lseekLk)(int32_t, int32_t, int32_t);      // 0x8097 __lseek_lk
    int32_t*  (*errnoPtr)(void);                          // 0x4E65 __getptd()->_terrno
    uint32_t* (*doserrnoPtr)(void);                       // 0x4E6E __getptd()->_tdoserrno
};

// 0x6FD3A214 _nfile: number of valid CRT handles; __ioinit (0x60EF) seeds it at startup. Only the alpha
// host seed (the MSVC 2003 CRT's initial 64-handle table) is fixed here; the real value is module state.
static uint32_t g_nfile = 64;
// 0x6FD3A220 _pioinfo: 64 block pointers, each 32 ioinfo records of 0x24 bytes, osfile at record +4
// (bit 0 = FOPEN). The blocks are contiguous, so it is modeled flat: block[fh>>5][fh&0x1f] == flat[fh].
static uint8_t g_ioinfo[0x400][0x24];

// 1.13c D2Game+0x0810B (__lseek): long __cdecl _lseek(int fh, long offset, int origin). An out-of-range
// handle (unsigned compare against _nfile) or a closed one (osfile FOPEN clear) sets errno EBADF (9) and
// _doserrno 0 and returns -1 without locking. Otherwise the handle is locked, FOPEN re-tested, and either
// the same EBADF pair -> -1 or __lseek_lk(fh, offset, origin) is taken; the unlock runs on both paths.
// A -1 from __lseek_lk passes through without touching the error cells.
// 1.13c D2Game+0x0810B
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_810b(int32_t fileHandle, int32_t offset, int32_t origin,
                                               const D2GameCrtLseekEnv* env) {
    const uint32_t fh = (uint32_t)fileHandle;          // `cmp [nfile], eax; jae` is unsigned
    if (fh < g_nfile) {
        uint8_t* io = g_ioinfo[fh];
        if ((io[4] & 0x01) != 0) {                     // FOPEN
            env->lockFHandle(fileHandle);
            int32_t result;
            if ((io[4] & 0x01) == 0) {                 // re-loaded under the lock
                *env->errnoPtr() = 9;                  // EBADF
                *env->doserrnoPtr() = 0;
                result = -1;
            } else {
                result = env->lseekLk(fileHandle, offset, origin);
            }
            env->unlockFHandle(fileHandle);
            return result;
        }
    }
    *env->errnoPtr() = 9;
    *env->doserrnoPtr() = 0;
    return -1;
}
