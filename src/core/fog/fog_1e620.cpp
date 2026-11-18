// Fog: split a D2QSSocket receive buffer into frames. Retail (disassembly, not the
// batch port) is stdcall ret 8 with EBX = socket and returns the carried-over byte count in EAX. The frame length is the
// u16 at src+sock[+0x11C]; the payload starts at src+sock[+0x118]. Length >= 0x2064 is fatal 0xFB, a bad handler is 0x101.
#include <cstring>

#include "fog_1e620.h"

namespace {
uint32_t ld32(const uint8_t* base, uint32_t off) {
    uint32_t v;
    memcpy(&v, base + off, 4);
    return v;
}
}  // namespace

int fog_qssocket_frames_1e620(uint8_t* sock, uint8_t* dst, int total, const FogFrameSeams* s) {
    uint8_t* src = dst;
    for (;;) {
        const int off = (int)ld32(sock, 0x11C);
        uint16_t lenw;
        memcpy(&lenw, src + off, 2);
        const uint32_t len = lenw;
        const int min = (int)ld32(sock, 0x118);
        const int remain = (int)(dst - src) + total;
        if (remain < min || remain < (int)len) {
            memmove(dst, src, (size_t)remain);   // retail: rep movsl then movsb, a forward copy
            return remain;
        }
        if (len >= 0x2064) s->fatal(0xFB);
        uint8_t* pkt = src + min;
        const uint32_t cnt = len - (uint32_t)min;   // retail EBP = len - min (queued when the frame is a payload)
        const uint32_t hdl = ld32(sock, 0x168);
        const uint32_t q = (uint32_t)(uintptr_t)(sock + 0x170);
        if (hdl) {
            if (s->isBadCodePtr(hdl)) s->fatal(0x101);
            const uint32_t ctx = ld32(sock, 0x16C);
            const uint32_t r = off > 0 ? s->handler((uint32_t)(uintptr_t)src, len, ctx)
                                       : s->handler((uint32_t)(uintptr_t)pkt, cnt, ctx);
            if (r) {   // consumed: the frame is dropped
                src += len;
                continue;
            }
            if (off > 0) s->queue(len, q, (uint32_t)(uintptr_t)src);
            else s->queue(cnt, q, (uint32_t)(uintptr_t)pkt);
        } else {
            s->queue(cnt, q, (uint32_t)(uintptr_t)pkt);
        }
        s->setEvent(ld32(sock, 0x114));
        src += len;
    }
}
