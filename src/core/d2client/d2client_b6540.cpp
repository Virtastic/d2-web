// 1.13c D2Client+0xB6540 (ecx = unit; returns 1): for a unit whose class indexes a valid Missiles row and whose 0xC388 value indexes a valid Skills row, builds a request block (0xB30F0 takes it in ECX) of type 0x4001 and walks the row's dword count (+0x6C) of spawns over the static offset tables at 0xD3BF8/0xD3C0C (period 5), then a 0x8001 block (0x15E0 value in the last slot; a hit with +0x64 set calls 0xA8B60(0xC)), then 0xC001 blocks over the counts at +0x70 and +0x74 on the tables at 0xD3C20/0xD3C68 (period 0x12). The tables are read through e.gl (the 0xB8 bytes at 0xD3BF8).
#include "cl_env.h"
extern "C" uint32_t d2c_b6540(uint32_t u, const cl::Env& e) {
    if (!u) return 1;
    int32_t idx = (int32_t)e.R(u + 4);
    if (idx < 0 || idx >= (int32_t)e.R(e.dt + 0xb6c)) return 1;
    const uint32_t mrow = e.R(e.dt + 0xb64) + (uint32_t)idx * 0x1a4;
    if (!mrow) return 1;
    const uint32_t sk = e.H(0xc388, u);
    const uint32_t ck = e.H(0xc3fa, u);
    if ((int32_t)sk < 0 || (int32_t)sk >= (int32_t)e.R(e.dt + 0xba0)) return 1;
    if (!(e.R(e.dt + 0xb98) + sk * 0x23c)) return 1;
    const uint32_t a = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    const uint32_t x = e.PD(u, 0xc3be, 0xc), y = e.PD(u, 0xc2ec, 0x10);
    const uint32_t g = e.gl;
    uint32_t q[23] = {};
    const uint32_t* qa = q;
    q[0] = 0x4001; q[1] = a; q[11] = sk; q[12] = ck;
    const uint32_t qp = (uint32_t)(uintptr_t)qa;
    int32_t w = e.R16s(mrow + 0x2c);
    if (w >= 0) {
        const uint32_t n = e.R(mrow + 0x6c);
        q[4] = (uint32_t)w;
        for (uint32_t i = 0; (int32_t)i < (int32_t)n; i++) {
            const uint32_t k = (i % 5) * 4;
            q[6] = e.R(g + k) + y; q[5] = e.R(g + 0x14 + k) + x;
            e.HB(0xb30f0, qp);
        }
    }
    w = e.R16s(mrow + 0x2e);
    if (w >= 0) {
        q[0] = 0x8001; q[4] = (uint32_t)w;
        q[19] = e.H(0x15e0, 0, 0, 0, 0, 0, sk, 0, ck);
        q[5] = x; q[6] = y;
        const uint32_t r = e.HB(0xb30f0, qp);
        if (r && e.R(r + 0x64)) e.H(0xa8b60, 0, 0, 0, 0, 0, 0xc);
    }
    int32_t cnt = 0;
    q[0] = 0xc001;
    w = e.R16s(mrow + 0x30);
    if (w >= 0) {
        cnt = (int32_t)e.R(mrow + 0x70);
        q[4] = (uint32_t)w;
        for (int32_t i = 0; i < cnt; i++) {
            const uint32_t k = ((uint32_t)i % 0x12) * 4;
            q[6] = e.R(g + 0x28 + k) + y; q[5] = e.R(g + 0x70 + k) + x;
            e.HB(0xb30f0, qp);
        }
    }
    w = e.R16s(mrow + 0x32);
    if (w >= 0) {
        const int32_t end = (int32_t)e.R(mrow + 0x74) + cnt;
        q[4] = (uint32_t)w;
        for (int32_t i = cnt; i < end; i++) {
            const uint32_t k = (uint32_t)(i % 0x12) * 4;
            q[6] = e.R(g + 0x28 + k) + y; q[5] = e.R(g + 0x70 + k) + x;
            e.HB(0xb30f0, qp);
        }
    }
    return 1;
}
