// 1.13c D2Gfx+0x03D16
// __write_lk, the unlocked tail of MSVC 2003's __write (caller 0x3ED2 validates the handle and locks):
// `int __cdecl __write_lk(int fh, const void* buf, unsigned count)`. fh indexes the CRT's _pioinfo block
// table: block pi[fh>>5], entry fh&0x1f, stride 0x24. The osfile bits read here are FAPPEND 0x20 (seek the
// handle to EOF first), FDEV 0x40 and FTEXT 0x80. Binary mode hands the whole buffer to WriteFile in one
// call. Text mode copies at most one 0x400-byte chunk at a time, expanding every '\n' to '\r' '\n' (the
// buffer can therefore hold 1025 bytes), and keeps going while the OS took the whole chunk and input
// remains. The return is the bytes written minus the inserted CRs, so a successful text write returns
// `count`. With nothing written: a Win32 error maps (ERROR_ACCESS_DENIED -> errno EBADF/doserrno 5,
// anything else through __dosmaperr), and no error means Ctrl-Z on a device file returns 0, else errno
// EINVAL (28). The caller 0x3ED2 passes the write result straight through, so -1 here leaves errno alone
// only on the __dosmaperr path, as in retail.
// ponytail: our build has no Win32 handle table and no WriteFile, so the pinned _pioinfo table, the
// WriteFile sink with GetLastError, __dosmaperr and the append seek come in as arguments (the same pinned
// model 0x3ED2 uses); errno/doserrno are this directory's getters.
#include <emscripten.h>
#include <cstddef>
#include <cstdint>

// MSVC 2003 Release CRT ioinfo, 0x24 bytes per handle.
struct D2GfxIoInfo {
    uint32_t osfhnd;     // +0x00
    uint8_t  osfile;     // +0x04
    uint8_t  pipech;     // +0x05
    uint8_t  pad[0x1E];  // +0x06 .. +0x23
};
static_assert(offsetof(D2GfxIoInfo, osfile) == 0x04, "osfile byte read at entry+4");
static_assert(sizeof(D2GfxIoInfo) == 0x24, "entry stride is 0x24");

// 0x3858 / 0x3861, defined in src/core/d2gfx/d2gfx_crt_errno.cpp.
extern "C" int* d2gfx_errno(void);
extern "C" uint32_t* d2gfx_doserrno(void);

typedef void (*D2GfxSeekEndFn)(uint32_t fh);                                           // __lseeki64_lk(fh, 0, SEEK_END)
typedef uint32_t (*D2GfxWriteFileFn)(uintptr_t handle, const void* buf, uint32_t count,
                                     uint32_t* written);                               // WriteFile
typedef uint32_t (*D2GfxLastErrorFn)(void);                                            // GetLastError
typedef void (*D2GfxDosMapErrFn)(uint32_t winerr);                                     // __dosmaperr

struct D2GfxWriteLkFns {
    D2GfxSeekEndFn seek_end;
    D2GfxWriteFileFn write_file;
    D2GfxLastErrorFn last_error;
    D2GfxDosMapErrFn dosmaperr;
};

extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2gfx_3d16(int32_t fh, const void* buf, uint32_t count,
                                              const uint32_t* pi, const D2GfxWriteLkFns* fn) {
    if (count == 0) return 0;
    const D2GfxIoInfo* io = (const D2GfxIoInfo*)(uintptr_t)pi[(uint32_t)fh >> 5];
    const D2GfxIoInfo* e = &io[(uint32_t)fh & 0x1f];
    if ((e->osfile & 0x20) != 0) fn->seek_end((uint32_t)fh);

    const uint8_t* p = (const uint8_t*)buf;
    uint32_t total = 0, lasterr;
    int32_t newlines = 0;
    if ((e->osfile & 0x80) == 0) {
        uint32_t written = 0;
        if (fn->write_file(e->osfhnd, buf, count, &written) == 0) {
            lasterr = fn->last_error();
        } else {
            lasterr = 0;
            total = written;
        }
    } else {
        uint8_t chunk[1025];  // 0x400 bytes, plus one when a '\n' lands at out 1023
        uint32_t in = 0;
        lasterr = 0;
        uint32_t out, written;
        do {
            out = 0; written = 0;
            while (in < count) {
                const uint8_t c = p[in++];
                if (c == '\n') { ++newlines; chunk[out++] = '\r'; }
                chunk[out++] = c;
                if (out >= 0x400) break;
            }
            if (fn->write_file(e->osfhnd, chunk, out, &written) == 0) {
                lasterr = fn->last_error();
                break;
            }
            total += written;
        } while (out <= written && in < count);
    }

    if (total != 0) return (int32_t)(total - (uint32_t)newlines);
    if (lasterr != 0) {
        if (lasterr == 5) {                       // ERROR_ACCESS_DENIED
            *d2gfx_errno() = 9;                   // EBADF
            *d2gfx_doserrno() = 5;
            return -1;
        }
        fn->dosmaperr(lasterr);
        return -1;
    }
    if ((e->osfile & 0x40) != 0 && p[0] == 0x1A) return 0;  // Ctrl-Z on a device
    *d2gfx_errno() = 0x1c;                        // EINVAL
    *d2gfx_doserrno() = 0;
    return -1;
}
