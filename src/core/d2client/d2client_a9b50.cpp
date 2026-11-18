// 1.13c D2Client+0xA9B50 (317 B, ret 4): advance one light/view node by a step. EAX = node s, stack param.
// s[8] (+0x20) == 0 and s[0] != 6: node = A4E20(EAX = key & 0x7F, ECX = key, EDX = table, stack type) with table = D2Client+0x10A608 (flag byte
// zero) or +0x109A08, plus type * 0x200; a hit sets x = (C1AE(node) >> 13) + 4, y = (C232(node) >> 13) + 4 (arithmetic shifts).
// cur (+0x18) moves 8 toward tgt (+0x1C); mode 2 first clears +0x2C and frees +0x30 (BF86(ECX = ptr, EDX = file 0xD4104, 0x4CC, 0)); cur >= 0x100 -> 0xF8.
// Then: f20 != 0 and cur <= 0: A8630; mode 0 and param > 1: A9480; mode != 2: A9860(EAX = param); mode 2: +0x2C == 0 allocates +0x30
// (BF7A(ECX = ((cur >> 3) * 2 + 1)^2 * 4, EDX = file, 0x4F1, 0)) and runs A9100; then A8F60 (EDI = ESI = s). A8630/A9480/A9860/A9100 read s through ESI.
#include <cstddef>
#include <emscripten.h>
#include "d2client_a9_env.h"
struct D2cA9b50S {
    int32_t type;      // +0x00
    uint32_t key;      // +0x04
    uint32_t flags;    // +0x08 (only the low byte is read)
    int32_t mode;      // +0x0C
    int32_t x;         // +0x10
    int32_t y;         // +0x14
    int32_t cur;       // +0x18
    int32_t tgt;       // +0x1C
    int32_t f20;       // +0x20
    uint32_t pad24;
    uint32_t next;     // +0x28 (the list link A9E30 follows)
    uint32_t p2c;      // +0x2C
    uint32_t p30;      // +0x30
};
static_assert(offsetof(D2cA9b50S, f20) == 0x20 && offsetof(D2cA9b50S, p2c) == 0x2c && offsetof(D2cA9b50S, p30) == 0x30, "layout");
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_a9b50(D2cA9b50S* s, int32_t param, const D2cA9Env* e) {
    const uint32_t sp = d2c_hp(s), file = 0xB2000000u | 0xD4104u;
    if (s->f20 == 0 && s->type != 6) {
        const uint32_t tbl = 0xB2000000u + ((uint8_t)s->flags == 0 ? 0x10A608u : 0x109A08u) + ((uint32_t)s->type << 9);
        const uint32_t node = e->call(e->ctx, 0xA4E20, s->key & 0x7f, s->key, tbl, 0, (uint32_t)s->type, 0, 0, 0, 0);
        if (node) {
            const int32_t px = (int32_t)e->call(e->ctx, 0xC1AE, 0, 0, 0, 0, node, 0, 0, 0, 0);
            const int32_t py = (int32_t)e->call(e->ctx, 0xC232, 0, 0, 0, 0, node, 0, 0, 0, 0);
            s->x = (px >> 13) + 4; s->y = (py >> 13) + 4;
        }
    }
    if (s->tgt != s->cur) {
        if (s->mode == 2) {
            const uint32_t old = s->p30;
            s->p2c = 0;
            if (old) e->call(e->ctx, 0xBF86, 0, old, file, 0, 0x4cc, 0, 0, 0, 0);
            s->p30 = 0;
        }
        s->cur += s->tgt > s->cur ? 8 : -8;
    }
    if (s->cur >= 0x100) s->cur = 0xf8;
    if (s->f20 != 0 && s->cur <= 0) { e->call(e->ctx, 0xA8630, 0, 0, 0, sp, 0, 0, 0, 0, 0); return; }
    if (s->mode == 0 && param > 1) { e->call(e->ctx, 0xA9480, 0, 0, 0, sp, 0, 0, 0, 0, 0); return; }
    if (s->mode != 2) { e->call(e->ctx, 0xA9860, (uint32_t)param, 0, 0, sp, 0, 0, 0, 0, 0); return; }
    if (s->p2c == 0) {
        if (s->p30 == 0) {
            const uint32_t n = (uint32_t)((s->cur >> 3) * 2 + 1);
            s->p30 = e->call(e->ctx, 0xBF7A, 0, n * n * 4, file, 0, 0x4f1, 0, 0, 0, 0);
        }
        e->call(e->ctx, 0xA9100, 0, 0, 0, sp, 0, 0, 0, 0, 0);
    }
    e->call(e->ctx, 0xA8F60, 0, 0, 0, sp, 0, 0, 0, 0, 0);
}
