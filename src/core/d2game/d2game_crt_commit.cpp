// src/core/d2game/d2game_crt_commit.cpp -- MSVC 2003 Release CRT __commit, transcribed from the cached
// Ghidra decompile build-wasm/ghidra-d2game/decomp/0x8B19.c. Alpha lane (OBJECTIVE v8.3): implementation
// + marker only, no spec, no vector.
//
// Host model: as d2game_crt_write.cpp (0x5879), the handle table/count are the host-owned retail globals
// 0x6FD3A220 (_pioinfo) and 0x6FD3A214 (nhandle), and the callees come in as seam pointers in retail
// argument order. FlushFileBuffers/GetLastError are host-stdio WinAPI seams. The SEH prolog/epilog
// helpers (0x40E4, 0x411F) are compiler injection, not logic, so no seam.
#include <cstdint>
#include <emscripten.h>
#include "d2game_crt_commit.h"

// 1.13c D2Game+0x08B19
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_8B19(int32_t _FileHandle, const D2GameCrtCommitSeams* s) {
    const uint32_t fd = (uint32_t)_FileHandle;
    uint32_t error;
    if (fd >= s->nhandle) goto bad;                          // EBADF, errno only
    if (!(s->pioinfo[fd >> 5][4 + (fd & 0x1f) * 0x24] & 1)) goto bad;   // not FOPEN
    s->lockFhandle(_FileHandle);
    if (s->pioinfo[fd >> 5][4 + (fd & 0x1f) * 0x24] & 1) {  // still open under the lock
        error = s->flushFileBuffers(s->getOsfHandle(_FileHandle)) ? 0 : s->getLastError();
        if (error == 0) goto done;
        *s->doserrnoPtr() = error;
    }
    *s->errnoPtr() = 9;                                     // EBADF
    error = 0xFFFFFFFF;
done:
    s->unlockFhandle(_FileHandle);
    return (int32_t)error;
bad:
    *s->errnoPtr() = 9;
    return -1;
}
