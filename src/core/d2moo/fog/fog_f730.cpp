// 1.13c Fog+0xF730 (ordinal 10149): int* __stdcall(mode, nq, p3, p4, p5, p6, p7, p8), ret 0x20 -- create a QServer. Module globals Fog+0x2FEE4 / 0x2FEE8 /
// 0x2FEEC / 0x2FEF0 and Fog+0x56E80 arrive as gl[0..4]. Allocations all go through 0x1B560 (EAX size, ECX 0, file 0x6FF78FAC, line); a null result is the
// fatal 0x10F30 log (it does not return in retail). Block 0x94: 0x313 zeroed dwords, then [3] = gl1, [2] = [6] = gl0, [4] = gl2, [0x306] = gl4;
// +0x14 (line 0x9F, gl0 dwords), +0x1C (0xA2, gl0 * 0x18 bytes, one critical section per 0x18), [9] = gl0, +0x20 (0xA9, gl0 dwords), +0x44 = gl2,
// +0x40 (0xAF, gl2 dwords), [0x22C] = gl3, [0x22B] (0xB4, gl3 dwords), the fixed fields, the queue arrays [0x21C] / [0x21D] (lines 0xDA / 0xDE, nq dwords
// zeroed), [0x21F] = nq. The platform probe (GetVersionExA, platform 2 and p4 == 0 sets [0] = 1) is pinned to a non-NT platform: [0] = 0. [0x21B] = mode;
// a mode other than 1 / 2 runs the winsock start 0xCDA0(0x101, &data); success runs 0x17850(p3) with EDI = the server (retail pushes the third
// argument, [esp+0x3CC] at the call; the decompile's 17850(mode) is wrong) (the [0] != 0 branch is unreachable here).
// InitializeCriticalSection writes nothing the vector can see and has no seam.
#include <cstring>
#include "fog_env.h"
static uint32_t f730_alloc(const FogEnv* e, uint32_t size, uint32_t line) {
    uint32_t p = fe_call(e, 0x1B560, size, 0, 0, 0, 0, 0x6FF78FAC, line);
    if (!p) fe_call(e, 0x10F30, 0, 0, 0, 0, 0, 2, 0x6FF78FAC, line, 0x6FF768EC);
    return p;
}
extern "C" uint32_t fog_f730(const FogEnv* e, const uint32_t* gl, uint32_t mode, uint32_t nq, uint32_t p3, uint32_t p4, uint32_t p5, uint32_t p6,
                             uint32_t p7, uint32_t p8) {
    (void)p4;
    uint32_t s = f730_alloc(e, 0xc4c, 0x94);
    if (!s) return 0;
    auto W = [&](uint32_t i, uint32_t v) { fe_w32(s + i * 4, v); };
    memset(fe_p(s), 0, 0x313 * 4);
    W(3, gl[1]); W(2, gl[0]); W(0x306, gl[4]); W(6, gl[0]); W(4, gl[2]);
    uint32_t z = f730_alloc(e, gl[0] * 4, 0x9f);
    if (!z) return 0;
    W(5, z);
    memset(fe_p(z), 0, gl[0] * 4);
    z = f730_alloc(e, gl[0] * 0x18, 0xa2);
    if (!z) return 0;
    W(7, z);
    memset(fe_p(z), 0, ((gl[0] * 3) & 0x1fffffff) * 8);
    W(9, gl[0]);
    z = f730_alloc(e, gl[0] * 4, 0xa9);
    if (!z) return 0;
    W(8, z);
    memset(fe_p(z), 0, gl[0] * 4);
    W(0x11, gl[2]);
    z = f730_alloc(e, gl[2] * 4, 0xaf);
    if (!z) return 0;
    W(0x10, z);
    memset(fe_p(z), 0, gl[2] * 4);
    W(0x22c, gl[3]);
    z = f730_alloc(e, gl[3] * 4, 0xb4);
    if (!z) return 0;
    W(0x22b, z);
    memset(fe_p(z), 0, gl[3] * 4);
    W(0x2f1, p6); W(0x2f0, p5); W(0x220, 0); W(0x213, 0); W(0x214, 0);
    W(0x2f2, p7); W(0x2f3, p8); W(0x222, 0xffffffff); W(0x224, 0); W(0x2f4, 1);
    W(0x2e4, 0xffffffff); W(0x2e5, 0xffffffff); W(0x2e6, 0xffffffff); W(0x2e7, 0xffffffff);
    z = f730_alloc(e, nq * 4, 0xda);
    if (!z) return 0;
    W(0x21c, z);
    for (int32_t i = 0; i < (int32_t)nq; i++) fe_w32(z + i * 4, 0);
    z = f730_alloc(e, nq * 4, 0xde);
    if (!z) return 0;
    W(0x21d, z);
    for (int32_t i = 0; i < (int32_t)nq; i++) fe_w32(z + i * 4, 0);
    W(0x21f, nq); W(0x21e, 0); W(0x223, 0);
    W(0, 0);
    W(0x21b, mode);
    if (mode != 1 && mode != 2 && fe_call(e, 0xCDA0, 0, 0, 0, 0, 0, 0x101, 0xB3000000) == 0) fe_call(e, 0x17850, 0, 0, 0, 0, 0, p3);
    return s;
}
