// src/core/d2game/d2game_crt_write.cpp -- MSVC 2003 Release CRT __write, transcribed from the cached
// Ghidra decompile build-wasm/ghidra-d2game/decomp/0x5879.c. Alpha lane (OBJECTIVE v8.3): implementation
// + marker only, no spec, no vector.
//
// Host model: the handle table and its count are retail globals 0x6FD3A220 (_pioinfo: 32 block pointers
// to 32 ioinfo records of 0x24 bytes) and 0x6FD3A214 (nhandle); they are host-owned and come in through
// the seams, so the host's open/close path shares the same records. ioinfo.osfile is record+4, FOPEN is
// bit 0x01. Callees that live elsewhere are seam pointers in retail argument order; a Ghidra call whose
// argument was passed in a register (0x5900) gets the explicit parameter here. The SEH prolog/epilog
// helpers the comments list (0x88DB, 0x411F, 0x40E4) are compiler injection, not logic, so no seam.
#include <cstdint>
#include <emscripten.h>

struct D2GameCrtWriteSeams {
    void     (*lockFhandle)(int32_t);                          // 0x88DB __lock_fhandle
    void     (*unlockFhandle)(int32_t);                        // 0x5900 _unlock_fhandle
    int32_t  (*writeNoLock)(int32_t, const void*, uint32_t);   // 0x56BD __write_nolock
    int32_t* (*errnoPtr)(void);                                // 0x4E65 __getptd()->_terrno
    uint32_t* (*doserrnoPtr)(void);                            // 0x4E6E __getptd()->_doserrno
    uint32_t  nhandle;                                         // 0x6FD3A214
    uint8_t** pioinfo;                                         // 0x6FD3A220
};

static int32_t d2game_5879_bad(const D2GameCrtWriteSeams* s) {
    *s->errnoPtr() = 9;                 // EBADF
    *s->doserrnoPtr() = 0;
    return -1;
}

// 1.13c D2Game+0x05879
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_5879(int32_t _FileHandle, void* _Buf, uint32_t _MaxCharCount,
                                               const D2GameCrtWriteSeams* s) {
    const uint32_t fd = (uint32_t)_FileHandle;
    int32_t result;
    if (fd >= s->nhandle) return d2game_5879_bad(s);
    uint8_t* osfile = s->pioinfo[fd >> 5] + 4 + (fd & 0x1f) * 0x24;
    if (!(*osfile & 1)) return d2game_5879_bad(s);       // not FOPEN
    s->lockFhandle(_FileHandle);
    if (!(*osfile & 1)) result = d2game_5879_bad(s);     // closed between the check and the lock
    else result = s->writeNoLock(_FileHandle, _Buf, _MaxCharCount);
    s->unlockFhandle(_FileHandle);
    return result;
}
