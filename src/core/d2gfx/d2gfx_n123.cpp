// D2Gfx CRT unlock path: __unlock_fhandle and its two 8-byte __finally epilogues.
#include <cstdint>
#include <Windows.h>

struct D2GfxIoInfo { uint8_t raw[0x24]; };  // layout in d2gfx_lock_fhandle.cpp; section at +0x0C
extern D2GfxIoInfo g_pioinfo[64 * 32];

// 1.13c D2Gfx+0x05D74
void d2gfx_unlock_fhandle_5d74(int32_t fh) {
    LeaveCriticalSection((LPCRITICAL_SECTION)(g_pioinfo[fh].raw + 0x0C));
}

// 1.13c D2Gfx+0x070E3
void d2gfx_sub_70e3(int32_t fh) { d2gfx_unlock_fhandle_5d74(fh); }

// 1.13c D2Gfx+0x075D4
void d2gfx_sub_75d4(int32_t fh) { d2gfx_unlock_fhandle_5d74(fh); }
