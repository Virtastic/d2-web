// 1.13c Fog+0xF3C0: uint __stdcall(srv, buf, len), ret 0xC -- shut a QServer down and free it; srv == 0 returns 1.
// buf != 0: 0xF2F0(srv, buf, len). Mode (+0x86C) not 1 or 2: 0xE9A0 (EAX = srv). Then 0xD5E0 (EBX = srv), 0xD710 (EDI = srv), 0xE7E0(srv).
// The four sockets at +0xB90 that are not -1: Ordinal_3 (thunk 0xCD82) closes each and the slot becomes -1. Ordinal_116 (0xCD8E).
// Locks +0x854 (EDX 0x16F), +0x894 (0x175), +0x8B4 (0x178): helper 0x11690, Enter, Delete. Free chains +0x878 (next +0x5BC, line 0x185) and +0x88C
// (next +0xC, line 0x18B), then +0x870 (0x18F), +0x874 (0x190), +0x40 (0x194), +0x8AC (0x195), +0x20 (0x197); lock +0x28 (0x198) deleted; free +0x14 (0x19B);
// the per-bucket locks +0x1C + i * 0x18 for i < Fog+0x2FEE4 (EDX 0x19F) deleted; free +0x1C (0x1A2), then srv (0x1A5). Frees are 0x1B0C0
// (EAX 0, ECX = &pointer, stack file 0x6FF78FAC, line). Returns 1.
#include "fog_env.h"
static void fe_free(const FogEnv* e, uint32_t p, uint32_t line) {
    uint32_t local = p;
    fe_call(e, 0x1B0C0, 0, (uint32_t)(uintptr_t)&local, 0, 0, 0, 0x6FF78FAC, line);
}
static void fe_lock_destroy(const FogEnv* e, uint32_t cs, uint32_t line) {
    fe_call(e, 0x11690, 0, cs, line);
    fe_enter(e, cs);
    if (e->destroy) e->destroy(e->ctx, cs);
}
extern "C" uint32_t fog_f3c0(const FogEnv* e, uint32_t srv, uint32_t buf, uint32_t len) {
    if (!srv) return 1;
    if (buf) fe_call(e, 0xF2F0, 0, 0, 0, 0, 0, srv, buf, len);
    uint32_t mode = fe_r32(srv + 0x86c);
    if (mode != 1 && mode != 2) fe_call(e, 0xE9A0, srv);
    fe_call(e, 0xD5E0, 0, 0, 0, srv);
    fe_call(e, 0xD710);
    fe_call(e, 0xE7E0, 0, 0, 0, 0, 0, srv);
    for (uint32_t i = 0; i < 4; i++) {
        uint32_t s = fe_r32(srv + 0xb90 + 4 * i);
        if (s != 0xFFFFFFFFu) { fe_call(e, 0xCD82, 0, 0, 0, 0, 0, s); fe_w32(srv + 0xb90 + 4 * i, 0xFFFFFFFFu); }
    }
    fe_call(e, 0xCD8E);
    fe_lock_destroy(e, srv + 0x854, 0x16f);
    fe_lock_destroy(e, srv + 0x894, 0x175);
    fe_lock_destroy(e, srv + 0x8b4, 0x178);
    for (uint32_t n = fe_r32(srv + 0x878); n;) { uint32_t nx = fe_r32(n + 0x5bc); fe_free(e, n, 0x185); fe_w32(srv + 0x878, nx); n = nx; }
    for (uint32_t n = fe_r32(srv + 0x88c); n;) { uint32_t nx = fe_r32(n + 0xc); fe_free(e, n, 0x18b); fe_w32(srv + 0x88c, nx); n = nx; }
    fe_free(e, fe_r32(srv + 0x870), 0x18f);
    fe_free(e, fe_r32(srv + 0x874), 0x190);
    fe_free(e, fe_r32(srv + 0x40), 0x194);
    fe_free(e, fe_r32(srv + 0x8ac), 0x195);
    fe_free(e, fe_r32(srv + 0x20), 0x197);
    fe_lock_destroy(e, srv + 0x28, 0x198);
    fe_free(e, fe_r32(srv + 0x14), 0x19b);
    for (uint32_t i = 0; (int32_t)i < (int32_t)e->lockCount; i++) fe_lock_destroy(e, fe_r32(srv + 0x1c) + i * 0x18, 0x19f);
    fe_free(e, fe_r32(srv + 0x1c), 0x1a2);
    fe_free(e, srv, 0x1a5);
    return 1;
}
