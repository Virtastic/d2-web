// D2Client+0xAB7D0: timed-event ring drain; env-seam style. Ordinals 10265/10024 unverified.
#include <cstdint>
#include <emscripten.h>
struct D2cAB7D0Env {
    uint8_t** queue;        // PTR_DAT_6fb8e694 (RVA unrecorded): current 500-slot page, entries 16 bytes
    const int32_t* now;     // DAT_6fbc97f8
    int32_t* active;        // DAT_6fbcbbe8
    const int32_t* limit;   // DAT_6fbacd84
    void (*fire)();         // D2Client+0xAB600
    uint32_t (*ord10265)(int32_t id);               // unverified ordinal
    void (*ord10024)(const void* fmt, uint32_t s);  // unverified ordinal
    const void* fmt;        // DAT_6fb8359f
    void (*exit_)(int32_t); // D2Client+0x3664 (_exit)
};
extern "C" {
// 1.13c D2Client+0xAB7D0
EMSCRIPTEN_KEEPALIVE void d2c_ab7d0(const D2cAB7D0Env* e) {
    int32_t now = *e->now;
    if (!*e->active || !*e->queue) return;
    if (*e->limit <= now) { *e->active = 0; return; }
    uint8_t* q = *e->queue;
    int32_t i = *(int32_t*)(q + 0x1f44);
    if (i > 499) { e->ord10024(e->fmt, e->ord10265(0x189)); e->exit_(-1); return; }
    if (!(*(int32_t*)(q + 0x1f48) != 0 || *(int32_t*)(q + 8000) != i) || *(int32_t*)(q + i * 0x10) > now) return;
    for (;;) {
        e->fire();
        q = *e->queue;
        i = ++*(int32_t*)(q + 0x1f44);
        uint8_t* nx = *(uint8_t**)(q + 0x1f48);
        if (!nx && *(int32_t*)(q + 8000) == i) break;
        if (i > 499) { q = nx; *e->queue = nx; }
        if (!q) return;
        if (now < *(int32_t*)(q + *(int32_t*)(q + 0x1f44) * 0x10)) return;
    }
}
}
