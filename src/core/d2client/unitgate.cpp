// D2Client unit-action gate helpers.
#include <cstdint>
#include "client_host.h"

// 1.13c D2Client+0x185E0. s: +0 flags, +4 p1 (+0x10 type), +8 p2 (dword). g = global at D2Client+0x1234C0.
extern "C" uint32_t d2_client_185e0(const uint32_t* s, uint32_t* g) {
    uint32_t f = s[0];
    if (f & 0x10) { *g = 0; return 0; }
    if (f & 4) { *g = (s[2] != 0) + 1; return 1; }
    if (!(f & 8)) return 1;
    if (*g == 0) return 0;
    const uint32_t* p2 = (const uint32_t*)(uintptr_t)s[2];
    if (!p2) return 1;
    int32_t t = (int32_t)*p2;
    if (t < 0 || !(t < 2 || t == 5)) return 0;
    const uint32_t* p1 = (const uint32_t*)(uintptr_t)s[1];
    if (!p1) return 1;
    uint32_t k = p1[4];
    return k != 2 && k != 3 && k != 6;
}

static uint32_t go(D2cHost h, D2cCall c) { return h(&c); }

// 1.13c D2Client+0xA54A0. EAX = u (unit-like: +0 x, +4 y, +0x10 kind, +0x60 arg of D2Common#11017), one stack arg (ret 4) that
// case 0x12 forwards to the 0xA4C50 callee (reads ESI = u). Callees go through the host seam in retail's order.
extern "C" uint32_t d2_client_a54a0(D2cHost h, const uint32_t* u, uint32_t uaddr, uint32_t sarg) {
    if (go(h, { 0xC202, 0, 0, 0, 0, 0, { u[0x18], 0, 0, 0 } }) != 0 || u[0] != 0) return 0;
    switch (u[4]) {
    case 0: case 4: case 7: case 8: case 9: case 10: case 11: case 12: case 0x11: case 0x13: return 0;
    case 0xD: return u[1] != 3;
    case 0xE: return u[1] != 6;
    case 0xF: case 0x10: return u[1] != 4 && u[1] != 6 && u[1] != 5;
    case 0x12: return go(h, { 0xA4C50, 0, 0, 0, 0, uaddr, { sarg, 0, 0, 0 } });
    default: return 1;
    }
}

// 1.13c D2Client+0x1BBC0. ESI = ctx (+0 flags, +4 unit, +8 p2, +0x1C c7). dt = *sgptDataTables (+0xB98 table base, +0xBA0 count,
// rows of 0x23C bytes, byte +0x14). g = D2Client+0x1234C0.
extern "C" uint32_t d2_client_1bbc0(D2cHost h, uint32_t* ctx, uint32_t ctxaddr, uint32_t* g, const uint32_t* dt) {
    if (!d2_client_185e0(ctx, g)) return 1;
    ctx[7] = go(h, { 0xC5CE, 0, 0, 0, 0, 0, { ctx[1], 0, 0, 0 } });
    if (!d2_client_a54a0(h, (const uint32_t*)(uintptr_t)ctx[1], ctx[1], ctx[7])) return 1;
    int32_t i = (int32_t)go(h, { 0xC42A, 0, 0, 0, 0, 0, { ctx[7], 0xB2000000u | 0xD6F7Cu, 0x9FD, 0 } });
    const uint8_t* row = 0;
    if (i >= 0 && i < (int32_t)dt[0xBA0 / 4]) row = (const uint8_t*)(uintptr_t)dt[0xB98 / 4] + (uint32_t)i * 0x23C;
    if ((row ? row[0x14] : 0) == 1 && !(ctx[0] & 0x40)) ctx[0] |= 0x80;
    go(h, { 0x1B7F0, ctxaddr, 0, 0, 0, 0, { 0, 0, 0, 0 } });
    return 1;
}

// 1.13c D2Client+0x1C890. Index of the first of 41 16-byte rows (table at D2Client+0xF8190) whose dword +0 == key; -1 if none.
extern "C" int32_t d2_client_1c890(const uint32_t* tbl, uint32_t key) {
    for (int32_t i = 0; i < 41; i++) if (tbl[i * 4] == key) return i;
    return -1;
}

// 1.13c D2Client+0x1BCC0. EDI = ctx (+0 flags, +4 unit, +8 p2, +0x1C c7), no args, returns 1. g = D2Client+0x1234C0.
// Callees (D2Common ordinals through thunks) go through the host seam; 0xCFC4 (#10809) returns its out word in out[0].
extern "C" uint32_t d2_client_1bcc0(D2cHost h, uint32_t* ctx, uint32_t ctxaddr, uint32_t* g) {
    if (!d2_client_185e0(ctx, g)) return 1;
    uint32_t ua = ctx[1];
    const uint32_t* u = (const uint32_t*)(uintptr_t)ua;
    ctx[7] = go(h, { 0xC5BC, 0, 0, 0, 0, 0, { ua, 0, 0, 0 } });
    if (!d2_client_a54a0(h, u, ua, ctx[7])) return 1;
    uint32_t ty = u ? u[1] : 0xFFFFFFFFu;
    D2cCall c = { 0xCFC4, 0, 0, 0, 0, 0, { ua, ty, 0xB3000000u, 0xB3000000u } };
    h(&c);
    uint32_t a = c.out[0] << 8;
    uint32_t ext = u[0x2C / 4];
    uint32_t R = go(h, { 0xC1C6, 0, 0, 0, 0, 0, { ua, 0x43, 0, 0 } });
    int32_t p = (int32_t)(R * a);   // retail: imul, then the /100 magic sequence (0x51EB851F, sar 5, + sign bit)
    int32_t q = (int32_t)(((int64_t)p * 0x51EB851F) >> 32) >> 5;
    q += (uint32_t)q >> 31;
    uint32_t fl = ctx[0];
    bool eq = go(h, { 0xC496, 0, 0, 0, 0, 0, { ext, 0, 0, 0 } }) == (uint32_t)q;
    bool f = eq == ((fl & 0x40) != 0);
    if (!ctx[2]) {
        if (!(fl & 0x20)) {
            if (go(h, { 0x2DA0, ua, 0, 0, 0, 0, { 0, 0, 0, 0 } })) return 1;
            ctx[0] = fl | 0x80;
        }
    } else {
        uint32_t p2 = ctx[2];
        if (p2 != go(h, { 0xA5F70, p2, 0, 0, 0, ua, { 0, 0, 0, 0 } }) || f) {
            go(h, { 0xC364, 0, 0, 0, 0, 0, { ext, p2, 0, 0 } });
            ctx[0] |= 0x80;
        }
    }
    go(h, { 0x1B7F0, ctxaddr, 0, 0, 0, 0, { 0, 0, 0, 0 } });
    return 1;
}
