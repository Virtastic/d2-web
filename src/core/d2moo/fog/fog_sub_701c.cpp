// 1.13c Fog+0x0701C -- MSVCRT _write_lk(fh, buf, cnt): an append slot seeks to the end first; a text slot expands \n to \r\n through a
// 1 KB stack chunk, one WriteFile per chunk, until a short write or the input ends; the result is the bytes written less the CRs added.
// Nothing written: ERROR_ACCESS_DENIED is errno 9 / _doserrno 5, another error goes through __dosmaperr, both -1; no error and a
// device slot whose first byte is ^Z is 0; otherwise errno 28 (ENOSPC), _doserrno 0, -1.
#include "fog_crt_host.h"
using namespace fogcrt;

int fog_writeLk(uint32_t fh, const char* buf, uint32_t cnt) {
    if (!cnt) return 0;
    IoInfo* io = host.slot(fh);
    if (io->flags & FAPPEND) host.lseek(fh, 0, 0, 2);
    uint32_t put = 0, total = 0, lfs = 0, err = 0;
    if (!(io->flags & FTEXT)) {
        if (!host.writeFile(io->handle, buf, cnt, &put)) err = host.lastError();
        else total = put;
    } else {
        const char* src = buf;
        for (;;) {
            char chunk[0x408];
            uint32_t n = 0;
            while (static_cast<uint32_t>(src - buf) < cnt) {
                char c = *src++;
                if (c == '\n') { ++lfs; chunk[n++] = '\r'; }
                chunk[n++] = c;
                if (n >= 0x400) break;
            }
            if (!host.writeFile(io->handle, chunk, n, &put)) { err = host.lastError(); break; }
            total += put;
            if (static_cast<int32_t>(n) > static_cast<int32_t>(put) || static_cast<uint32_t>(src - buf) >= cnt) break;
        }
    }
    if (total) return static_cast<int>(total - lfs);
    if (err) {
        if (err == 5) { *host.errnoPtr() = 9; *host.doserrnoPtr() = 5; }
        else host.dosmaperr(err);
        return -1;
    }
    if ((io->flags & FDEV) && *buf == 0x1a) return 0;
    *host.errnoPtr() = 0x1c; *host.doserrnoPtr() = 0;
    return -1;
}
