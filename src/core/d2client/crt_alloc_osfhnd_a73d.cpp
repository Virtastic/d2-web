// src/core/d2client/crt_alloc_osfhnd_a73d.cpp -- 1.13c D2Client.dll __alloc_osfhnd reimplemented
// from build-wasm/ghidra-d2client/decomp/0xA73D.c (alpha lane). This is the MSVC 2003 Release CRT
// body the linker pulled into the DLL, but its output is a descriptor number derived from the fd
// table, not host state, so it is implemented rather than filed as CRT glue.
//
// Retail table: +0x1239A0 holds 0x40 block pointers; a block is 0x20 records of 0x24 bytes:
// +0 osfhnd, +4 osfile, +5 pipech, +8 lockinitflag, +0xC CRITICAL_SECTION. +0x123990 is _nfile.
// Recovered callees are called directly; __lock_fhandle (0xA67B) is still UNMAPPED, so it is a seam.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>
#include <Windows.h>   // CRITICAL_SECTION: host (kernel) state, a no-op under wasm

extern uint32_t g_pioinfo[0x20]; // +0x1239A0, shared with the other D2Client fd-table rows
extern uint32_t g_fdcount;       // +0x123990 _nfile

struct D2ClientBatch04860Env;
struct D2ClientBatch4857Env;
struct D2ClientInitCritSecEnv;

extern "C" int32_t d2_client_47c0(int32_t locknum, const D2ClientBatch04860Env* env); // __mtinitlocknum
extern "C" void    d2_client_4860(int32_t locknum, const D2ClientBatch04860Env* env); // __lock
extern "C" void*   d2_client_7d5f(uint32_t size, const D2ClientBatch4857Env* env);    // _malloc
extern "C" int32_t d2_client_908f(void* cs, uint32_t spin, const D2ClientInitCritSecEnv* env);
extern "C" void    d2_client_47ab(uint32_t idx);                                      // _unlock

struct D2ClientAllocOsfhndEnv {
    const D2ClientBatch04860Env* lock;          // for __mtinitlocknum / __lock
    const D2ClientBatch4857Env* malloc_;        // for _malloc
    const D2ClientInitCritSecEnv* init_cs;      // for ___crtInitCritSecAndSpinCount
    int32_t (*lock_fhandle)(int32_t fd);        // 0x0A67B __lock_fhandle, still UNMAPPED
};

static inline uint8_t* fd_rec(uint32_t block, uint32_t off) {
    return (uint8_t*)(uintptr_t)(block + off);
}

// 1.13c D2Client+0x0A73D (356 B, caller 0x0A180 __tsopen_lk): find a free descriptor. Lock 11 guards
// the table; an empty block is allocated and its 0x20 records initialised (osfhnd -1, osfile 0,
// pipech '\n', lockinitflag 0) and __nfile grows by 0x20. In an existing block a record is taken when
// osfile bit 0 is clear; a record with lockinitflag 0 first initialises its critical section under
// lock 10 (a failed initialiser unwinds both locks and returns -1). The winning descriptor is
// (block << 5) + slot; the new-block path returns -1 if __lock_fhandle refuses it.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_a73d(const D2ClientAllocOsfhndEnv* env) {
    int32_t result = -1;
    if (d2_client_47c0(0xB, env->lock) == 0) return -1;
    d2_client_4860(0xB, env->lock);
    for (uint32_t i = 0; i < 0x40; ++i) {
        uint32_t block = g_pioinfo[i];
        if (block == 0) {
            void* mem = d2_client_7d5f(0x480, env->malloc_);
            if (mem != nullptr) {
                block = (uint32_t)(uintptr_t)mem;
                g_pioinfo[i] = block;
                g_fdcount += 0x20;
                for (uint32_t off = 0; off < 0x480; off += 0x24) {
                    uint8_t* rec = fd_rec(block, off);
                    *(uint32_t*)rec = 0xFFFFFFFFu;  // +0 osfhnd = -1
                    rec[4] = 0;                     // +4 osfile
                    rec[5] = 10;                    // +5 pipech = '\n'
                    *(uint32_t*)(rec + 8) = 0;      // +8 lockinitflag
                }
                result = (int32_t)(i << 5);
                if (env->lock_fhandle(result) == 0) result = -1;
            }
            break;
        }
        for (uint32_t off = 0; off < 0x480; off += 0x24) {
            uint8_t* rec = fd_rec(block, off);
            if ((rec[4] & 1) != 0) continue;        // osfile FOPEN: descriptor in use
            if (*(uint32_t*)(rec + 8) == 0) {
                d2_client_4860(10, env->lock);
                if (*(uint32_t*)(rec + 8) == 0) {
                    if (d2_client_908f(rec + 0xC, 4000, env->init_cs) == 0) {
                        // __local_unwind2(local_14, -1) runs the active finalies: unlock 10 then 11.
                        d2_client_47ab(10);
                        d2_client_47ab(0xB);
                        return -1;
                    }
                    ++*(uint32_t*)(rec + 8);
                }
                d2_client_47ab(10);                 // FUN_6faba816: _unlock(10)
            }
            EnterCriticalSection((LPCRITICAL_SECTION)(rec + 0xC));
            if ((rec[4] & 1) == 0) {
                *(uint32_t*)rec = 0xFFFFFFFFu;
                result = (int32_t)(off / 0x24 + i * 0x20);
                break;
            }
            LeaveCriticalSection((LPCRITICAL_SECTION)(rec + 0xC));
        }
        if (result != -1) break;
    }
    d2_client_47ab(0xB);                            // FUN_6faba8b0: _unlock(11)
    return result;
}
