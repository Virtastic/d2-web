// src/core/d2game/d2game_crt_free_osfhnd.cpp -- MSVC 2003 Release CRT __free_osfhnd, transcribed from the
// cached Ghidra decompile build-wasm/ghidra-d2game/decomp/0x881B.c. Alpha lane (OBJECTIVE v8.3):
// implementation + marker only, no spec, no vector.
//
// Host model follows src/core/d2game/d2game_crt_write.cpp: the handle table and its count are retail
// globals 0x6FD3A220 (_pioinfo: 32 block pointers to 32 ioinfo records of 0x24 bytes) and 0x6FD3A214
// (_nfile); they are host-owned and come in through the seam, so the host's open/close path shares the
// same records. ioinfo.osfhnd is record+0, ioinfo.osfile at record+4 (bit 0x01 = FOPEN). 0x6FD2FD78 is
// _osplatform (1 = NT, the only value retail sets); it gates the SetStdHandle clears. The two callees
// that return the thread error cells (0x4E65 _errno, 0x4E6E _doserrno) are seams here, as in __write.
#include <cstdint>
#include <emscripten.h>

struct D2GameCrtFreeOsfhndEnv {
    int32_t*  (*errnoPtr)(void);                               // 0x4E65 __getptd()->_terrno
    uint32_t* (*doserrnoPtr)(void);                            // 0x4E6E __getptd()->_tdoserrno
    int32_t   (*setStdHandle)(uint32_t, void*);                // kernel32 SetStdHandle
    uint32_t  osplatform;                                      // 0x6FD2FD78 _osplatform
    uint32_t  nhandle;                                         // 0x6FD3A214 _nfile
    uint8_t** pioinfo;                                         // 0x6FD3A220 _pioinfo
};

// 1.13c D2Game+0x0881b
// __free_osfhnd(fh): a handle below _nfile whose ioinfo record has FOPEN set and a live OS handle clears
// that OS handle to -1 and returns 0. On an NT platform (_osplatform == 1) fds 0, 1 and 2 additionally
// clear the matching standard handle (STD_INPUT/OUTPUT/ERROR_HANDLE, -10/-11/-12) first. Anything else
// -- out of range, not FOPEN, or an already -1 handle -- sets errno EBADF (9) and _doserrno 0 and
// returns -1.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_881B(int32_t fileHandle, const D2GameCrtFreeOsfhndEnv* s) {
    const uint32_t fd = (uint32_t)fileHandle;
    if (fd < s->nhandle) {
        uint8_t* io = s->pioinfo[fd >> 5] + (fd & 0x1f) * 0x24;
        int32_t osfhnd;
        __builtin_memcpy(&osfhnd, io, 4);
        if ((io[4] & 0x01) != 0 && osfhnd != -1) {            // FOPEN and a live OS handle
            if (s->osplatform == 1) {
                uint32_t nStdHandle = 0;
                if (fileHandle == 0) nStdHandle = 0xFFFFFFF6u;      // STD_INPUT_HANDLE
                else if (fileHandle == 1) nStdHandle = 0xFFFFFFF5u; // STD_OUTPUT_HANDLE
                else if (fileHandle == 2) nStdHandle = 0xFFFFFFF4u; // STD_ERROR_HANDLE
                if (nStdHandle != 0) s->setStdHandle(nStdHandle, nullptr);
            }
            const uint32_t closed = 0xFFFFFFFFu;
            __builtin_memcpy(io, &closed, 4);
            return 0;
        }
    }
    *s->errnoPtr() = 9;                 // EBADF
    *s->doserrnoPtr() = 0;
    return -1;
}
