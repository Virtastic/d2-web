// BNClient 0xCFA0 (EAX = bytes already on disk, ECX = name; stack sock, total, FILETIME*, mode; ret 0x10): the download
// receive loop. Opens the file for write (OPEN_ALWAYS), seeks to `done` when nonzero; in mode 0x80000002 the progress word
// DAT+0x3F508 = done*100/total and Sleep(0). Then while done < total: recv(sock, buf, min(total-done, 0x5B4), 0); a result
// of -1 or <= 0, or the cancel word DAT+0x3F614 set, closes and returns 0; else write, SetFileTime(h, ft, ft, ft),
// done += r and (mode 0x80000002) the progress word again. Closes and returns 1.
#include "core/bnclient/bnclient_file_seams.h"
#include <emscripten.h>

struct BnCfa0Seams {
    const BnFileSeams* f;
    int32_t (*recv)(uint32_t sock, uint8_t* buf, uint32_t n, uint32_t flags);   // 0x7FEE WSOCK32 16
    void (*sleep)(uint32_t ms);                                                  // IAT 0x1905C Sleep
    uint32_t* progress;                                                          // DAT+0x3F508
    const uint32_t* cancel;                                                      // DAT+0x3F614
};

// 1.13c BNClient+0x0CFA0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_recv_to_file_cfa0(uint32_t done, const char* name, uint32_t sock, uint32_t total,
                                                                   const uint32_t* ft, uint32_t mode, const BnCfa0Seams* s) {
    uint8_t buf[0x5B4];
    uint32_t written = 0;
    const uint32_t h = s->f->open(name, 0x40000000u, 0, 0, 4, 0x80, 0);
    if (h == 0xFFFFFFFFu) return 0;
    if (done != 0) s->f->seek(h, done, 0, 0);
    if (mode == 0x80000002u) { *s->progress = done * 100u / total; s->sleep(0); }
    while (done < total) {
        uint32_t n = total - done;
        if (n > 0x5B4) n = 0x5B4;
        const int32_t r = s->recv(sock, buf, n, 0);
        if (r == -1 || r <= 0 || *s->cancel != 0) { s->f->close(h); return 0; }
        s->f->write(h, buf, (uint32_t)r, &written, 0);
        s->f->set_time(h, ft, ft, ft);
        done += (uint32_t)r;
        if (mode == 0x80000002u) *s->progress = done * 100u / total;
    }
    s->f->close(h);
    return 1;
}
