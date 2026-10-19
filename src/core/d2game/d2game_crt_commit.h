#pragma once

#include <cstdint>

// Seam environment for D2Game+0x8B19 (__commit). These callbacks mirror the retail OS/CRT calls;
// the handle table pointers and count are the D2Game module state passed to the existing model.
struct D2GameCrtCommitSeams {
    int32_t  (*getOsfHandle)(int32_t);        // 0x889A __get_osfhandle
    int32_t  (*lockFhandle)(int32_t);         // 0x88DB __lock_fhandle
    void     (*unlockFhandle)(int32_t);       // 0x8BB9 __unlock_fhandle
    int32_t* (*errnoPtr)(void);               // 0x4E65 __getptd()->_terrno
    uint32_t* (*doserrnoPtr)(void);           // 0x4E6E __getptd()->_doserrno
    int32_t  (*flushFileBuffers)(int32_t);    // WinAPI FlushFileBuffers
    uint32_t (*getLastError)(void);           // WinAPI GetLastError
    uint32_t  nhandle;                        // 0x6FD3A214
    uint8_t** pioinfo;                        // 0x6FD3A220
};

extern "C" int32_t d2_fn_d2game_8B19(int32_t fileHandle, const D2GameCrtCommitSeams* seams);
