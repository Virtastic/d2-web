// src/core/d2game/d2game_crt_alloc_osfhnd.cpp -- MSVC 2003 Release CRT __alloc_osfhnd, transcribed from
// the cached Ghidra decompile build-wasm/ghidra-d2game/decomp/0x899D.c, plus its UNMAPPED callee 0x8B10
// (the unlock(0xB) thunk, alongside its 0x8A76 sibling in d2game_crt_unlock_thunks.cpp).
//
// Host model follows src/core/d2game/d2game_batch_c2150.cpp: the handle table and its count are retail
// globals 0x6FD3A220 (_pioinfo: 64 block pointers to 32 ioinfo records of 0x24 bytes) and 0x6FD3A214
// (_nfile), host-owned and passed in through the seam so the open/close path shares the same records.
// ioinfo.osfhnd is record+0, osfile record+4 (bit 0x01 = FOPEN), pipech record+5, lockinitflag record+8
// and the embedded CRITICAL_SECTION record+0xC. Callees src/ owns are called directly; the rest are seam
// pointers in retail argument order.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include <Windows.h>      // CRITICAL_SECTION no-ops

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }

// 0x8A76 __unlock(10) and 0x8B10 __unlock(0xB) (d2game_crt_unlock_thunks.cpp).
extern "C" void d2_fn_d2game_8a76(void);
extern "C" void d2_fn_d2game_8b10(void);

// Callees the cached decompile names but this file does not implement.
struct D2GameCrtAllocOsfhndEnv {
    int32_t  (*mtInitLockNum)(int32_t lockNum);                  // 0x3F8B __mtinitlocknum
    void     (*lock)(int32_t lockNum);                           // 0x402B __lock
    int32_t  (*lockFhandle)(int32_t fileHandle);                 // 0x88DB __lock_fhandle
    int32_t  (*initCritSec)(void* cs, uint32_t spin);            // 0x7DE3 ___crtInitCritSecAndSpinCount
    void*    (*malloc)(uint32_t size);                           // 0x5F78 _malloc
    uint8_t** pioinfo;                                           // 0x6FD3A220 _pioinfo
    uint32_t* nfile;                                             // 0x6FD3A214 _nfile
};

// 1.13c D2Game+0x0899D
// __alloc_osfhnd(): find a free ioinfo slot and return its file handle, or -1. The lock table must be
// initialised first (lock 0xB). Each empty block pointer is filled with one malloc(0x480) = 32 records
// of 0x24 bytes, initialised to osfhnd = -1, osfile = 0, pipech = 10 and lockinitflag = 0; _nfile grows
// by 32 and the first handle of the block is locked. In an existing block each record that is not FOPEN
// has its embedded critical section lazily created under __lock(10) (init failure unwinds, releasing
// both locks) and entered; a record still not FOPEN after entering gets osfhnd = -1 and its handle is
// returned. The decompile's goto-skipped unlock is the __local_unwind2 finally; both unlocks are made
// explicit here.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_899d(const D2GameCrtAllocOsfhndEnv* env) {
    int32_t fh = -1;
    if (env->mtInitLockNum(0xB) == 0) return -1;
    env->lock(0xB);
    for (uint32_t i = 0; i < 0x40 && fh == -1; ++i) {
        uint8_t* pio = env->pioinfo[i];
        if (pio == nullptr) {
            pio = (uint8_t*)env->malloc(0x480);
            if (pio != nullptr) {
                env->pioinfo[i] = pio;
                *env->nfile += 0x20;
                for (uint32_t off = 0; off < 0x480; off += 0x24) {
                    uint8_t* rec = pio + off;
                    rec[4] = 0;                              // osfile
                    wr32(rec, 0, 0xFFFFFFFFu);               // osfhnd
                    rec[5] = 10;                             // pipech '\n'
                    wr32(rec, 8, 0);                         // lockinitflag
                }
                fh = (int32_t)(i << 5);
                if (env->lockFhandle(fh) == 0) fh = -1;
            }
            break;
        }
        for (uint32_t j = 0; j < 0x20; ++j) {
            uint8_t* rec = pio + j * 0x24;
            if ((rec[4] & 0x01) != 0) continue;              // FOPEN
            if (rd32(rec, 8) == 0) {
                env->lock(10);
                if (rd32(rec, 8) == 0) {
                    if (env->initCritSec(rec + 0xC, 4000) == 0) {
                        d2_fn_d2game_8a76();                 // unwind: unlock(10)
                        d2_fn_d2game_8b10();                 // finally: unlock(0xB)
                        return -1;
                    }
                    wr32(rec, 8, rd32(rec, 8) + 1);
                }
                d2_fn_d2game_8a76();
            }
            EnterCriticalSection((LPCRITICAL_SECTION)(rec + 0xC));
            if ((rec[4] & 0x01) == 0) {
                wr32(rec, 0, 0xFFFFFFFFu);
                fh = (int32_t)(i * 0x20 + j);
                break;
            }
            LeaveCriticalSection((LPCRITICAL_SECTION)(rec + 0xC));
        }
    }
    d2_fn_d2game_8b10();                                     // __finally: unlock(0xB)
    return fh;
}
