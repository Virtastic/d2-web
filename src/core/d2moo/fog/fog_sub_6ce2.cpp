// 1.13c Fog+0x06CE2 -- MSVCRT _read_lk(fh, buf, cnt): text-mode read with CRLF folding, ^Z EOF, lookahead byte.
#include "fog_crt_host.h"

namespace fogcrt { Host host{}; }
using namespace fogcrt;

namespace {
void setErr(int e, uint32_t dos) { *host.errnoPtr() = e; *host.doserrnoPtr() = dos; }
}

int fog_readLk(uint32_t fh, char* buf, uint32_t cnt) {
    if (!cnt) return 0;
    IoInfo* io = host.slot(fh);
    if (io->flags & FEOFLAG) return 0;
    char* out = buf;
    int pre = 0;
    if ((io->flags & (FDEV | FPIPE)) && io->peek != '\n') {
        cnt--; *buf = io->peek; out = buf + 1; pre = 1; io->peek = '\n';
    }
    uint32_t got = 0;
    if (!host.readFile(io->handle, out, cnt, &got)) {
        uint32_t e = host.lastError();
        if (e == 5) setErr(9, 5);
        else if (e == 0x6d) return 0;
        else host.dosmaperr(e);
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
        char* nxt = src + 1; char la = 0;
        if ((!host.readFile(io->handle, &la, 1, &got) && host.lastError() != 0) || got == 0) {
            src = nxt; *dst++ = '\r'; continue;
        }
        if (io->flags & (FDEV | FPIPE)) {
            if (la == '\n') { src = nxt; *dst++ = '\n'; }
            else { *dst++ = '\r'; io->peek = la; src = nxt; }
        } else {
            if (dst == buf && la == '\n') { src = nxt; *dst++ = '\n'; continue; }
            host.lseek(fh, -1, -1, 1);
            if (la == '\n') { src = nxt; continue; }
            src = nxt; *dst++ = '\r';
        }
    }
    return (int)(dst - buf);
}
