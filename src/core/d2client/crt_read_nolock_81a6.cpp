// src/core/d2client/crt_read_nolock_81a6.cpp -- UNMAPPED 1.13c D2Client.dll row, alpha lane:
// __read_nolock 0x81A6 (477 B), transcribed from build-wasm/ghidra-d2client/decomp/0x81A6.c.
// Base 0x6FAB0000. It is the MSVC 2003 Release CRT body the DLL links; its caller is __read 0x8383,
// which does the fd validation and __lock_fhandle around it.
//
// The fd table, the per-thread errno pair and __dosmaperr are recovered elsewhere and called directly
// (d2client_batch_03f82.cpp, d2client_batch_4857.cpp, d2client_batch_b56.cpp). ReadFile, GetLastError
// and __lseeki64_lk are host seams AFTER the retail arguments, in retail call order (the
// d2client_batch_1e70.cpp pattern).
#include <cstdint>
#include <emscripten.h>

// +0x1239A0  DAT_6fbd39a0: the file-descriptor table (d2client_batch_03f82.cpp).
extern uint32_t g_pioinfo[0x20];
extern "C" int32_t* d2_client_579c(void);   // 0x0579C _errno
extern "C" int32_t* d2_client_57a5(void);   // 0x057A5 _doserrno

struct D2ClientReadNolockEnv {
    uint32_t (*read_file)(uint32_t handle, void* buf, uint32_t count, uint32_t* read,
                          uint32_t overlapped);   // IAT ReadFile
    uint32_t (*get_last_error)(void);             // IAT GetLastError
    void     (*dosmaperr)(uint32_t dos_error);    // 0x057AE __dosmaperr
    int64_t  (*lseeki64_lk)(uint32_t fd, int32_t lo, int32_t hi, uint32_t origin); // 0x0A4BC
};

// 1.13c D2Client+0x081A6
// __read_nolock(fd, buf, count): a zero count or a descriptor already at EOF (FEOFLAG, +4 bit 0x02)
// returns 0. A pushback byte (+5) that is not LF, on a pipe/device descriptor (flags 0x08/0x40), is
// copied into buf first and the slot reset to LF. ReadFile fills the rest; failure maps
// GetLastError 5 to errno 9 / doserrno 5, 0x6D (broken pipe) to 0, anything else through __dosmaperr
// and -1. Binary descriptors (FTEXT 0x80 clear) return the total produced. Text descriptors set
// FCRLF (0x04) when the first buffer byte is LF, translate CRLF to LF, keep lone CR, collapse a CR
// that ends the block with a one-byte lookahead (seeked back unless pipe/device, whose byte goes to
// the pushback slot) and stop at 0x1A, which sets FEOFLAG unless FDEV (0x40) is set.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_81a6(uint32_t fd, char* buf, uint32_t count,
                                            const D2ClientReadNolockEnv* env) {
    if (count == 0) return 0;
    uint8_t* rec = (uint8_t*)(uintptr_t)(g_pioinfo[fd >> 5] + (fd & 0x1F) * 0x24);
    uint8_t* flags = rec + 4;
    if ((*flags & 0x02) != 0) return 0;                       // FEOFLAG

    char* out = buf;        // pcVar9
    uint32_t produced = 0;  // local_c (pushback byte)
    if ((*flags & 0x48) != 0 && (char)rec[5] != '\n') {       // FPIPE|FDEV pushback
        --count;
        *buf = (char)rec[5];
        out = buf + 1;
        produced = 1;
        rec[5] = '\n';
    }

    uint32_t read = 0;      // local_10
    if (env->read_file(*(uint32_t*)rec, out, count, &read, 0) == 0) {
        const uint32_t err = env->get_last_error();
        if (err == 5) {
            *d2_client_579c() = 9;
            *d2_client_57a5() = 5;
        } else if (err == 0x6D) {
            return 0;
        } else {
            env->dosmaperr(err);
        }
        return -1;
    }
    if ((*flags & 0x80) == 0) return (int32_t)(produced + read);   // binary
    if (read == 0 || *buf != '\n') *flags &= 0xFB;                 // clear FCRLF
    else                           *flags |= 0x04;                 // set FCRLF

    char* end = buf + produced + read;   // local_c
    char* src = buf;                     // param_3
    out = buf;                           // pcVar9
    while (src < end) {
        const char c = *src;
        char* next = nullptr;            // pcVar2
        if (c == '\x1A') {
            if ((*flags & 0x40) == 0) *flags |= 0x02;
            break;
        }
        if (c == '\r') {
            if (src < end - 1) {
                if (src[1] == '\n') { next = src + 2; *out++ = '\n'; goto tail; }
                ++src;
                *out = '\r';
            } else {
                next = src + 1;
                char look = 0;
                uint32_t got = 0;
                const uint32_t ok = env->read_file(*(uint32_t*)rec, &look, 1, &got, 0);
                const uint32_t err = ok ? 0 : env->get_last_error();
                if ((ok == 0 && err != 0) || got == 0) {
                    ++src;
                    *out = '\r';                                   // lone trailing CR
                } else if ((*flags & 0x48) == 0) {
                    if (out == buf && look == '\n') { *out++ = '\n'; goto tail; }
                    env->lseeki64_lk(fd, -1, -1, 1);
                    if (look == '\n') goto tail;                   // CR dropped, LF re-read next call
                    ++src;
                    *out = '\r';
                } else if (look == '\n') {
                    *out = '\n';
                } else {
                    *out = '\r';
                    rec[5] = (uint8_t)look;
                    src = next;
                }
            }
            ++out;
            if (next == nullptr) next = src;
        } else {
            *out++ = c;
            next = src + 1;
        }
    tail:
        src = next;
    }
    return (int32_t)(out - buf);
}
