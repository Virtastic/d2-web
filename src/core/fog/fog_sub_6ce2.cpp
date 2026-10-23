// 1.13c Fog+0x06CE2 -- MSVCRT _read_lk(fh, buf, cnt): text-mode read with CRLF folding, ^Z EOF, lookahead byte.
#include <cstdint>

namespace {
// ioinfo slot (0x24 bytes in retail): handle, flags (+4), peek char (+5).
struct FogIoInfo { void* handle; uint8_t flags; char peek; };
enum { FEOFLAG = 0x02, FCRLF = 0x04, FPIPE = 0x08, FDEV = 0x40, FTEXT = 0x80 };

// Host seams for Win32 / CRT calls.
struct FogReadHost {
    FogIoInfo* (*slot)(uint32_t fh);
    int (*readFile)(void* h, void* buf, uint32_t n, uint32_t* got);   // nonzero = ok
    uint32_t (*lastError)();
    void (*dosmaperr)(uint32_t err);
    void (*setErrnoBadf)();           // errno = EBADF
    void (*setDosErrAccess)();        // _doserrno = 5
    void (*seekBackOne)(uint32_t fh); // __lseeki64_lk(fh, -1, SEEK_CUR)
};
FogReadHost g_host{};
}

int fog_readLk(uint32_t fh, char* buf, uint32_t cnt) {
    if (!cnt) return 0;
    FogIoInfo* io = g_host.slot(fh);
    if (io->flags & FEOFLAG) return 0;
    char* out = buf;
    int pre = 0;
    if ((io->flags & (FDEV | FPIPE)) && io->peek != '\n') {
        cnt--; *buf = io->peek; out = buf + 1; pre = 1; io->peek = '\n';
    }
    uint32_t got;
    if (!g_host.readFile(io->handle, out, cnt, &got)) {
        uint32_t e = g_host.lastError();
        if (e == 5) { g_host.setErrnoBadf(); g_host.setDosErrAccess(); }
        else if (e == 0x6d) return 0;
        else g_host.dosmaperr(e);
        return -1;
    }
    if (!(io->flags & FTEXT)) return pre + (int)got;
    if (got == 0 || *buf != '\n') io->flags &= ~FCRLF; else io->flags |= FCRLF;
    char* end = buf + pre + got;
    char* src = buf; char* dst = buf;
    while (src < end) {
        char c = *src;
        if (c == 0x1a) { if (!(io->flags & FDEV)) io->flags |= FEOFLAG; break; }
        if (c != '\r') { *dst++ = c; src++; continue; }
        if (src < end - 1) {
            if (src[1] == '\n') { src += 2; *dst++ = '\n'; }
            else { src++; *dst++ = '\r'; }
            continue;
        }
        char* nxt = src + 1; char la; uint32_t g2;
        if ((!g_host.readFile(io->handle, &la, 1, &g2) && g_host.lastError() != 0) || g2 == 0) {
            src = nxt; *dst++ = '\r'; continue;
        }
        if (io->flags & (FDEV | FPIPE)) {
            if (la == '\n') { src = nxt; *dst++ = '\n'; }
            else { *dst++ = '\r'; io->peek = la; src = nxt; }
        } else {
            if (dst == buf && la == '\n') { src = nxt; *dst++ = '\n'; continue; }
            g_host.seekBackOne(fh);
            if (la == '\n') { src = nxt; continue; }
            src = nxt; *dst++ = '\r';
        }
    }
    return (int)(dst - buf);
}
