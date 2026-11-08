// 1.13c Storm+0x27B80: stream service step over the AUDIOSTREAM list.
// Record words are 32-bit retail addresses. The globals and every callee are seams so the step runs on fixed inputs:
// cursor = 0x25DC0 (ESI = record), fill = 0x22C50, lock/unlock/stop = the buffer's vtable +0x2C/+0x4C/+0x48,
// queue = 0x27A70 (EBX = owner, 13 stdcall words).
#include <cstdint>
#include <cstring>

struct Storm27B80Env {
    uint32_t g4311c, g459cc, g43114, g43140;
    uint32_t (*cursor)(uint32_t rec);
    void (*fill)(uint32_t p, uint32_t n, uint32_t byte);
    int32_t (*lock)(uint32_t buf, uint32_t off, uint32_t len, uint32_t* p, uint32_t* n);
    void (*unlock)(uint32_t buf, uint32_t p, uint32_t n);
    uint32_t (*stop)(uint32_t buf);
    void (*queue)(uint32_t owner, const uint32_t* a);
};

namespace {
inline uint32_t& W(uint32_t base, uint32_t off) { return *(uint32_t*)(uintptr_t)(base + off); }
}

extern "C" uint32_t Storm13c_27B80(const Storm27B80Env* e) {
    if (e->g4311c != 0) return e->g4311c;
    uint32_t ret = e->g459cc;
    const uint32_t g = e->g43114;
    for (uint32_t s = ((int32_t)e->g459cc > 0) ? e->g459cc : 0; (int32_t)s > 0; s = W(s, 4)) {
        const uint32_t owner = W(s, 8);
        bool toTail = true;   // fall into the per-state code (LAB_6fc17ce0)
        if (W(s, 0x18) == 3 || (W(s, 0x18) == 2 && W(s, 0x24) + W(s, 0x20) <= W(owner, 0x11c))) {
            if (W(s, 0x14) != 0) {
                W(owner, 0x11c) = W(s, 0x20); W(s, 0x30) = 0; W(s, 0x1c) = 0;
            } else {
                toTail = false;
                ret = e->cursor(s);
                if (ret != 0xFFFFFFFFu) {
                    if (W(s, 0x2c) < W(s, 0x24)) {
                        if (g <= (ret - W(s, 0xc)) % W(s, 0x40)) {
                            uint32_t n = W(s, 0x40) - W(s, 0xc);
                            if (n > g) n = g;
                            uint32_t p = 0;
                            if (e->lock(W(s, 0x3c), W(s, 0xc), n, &p, &n) >= 0) {
                                e->fill(p, n, *(uint8_t*)(uintptr_t)(s + 0x48));
                                e->unlock(W(s, 0x3c), p, n);
                                const uint32_t c = W(s, 0xc) + n;
                                W(s, 0x30) += n; W(s, 0x1c) += n; W(s, 0xc) = c;
                                if (W(s, 0x40) <= c) W(s, 0xc) = c - W(s, 0x40);
                            }
                        }
                        W(s, 0x18) = 3;
                        toTail = true;
                    } else {
                        ret = e->stop(W(s, 0x3c));
                        W(s, 0x18) = 4;
                    }
                }
            }
        }
        if (!toTail) continue;
        ret = W(s, 0x18);
        if (ret == 2) {
            ret = e->cursor(s);
            if (ret != 0xFFFFFFFFu) {
                const uint32_t sz = W(s, 0x40), d = ret - W(s, 0xc);
                ret = d / sz;
                if (g <= d % sz) {
                    uint32_t len = sz - W(s, 0xc);
                    if (g < len) len = g;
                    const uint32_t pos = W(owner, 0x11c);
                    const uint32_t left = (W(s, 0x24) + W(s, 0x20)) - pos;
                    if (left <= len) len = left;
                    if (e->g43140 != 0) {
                        const uint32_t al = (W(W(owner, 0x110), 0x120) - 1u) & (len + pos);
                        if (al != 0 && al < len) len -= al;
                    }
                    const uint32_t a[13] = { pos, 0, 0, W(s, 0x3c), W(s, 0xc), s, len,
                                             ((W(s, 0x40) + g * (uint32_t)-2) * 1000u) / W(s, 0x10), 0, 0, 1, 0, 0 };
                    e->queue(owner, a);
                    W(owner, 0x11c) += len;
                    ret = W(s, 0xc) + g;
                    W(s, 0xc) = ret;
                    if (W(s, 0x40) <= ret) { ret -= W(s, 0x40); W(s, 0xc) = ret; }
                }
            }
        } else if (ret == 0) {
            uint32_t total = W(s, 0x24) - W(s, 0x28), done = 0, delay = 0;
            if (W(s, 0x40) < total) total = W(s, 0x40);
            W(owner, 0x11c) = W(s, 0x20) + W(s, 0x28);
            if (e->g43140 != 0) {
                done = (total <= g) ? total : g;
                const uint32_t pos = W(owner, 0x11c);
                const uint32_t al = (W(W(owner, 0x110), 0x120) - 1u) & (pos + done);
                if (al != 0 && al < done) done -= al;
                const uint32_t a[13] = { pos, 0, 0, W(s, 0x3c), 0, s, done, 0, 0, 0, 1, 0, 0 };
                e->queue(owner, a);
                W(owner, 0x11c) += done;
            }
            while (done < total) {
                const uint32_t c = (total - done < g) ? total - done : g;
                const uint32_t a[13] = { W(owner, 0x11c), 0, 0, W(s, 0x3c), done, s, c, delay, 0, 0, 1, 0, 0 };
                e->queue(owner, a);
                done += g;
                delay += (g * 1000u) / W(s, 0x10);
                W(owner, 0x11c) += c;
            }
            ret = done / W(s, 0x40);
            W(s, 0x18) = 1;
            W(s, 0xc) = done % W(s, 0x40);
        }
    }
    return ret;
}
