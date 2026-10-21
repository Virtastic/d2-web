#include <cstdint>

// 1.13c D2Common+0x08F13 _read_lk(fh, buf, n): MSVC CRT low-level read. ioinfo entry (0x24 bytes):
// +0 OS handle, +4 flags (0x02 EOF, 0x04 pending-LF, 0x08/0x40 pipe/device, 0x80 text mode), +5 peek char.
// Binary mode returns the raw count; text mode folds CRLF to LF, stops at Ctrl-Z (sets EOF unless
// 0x40), and peeks one byte after a trailing CR (seek back on files, else stash in +5).
// Env seam: ReadFile/GetLastError/errno/lseek are owner-supplied.
struct D2ReadEnv {
    uint8_t* (*ioinfo)(uint32_t fh);                                  // &DAT_6fdf1720[fh>>5] + (fh&31)*0x24
    int (*readFile)(void* h, void* buf, uint32_t n, uint32_t* got);   // 0 on failure
    uint32_t (*getLastError)();
    void (*setErrnoDos)(int err, uint32_t dos);                       // FUN_6fd544bc/c5 pair
    void (*dosMapErr)(uint32_t);                                      // __dosmaperr
    void (*seekBack1)(uint32_t fh);                                   // __lseeki64_lk(fh,-1,-1,1)
};

int d2common_read_lk_8f13(D2ReadEnv& e, uint32_t fh, char* buf, uint32_t n, void* (*handleOf)(uint8_t*))
{
    if (!n) return 0;
    uint8_t* io = e.ioinfo(fh);
    if (io[4] & 2) return 0;
    char* dst = buf;
    int pre = 0;
    if ((io[4] & 0x48) && io[5] != '\n') {
        n--; *dst++ = (char)io[5]; pre = 1; io[5] = '\n';
    }
    uint32_t got;
    if (!e.readFile(handleOf(io), dst, n, &got)) {
        uint32_t err = e.getLastError();
        if (err == 5) { e.setErrnoDos(9, 5); }
        else { if (err == 0x6d) return 0; e.dosMapErr(err); }
        return -1;
    }
    if (!(io[4] & 0x80)) return pre + (int)got;
    if (got == 0 || *buf != '\n') io[4] &= 0xfb; else io[4] |= 4;
    char* end = buf + pre + got;
    char* p = buf;
    char* out = buf;
    while (p < end) {
        char c = *p;
        if (c == 0x1a) { if (!(io[4] & 0x40)) io[4] |= 2; break; }
        if (c != '\r') { *out++ = c; p++; continue; }
        if (p < end - 1) {
            if (p[1] == '\n') { p += 2; *out++ = '\n'; }
            else { p++; *out++ = '\r'; }
            continue;
        }
        char nx;
        uint32_t g;
        if ((!e.readFile(handleOf(io), &nx, 1, &g) && e.getLastError() != 0) || g == 0) { p++; *out++ = '\r'; continue; }
        if (!(io[4] & 0x48)) {
            if (out == buf && nx == '\n') { p++; *out++ = '\n'; continue; }
            e.seekBack1(fh);
            if (nx == '\n') { p++; continue; }
            p++; *out++ = '\r'; continue;
        }
        if (nx == '\n') { p++; *out++ = '\n'; }
        else { *out++ = '\r'; io[5] = (uint8_t)nx; p++; }
    }
    return (int)(out - buf);
}
