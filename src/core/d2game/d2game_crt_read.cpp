// src/core/d2game/d2game_crt_read.cpp -- MSVC 2003 Release CRT _read (0x50ED) and __read (0x52CA),
// transcribed from the cached Ghidra decompiles build-wasm/ghidra-d2game/decomp/0x50ED.c and 0x52CA.c.
// Alpha lane (OBJECTIVE v8.3): implementation + marker only, no spec, no vector.
//
// Host model follows src/core/d2game/d2game_crt_write.cpp: the handle table is retail global 0x6FD3A220
// (_pioinfo: block pointers to ioinfo records of 0x24 bytes) and comes in through the seam, so the host's
// open/close path shares the same records. ioinfo.osfhnd is record+0, ioinfo.osfile at record+4 (bit
// 0x01 FOPEN, 0x02 FEOFLAG, 0x04 FCRLF, 0x40 FDEV, 0x80 FTEXT) and the one-byte pushback slot (pipech)
// at record+5. Callees elsewhere are seam pointers in retail argument order: 0x4E65 _errno, 0x4E6E
// _doserrno, 0x4E77 __dosmaperr, 0x871C __lseeki64_lk, plus kernel32 ReadFile/GetLastError. The SEH
// unwind is compiler injection, so no seam.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2GameCrtReadEnv {
    int32_t   (*readFile)(uint32_t handle, void* buf, uint32_t len,
                          uint32_t* read, uint32_t overlapped);   // kernel32 ReadFile
    uint32_t  (*getLastError)(void);                              // kernel32 GetLastError
    void      (*dosmaperr)(uint32_t err);                         // 0x4E77 __dosmaperr
    int64_t   (*lseeki64lk)(int32_t fd, int32_t lo, int32_t hi, int32_t origin); // 0x871C __lseeki64_lk
    int32_t*  (*errnoPtr)(void);                                  // 0x4E65 __getptd()->_terrno
    uint32_t* (*doserrnoPtr)(void);                               // 0x4E6E __getptd()->_tdoserrno
    uint8_t** pioinfo;                                            // 0x6FD3A220 _pioinfo
};

static uint32_t rd32(const uint8_t* p) {
    uint32_t v;
    std::memcpy(&v, p, 4);
    return v;
}

// 1.13c D2Game+0x050ED
// _read(fd, buf, count). count == 0 returns 0. A handle whose ioinfo flags carry FEOFLAG returns 0
// (sticky EOF). Otherwise, if FDEV (0x40) or FPIPE (0x08) is set and the pushback slot holds a byte
// other than '\n', that byte is delivered as buf[0] (count is decremented) and the slot is reset to
// '\n'. One ReadFile fills the rest. Failure: ERROR_BROKEN_PIPE (0x6D) returns 0, ERROR_INVALID_HANDLE
// (5) sets errno 9 / doserrno 5, any other error goes through __dosmaperr; all but the broken pipe
// return -1. Without FTEXT (0x80) the byte count (pushback + read) is returned as is. With FTEXT the
// buffer is translated in place: FCRLF (0x04) is set when the read ended on '\n', cleared otherwise;
// a leading ^Z (0x1A) sets FEOFLAG (0x02) when FDEV is clear and ends the copy; '\r\n' collapses to
// '\n'; a bare '\r' is kept unless it is the buffer's last byte, in which case one more byte is read --
// on a read error or EOF it is kept, with FDEV/FPIPE clear a following '\n' is pushed back on the file
// (or, when no byte was produced yet, delivered as '\n') and the '\r' dropped, otherwise a following
// non-'\n' byte is stored in the pushback slot and the '\r' kept. Returns the translated length.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_50ED(uint32_t fd, char* buf, uint32_t count,
                                               const D2GameCrtReadEnv* env) {
    if (count == 0) return 0;
    uint8_t* io = env->pioinfo[fd >> 5] + (uint32_t)(fd & 0x1F) * 0x24;
    if ((io[4] & 0x02) != 0) return 0;                       // FEOFLAG
    const uint32_t handle = rd32(io);

    char* dst = buf;
    uint32_t carried = 0;
    if (((io[4] & 0x48) != 0) && ((char)io[5] != '\n')) {
        --count;
        *buf = (char)io[5];
        dst = buf + 1;
        carried = 1;
        io[5] = '\n';
    }

    uint32_t got = 0;
    if (!env->readFile(handle, dst, count, &got, 0)) {
        const uint32_t err = env->getLastError();
        if (err == 5) {                                      // ERROR_INVALID_HANDLE
            *env->errnoPtr() = 9;                            // EBADF
            *env->doserrnoPtr() = 5;
        } else {
            if (err == 0x6D) return 0;                       // ERROR_BROKEN_PIPE
            env->dosmaperr(err);
        }
        return -1;
    }
    if ((io[4] & 0x80) == 0) return (int32_t)(carried + got);  // not FTEXT

    if (got == 0 || *buf != '\n') io[4] &= (uint8_t)~0x04;    // clear FCRLF
    else io[4] |= 0x04;                                       // set FCRLF

    char* const end = buf + carried + got;
    char* src = buf;
    dst = buf;
    while (src < end) {
        const char c = *src;
        char* next;
        if (c == '\x1A') {
            if ((io[4] & 0x40) == 0) io[4] |= 0x02;           // set FEOFLAG
            break;
        }
        if (c == '\r') {
            next = src + 1;                                  // bare '\r' path advances one
            if (src < end - 1) {
                if (src[1] == '\n') { next = src + 2; *dst++ = '\n'; src = next; continue; }
            } else {
                char extra = 0;
                uint32_t n = 0;
                next = src + 1;
                const int32_t ok = env->readFile(handle, &extra, 1, &n, 0);
                if (((ok == 0) && (env->getLastError() != 0)) || n == 0) {
                    // read failed / EOF: keep the '\r'
                } else if ((io[4] & 0x48) == 0) {
                    if (dst == buf && extra == '\n') { *dst++ = '\n'; src = next; continue; }
                    env->lseeki64lk((int32_t)fd, -1, -1, 1);  // push the byte back
                    if (extra == '\n') { src = next; continue; }  // LF stays for the next read
                    // not '\n': keep the '\r'
                } else if (extra == '\n') {
                    *dst++ = '\n'; src = next; continue;
                } else {
                    io[5] = (uint8_t)extra;                   // save pushback byte
                    *dst++ = '\r'; src = next; continue;
                }
            }
            *dst++ = '\r';
            src = next;
            continue;
        }
        *dst++ = c;
        ++src;
    }
    return (int32_t)(dst - buf);
}

// __read's seams, mirroring D2GameCrtWriteSeams in d2game_crt_write.cpp: callees that live elsewhere in
// retail argument order, plus the handle-table globals. The SEH prolog/epilog helpers (0x40E4, 0x411F)
// are compiler injection, not logic, so no seam; 0x5351 is __read's unlock thunk over 0x897B.
struct D2GameCrtReadSeams {
    void     (*lockFhandle)(int32_t);                          // 0x88DB __lock_fhandle (result unused)
    void     (*unlockFhandle)(int32_t);                        // 0x5351 __read's unlock thunk
    int32_t  (*readNoLock)(int32_t, void*, uint32_t);          // 0x50ED _read, env closed over by the host
    int32_t* (*errnoPtr)(void);                                // 0x4E65 __getptd()->_terrno
    uint32_t* (*doserrnoPtr)(void);                            // 0x4E6E __getptd()->_tdoserrno
    uint32_t  nhandle;                                         // 0x6FD3A214
    uint8_t** pioinfo;                                         // 0x6FD3A220
};

static int32_t d2game_52ca_bad(const D2GameCrtReadSeams* s) {
    *s->errnoPtr() = 9;                 // EBADF
    *s->doserrnoPtr() = 0;
    return -1;
}

// 1.13c D2Game+0x052CA
// __read(fd, buf, count): a handle outside [0, nhandle) or whose ioinfo FOPEN bit (record+4, 0x01) is
// clear sets errno 9 / doserrno 0 and returns -1. Otherwise __lock_fhandle(fd) is taken and FOPEN is
// re-tested (closed between the check and the lock takes the same error path); _read does the work and
// the unlock thunk always runs before the result (including -1) is returned.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_52CA(int32_t _FileHandle, void* _DstBuf,
                                               uint32_t _MaxCharCount,
                                               const D2GameCrtReadSeams* s) {
    const uint32_t fd = (uint32_t)_FileHandle;
    if (fd >= s->nhandle) return d2game_52ca_bad(s);
    uint8_t* osfile = s->pioinfo[fd >> 5] + 4 + (fd & 0x1f) * 0x24;
    if (!(*osfile & 1)) return d2game_52ca_bad(s);         // not FOPEN
    s->lockFhandle(_FileHandle);
    int32_t result;
    if (!(*osfile & 1)) result = d2game_52ca_bad(s);       // closed between the check and the lock
    else result = s->readNoLock(_FileHandle, _DstBuf, _MaxCharCount);
    s->unlockFhandle(_FileHandle);
    return result;
}
