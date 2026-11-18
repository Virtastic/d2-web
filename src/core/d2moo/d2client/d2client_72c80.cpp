// 1.13c D2Client+0x72C80 -- skill sound/event queuer. EBX = unit, stack skill (ret 4).
#include "cl_env.h"
extern "C" void d2c_72c80(uint32_t unit /*EBX*/, int32_t skill, const cl::Env& e) {
    if (skill < 0 || skill >= (int32_t)e.R(e.dt + 0xc4)) return;
    uint32_t rec = (uint32_t)skill * 0x3c + e.R(e.dt + 0xbc);
    if (!rec) return;
    if (e.R8(e.bm + 0xc) & e.R8(rec + 0x14)) return;
    int32_t c = e.R16s(rec + 0xa) < 0 ? -1 : e.R16s(rec + 0xa);
    int32_t a = e.R16s(rec + 2) < 0 ? -1 : e.R16s(rec + 2);
    int32_t b = e.R16s(rec + 4) < 0 ? -1 : e.R16s(rec + 4);
    uint32_t has = e.H(0xcb92, (uint32_t)skill);
    auto adj = [&](int32_t& v) {
        uint32_t s = e.H(0xcb80, (uint32_t)v);
        v += (int32_t)e.H(0x1380, 0, 0, 0, 0, 0, 0, unit + 0x20, 0, 0, s);
    };
    if (c != -1) adj(c);
    if (a != -1) adj(a);
    if (b != -1) adj(b);
    if (has) {
        e.H(0xc30a, unit, (uint32_t)skill, 0);
        if (e.H(0xcbaa, unit)) c = -1;
        e.H(0xc30a, unit, (uint32_t)skill, 1);
    }
    if (c != -1) {
        if (has) e.H(0x6b300, unit, 9, 0, (uint32_t)a, (uint32_t)b, (uint32_t)c);
        else     e.H(0x6b300, unit, 4, 0, (uint32_t)a, (uint32_t)b, (uint32_t)c);
        return;
    }
    if (a != -1) {
        if (has) e.H(0x6b300, unit, 8, 0, (uint32_t)skill, 0, (uint32_t)a);
        else     e.H(0x6b300, unit, 3, 0, 0, 0, (uint32_t)a);
    }
    if (b != -1) {
        if (has) e.H(0x6b300, unit, 8, 0, (uint32_t)skill, 0, (uint32_t)b);
        else     e.H(0x6b300, unit, 3, 0, 0, 0, (uint32_t)b);
    }
}
