// Register-arg unit helpers (EAX/ESI/EDI carry the unit); env-seam style, callees/globals come from the host.
#include <cstdint>
#include <emscripten.h>
#include "batch_b46_env.h"
extern "C" {
// 1.13c D2Client+0x4BFF0  (EDI = unit)
EMSCRIPTEN_KEEPALIVE const uint8_t* d2c_4bff0(const B46Env* e, const int32_t* u) {
    if (!u || u[0] != 1) return nullptr;
    uint32_t t = (uint32_t)u[4];
    if ((int32_t)t >= 0x10) return nullptr;
    int32_t c = u[1];
    if ((t == 0xb && (c == 0xf3 || c == 0x14d))) return e->tbl22b0;
    if (t == 10 && (c == 0x21f || c == 0x220 || c == 0x23a)) return e->tbl22bc;
    uint8_t* p = e->f1260();
    if (!p) return nullptr;
    if ((1u << (t & 7)) & p[(t >> 3) + 0x104]) return e->tbl2268 + t * 0xc;
    if (t == 0 && e->o10707()) return e->tbl2268;
    return e->tbl21a8 + (uint32_t)u[4] * 0xc;
}
// 1.13c D2Client+0x4C150  (EAX = unit)
EMSCRIPTEN_KEEPALIVE void d2c_4c150(const B46Env* e, int32_t* u) {
    if (!u || u[0] != 1) return;
    int32_t i = u[1];
    int32_t n = *(int32_t*)(e->dataTables + 0xa80);
    if (i < 0 || i >= n) return;
    uint8_t* row = *(uint8_t**)(e->dataTables + 0xa78) + (uint32_t)i * 0x1a8;
    uint8_t* pl = e->f1260();
    if (!pl) return;
    uint8_t b = e->o11077();
    if (b < pl[0x11e]) b = pl[0x11e];
    uint32_t v = b;
    if (e->o10826(e->o10331()) == 8) {
        if (e->g_cc0a0 == 0) { e->o10024(e->o10265(0x60)); e->exit_(-1); return; }
        if (e->g_c5591 != 0 && !((uint32_t)u[0x31] >> 0x15 & 1) && row[0x4c] != 1 && row[0x4c] != 2) v = 3;
        else if (b == 0) return;
    } else if (b == 0) return;
    int32_t* r = e->f58c20(v);
    if (u[0x19]) e->f55420();
    u[0x19] = (int32_t)(intptr_t)r;
    e->f54b00(v);
}
// 1.13c D2Client+0x4C370  (EAX = unit, stack/ecx pair: param_2 = word)
EMSCRIPTEN_KEEPALIVE void d2c_4c370(const B46Env* e, int32_t* u, uint16_t w) {
    if (!u || u[0] != 1) return;
    int32_t* p = (int32_t*)(intptr_t)u[5];
    if (!p) return;
    *(uint16_t*)(p + 10) = w;
    int32_t i = u[1];
    if (i >= 0 && i < *(int32_t*)(e->dataTables + 0xa80))
        *p = (int32_t)(intptr_t)(*(uint8_t**)(e->dataTables + 0xa78) + (uint32_t)i * 0x1a8);
    else *p = 0;
}
// 1.13c D2Client+0x4DC30  (EAX = unit)
EMSCRIPTEN_KEEPALIVE void d2c_4dc30(const B46Env* e, const int32_t* u) {
    if (!u || u[0] != 1) return;
    uint32_t t = (uint32_t)u[4];
    if (t < 0x10 && t != 0 && t != 0xc) { e->f55fb0(); e->o10770(u[0xb]); e->f2a6c0(); }
}
// 1.13c D2Client+0x4EE40  (ESI = unit, param = struct with mode at +4)
EMSCRIPTEN_KEEPALIVE int32_t d2c_4ee40(const B46Env* e, int32_t* u, const int32_t* p) {
    if (u && u[0] == 1) {
        int32_t i = u[1];
        if (i < 0 || i >= *(int32_t*)(e->dataTables + 0xa80)) return 0;
        uint8_t* row = *(uint8_t**)(e->dataTables + 0xa78) + (uint32_t)i * 0x1a8;
        if (row[0x1a4] == 1 && u[4] == 0 && e->o10426()) {
            int32_t z[7] = {0};
            e->fdc70(9, u, z);
        }
    } else if (!u) return 0;   // null unit falls out before the switch
    switch (p[1]) {
    case 0: break;
    case 1: { int32_t r = e->o10426(); if (r) return r; break; }
    case 2:
        if (e->o10426()) {
            u[0x11] = u[0x12] - 0x100; *(uint16_t*)(u + 0x13) = 0;
            if (u[4] == 0) u[0x31] |= 0x20;
            return 0;
        }
        break;
    case 3: u[0x11] = 0x100; *(uint16_t*)(u + 0x13) = 0; return 0;
    default: return 0;
    }
    e->o10853();
    return 0;
}
// 1.13c D2Client+0x4F770  (returns the raw 32-bit constant chosen; callee reads it as a float)
EMSCRIPTEN_KEEPALIVE uint32_t d2c_4f770(const B46Env* e) {
    if (e->g_bf78 == 0 && e->g_bbfc && *e->g_bbfc) {
        int32_t h = e->o10826(e->o10331());
        int8_t c = e->o10215(h);
        e->o10499(h);
        if (c) return *e->k38f4;
    }
    return *e->k7578;
}
// 1.13c D2Client+0x4FFC0
EMSCRIPTEN_KEEPALIVE void d2c_4ffc0(const B46Env* e) {
    int32_t a = *e->g_cc300p;
    if (a) { e->o10065(a); e->o10043(0x7a, 0); *e->g_cc300p = 0; }
    int32_t b = *e->g_cc304p;
    if (b) { e->o10065(b); e->o10043(0x7a, 0); *e->g_cc304p = 0; }
}
// 1.13c D2Client+0x510B0  (squared distance between two points; callee pairs are register-bound)
EMSCRIPTEN_KEEPALIVE int32_t d2c_510b0(const B46Env* e) {
    int32_t a = e->o10651(), b = e->o10651(), c = e->o11142(), d = e->o11142();
    return (int32_t)((uint32_t)(c - d) * (uint32_t)(c - d) + (uint32_t)(a - b) * (uint32_t)(a - b));
}
}
