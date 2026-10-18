// src/core/d2gfx/d2gfx_crt_helpers.cpp -- 1.13c D2Gfx CRT stdio leaves, reimplemented from the cached
// decompile (ghidra-d2gfx/decomp/0x3BF7.c). Alpha batch lane (OBJECTIVE v8.3): implementation + marker
// only, no spec, no vector. Host model follows src/core/d2game/d2game_batch_3e70.cpp: callees whose own
// rows are still UNMAPPED are seams on a caller-supplied struct in retail argument order; module globals
// are statics at the retail RVA named in each comment.
#include <cstdint>

// 0x3858 __errno and 0x3861 _doserrno, implemented in d2gfx_crt_errno.cpp (same module).
extern "C" int* d2gfx_errno(void);
extern "C" uint32_t* d2gfx_doserrno(void);

// 0x1E90C _nfile: the number of valid CRT handles; __ioinit (0x2D1C) seeds it with the CRT's initial
// 64-entry table at startup. Only the alpha host seed is fixed here; the real value is module state.
static uint32_t g_nfile = 64;
// 0x1E920 _pioinfo: 64 block pointers, each block 32 ioinfo records of 0x24 bytes. Modeled as the 64
// blocks themselves (the pointers are the block bases); osfile is +4 in a record.
static uint8_t g_ioinfo[64][32 * 0x24];

// The callees this row's own decompile calls and whose rows are not ours yet:
// 0x5C93 __get_osfhandle, the two kernel32 imports SetFilePointer / GetLastError reached through IAT
// slots (the negative RVAs 0x-6FA7FFBF / 0x-6FA7FFC0 in the export), and 0x386A __dosmaperr.
struct D2GfxLseekLkHost {
    uint32_t (*get_osfhandle)(int32_t fh);
    uint32_t (*set_file_pointer)(uint32_t handle, int32_t offset, int32_t* distance_high, uint32_t origin);
    uint32_t (*get_last_error)(void);
    void     (*dosmaperr)(uint32_t err);
};

// 1.13c D2Gfx+0x03BF7 __lseek_lk (116 bytes): DWORD __cdecl __lseek_lk(uint fh, LONG offset, DWORD origin).
// hFile = __get_osfhandle(fh); INVALID_HANDLE_VALUE sets errno = EBADF (9) and returns -1, with no
// _doserrno write and no pioinfo access. Otherwise SetFilePointer(hFile, offset, NULL, origin): on
// success (or on INVALID_SET_FILE_POINTER with GetLastError() == NO_ERROR) the FEOFLAG bit (bit 1) of the
// handle's ioinfo osfile byte is cleared and the result is returned; any other error goes to __dosmaperr,
// which owns errno/_doserrno, and the result is -1.
// 1.13c D2Gfx+0x03BF7
extern "C"
int32_t d2gfx_lseek_lk(int32_t fh, int32_t offset, int32_t origin, const D2GfxLseekLkHost* host) {
    uint32_t handle = host->get_osfhandle(fh);
    if (handle == 0xFFFFFFFFu) {
        *d2gfx_errno() = 9;                                  // EBADF
        return -1;
    }
    int32_t result = (int32_t)host->set_file_pointer(handle, offset, nullptr, origin);
    uint32_t err = (result == -1) ? host->get_last_error() : 0;
    if (err == 0) {
        uint8_t* osfile = &g_ioinfo[(uint32_t)fh >> 5][((uint32_t)fh & 0x1F) * 0x24 + 4];
        *osfile &= 0xFD;                                          // clear FEOFLAG
    } else {
        host->dosmaperr(err);
        result = -1;
    }
    return result;
}

// 1.13c D2Gfx+0x05D96 __lseeki64_lk (131 bytes): long long __cdecl __lseeki64_lk(uint fh, LONG low,
// LONG high, DWORD origin), the 64-bit sibling of 0x3BF7 and the tail of __lseeki64 (caller 0x3D16 seeks
// to EOF through it). hFile = __get_osfhandle(fh); INVALID_HANDLE_VALUE sets errno = EBADF (9) and returns
// -1. Otherwise SetFilePointer(hFile, low, &high, origin), where `high` is in/out; a returned low of
// INVALID_SET_FILE_POINTER with GetLastError() != 0 goes through __dosmaperr and returns -1, while a
// clean result (including that sentinel with NO_ERROR) clears FEOFLAG (bit 1) of the handle's ioinfo
// osfile byte and returns ((int64)high << 32) | low.
// 1.13c D2Gfx+0x05D96
extern "C"
int64_t d2gfx_lseeki64_lk(uint32_t fh, int32_t offset_low, int32_t offset_high, uint32_t origin,
                          const D2GfxLseekLkHost* host) {
    uint32_t handle = host->get_osfhandle((int32_t)fh);
    if (handle == 0xFFFFFFFFu) {
        *d2gfx_errno() = 9;                                  // EBADF
        return -1;
    }
    int32_t high = offset_high;
    uint32_t low = host->set_file_pointer(handle, offset_low, &high, origin);
    uint32_t err = (low == 0xFFFFFFFFu) ? host->get_last_error() : 0;
    if (err == 0) {
        uint8_t* osfile = &g_ioinfo[fh >> 5][(fh & 0x1F) * 0x24 + 4];
        *osfile &= 0xFD;                                     // clear FEOFLAG
        return ((int64_t)high << 32) | (int64_t)(uint64_t)low;
    }
    host->dosmaperr(err);
    return -1;
}

// 1.13c D2Gfx+0x05C93 __get_osfhandle (65 bytes): intptr_t __cdecl __get_osfhandle(int fh). An
// out-of-range handle (`cmp [nfile], eax; jae` -- unsigned, so a negative int is out of range) or a
// closed one (ioinfo osfile FOPEN bit clear) sets errno = EBADF (9) and _doserrno = 0 and returns -1;
// the block table is not indexed on that path. Otherwise it returns the handle's osfhnd dword (record
// offset 0). No locking, no other state touched.
// 1.13c D2Gfx+0x05C93
extern "C"
int32_t d2gfx_get_osfhandle(int32_t fh) {
    const uint32_t ufh = (uint32_t)fh;
    if (ufh < g_nfile) {
        uint8_t* io = &g_ioinfo[ufh >> 5][(ufh & 0x1F) * 0x24];
        if ((io[4] & 0x01) != 0) return *(int32_t*)io;       // osfhnd
    }
    *d2gfx_errno() = 9;                                      // EBADF
    *d2gfx_doserrno() = 0;
    return -1;
}
